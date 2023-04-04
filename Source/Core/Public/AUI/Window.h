#pragma once

#include "AUI/Widget.h"
#include "AUI/WidgetLocator.h"
#include <SDL_events.h>
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
 *
 * Note: Windows derive from Widget so that they can be easily added to the
 *       WidgetLocator, but they are not meant to be composed like regular
 *       widgets.
 * Note: To match the expected Widget behavior, a Window's fullExtent and
 *       clippedExtent are window-relative (in this case, meaning x and y are
 *       0). To get a Window's position on the screen, use scaledExtent.
 */
class Window : public Widget
{
public:
    Window(const SDL_Rect& inLogicalExtent, const std::string& inDebugName);

    virtual ~Window() = default;

    /**
     * Builds a path containing all tracked widgets that are underneath the
     * given actual-space point.
     *
     * The returned path is in the order that an event should travel.
     *
     * @param actualPoint  The point in actual space to test widgets with.
     * @return A widget path, ordered with the root-most widget at the front
     *         and the leaf-most widget at the back.
     */
    WidgetPath getPathUnderPoint(const SDL_Point& actualPoint);

    /**
     * Calls the onTick() of all of our children.
     */
    void tick(double timestepS);

    /**
     * Clears the old widget positions from widgetLocator and performs the
     * layout pass.
     *
     * @post This window and all children have up-to-date extents, and are
     *       added to this window's widgetLocator in the correct order.
     */
    virtual void updateLayout();

    /**
     * Renders this window to the current rendering target.
     *
     * The default implementation simply calls render() on all widgets in our
     * children list. Some overrides may directly call SDL functions like
     * SDL_RenderCopy().
     */
    virtual void render();

protected:
    /**
     * Used to efficiently build an in-order list of widgets that were hit by
     * e.g. a mouse click event.
     * This Window's children are added to this locator at the end of their
     * updateLayout(). Since the layout pass iterates our children in their
     * rendering order, the locator will end up with a properly ordered grid of
     * widgets to use for hit testing.
     */
    WidgetLocator widgetLocator;

private:
    // We hide the Widget update/render implementations, because Windows have
    // different needs. Specifically, they don't receive layout info from
    // their parent, and they own their widgetLocator.
    using Widget::updateLayout;
    using Widget::render;
};

} // namespace AUI
