#pragma once

#include "AUI/Widget.h"
#include "AUI/WidgetLocator.h"
#include <SDL2/SDL_events.h>
#include <vector>

namespace AUI
{
/**
 * This class represents a UI window.
 *
 * Windows are the second layer of UI construct used by this library. Screens
 * own a stack of Windows, Windows own a list of Widgets (and Widgets can
 * own child Widgets).
 *
 * Windows facilitate the organization of UI widgets into spatial groups, and
 * can be dragged around and overlapped. Widgets themselves don't support
 * overlapping, so things like dialog boxes must be built as separate windows.
 *
 * Windows can be:
 *   Dragged around the screen (not yet implemented)
 *   Resized (not yet implemented)
 *   Moved in front/behind each other (not yet implemented)
 */
class Window : public Widget
{
public:
    Window(Screen& screen, const SDL_Rect& inLogicalExtent,
              const std::string& inDebugName);

    virtual ~Window() = default;

    /**
     * Calls the onTick() of all of our children.
     */
    void tick(double timestepS);

    /**
     * Clears the old widget positions from the widget locator and starts
     * the layout pass.
     *
     * @post This widget and all children have up-to-date extents, and are
     *       added to this window's widgetLocator in the correct order.
     */
    void updateLayout(const SDL_Rect& parentExtent);

    //-------------------------------------------------------------------------
    // Base class overrides
    //-------------------------------------------------------------------------
    Widget* handleOSEvent(SDL_Event& event) override;

protected:
    /**
     * Propagates the given event through our child widgets.
     * If a child handles the event, updates our internal tracking.
     */
    Widget* passOSEventToChildren(SDL_Event& event);

    /**
     * Used to efficiently build an in-order list of widgets that were hit by
     * e.g. a mouse click event.
     * This Window's children are added to this locator at the end of their
     * updateLayout(). Since the layout pass iterates our children in their
     * rendering order, the locator will end up with a properly ordered grid of
     * widgets to use for hit testing.
     */
    WidgetLocator widgetLocator;

    /** The last child widget that consumed a MouseMove event. */
    Widget* lastHoveredChild;
    /** The last child widget that consumed a MouseButtonDown event.
        Cleared when the corresponding MouseButtonUp occurs. */
    Widget* lastClickedChild;
};

} // namespace AUI
