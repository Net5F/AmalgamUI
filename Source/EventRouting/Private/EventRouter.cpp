#include "AUI/EventRouter.h"
#include "AUI/Screen.h"
#include "AUI/Window.h"
#include "AUI/Widget.h"
#include "AUI/Internal/Ignore.h"
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
    HandlerReturn handlerReturn{};
    SDL_Point cursorPosition{event.x, event.y};
    WidgetPath clickPath{getPathUnderCursor(cursorPosition)};
    if (!(clickPath.empty())) {
        // Perform a MouseDown or MouseDoubleClick depending on how many
        // clicks occurred.
        MouseButtonType buttonType{translateSDLButtonType(event.button)};
        if (event.clicks == 1) {
            handlerReturn = handleMouseDown(buttonType, cursorPosition, clickPath);
        }
        else {
            handlerReturn = handleMouseDoubleClick(buttonType, cursorPosition, clickPath);
        }

        processEventResult(handlerReturn.eventResult);

        // If the event was handled and it didn't explicitly set keyboard
        // focus, see if any of the clicked widgets can take focus.
        if (handlerReturn.eventResult.wasHandled && (handlerReturn.eventResult.setFocus == nullptr)) {
            // Build a path that ends at the widget that handled the event.
            WidgetPath truncatedPath(clickPath.begin(), (handlerReturn.handlerWidget + 1));

            setFocusIfFocusable(truncatedPath);
        }
    }

    // If the click wasn't handled and a widget has focus, drop it.
    if (!(handlerReturn.eventResult.wasHandled)) {
        handleDropFocus(FocusLostType::Click);
    }

    return handlerReturn.eventResult.wasHandled;
}

bool EventRouter::handleMouseButtonUp(SDL_MouseButtonEvent& event)
{
    // If the mouse is captured, pass the event to the captor widget.
    EventResult eventResult{};
    if (!(mouseCapturePath.empty())) {
        WidgetWeakRef& widgetWeakRef{mouseCapturePath.back()};
        if (widgetWeakRef.isValid()) {
            // Pass the MouseUp event to the widget.
            Widget& widget{widgetWeakRef.get()};
            MouseButtonType buttonType{translateSDLButtonType(event.button)};
            SDL_Point cursorPosition{event.x, event.y};

            eventResult = widget.onMouseUp(buttonType, cursorPosition);
        }
    }

    processEventResult(eventResult);

    return eventResult.wasHandled;
}

bool EventRouter::handleMouseWheel(SDL_MouseWheelEvent& event)
{
    // Normalize the scroll direction.
    int amountScrolled{event.y};
    if (event.direction == SDL_MOUSEWHEEL_FLIPPED) {
        amountScrolled *= -1;
    }

    // If the mouse is captured, pass the event to the captor widget.
    EventResult eventResult{};
    if (!(mouseCapturePath.empty())) {
        WidgetWeakRef& widgetWeakRef{mouseCapturePath.back()};
        if (widgetWeakRef.isValid()) {
            // Pass the MouseWheel event to the widget.
            Widget& widget{widgetWeakRef.get()};
            eventResult = widget.onMouseWheel(amountScrolled);
        }
    }
    else {
        // The mouse isn't captured. Check if the cursor is hovering over an
        // AUI window.
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
                eventResult = widget.onMouseWheel(amountScrolled);

                // If the event was handled, break early.
                if (eventResult.wasHandled) {
                    break;
                }
            }
        }
    }

    processEventResult(eventResult);

    return eventResult.wasHandled;
}

