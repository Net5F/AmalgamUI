#pragma once

#include "AUI/Container.h"

namespace AUI
{
/**
 * Lays out widgets in a grid that grows horizontally.
 *
 * Supports horizontal scrolling with the mouse wheel.
 *
 * TODO: After scrolling the list, our elements still have hover states based
 *       on their pre-scroll positions. We need to find a way to update them.
 */
class HorizontalGridContainer : public Container
{
public:
    //-------------------------------------------------------------------------
    // Public interface
    //-------------------------------------------------------------------------
    HorizontalGridContainer(const SDL_Rect& inLogicalExtent,
                            const std::string& inDebugName
                            = "HorizontalGridContainer");

    virtual ~HorizontalGridContainer() = default;

    /**
     * The number of rows to arrange widgets in. Layout occurs down the rows,
     * then across to the next column
     */
    void setNumRows(unsigned int inNumRows);

    /**
     * Sets the width of a grid cell. The elements of this container will be
     * rendered starting at the top left of their cell.
     */
    void setCellWidth(unsigned int inLogicalCellWidth);

    /**
     * Sets the height of a grid cell. The elements of this container will be
     * rendered starting at the top left of their cell.
     */
    void setCellHeight(unsigned int inLogicalCellHeight);

    /**
     * If true, horizontal scrolling with the mouse wheel will be enabled.
     */
    void setScrollingEnabled(bool isEnabled);

    //-------------------------------------------------------------------------
    // Base class overrides
    //-------------------------------------------------------------------------
    EventResult onMouseWheel(int amountScrolled) override;

    void updateLayout(const SDL_Point& startPosition,
                      const SDL_Rect& availableExtent,
                      WidgetLocator* widgetLocator) override;

private:
    /** The default logical pixel width of this container's cells. */
    static constexpr int LOGICAL_DEFAULT_CELL_WIDTH = 100;

    /**
     * Scrolls the visible elements in the container left or right, bringing
     * offscreen elements onto the screen.
     *
     * If there aren't any offscreen elements in the selected direction, does
     * nothing.
     *
     * @param scrollLeft If true, scrolls left by 1 element. If false, scrolls 
     *                   down by 1 element.
     */
    void scrollElements(bool scrollLeft);

    /** The number of columns to render widgets in. */
    unsigned int numRows;

    /** The width in logical space of a grid cell. */
    int logicalCellWidth;
    /** The scaled width in actual space of a grid cell. */
    int scaledCellWidth;

    /** The height in logical space of a grid cell. */
    int logicalCellHeight;
    /** The scaled height in actual space of a grid cell. */
    int scaledCellHeight;

    /** If true, mouse wheel events should scroll this container's elements 
        horizontally. */
    bool isScrollingEnabled;

    /** How many columns to the right we're currently scrolled. */
    unsigned int columnScroll;
};

} // namespace AUI
