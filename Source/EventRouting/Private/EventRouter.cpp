#include "AUI/EventRouter.h"
#include "AUI/Screen.h"
#include "AUI/Window.h"
#include "AUI/Widget.h"
#include "AUI/SDLHelpers.h"
#include "AUI/Core.h"
#include "AUI/Image.h"
#include "AUI/Internal/Log.h"
#include "AUI/Internal/AUIAssert.h"
#include <algorithm>

namespace AUI
{

EventRouter::EventRouter(Screen& inScreen)
: screen{inScreen}
, lastHoveredWidgetPath{}
, mouseCapturePath{}
, focusPath{}
, dragPath{}
, dragOrigin{}
, dragUnderway{false}
{
}

bool EventRouter::handleMouseButtonDown(SDL_MouseButtonEvent& event)
{
    // Check if the cursor is over an AUI window, or if it missed.
    RouterReturnData returnData{};
    SDL_Point cursorPosition{event.x, event.y};
    WidgetPath clickPath{getPathUnderCursor(cursorPosition)};
    if (!(clickPath.empty())) {
        Widget* previousFocusedWidget{getFocusedWidget()};

        // Route a MouseDown or MouseDoubleClick depending on how many clicks
        // occurred.
        MouseButtonType buttonType{translateSDLButtonType(event.button)};
        if (event.clicks == 1) {
            returnData = routeMouseDown(buttonType, cursorPosition, clickPath);
        }
        else {
            returnData
                = routeMouseDoubleClick(buttonType, cursorPosition, clickPath);
        }

        // If the event was handled, end the path at the widget that
        // handled it.
        WidgetPath truncatedPath{clickPath};
        if (returnData.eventWasHandled) {
            truncatedPath
                = WidgetPath(clickPath.begin(), (returnData.handlerWidget + 1));
        }

        // If the focus target wasn't changed during event handling, see if any
        // of the clicked widgets can take focus.
        Widget* currentFocusedWidget{getFocusedWidget()};
        if (previousFocusedWidget == currentFocusedWidget) {
            // Try to set focus to a widget in the path.
            // Note: The event must be handled for a widget to gain focus.
            if ((!returnData.eventWasHandled)
                || !setFocusIfFocusable(truncatedPath)) {
                // Event wasn't handled or nothing in truncatedPath took focus.
                // We didn't re-click the focused widget (if there is one), so 
                // we need to drop it.
                dropFocus(FocusLostType::Click);
            }
        }
        else {
            // Focus target was changed (set or dropped). In both cases, we
            // want to respect the change instead of looking for a new target.
        }

        // If we clicked a drag/droppable widget, track it.
        setDragIfDraggable(truncatedPath, cursorPosition);
    }
    else {
        // Empty click. If there's a focus target, drop it.
        dropFocus(FocusLostType::Click);
    }

    return returnData.eventWasHandled;
}

bool EventRouter::handleMouseButtonUp(SDL_MouseButtonEvent& event)
{
    // If we're dragging, route the Drop event.
    SDL_Point cursorPosition{event.x, event.y};
    if (dragUnderway) {
        WidgetPath hoverPath{getPathUnderCursor(cursorPosition)};
        routeDrop(hoverPath);
    }

    // Clear the drag path, in case we were detecting a drag.
    dragPath.clear();

    // If the mouse is captured, pass the MouseUp event to the captor widget.
    bool eventWasHandled{false};
    if (!(mouseCapturePath.empty())) {
        WidgetWeakRef& widgetWeakRef{mouseCapturePath.back()};
        if (widgetWeakRef.isValid()) {
            Widget& widget{widgetWeakRef.get()};
            MouseButtonType buttonType{translateSDLButtonType(event.button)};

            // Pass the MouseUp event to the widget.
            EventResult eventResult{widget.onMouseUp(
                buttonType, screenToWindowRelative(cursorPosition))};
            processEventResult(eventResult);

            eventWasHandled = eventResult.wasHandled;
        }
        else {
            // Mouse capture is no longer valid. Release it.
            setMouseCapture(nullptr);
        }
    }

    return eventWasHandled;
}

bool EventRouter::handleMouseWheel(SDL_MouseWheelEvent& event)
{
    // Normalize the scroll direction.
    int amountScrolled{event.y};
    if (event.direction == SDL_MOUSEWHEEL_FLIPPED) {
        amountScrolled *= -1;
    }

    // If the mouse captor has become invalid, release capture and refresh 
    // the mouse hover path.
    if (isMouseCaptorInvalid()) {
        setMouseCapture(nullptr);
    }

    // If the mouse is captured, pass the event to the captor widget.
    bool eventWasHandled{false};
    if (!(mouseCapturePath.empty())) {
        WidgetWeakRef& widgetWeakRef{mouseCapturePath.back()};
        if (widgetWeakRef.isValid()) {
            // Pass the MouseWheel event to the widget.
            Widget& widget{widgetWeakRef.get()};
            EventResult eventResult{widget.onMouseWheel(amountScrolled)};
            processEventResult(eventResult);

            eventWasHandled = eventResult.wasHandled;
        }
    }
    else {
        // The mouse isn't captured. If the cursor is hovering over an AUI
        // window, bubble the event through the hovered widgets.
        SDL_Point cursorPosition{};
        SDL_GetMouseState(&(cursorPosition.x), &(cursorPosition.y));
        WidgetPath hoverPath{getPathUnderCursor(cursorPosition)};
        if (!(hoverPath.empty())) {
            // Perform the bubbling pass (leaf -> root, MouseWheel).
            for (auto it = hoverPath.rbegin(); it != hoverPath.rend(); ++it) {
                // If the widget is gone, skip it.
                WidgetWeakRef& widgetWeakRef{*it};
                if (!(widgetWeakRef.isValid())) {
                    continue;
                }

                // Pass the MouseWheel event to the widget.
                Widget& widget{widgetWeakRef.get()};
                EventResult eventResult{widget.onMouseWheel(amountScrolled)};
                processEventResult(eventResult);

                // If the event was handled, break early.
                if (eventResult.wasHandled) {
                    eventWasHandled = true;
                    break;
                }
            }
        }
    }

    return eventWasHandled;
}

bool EventRouter::handleMouseMove(SDL_MouseMotionEvent& event)
{
    // If the mouse captor has become invalid, release capture (we'll rebuild 
    // the hover path below).
    if (isMouseCaptorInvalid()) {
        mouseCapturePath.clear();
    }

    // Build the event path based on whether the mouse is captured or not.
    SDL_Point cursorPosition{event.x, event.y};
    WidgetPath eventPath{};
    if (!(mouseCapturePath.empty())) {
        eventPath = mouseCapturePath;
    }
    else {
        eventPath = getPathUnderCursor(cursorPosition);
    }

    // If we've dragged a widget past the trigger distance, start a drag event.
    if (!dragUnderway && !(dragPath.empty())
        && (SDLHelpers::squaredDistance(dragOrigin, cursorPosition)
            > Core::getSquaredDragTriggerDistance())) {
        // Note: This will also send a MouseLeave event to previously-hovered
        //       widgets, since we're switching to DragEnter/DragLeave.
        routeDragStart();
    }

    // Route MouseEnter/MouseLeave (or DragEnter/DragLeave) events.
    routeMouseEnterAndLeave(eventPath);

    // Route the MouseMove (or DragMove) event.
    bool eventWasHandled{routeMouseMove(cursorPosition, eventPath)};

    // Save the new hovered widget path.
    lastHoveredWidgetPath = eventPath;

    return eventWasHandled;
}

bool EventRouter::handleKeyDown(SDL_KeyboardEvent& event)
{
    // If we have a valid focused widget, route the event down the focus path.
    bool eventWasHandled{false};
    if (!(focusPath.empty()) && focusPath.back().isValid()) {
        if (event.type == SDL_KEYDOWN) {
            eventWasHandled = routeFocusedKeyDown(event.keysym.sym);
        }
        else {
            eventWasHandled = routeKeyUp(event.keysym.sym);
        }
    }

    // If an escape key press wasn't handled, drop focus.
    if ((event.type == SDL_KEYDOWN) && !eventWasHandled
        && (event.keysym.sym == SDLK_ESCAPE)) {
        dropFocus(FocusLostType::Escape);
        eventWasHandled = true;
    }

    // If a KeyDown wasn't handled by our widgets, route it to the screen.
    if ((event.type == SDL_KEYDOWN) && !eventWasHandled) {
        eventWasHandled = screen.onKeyDown(event.keysym.sym);
    }

    return eventWasHandled;
}

bool EventRouter::handleTextInput(SDL_TextInputEvent& event)
{
    // If we don't have a focus path or the focused widget is gone, return
    // early.
    if (focusPath.empty() || !(focusPath.back().isValid())) {
        return false;
    }

    // Perform the bubbling pass (leaf -> root, TextInput).
    bool eventWasHandled{false};
    for (auto it = focusPath.rbegin(); it != focusPath.rend(); ++it) {
        // If the widget is gone, skip it.
        WidgetWeakRef& widgetWeakRef{*it};
        if (!(widgetWeakRef.isValid())) {
            continue;
        }

        // Pass the TextInput event to the widget.
        Widget& widget{widgetWeakRef.get()};
        EventResult eventResult{widget.onTextInput(event.text)};
        processEventResult(eventResult);

        // If the event was handled, break early.
        if (eventResult.wasHandled) {
            eventWasHandled = true;
            break;
        }
    }

    return eventWasHandled;
}

void EventRouter::setFocus(const Widget* widget)
{
    WidgetPath focusPath{getPathUnderWidget(widget)};
    if (!setFocusIfFocusable(focusPath)) {
        AUI_LOG_ERROR("Failed to set focus.");
    }
}

void EventRouter::dropFocus()
{
    dropFocus(FocusLostType::Requested);
}

Image* EventRouter::getDragDropImage()
{
    if (dragUnderway && !(dragPath.empty())) {
        WidgetWeakRef& widgetWeakRef{dragPath.back()};
        if (widgetWeakRef.isValid()) {
            Widget& widget{widgetWeakRef.get()};

            return widget.getDragDropImage();
        }
    }

    return nullptr;
}

MouseButtonType EventRouter::translateSDLButtonType(Uint8 sdlButtonType)
{
    switch (sdlButtonType) {
        case SDL_BUTTON_LEFT: {
            return MouseButtonType::Left;
        }
        case SDL_BUTTON_MIDDLE: {
            return MouseButtonType::Middle;
        }
        case SDL_BUTTON_RIGHT: {
            return MouseButtonType::Right;
        }
        case SDL_BUTTON_X1: {
            return MouseButtonType::X1;
        }
        case SDL_BUTTON_X2: {
            return MouseButtonType::X2;
        }
        default: {
            AUI_LOG_FATAL("Received invalid button type.");
            return MouseButtonType::Invalid;
        }
    }
}

SDL_Point EventRouter::screenToWindowRelative(const SDL_Point& cursorPosition)
{
    // If the cursor is hovering over a window, calculate the relative position.
    Window* hoveredWindow{screen.getWindowUnderPoint(cursorPosition)};
    if (hoveredWindow != nullptr) {
        SDL_Rect windowExtent{hoveredWindow->getScaledExtent()};
        SDL_Point windowRelativeCursor{cursorPosition};
        windowRelativeCursor.x -= windowExtent.x;
        windowRelativeCursor.y -= windowExtent.y;

        return windowRelativeCursor;
    }
    else {
        // Not hovering over a window, return the same position.
        return cursorPosition;
    }
}

WidgetPath EventRouter::getPathUnderCursor(const SDL_Point& cursorPosition)
{
    // Check if the cursor is hovering over an AUI window.
    Window* hoveredWindow{screen.getWindowUnderPoint(cursorPosition)};
    if (hoveredWindow != nullptr) {
        // The cursor is over one of our windows. Return the widget path that
        // the cursor is hovering over.
        return hoveredWindow->getPathUnderPoint(cursorPosition);
    }

    return WidgetPath{};
}

WidgetPath EventRouter::getPathUnderWidget(const Widget* widget)
{
    // Get the widget's parent window.
    Window* hoveredWindow{screen.getWidgetParentWindow(widget)};
    if (hoveredWindow != nullptr) {
        // Return the path under the widget.
        return hoveredWindow->getPathUnderWidget(widget);
    }

    return WidgetPath{};
}

EventRouter::RouterReturnData
    EventRouter::routeMouseDown(MouseButtonType buttonType,
                                const SDL_Point& cursorPosition,
                                WidgetPath& clickPath)
{
    // Perform the tunneling pass (root -> leaf, PreviewMouseDown).
    RouterReturnData returnData{false, false, clickPath.end()};
    for (auto it = clickPath.begin(); it != clickPath.end(); ++it) {
        // If the widget is gone, skip it.
        WidgetWeakRef& widgetWeakRef{*it};
        if (!(widgetWeakRef.isValid())) {
            continue;
        }

        // Pass the PreviewMouseDown event to the widget.
        Widget& widget{widgetWeakRef.get()};
        EventResult eventResult{widget.onPreviewMouseDown(
            buttonType, screenToWindowRelative(cursorPosition))};
        processEventResult(eventResult);

        // Track whether focus was explicitly set.
        if (eventResult.setFocus != nullptr) {
            returnData.focusWasSet = true;
        }

        // If the event was handled, break early.
        if (eventResult.wasHandled) {
            returnData.eventWasHandled = true;
            returnData.handlerWidget = it;
            break;
        }
    }

    // If a widget didn't handle the event during the preview pass, perform
    // the bubbling pass (leaf -> root, MouseDown).
    if (!(returnData.eventWasHandled)) {
        for (std::size_t i = clickPath.size(); i-- > 0;) {
            // If the widget is gone, skip it.
            WidgetWeakRef& widgetWeakRef{clickPath[i]};
            if (!(widgetWeakRef.isValid())) {
                continue;
            }

            // Pass the MouseDown event to the widget.
            Widget& widget{widgetWeakRef.get()};
            EventResult eventResult{widget.onMouseDown(
                buttonType, screenToWindowRelative(cursorPosition))};
            processEventResult(eventResult);

            // Track whether focus was explicitly set.
            if (eventResult.setFocus != nullptr) {
                returnData.focusWasSet = true;
            }

            // If the event was handled, break early.
            if (eventResult.wasHandled) {
                returnData.eventWasHandled = true;
                returnData.handlerWidget = (clickPath.begin() + i);
                break;
            }
        }
    }

    return returnData;
}

EventRouter::RouterReturnData
    EventRouter::routeMouseDoubleClick(MouseButtonType buttonType,
                                       const SDL_Point& cursorPosition,
                                       WidgetPath& clickPath)
{
    // Perform the bubbling pass (leaf -> root, MouseDoubleClick).
    RouterReturnData returnData{};
    for (std::size_t i = clickPath.size(); i-- > 0;) {
        // If the widget is gone, skip it.
        WidgetWeakRef& widgetWeakRef{clickPath[i]};
        if (!(widgetWeakRef.isValid())) {
            continue;
        }

        // Pass the MouseDoubleClick event to the widget.
        Widget& widget{widgetWeakRef.get()};
        EventResult eventResult{widget.onMouseDoubleClick(
            buttonType, screenToWindowRelative(cursorPosition))};
        processEventResult(eventResult);

        // Track whether focus was explicitly set.
        if (eventResult.setFocus != nullptr) {
            returnData.focusWasSet = true;
        }

        // If the event was handled, break early.
        if (eventResult.wasHandled) {
            returnData.eventWasHandled = true;
            returnData.handlerWidget = (clickPath.begin() + i);
            break;
        }
    }

    return returnData;
}

void EventRouter::routeMouseEnterAndLeave(WidgetPath& hoverPath)
{
    // Send MouseLeave events to any widgets that are no longer hovered.
    for (WidgetWeakRef& widgetWeakRef : lastHoveredWidgetPath) {
        // If the widget is gone, skip it.
        if (!(widgetWeakRef.isValid())) {
            continue;
        }

        // Check if the widget is in hoverPath.
        auto result{std::find_if(hoverPath.begin(), hoverPath.end(),
                                 [&widgetWeakRef](WidgetWeakRef& refToCompare) {
                                     return (&(widgetWeakRef.get())
                                             == &(refToCompare.get()));
                                 })};

        // If the widget is no longer hovered, pass a MouseLeave event to it.
        if (result == hoverPath.end()) {
            Widget& widget{widgetWeakRef.get()};
            if (dragUnderway) {
                widget.onDragLeave();
            }
            else {
                widget.onMouseLeave();
            }
        }
    }

    // Send MouseEnter events to all newly hovered widgets.
    for (WidgetWeakRef& widgetWeakRef : hoverPath) {
        // If the widget is gone, skip it.
        if (!(widgetWeakRef.isValid())) {
            continue;
        }

        // Check if the widget is in lastHoveredWidgetPath.
        auto result{std::find_if(
            lastHoveredWidgetPath.begin(), lastHoveredWidgetPath.end(),
            [&widgetWeakRef](WidgetWeakRef& refToCompare) {
                return (&(widgetWeakRef.get()) == &(refToCompare.get()));
            })};

        // If the widget is new, pass a MouseEnter event to it.
        if (result == lastHoveredWidgetPath.end()) {
            Widget& widget{widgetWeakRef.get()};
            if (dragUnderway) {
                widget.onDragEnter();
            }
            else {
                widget.onMouseEnter();
            }
        }
    }
}

bool EventRouter::routeMouseMove(const SDL_Point& cursorPosition,
                                 WidgetPath& hoverPath)
{
    // Perform the bubbling pass (leaf -> root, MouseMove).
    bool eventWasHandled{false};
    for (auto it = hoverPath.rbegin(); it != hoverPath.rend(); ++it) {
        // If the widget is gone, skip it.
        WidgetWeakRef& widgetWeakRef{*it};
        if (!(widgetWeakRef.isValid())) {
            continue;
        }

        // Pass the MouseMove (or DragMove) event to the widget.
        Widget& widget{widgetWeakRef.get()};
        EventResult eventResult{};
        if (dragUnderway) {
            widget.onDragMove(screenToWindowRelative(cursorPosition));
        }
        else {
            widget.onMouseMove(screenToWindowRelative(cursorPosition));
        }
        processEventResult(eventResult);

        // If the event was handled, break early.
        if (eventResult.wasHandled) {
            eventWasHandled = true;
            break;
        }
    }

    return eventWasHandled;
}

bool EventRouter::setFocusIfFocusable(WidgetPath& eventPath)
{
    // Reverse iterate eventPath, looking for a focusable widget.
    for (std::size_t i = eventPath.size(); i-- > 0;) {
        WidgetWeakRef& widgetWeakRef{eventPath[i]};
        if (!(widgetWeakRef.isValid())) {
            continue;
        }

        // If this widget is focusable, set the path from eventPath's root
        // to this widget as the focus path.
        Widget& widget{widgetWeakRef.get()};
        if (widget.getIsFocusable()) {
            auto endIt{eventPath.begin() + i + 1};
            WidgetPath newFocusPath(eventPath.begin(), endIt);
            setFocus(newFocusPath);
            return true;
        }
    }

    return false;
}

void EventRouter::setFocus(WidgetPath& newFocusPath)
{
    // If the given path is valid for focusing.
    if (!(newFocusPath.empty()) && newFocusPath.back().isValid()) {
        // If there's an existing focus, check if it's the same widget.
        if (!(focusPath.empty()) && focusPath.back().isValid()) {
            Widget& oldFocusedWidget{focusPath.back().get()};
            if (&oldFocusedWidget != &(newFocusPath.back().get())) {
                // Not the same, drop focus to make room for the new widget.
                oldFocusedWidget.onFocusLost(FocusLostType::NewFocus);
            }
            else {
                // The same widget is already focused, do nothing.
                return;
            }
        }
        else if (focusPath.empty()) {
            // If there was no focused widget, text input events will have been
            // disabled. Enable them.
            SDL_StartTextInput();
        }

        // Pass a FocusGained event to the new widget.
        Widget& newFocusedWidget{newFocusPath.back().get()};
        EventResult eventResult{newFocusedWidget.onFocusGained()};

        AUI_ASSERT((eventResult.setFocus != &newFocusedWidget),
                   "Tried to recursively set focus.");

        processEventResult(eventResult);

        // Save the new path as the current focus path.
        focusPath = newFocusPath;
    }
}

void EventRouter::dropFocus(FocusLostType focusLostType)
{
    // If a widget is focused.
    if (!(focusPath.empty())) {
        // If the focused widget is still alive, pass a FocusLost event to it.
        if (focusPath.back().isValid()) {
            Widget& oldFocusedWidget{focusPath.back().get()};
            oldFocusedWidget.onFocusLost(focusLostType);
        }

        focusPath.clear();

        // Stop generating text input events, since there's no focused widget
        // to handle them.
        SDL_StopTextInput();
    }
}

bool EventRouter::routeFocusedKeyDown(SDL_Keycode keyCode)
{
    // Perform the tunneling pass (root -> leaf, PreviewKeyDown).
    bool eventWasHandled{false};
    for (auto it = focusPath.begin(); it != focusPath.end(); ++it) {
        // If the widget is gone, skip it.
        WidgetWeakRef& widgetWeakRef{*it};
        if (!(widgetWeakRef.isValid())) {
            continue;
        }

        // Pass the PreviewKeyDown event to the widget.
        Widget& widget{widgetWeakRef.get()};
        EventResult eventResult{widget.onPreviewKeyDown(keyCode)};
        processEventResult(eventResult);

        // If the event was handled, break early.
        if (eventResult.wasHandled) {
            eventWasHandled = true;
            break;
        }
    }

    // If a widget didn't handle the event during the preview pass, perform
    // the bubbling pass (leaf -> root, KeyDown).
    if (!eventWasHandled) {
        for (auto it = focusPath.rbegin(); it != focusPath.rend(); ++it) {
            // If the widget is gone, skip it.
            WidgetWeakRef& widgetWeakRef{*it};
            if (!(widgetWeakRef.isValid())) {
                continue;
            }

            // Pass the KeyDown event to the widget.
            Widget& widget{widgetWeakRef.get()};
            EventResult eventResult{widget.onKeyDown(keyCode)};
            processEventResult(eventResult);

            // If the event was handled, break early.
            if (eventResult.wasHandled) {
                eventWasHandled = true;
                break;
            }
        }
    }

    return eventWasHandled;
}

bool EventRouter::routeKeyUp(SDL_Keycode keyCode)
{
    // Perform the bubbling pass (leaf -> root, KeyUp).
    bool eventWasHandled{false};
    for (auto it = focusPath.rbegin(); it != focusPath.rend(); ++it) {
        // If the widget is gone, skip it.
        WidgetWeakRef& widgetWeakRef{*it};
        if (!(widgetWeakRef.isValid())) {
            continue;
        }

        // Pass the KeyUp event to the widget.
        Widget& widget{widgetWeakRef.get()};
        EventResult eventResult{widget.onKeyUp(keyCode)};
        processEventResult(eventResult);

        // If the event was handled, break early.
        if (eventResult.wasHandled) {
            eventWasHandled = true;
            break;
        }
    }

    return eventWasHandled;
}

void EventRouter::setDragIfDraggable(WidgetPath& eventPath,
                                     SDL_Point& cursorPosition)
{
    // Reverse iterate eventPath, looking for a drag/droppable widget.
    for (std::size_t i = eventPath.size(); i-- > 0;) {
        WidgetWeakRef& widgetWeakRef{eventPath[i]};
        if (!(widgetWeakRef.isValid())) {
            continue;
        }

        // If this widget is drag/droppable, set it as the new dragPath.
        Widget& widget{widgetWeakRef.get()};
        if (widget.getIsDragDroppable()) {
            dragPath.clear();
            dragPath.push_back(widget);
            dragOrigin = cursorPosition;
            return;
        }
    }
}

void EventRouter::routeDragStart()
{
    // We're no longer considered to be hovering the previously hovered
    // widgets. Send them a MouseLeave and clear the path.
    for (WidgetWeakRef& widgetWeakRef : lastHoveredWidgetPath) {
        if (widgetWeakRef.isValid()) {
            widgetWeakRef.get().onMouseLeave();
        }
    }
    lastHoveredWidgetPath.clear();

    // Since the drag path stops at the first drag/droppable widget, we only
    // need to send the DragStart to the root.
    if (!(dragPath.empty())) {
        WidgetWeakRef& widgetWeakRef{dragPath.back()};
        if (widgetWeakRef.isValid()) {
            Widget& widget{widgetWeakRef.get()};
            if (widget.getIsDragDroppable()) {
                widget.onDragStart();
            }
        }
    }

    dragUnderway = true;
}

void EventRouter::routeDrop(WidgetPath& hoverPath)
{
    AUI_ASSERT(!(dragPath.empty()),
               "Tried to route drop with empty drag path.");

    // If the dragged widget hasn't been invalidated, route the Drop event
    // through the hovered widgets.
    if (dragPath.back().isValid()) {
        Widget& draggedWidget{dragPath.back().get()};
        if (const DragDropData
            * dragDropData{draggedWidget.getDragDropData()}) {
            // Perform the bubbling pass (leaf -> root, Drop).
            for (std::size_t i = hoverPath.size(); i-- > 0;) {
                // If the widget is gone, skip it.
                WidgetWeakRef& widgetWeakRef{hoverPath[i]};
                if (!(widgetWeakRef.isValid())) {
                    continue;
                }

                // Pass the Drop event to the widget.
                Widget& widget{widgetWeakRef.get()};
                EventResult eventResult{widget.onDrop(*dragDropData)};
                processEventResult(eventResult);

                // If the event was handled, break early.
                if (eventResult.wasHandled) {
                    break;
                }
            }
        }
    }

    // Since the drag path stops at the first drag/droppable widget, we only
    // need to send the DragEnd to the root.
    if (!(dragPath.empty())) {
        WidgetWeakRef& widgetWeakRef{dragPath.back()};
        if (widgetWeakRef.isValid()) {
            Widget& widget{widgetWeakRef.get()};
            if (widget.getIsDragDroppable()) {
                widget.onDragEnd();
            }
        }
    }

    // We're no longer considered to be dragging over the previously hovered
    // widgets. Send them a DragLeave and clear the path.
    for (WidgetWeakRef& widgetWeakRef : lastHoveredWidgetPath) {
        if (widgetWeakRef.isValid()) {
            widgetWeakRef.get().onDragLeave();
        }
    }
    lastHoveredWidgetPath.clear();

    // Send MouseEnter to any hovered widgets.
    // Note: Since we cleared lastHoveredWidgetPath, this won't send any
    //       MouseLeave, and all widgets in hoverPath will get a MouseEnter.
    dragUnderway = false;
    routeMouseEnterAndLeave(hoverPath);

    // Refresh the hovered widget path.
    lastHoveredWidgetPath = hoverPath;
}

void EventRouter::processEventResult(const EventResult& eventResult)
{
    // If mouse capture was requested.
    if (eventResult.setMouseCapture != nullptr) {
        // Release any previous mouse capture and set the new captor widget.
        setMouseCapture(eventResult.setMouseCapture);
    }

    // If mouse capture release was requested, release it.
    if (eventResult.releaseMouseCapture) {
        setMouseCapture(nullptr);
    }

    // If focus was requested.
    if (eventResult.setFocus != nullptr) {
        AUI_ASSERT(eventResult.setFocus->getIsFocusable(),
                   "Tried to set focus to a widget that isn't focusable.");

        // Set focus to the new path.
        WidgetPath newFocusPath{getPathUnderWidget(eventResult.setFocus)};
        setFocus(newFocusPath);
    }
    // Else if dropping focus was requested.
    else if (eventResult.dropFocus) {
        dropFocus(FocusLostType::Requested);
    }
}

bool EventRouter::isMouseCaptorInvalid()
{
    // If there's a mouse captor and it has become invalid, return true.
    // Note: We don't count hidden widgets as invalid because they should still 
    //       receive events and have an opportunity to release capture. 
    if (!(mouseCapturePath.empty())) {
        WidgetWeakRef& widgetWeakRef{mouseCapturePath.back()};
        if (!(widgetWeakRef.isValid())) {
            return true;
        }
    }

    return false;
}

void EventRouter::setMouseCapture(AUI::Widget* newCaptorWidget)
{
    // Clear any existing mouse capture.
    // Note: We expect any captor to also be in lastHoveredWidgetPath, so we 
    //       don't need to pass it a MouseLeave (it'll happen below).
    mouseCapturePath.clear();

    // If we were given a new captor widget, set it.
    if (newCaptorWidget) {
        mouseCapturePath.push_back(*newCaptorWidget);
    }

    // Build a current hovered widget path based on whether the mouse is 
    // captured or not.
    SDL_Point cursorPosition{};
    SDL_GetMouseState(&(cursorPosition.x), &(cursorPosition.y));
    WidgetPath currentHoveredWidgetPath{};
    if (!(mouseCapturePath.empty())) {
        currentHoveredWidgetPath = mouseCapturePath;
    }
    else {
        currentHoveredWidgetPath = getPathUnderCursor(cursorPosition);
    }

    // Route MouseEnter/MouseLeave (or DragEnter/DragLeave) events.
    routeMouseEnterAndLeave(currentHoveredWidgetPath);

    // Save the new hovered widget path.
    lastHoveredWidgetPath = currentHoveredWidgetPath;
}

Widget* EventRouter::getFocusedWidget()
{
    if (!focusPath.empty()) {
        WidgetWeakRef& focusedWidgetRef{focusPath.back()};
        return &(focusedWidgetRef.get());
    }
    else {
        return nullptr;
    }
}

} // End namespace AUI