bool EventRouter::handleMouseMove(SDL_MouseMotionEvent& event)
{
    // If the mouse is captured, pass the event to the captor widget.
    SDL_Point cursorPosition{event.x, event.y};
    EventResult eventResult{};
    if (!(mouseCapturePath.empty())) {
        WidgetWeakRef& widgetWeakRef{mouseCapturePath.back()};
        if (widgetWeakRef.isValid()) {
            // Pass the MouseMove event to the widget.
            Widget& widget{widgetWeakRef.get()};
            eventResult = widget.onMouseMove(cursorPosition);
        }
    }
    else {
        // The mouse isn't captured. Check if the cursor is hovering over an
        // AUI window.
        WidgetPath hoverPath{getPathUnderCursor(cursorPosition)};
        if (!(hoverPath.empty())) {
            // The cursor is over one of our windows. Route the MouseEnter
            // and MouseLeave events.
            handleMouseEnterAndLeave(hoverPath);

            // Route the MouseMove event.
            eventResult = handleUncapturedMouseMove(cursorPosition, hoverPath);

            // Save the new hovered widget path.
            lastHoveredWidgetPath = hoverPath;
        }
    }

    processEventResult(eventResult);

    return eventResult.wasHandled;
}

bool EventRouter::handleKeyDown(SDL_KeyboardEvent& event)
{
    // If we don't have a focus path or the focused widget is gone, return
    // early.
    if (focusPath.empty() || !(focusPath.back().isValid())) {
        return false;
    }

    bool wasHandled{false};
    if (event.type == SDL_KEYDOWN) {
        wasHandled = handleKeyDownInternal(event.keysym.sym);
    }
    else {
        wasHandled = handleKeyUp(event.keysym.sym);
    }

    return wasHandled;
}

bool EventRouter::handleTextInput(SDL_TextInputEvent& event)
{
    // If we don't have a focus path or the focused widget is gone, return
    // early.
    if (focusPath.empty() || !(focusPath.back().isValid())) {
        return false;
    }

    // Perform the bubbling pass (leaf -> root, TextInput).
    EventResult eventResult{};
    for (auto it = (focusPath.end() - 1); it != focusPath.begin(); --it) {
        // If the widget is gone, skip it.
        WidgetWeakRef& widgetWeakRef{*it};
        if (!(widgetWeakRef.isValid())) {
            continue;
        }

        // Pass the TextInput event to the widget.
        Widget& widget{widgetWeakRef.get()};
        eventResult = widget.onTextInput(event.text);

        // If the event was handled, break early.
        if (eventResult.wasHandled) {
            break;
        }
    }

    return eventResult.wasHandled;
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
    // Calc the center of the given widget.
    SDL_Rect widgetExtent{widget->getRenderExtent()};
    SDL_Point widgetCenter{};
    widgetCenter.x = widgetExtent.x + (widgetExtent.w / 2);
    widgetCenter.y = widgetExtent.y + (widgetExtent.h / 2);

    // Get the widget's parent window.
    Window* hoveredWindow{screen.getWindowUnderPoint(widgetCenter)};
    AUI_ASSERT((hoveredWindow != nullptr), "Widget was somehow not within a Window.");

    // Return the path under the widget.
    return hoveredWindow->getPathUnderPoint(widgetCenter);
}

EventRouter::HandlerReturn EventRouter::handleMouseDown(MouseButtonType buttonType,
                                  const SDL_Point& cursorPosition, WidgetPath& clickPath)
{
    // Perform the tunneling pass (root -> leaf, PreviewMouseDown).
    EventResult eventResult{};
    WidgetPath::iterator handlerWidget{nullptr};
    for (auto it = clickPath.begin(); it != clickPath.end(); ++it) {
        // If the widget is gone, skip it.
        WidgetWeakRef& widgetWeakRef{*it};
        if (!(widgetWeakRef.isValid())) {
            continue;
        }

        // Pass the PreviewMouseDown event to the widget.
        Widget& widget{widgetWeakRef.get()};
        eventResult = widget.onPreviewMouseDown(buttonType, cursorPosition);

        // If the event was handled, break early.
        if (eventResult.wasHandled) {
            handlerWidget = it;
            break;
        }
    }

    // If a widget didn't handle the event during the preview pass, perform
    // the bubbling pass (leaf -> root, MouseDown).
    if (!(eventResult.wasHandled)) {
        for (auto it = (clickPath.end() - 1); it != clickPath.begin(); --it) {
            // If the widget is gone, skip it.
            WidgetWeakRef& widgetWeakRef{*it};
            if (!(widgetWeakRef.isValid())) {
                continue;
            }

            // Pass the MouseDown event to the widget.
            Widget& widget{widgetWeakRef.get()};
            eventResult = widget.onMouseDown(buttonType, cursorPosition);

            // If the event was handled, break early.
            if (eventResult.wasHandled) {
                handlerWidget = it;
                break;
            }
        }
    }

    return {eventResult, handlerWidget};
}

