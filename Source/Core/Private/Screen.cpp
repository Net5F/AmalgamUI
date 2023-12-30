#include "AUI/Screen.h"
#include "AUI/Core.h"
#include "AUI/Image.h"
#include "AUI/Internal/Log.h"
#include <SDL_rect.h>

namespace AUI
{
Screen::Screen(const std::string& inDebugName)
: debugName{inDebugName}
, eventRouter{*this}
, pendingFocusTarget{}
{
}

Window* Screen::getWindowUnderPoint(const SDL_Point& point)
{
    for (auto it = windows.rbegin(); it != windows.rend(); ++it) {
        // If the window isn't visible, skip it.
        Window& window{it->get()};
        if (!(window.getIsVisible())) {
            continue;
        }

        // If the window contains the given point, return it.
        if (SDL_PointInRect(&point, &(window.getScaledExtent()))) {
            return &window;
        }
    }

    return nullptr;
}

Window* Screen::getWidgetParentWindow(const Widget* widget)
{
    for (auto it = windows.rbegin(); it != windows.rend(); ++it) {
        // If the window isn't visible, skip it.
        Window& window{it->get()};
        if (!(window.getIsVisible())) {
            continue;
        }

        // If the window contains the given widget, return it.
        if (window.containsWidget(widget)) {
            return &window;
        }
    }

    return nullptr;
}

void Screen::setFocus(const Widget* widget)
{
    // If the widget is in the layout, set focus to it.
    if (getWidgetParentWindow(widget) != nullptr) {
        eventRouter.setFocus(widget);
    }
    else {
        // The widget isn't in the layout. If you hit this, make sure the
        // widget is visible and has been through a layout pass before trying
        // to set focus to it.
        AUI_LOG_ERROR("Tried to set focus to widget that isn't in the layout.");
    }
}

void Screen::dropFocus()
{
    eventRouter.dropFocus();
}

void Screen::setFocusAfterNextLayout(Widget* widget)
{
    pendingFocusTarget.emplace(*widget);
}

bool Screen::handleOSEvent(SDL_Event& event)
{
    // TODO: Either here or in EventRouter, move windows to the
    //       top of the stack when they're clicked.
    // Pass the event to the appropriate handler.
    switch (event.type) {
        case SDL_MOUSEBUTTONDOWN: {
            return eventRouter.handleMouseButtonDown(event.button);
        }
        case SDL_MOUSEBUTTONUP: {
            return eventRouter.handleMouseButtonUp(event.button);
        }
        case SDL_MOUSEMOTION: {
            return eventRouter.handleMouseMove(event.motion);
        }
        case SDL_MOUSEWHEEL: {
            return eventRouter.handleMouseWheel(event.wheel);
        }
        case SDL_KEYDOWN: {
            return eventRouter.handleKeyDown(event.key);
        }
        case SDL_TEXTINPUT: {
            return eventRouter.handleTextInput(event.text);
        }
        default:
            break;
    }

    return false;
}

bool Screen::onKeyDown(SDL_Keycode)
{
    return false;
}

void Screen::tick(double timestepS)
{
    // Call every visible window's onTick().
    for (Window& window : windows) {
        if (!(window.getIsVisible())) {
            continue;
        }

        window.onTick(timestepS);
    }
}

void Screen::render()
{
    // Update our visible window's layouts.
    for (Window& window : windows) {
        if (window.getIsVisible()) {
            window.updateLayout();
        }
    }

    // If we have a pending focus target, set it.
    if (pendingFocusTarget && pendingFocusTarget.value().isValid()) {
        setFocus(&(pendingFocusTarget.value().get()));
        pendingFocusTarget.reset();
    }

    // Render our visible windows.
    for (Window& window : windows) {
        if (window.getIsVisible()) {
            window.render();
        }
    }

    // If we're dragging a widget, render its drag drop image at the current
    // mouse position.
    if (Image* dragDropImage{eventRouter.getDragDropImage()}) {
        SDL_Point cursorPosition{};
        SDL_GetMouseState(&(cursorPosition.x), &(cursorPosition.y));

        dragDropImage->render(cursorPosition);
    }
}

} // namespace AUI
