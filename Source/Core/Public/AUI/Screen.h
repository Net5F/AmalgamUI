#pragma once

#include "AUI/Window.h"
#include "AUI/EventRouter.h"
#include "AUI/WidgetWeakRef.h"
#include <SDL_events.h>
#include <vector>
#include <optional>

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
     * Returns the topmost window under the given point if one was found, else
     * nullptr.
     */
    Window* getWindowUnderPoint(const SDL_Point& point);

    /**
     * Returns the given widget's parent window.
     */
    Window* getWidgetParentWindow(const Widget* widget);

    /**
     * Attempts to set focus to the given widget.
     * Fails if the given widget isn't in the current layout.
     */
    void setFocus(const Widget* widget);

    /**
     * If a widget is currently focused, drops it.
     */
    void dropFocus();

    /**
     * Saves the given widget pointer and attempts to set focus to it after the 
     * next layout update.
     * Use this if you've just made a widget visible and want it to be the 
     * focus target. You can't set focus to it immediately because it isn't 
     * yet in the layout, but this will do it at the correct time.
     */
    void setFocusAfterNextLayout(Widget* widget);

    /**
     * Passes the given SDL event to the EventRouter, where translation and
     * routing occurs.
     *
     * @return true if the event was consumed, else false.
     */
    bool handleOSEvent(SDL_Event& event);

    /**
     * Called when a key press isn't handled by any of our widgets.
     *
     * KeyDown events are first routed to the focused widgets. If they don't
     * handle the event or no widgets are focused, the event will then be
     * routed to this function.
     *
     * Note: This is intended to be used to open windows on key press, e.g.
     *       opening a menu when the escape key is pressed.
     *
     * @return true if the KeyDown was handled, else false.
     */
    virtual bool onKeyDown(SDL_Keycode keyCode);

    /**
     * Call the onTick() of all of our visible windows.
     *
     * @param timestepS  The amount of time that has passed since the last
     *                   tick() call, in seconds.
     */
    virtual void tick(double timestepS);

    /**
     * Renders all UI graphics for this screen to the current rendering target.
     */
    virtual void render();

protected:
    /** The user-assigned name associated with this screen.
        Only useful for debugging. For performance reasons, avoid using it
        in real logic. */
    std::string debugName;

    /** An ordered list of references to this screen's windows.
        Windows must be added to this list to be involved in layout, rendering,
        and event propagation.
        Windows must be separate members of the derived class. This list only
        holds references to those members.
        This list's elements are in rendering order (rendering happens from
        front -> back, events propagate from back -> front). */
    std::vector<std::reference_wrapper<Window>> windows;

    /** Translates SDL events to AUI events and handles routing them. */
    EventRouter eventRouter;

    /** If non-empty, the referenced widget will be given focus after the next 
        layout update. */
    std::optional<WidgetWeakRef> pendingFocusTarget;
};

} // namespace AUI
