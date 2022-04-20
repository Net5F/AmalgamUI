#pragma once

#include "AUI/WidgetPath.h"
#include "AUI/WidgetWeakRef.h"
#include <SDL_rect.h>
#include <vector>
#include <unordered_map>

namespace AUI
{

class Widget;

/**
 * A spatial partitioning grid that tracks where widgets are located.
 *
 * Used to quickly find which widgets were hit by e.g. a mouse click event.
 *
 * Internally, widgets are organized into "cells", each of which has a size
 * corresponding to a configurable cell width. This value can be tweaked to
 * affect performance.
 */
class WidgetLocator
{
public:
    /**
     * @param inScreenExtent  The actual screen-space extent that this locator
     *                        should cover.
     */
    WidgetLocator(const SDL_Rect& inScreenExtent);

    // There shouldn't be any reason to copy or move a locator. If it's needed,
    // we can revisit this.
    WidgetLocator(const WidgetLocator& other) = delete;
    WidgetLocator(WidgetLocator&& other) = delete;

    /**
     * Adds the given widget to the locator.
     *
     * The widget's current position will be stored. To update the widget's
     * position in this locator, call clear() and re-add the widget.
     *
     * Note: Widgets are layered according to the order that they're added
     *       in. E.g. if 2 overlapping widgets are added, the second will
     *       be considered to be in front of the first.
     *
     * Note: Assumes the given widget is fully within this locator's extent.
     *       Don't pass in widgets that are outside its bounds.
     *
     * @param widget  The widget to set the location of.
     */
    void addWidget(Widget* widget);

    /**
     * If we're tracking the given widget, removes it from the widgetGrid and
     * widgetMap.
     *
     * Note: Typically you'll remove widgets by calling clear() and adding
     *       them back in order. This is necessary to enforce your desired
     *       layering.
     */
    void removeWidget(Widget* widget);

    /**
     * Builds a path containing all tracked widgets that are underneath the
     * given actual-space point.
     *
     * The returned path is ordered from root-most -> leaf-most.
     *
     * @param actualPoint  The point in actual space to test widgets with.
     * @return A widget path, ordered with the root-most widget at the front
     *         and the leaf-most widget at the back.
     */
    WidgetPath getPathUnderPoint(const SDL_Point& actualPoint);

    /**
     * Clears all of our internal data structures, getting rid of any tracked
     * widgets.
     */
    void clear();

    /**
     * Sets the part of the screen (in actual space) that this widget locator
     * covers.
     *
     * Typically, this will match the extent of the window that this locator
     * belongs to.
     *
     * All tracked widgets must be fully within these bounds.
     *
     * @param inScreenExtent  The actual screen-space extent that this locator
     *                        should cover.
     */
    void setExtent(const SDL_Rect& inScreenExtent);

    /**
     * Sets the width of the cells in the spatial partitioning grid.
     * Note: This isn't typically necessary, the default value should be fine
     *       in most cases.
     */
    void setCellWidth(float inCellWidth);

    // Testing interface, you probably don't need to use these.
    SDL_Rect getGridCellExtent();

private:
    /** The default logical pixel width of the cells in the spatial
        partitioning grid. */
    static constexpr float LOGICAL_DEFAULT_CELL_WIDTH = 128;

    /**
     * Removes the given widget from the cells within the given extent.
     *
     * Note: This leaves the widget in the entityMap. Only the tracked
     *       location is cleared out.
     */
    void clearWidgetLocation(Widget* widget, const SDL_Rect& cellClearExtent);

    /**
     * Returns the index in the widgetGrid vector where the cell with the given
     * coordinates can be found.
     */
    inline unsigned int linearizeCellIndex(int x, int y) const
    {
        return (y * gridCellExtent.w) + x;
    }

    /**
     * Converts the given screen extent to a cell extent.
     */
    SDL_Rect screenToCellExtent(const SDL_Rect& screenExtent);

    /** The width of a grid cell in logical-space pixels. */
    float cellWidth;

    /** The grid's extent in actual screen space. */
    SDL_Rect gridScreenExtent;

    /** The grid's extent in actual screen space, with cells as the unit. */
    SDL_Rect gridCellExtent;

    /** The outer vector is a 2D grid stored in row-major order, holding the
        grid's cells.
        Each element in the grid is a vector of widgets--the widgets that
        currently intersect with that cell. */
    std::vector<std::vector<WidgetWeakRef>> widgetGrid;

    /** A map of widget pointer -> the cells that the widget is located in.
        Used to easily clear out old widget data before setting their new
        location.
        Note: The widget pointers in this map are not safe to reference, as
              they may have gone invalid since they were added. We're only
              using them as identifiers. */
    std::unordered_map<Widget*, SDL_Rect> widgetMap;
};

} // End namespace AUI
