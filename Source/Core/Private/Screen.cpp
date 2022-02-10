#include "AUI/Screen.h"
#include "AUI/Internal/Log.h"

namespace AUI
{
Screen::Screen(const std::string& inDebugName)
: lastHoveredWindow{nullptr}
, lastClickedWindow{nullptr}
, debugName{inDebugName}
{
}

bool Screen::handleOSEvent(SDL_Event& event)
{
    // Propagate the event through our visible windows.
    for (auto it = windows.rbegin(); it != windows.rend(); ++it) {
        // If the window isn't visible, skip it.
        Window& window{it->get()};
        if (!(window.getIsVisible())) {
            continue;
        }

        // If the window consumed this event, update our tracking and return
        // true.
        Widget* consumer{window.handleOSEvent(event)};
        if (consumer != nullptr) {
            if (event.type == SDL_MOUSEMOTION) {
                // If a new window was hovered, send the MouseMove to the last
                // hovered window so it can unhover itself.
                if ((lastHoveredWindow != nullptr)
                    && (&window != lastHoveredWindow)) {
                    lastHoveredWindow->handleOSEvent(event);
                }

                // Track the hovered window.
                lastHoveredWindow = &window;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                // If a new window was clicked, send the MouseDown to the last
                // clicked window so it can deselect itself.
                if ((lastClickedWindow != nullptr)
                    && (&window != lastClickedWindow)) {
                    lastClickedWindow->handleOSEvent(event);
                }

                // Track the clicked window.
                lastClickedWindow = &window;
            }
            else if (event.type == SDL_MOUSEBUTTONUP) {
                // If the mouse wasn't released over our last clicked window,
                // send it the MouseUp event so it can resolve its state.
                if ((lastClickedWindow != nullptr)
                    && (&window != lastClickedWindow)) {
                    lastClickedWindow->handleOSEvent(event);
                }
            }

            return true;
        }
    }

    // If the mouse moved outside of a hovered window, alert it and clear our
    // tracking.
    if ((event.type == SDL_MOUSEMOTION)
        && (lastHoveredWindow != nullptr)) {
        lastHoveredWindow->handleOSEvent(event);
        lastHoveredWindow = nullptr;
    }
    // If the mouse clicked outside of a clicked window, alert it and clear our
    // tracking.
    else if ((event.type == SDL_MOUSEBUTTONDOWN)
        && (lastClickedWindow != nullptr)) {
        lastClickedWindow->handleOSEvent(event);
        lastClickedWindow = nullptr;
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
    // Update our visible window's layouts.
    for (Window& window : windows)
    {
        if (window.getIsVisible()) {
            window.updateLayout({0, 0, 0, 0});
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

} // namespace AUI
