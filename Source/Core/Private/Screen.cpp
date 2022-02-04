#include "AUI/Screen.h"
#include <algorithm>

namespace AUI
{
Screen::Screen(const std::string& inDebugName)
: lastHoveredWidget{nullptr}
, lastClickedWidget{nullptr}
, debugName{inDebugName}
{
}

bool Screen::handleOSEvent(SDL_Event& event)
{
    for (auto it = children.rbegin(); it != children.rend(); ++it) {
        // If the child isn't visible, skip it.
        Widget& child{it->get()};
        if (!(child.getIsVisible())) {
            continue;
        }

        // If this child consumed this event, update our tracking and return
        // true.
        Widget* consumer{child.handleOSEvent(event)};
        if (consumer != nullptr) {
            if (event.type == SDL_MOUSEMOTION) {
                // If a new widget was hovered, send the MouseMove to the last
                // hovered widget so it can unhover itself.
                if ((lastHoveredWidget != nullptr)
                    && (consumer != lastHoveredWidget)) {
                    lastHoveredWidget->handleOSEvent(event);
                }

                // Track the hovered widget.
                lastHoveredWidget = consumer;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                // If a new widget was clicked, send the MouseDown to the last
                // clicked widget so it can deselect itself.
                if ((lastClickedWidget != nullptr)
                    && (consumer != lastClickedWidget)) {
                    lastClickedWidget->handleOSEvent(event);
                }

                // Track the clicked widget.
                lastClickedWidget = consumer;
            }
            else if (event.type == SDL_MOUSEBUTTONUP) {
                // If the mouse wasn't released over our last clicked widget,
                // send it the MouseUp event so it can resolve its state.
                if (consumer != lastClickedWidget) {
                    lastClickedWidget->handleOSEvent(event);
                }
            }

            return true;
        }
    }

    // If the mouse moved outside of the hovered widget, clear it.
    if ((event.type == SDL_MOUSEMOTION)
        && (lastHoveredWidget != nullptr)) {
        lastHoveredWidget = nullptr;
    }
    // If the mouse clicked outside of the clicked widget, clear it.
    else if ((event.type == SDL_MOUSEBUTTONDOWN)
        && (lastClickedWidget != nullptr)) {
        lastClickedWidget = nullptr;
    }

    return false;
}

void Screen::tick(double timestepS)
{
    // Call every widget's tick.
    for (Widget& child : children) {
        // If the child isn't visible, skip it.
        if (!(child.getIsVisible())) {
            continue;
        }

        child.onTick(timestepS);
    }
}

void Screen::render()
{
    // Update our children's layouts.
    for (Widget& child : children)
    {
        child.updateLayout({0, 0, 0, 0});
    }

    // Render our children.
    for (Widget& child : children)
    {
        if (child.getIsVisible()) {
            child.render();
        }
    }
}

} // namespace AUI