EventRouter::HandlerReturn EventRouter::handleMouseDoubleClick(MouseButtonType buttonType,
                                  const SDL_Point& cursorPosition, WidgetPath& clickPath)
{
    // Perform the bubbling pass (leaf -> root, MouseDoubleClick).
    EventResult eventResult{};
    WidgetPath::iterator handlerWidget;
    for (auto it = (clickPath.end() - 1); it != clickPath.begin(); --it) {
        // If the widget is gone, skip it.
        WidgetWeakRef& widgetWeakRef{*it};
        if (!(widgetWeakRef.isValid())) {
            continue;
        }

        // Pass the MouseDoubleClick event to the widget.
        Widget& widget{widgetWeakRef.get()};
        eventResult = widget.onMouseDoubleClick(buttonType, cursorPosition);

        // If the event was handled, break early.
        if (eventResult.wasHandled) {
            handlerWidget = it;
            break;
        }
    }

    return {eventResult, handlerWidget};
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
        auto result{std::find_if(lastHoveredWidgetPath.begin(),
            lastHoveredWidgetPath.end(), [&widgetWeakRef](WidgetWeakRef& refToCompare){
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
        auto result{std::find_if(hoverPath.begin(),
            hoverPath.end(), [&widgetWeakRef](WidgetWeakRef& refToCompare){
                return (&(widgetWeakRef.get()) == &(refToCompare.get()));
            })};

        // If the widget is no longer hovered, pass a MouseLeave event to it.
        if (result == hoverPath.end()) {
            Widget& widget{widgetWeakRef.get()};
            widget.onMouseLeave();
        }
    }
}

EventResult EventRouter::handleUncapturedMouseMove(const SDL_Point& cursorPosition, WidgetPath& hoverPath)
{
    // Perform the bubbling pass (leaf -> root, MouseMove).
    EventResult eventResult{};
    for (auto it = hoverPath.rbegin(); it != hoverPath.rend(); ++it) {
        // If the widget is gone, skip it.
        WidgetWeakRef& widgetWeakRef{*it};
        if (!(widgetWeakRef.isValid())) {
            continue;
        }

        // Pass the MouseMove event to the widget.
        Widget& widget{widgetWeakRef.get()};
        eventResult = widget.onMouseMove(cursorPosition);

        // If the event was handled, break early.
        if (eventResult.wasHandled) {
            break;
        }
    }

    processEventResult(eventResult);

    return eventResult;
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
        AUI_ASSERT(eventResult.setFocus->getIsFocusable(), "Tried to set focus to a "
            "widget that isn't focusable.");

        // Set focus to the new path.
        WidgetPath newFocusPath{getPathUnderWidget(eventResult.setFocus)};
        handleSetFocus(newFocusPath);
    }
    // Else if dropping focus was requested.
    else if (eventResult.dropFocus) {
        handleDropFocus(FocusLostType::Requested);
    }
}

void EventRouter::setFocusIfFocusable(WidgetPath& eventPath)
{
    // Reverse iterate eventPath, looking for a focusable widget.
    for (auto it = (eventPath.end() - 1); it != eventPath.begin(); --it) {
        WidgetWeakRef& widgetWeakRef{*it};
        if (!(widgetWeakRef.isValid())) {
            continue;
        }

        // If this widget is focusable, set the path from eventPath's root
        // to this widget as the focus path.
        Widget& widget{widgetWeakRef.get()};
        if (widget.getIsFocusable()) {
            WidgetPath newFocusPath(eventPath.begin(), (it + 1));
            handleSetFocus(newFocusPath);
            return;
        }
    }
}

void EventRouter::handleSetFocus(WidgetPath& newFocusPath)
{
    // If the given path is valid for focusing.
    if (!(newFocusPath.empty()) && newFocusPath.back().isValid()) {
        // If there's an existing focused widget, pass a FocusLost event to it.
        if (!(focusPath.empty()) && focusPath.back().isValid()) {
            Widget& oldFocusedWidget{focusPath.back().get()};

            oldFocusedWidget.onFocusLost(FocusLostType::NewFocus);
        }
        else if (focusPath.empty()) {
            // If there was no focused widget, text input events will have been
            // disabled. Enable them.
            SDL_StartTextInput();
        }

        // Pass a FocusGained event to the new widget.
        Widget& newFocusedWidget{newFocusPath.back().get()};
        EventResult eventResult{newFocusedWidget.onFocusGained()};

        AUI_ASSERT((eventResult.setFocus != &newFocusedWidget), "Tried to "
            "recursively set focus.");

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

bool EventRouter::handleKeyDownInternal(SDL_Keycode keyCode)
{
    // Perform the tunneling pass (root -> leaf, PreviewKeyDown).
    EventResult eventResult{};
    for (auto it = focusPath.begin(); it != focusPath.end(); ++it) {
        // If the widget is gone, skip it.
        WidgetWeakRef& widgetWeakRef{*it};
        if (!(widgetWeakRef.isValid())) {
            continue;
        }

        // Pass the PreviewKeyDown event to the widget.
        Widget& widget{widgetWeakRef.get()};
        eventResult = widget.onPreviewKeyDown(keyCode);

        // If the event was handled, break early.
        if (eventResult.wasHandled) {
            break;
        }
    }

    // If a widget didn't handle the event during the preview pass, perform
    // the bubbling pass (leaf -> root, KeyDown).
    if (!(eventResult.wasHandled)) {
        for (auto it = (focusPath.end() - 1); it != focusPath.begin(); --it) {
            // If the widget is gone, skip it.
            WidgetWeakRef& widgetWeakRef{*it};
            if (!(widgetWeakRef.isValid())) {
                continue;
            }

            // Pass the KeyDown event to the widget.
            Widget& widget{widgetWeakRef.get()};
            eventResult = widget.onKeyDown(keyCode);

            // If the event was handled, break early.
            if (eventResult.wasHandled) {
                break;
            }
        }
    }

    // If an escape key press wasn't handled, drop focus.
    if (!(eventResult.wasHandled) && (keyCode == SDLK_ESCAPE)) {
        handleDropFocus(FocusLostType::Escape);
        eventResult.wasHandled = true;
    }

    processEventResult(eventResult);

    return eventResult.wasHandled;
}

bool EventRouter::handleKeyUp(SDL_Keycode keyCode)
{
    // Perform the bubbling pass (leaf -> root, KeyUp).
    EventResult eventResult{};
    for (auto it = (focusPath.end() - 1); it != focusPath.begin(); --it) {
        // If the widget is gone, skip it.
        WidgetWeakRef& widgetWeakRef{*it};
        if (!(widgetWeakRef.isValid())) {
            continue;
        }

        // Pass the KeyUp event to the widget.
        Widget& widget{widgetWeakRef.get()};
        eventResult = widget.onKeyUp(keyCode);

        // If the event was handled, break early.
        if (eventResult.wasHandled) {
            break;
        }
    }

    processEventResult(eventResult);

    return eventResult.wasHandled;
}

} // End namespace AUI
