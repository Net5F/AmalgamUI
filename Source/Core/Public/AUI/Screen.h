#pragma once

#include "AUI/Window.h"
#include "AUI/EventRouter.h"
#include <SDL2/SDL_events.h>
#include <vector>

namespace AUI
{
/**
 * Represents a UI screen.
 *
 * Screens are the first layer of UI construct used by this library. Screens
 * own a stack of Windows, Windows own a list of Widgets (and Widgets can
 * own child Widgets).
 *
 * Screens facilitate window management. For example, if a window is clicked,
 * the screen may bring it to the front of its list of windows.
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
     * Passes the given SDL event to the EventRouter, where translation and
     * routing occurs.
     *
     * @return true if the event was consumed, else false.
     */
    bool handleOSEvent(SDL_Event& event);

    /**
     * Call the onTick() of all of our visible windows.
     *
     * @param timestepS  The amount of time that has passed since the last
     *                   tick() call, in seconds.
     */
    void tick(double timestepS);

    /**
     * Renders all graphics for this screen to the current rendering target.
     */
    virtual void render();

    /**
     * @return The topmost window under the given point if one was found, else
     *         nullptr.
     */
    Window* getWindowUnderPoint(const SDL_Point& point);

protected:
    /** The user-assigned name associated with this screen.
        Only useful for debugging. For performance reasons, avoid using it
        in real logic. */
    std::string debugName;

    /** An ordered list of references to this screen's windows.
        Windows must be added to this list to be involved in layout, rendering,
        and event propagation.
        Windows must be separate members of the class. This list only
        holds references to those members.
        This list's elements are in rendering order (rendering happens from
        front -> back, events propagate from back -> front). */
    std::vector<std::reference_wrapper<Window>> windows;

    /** Translates SDL events to AUI events and handles routing them. */
    EventRouter eventRouter;
};

} // namespace AUI
