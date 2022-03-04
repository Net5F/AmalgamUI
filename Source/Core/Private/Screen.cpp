#include "AUI/Screen.h"
#include "AUI/Core.h"
#include "AUI/SDLHelpers.h"
#include "AUI/Internal/Ignore.h"
#include "AUI/Internal/Log.h"

namespace AUI
{
Screen::Screen(const std::string& inDebugName)
: debugName{inDebugName}
, eventRouter{*this}
{
}

bool Screen::handleOSEvent(SDL_Event& event)
{
    // TODO: Either here or in EventRouter, move clicked windows to the
    //       top of the stack.
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

void Screen::tick(double timestepS)
{
    // Call every window's tick.
    for (Window& window : windows) {
        // If the window isn't visible, skip it.
        if (!(window.getIsVisible())) {
            continue;
        }

        window.tick(timestepS);
    }
}

void Screen::render()
{
    // TODO: Combine the update and render into 1 loop and test.
    // Update our visible window's layouts.
    for (Window& window : windows)
    {
        if (window.getIsVisible()) {
            ScreenResolution actualScreenSize{Core::getActualScreenSize()};
            window.updateLayout({0, 0, actualScreenSize.width
                , actualScreenSize.height});
        }
    }

    // Render our visible windows.
    for (Window& window : windows)
    {
        if (window.getIsVisible()) {
            window.render();
        }
    }
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
        if (SDLHelpers::pointInRect(point, window.getRenderExtent())) {
            return &window;
        }
    }

    return nullptr;
}

} // namespace AUI
