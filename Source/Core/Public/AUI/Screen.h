#pragma once

#include "AUI/Widget.h"
#include "AUI/Internal/Log.h"
#include <SDL2/SDL_events.h>
#include <vector>
#include <unordered_map>

namespace AUI
{
/**
 * This class represents a UI screen.
 *
 * Screens facilitate the organization of UI widgets, and provide an easy way
 * to switch between sets of widgets.
 *
 * An appropriate screen may be a title screen, settings screen, or a world
 * screen that displays UI elements while allowing the user to see the world
 * behind it.
 */
class Screen
{
public:
    Screen(const std::string& inDebugName);

    virtual ~Screen() = default;

    /**
     * Propagates the given event through this screen's children.
     *
     * @return true if the event was consumed, else false.
     */
    bool handleOSEvent(SDL_Event& event);

    /**
     * Call the onTick() of all of our visible children.
     *
     * @param timestepS  The amount of time that has passed since the last
     *                   tick() call, in seconds.
     */
    void tick(double timestepS);

    /**
     * Renders all graphics for this screen to the current rendering target.
     */
    virtual void render();

protected:
    /** An ordered list of references to this widget's children.
        Widgets must be added to this list to be involved in layout, rendering,
        and event propagation.
        Child widgets must be separate members of the class. This list only
        holds references to those members.
        This list's elements are in rendering order (rendering happens from
        front -> back, events propagate from back -> front). */
    std::vector<std::reference_wrapper<Widget>> children;

private:
    /** The user-assigned name associated with this screen.
        Only useful for debugging. For performance reasons, avoid using it
        in real logic. */
    std::string debugName;
};

} // namespace AUI
