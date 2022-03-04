#include "AUI/EventRouter.h"
#include "AUI/EventResult.h"
#include "AUI/Screen.h"
#include "AUI/Window.h"
#include "AUI/Widget.h"
#include "AUI/Internal/Ignore.h"
#include "AUI/Internal/Log.h"
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
    EventResult eventResult{};
    SDL_Point cursorPosition{event.x, event.y};
    WidgetPath clickPath{getPathUnderCursor(cursorPosition)};
    if (!(clickPath.empty())) {
        AUI_LOG_INFO("cursorPosition: %d, %d", cursorPosition.x, cursorPosition.y);
        // Perform a MouseDown or MouseDoubleClick depending on how many
        // clicks occurred.
        MouseButtonType buttonType{translateSDLButtonType(event.button)};
        if (event.clicks == 1) {
            // TODO: Subtract the widget's renderExtent from cursorPosition
            //       before passing it in.
            eventResult = handleMouseDown(buttonType, cursorPosition, clickPath);
        }
        else {
            eventResult =  handleMouseDoubleClick(buttonType, cursorPosition, clickPath);
        }

        processEventResult(eventResult);

        return eventResult.wasConsumed;
    }

    return false;
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

    return eventResult.wasConsumed;
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
            EventResult eventResult{};
            for (auto it = hoverPath.rbegin(); it != hoverPath.rend(); ++it) {
                // If the widget is gone, skip it.
                WidgetWeakRef& widgetWeakRef{*it};
                if (!(widgetWeakRef.isValid())) {
                    continue;
                }

                // Pass the MouseWheel event to the widget.
                Widget& widget{widgetWeakRef.get()};
                eventResult = widget.onMouseWheel(amountScrolled);

                // If the event was consumed, break early.
                if (eventResult.wasConsumed) {
                    break;
                }
            }
        }
    }

    processEventResult(eventResult);

    return eventResult.wasConsumed;
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

    return eventResult.wasConsumed;
}

bool EventRouter::handleKeyDown(SDL_KeyboardEvent& event)
{
    ignore(event);
    return false;
}

bool EventRouter::handleTextInput(SDL_TextInputEvent& event)
{
    ignore(event);
    return false;
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

EventResult EventRouter::handleMouseDown(MouseButtonType buttonType,
                                  const SDL_Point& cursorPosition, WidgetPath& clickPath)
{
    // Perform the tunneling pass (root -> leaf, PreviewMouseDown).
    EventResult eventResult{};
    for (auto it = clickPath.begin(); it != clickPath.end(); ++it) {
        // If the widget is gone, skip it.
        WidgetWeakRef& widgetWeakRef{*it};
        if (!(widgetWeakRef.isValid())) {
            continue;
        }

        // Pass the PreviewMouseDown event to the widget.
        Widget& widget{widgetWeakRef.get()};
        eventResult = widget.onPreviewMouseDown(buttonType, cursorPosition);

        // If the event was consumed, break early.
        if (eventResult.wasConsumed) {
            break;
        }
    }

    // If a widget didn't consume the event during the preview pass, perform
    // the bubbling pass (leaf -> root, MouseDown).
    if (!(eventResult.wasConsumed)) {
        for (auto it = clickPath.rbegin(); it != clickPath.rend(); ++it) {
            // If the widget is gone, skip it.
            WidgetWeakRef& widgetWeakRef{*it};
            if (!(widgetWeakRef.isValid())) {
                continue;
            }

            // Pass the MouseDown event to the widget.
            Widget& widget{widgetWeakRef.get()};
            AUI_LOG_INFO("Sending MouseDown to %s", widget.getDebugName().c_str());
            eventResult = widget.onMouseDown(buttonType, cursorPosition);

            // If the event was consumed, break early.
            if (eventResult.wasConsumed) {
                break;
            }
        }
    }

    AUI_LOG_INFO("eventResult.wasConsumed: %u", eventResult.wasConsumed);
    return eventResult;
}

EventResult EventRouter::handleMouseDoubleClick(MouseButtonType buttonType,
                                  const SDL_Point& cursorPosition, WidgetPath& clickPath)
{
    // Perform the bubbling pass (leaf -> root, MouseDoubleClick).
    EventResult eventResult{};
    for (auto it = clickPath.rbegin(); it != clickPath.rend(); ++it) {
        // If the widget is gone, skip it.
        WidgetWeakRef& widgetWeakRef{*it};
        if (!(widgetWeakRef.isValid())) {
            continue;
        }

        // Pass the MouseDoubleClick event to the widget.
        Widget& widget{widgetWeakRef.get()};
        eventResult = widget.onMouseDoubleClick(buttonType, cursorPosition);

        // If the event was consumed, break early.
        if (eventResult.wasConsumed) {
            break;
        }
    }

    return eventResult;
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

        // If the event was consumed, break early.
        if (eventResult.wasConsumed) {
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
}

} // End namespace AUI
