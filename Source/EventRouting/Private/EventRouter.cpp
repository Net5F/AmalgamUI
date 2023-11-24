#include "AUI/EventRouter.h"
#include "AUI/Screen.h"
#include "AUI/Window.h"
#include "AUI/Widget.h"
#include "AUI/Internal/Log.h"
#include "AUI/Internal/AUIAssert.h"
#include <algorithm>

namespace AUI
{

EventRouter::EventRouter(Screen& inScreen)
: screen{inScreen}
{
}

bool EventRouter::handleMouseButtonDown(SDL_MouseButtonEvent& event)
{
    // Check if the cursor is over an AUI window, or if it missed.
    HandlerReturnData returnData{};
    SDL_Point cursorPosition{event.x, event.y};
    WidgetPath clickPath{getPathUnderCursor(cursorPosition)};
    if (!(clickPath.empty())) {
        Widget* previousFocusedWidget{getFocusedWidget()};

        // Perform a MouseDown or MouseDoubleClick depending on how many
        // clicks occurred.
        MouseButtonType buttonType{translateSDLButtonType(event.button)};
        if (event.clicks == 1) {
            returnData
                = handleMouseDown(buttonType, cursorPosition, clickPath);
        }
        else {
            returnData
                = handleMouseDoubleClick(buttonType, cursorPosition, clickPath);
        }

        // If the focus target wasn't changed during event handling, see if any
        // of the clicked widgets can take focus.
        Widget* currentFocusedWidget{getFocusedWidget()};
        if (previousFocusedWidget == currentFocusedWidget) {
            // If the event was handled, end the path at the widget that 
            // handled it.
            WidgetPath truncatedPath{clickPath};
            if (returnData.eventWasHandled) {
                truncatedPath = WidgetPath(clickPath.begin(),
                                           (returnData.handlerWidget + 1));
            }

            // Try to set focus to a widget in the path.
            if (!setFocusIfFocusable(truncatedPath)) {
                // Nothing in truncatedPath took focus. We didn't re-click the
                // focused widget (if there is one), so we need to drop it.
                handleDropFocus(FocusLostType::Click);
            }
        }
        else {
            // Focus target was changed (set or dropped). In both cases, we 
            // want to respect the change instead of looking for a new target.
        }
    }
    else {
        // Empty click. If there's a focus target, drop it.
        handleDropFocus(FocusLostType::Click);
    }

    return returnData.eventWasHandled;
}

bool EventRouter::handleMouseButtonUp(SDL_MouseButtonEvent& event)
{
    // If the mouse is captured, pass the event to the captor widget.
    bool eventWasHandled{false};
    if (!(mouseCapturePath.empty())) {
        WidgetWeakRef& widgetWeakRef{mouseCapturePath.back()};
        if (widgetWeakRef.isValid()) {
            // Pass the MouseUp event to the widget.
            Widget& widget{widgetWeakRef.get()};
            MouseButtonType buttonType{translateSDLButtonType(event.button)};
            SDL_Point cursorPosition{event.x, event.y};

            EventResult eventResult{widget.onMouseUp(
                buttonType, screenToWindowRelative(cursorPosition))};
            processEventResult(eventResult);

            eventWasHandled = eventResult.wasHandled;
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
    // If the mouse is captured, pass the event to the captor widget.
    SDL_Point cursorPosition{event.x, event.y};
    bool eventWasHandled{false};
    if (!(mouseCapturePath.empty())) {
        WidgetWeakRef& widgetWeakRef{mouseCapturePath.back()};
        if (widgetWeakRef.isValid()) {
            // Pass the MouseMove event to the widget.
            Widget& widget{widgetWeakRef.get()};
            EventResult eventResult{
                widget.onMouseMove(screenToWindowRelative(cursorPosition))};
            processEventResult(eventResult);

            eventWasHandled = eventResult.wasHandled;
        }
    }
    else {
        // The mouse isn't captured. If the cursor is hovering over an AUI 
        // window, pass the event through the hovered widgets.
        WidgetPath hoverPath{getPathUnderCursor(cursorPosition)};
        if (!(hoverPath.empty())) {
            // The cursor is over one of our windows. Route the MouseEnter
            // and MouseLeave events.
            handleMouseEnterAndLeave(hoverPath);

            // Route the MouseMove event.
            eventWasHandled = handleUncapturedMouseMove(cursorPosition, hoverPath);

            // Save the new hovered widget path.
            lastHoveredWidgetPath = hoverPath;
        }
    }

    return eventWasHandled;
}

bool EventRouter::handleKeyDown(SDL_KeyboardEvent& event)
{
    // If we have a valid focused widget, route the event down the focus path.
    bool eventWasHandled{false};
    if (!(focusPath.empty()) && focusPath.back().isValid()) {
        if (event.type == SDL_KEYDOWN) {
            eventWasHandled = handleFocusedKeyDown(event.keysym.sym);
        }
        else {
            eventWasHandled = handleKeyUp(event.keysym.sym);
        }
    }

    // If an escape key press wasn't handled, drop focus.
    if ((event.type == SDL_KEYDOWN) && !eventWasHandled
        && (event.keysym.sym == SDLK_ESCAPE)) {
        handleDropFocus(FocusLostType::Escape);
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
    handleDropFocus(FocusLostType::Requested);
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

EventRouter::HandlerReturnData
    EventRouter::handleMouseDown(MouseButtonType buttonType,
                                 const SDL_Point& cursorPosition,
                                 WidgetPath& clickPath)
{
    // Perform the tunneling pass (root -> leaf, PreviewMouseDown).
    HandlerReturnData returnData{false, false, clickPath.end()};
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

EventRouter::HandlerReturnData
    EventRouter::handleMouseDoubleClick(MouseButtonType buttonType,
                                        const SDL_Point& cursorPosition,
                                        WidgetPath& clickPath)
{
    // Perform the bubbling pass (leaf -> root, MouseDoubleClick).
    HandlerReturnData returnData{};
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

void EventRouter::handleMouseEnterAndLeave(WidgetPath& hoverPath)
{
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
            widget.onMouseEnter();
        }
    }

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
            widget.onMouseLeave();
        }
    }
}

bool
    EventRouter::handleUncapturedMouseMove(const SDL_Point& cursorPosition,
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

        // Pass the MouseMove event to the widget.
        Widget& widget{widgetWeakRef.get()};
        EventResult eventResult{
            widget.onMouseMove(screenToWindowRelative(cursorPosition))};
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
            handleSetFocus(newFocusPath);
            return true;
        }
    }

    return false;
}

void EventRouter::handleSetFocus(WidgetPath& newFocusPath)
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

void EventRouter::handleDropFocus(FocusLostType focusLostType)
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

bool EventRouter::handleFocusedKeyDown(SDL_Keycode keyCode)
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

bool EventRouter::handleKeyUp(SDL_Keycode keyCode)
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

void EventRouter::processEventResult(const EventResult& eventResult)
{
    // If mouse capture was requested.
    if (eventResult.setMouseCapture != nullptr) {
        // Set the new captor widget.
        mouseCapturePath.clear();
        mouseCapturePath.push_back(*(eventResult.setMouseCapture));

        // We're no longer considered to be hovering the previously hovered
        // widgets. Send them a MouseLeave and clear the path.
        for (WidgetWeakRef& widgetWeakRef : lastHoveredWidgetPath) {
            if (!(widgetWeakRef.isValid())) {
                continue;
            }

            // Pass a MouseLeave event to the widget.
            Widget& widget{widgetWeakRef.get()};
            widget.onMouseLeave();
        }

        lastHoveredWidgetPath.clear();
    }

    // If mouse capture release was requested.
    if (eventResult.releaseMouseCapture) {
        mouseCapturePath.clear();
    }

    // If focus was requested.
    if (eventResult.setFocus != nullptr) {
        AUI_ASSERT(eventResult.setFocus->getIsFocusable(),
                   "Tried to set focus to a widget that isn't focusable.");

        // Set focus to the new path.
        WidgetPath newFocusPath{getPathUnderWidget(eventResult.setFocus)};
        handleSetFocus(newFocusPath);
    }
    // Else if dropping focus was requested.
    else if (eventResult.dropFocus) {
        handleDropFocus(FocusLostType::Requested);
    }
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
