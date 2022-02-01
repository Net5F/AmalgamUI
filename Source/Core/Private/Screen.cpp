#include "AUI/Screen.h"
#include <algorithm>

namespace AUI
{
Screen::Screen(const std::string& inDebugName)
: debugName{inDebugName}
{
}

bool Screen::handleOSEvent(SDL_Event& event)
{
    bool eventHandled{false};
    for (auto it = children.rbegin(); it != children.rend(); ++it) {
        // If the child isn't visible, skip it.
        Widget& child{it->get()};
        if (!(child.getIsVisible())) {
            continue;
        }

        // Call the child's handler and track if the event was handled.
        if (child.handleOSEvent(event)) {
            eventHandled = true;
        }
    }

    return eventHandled;
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
