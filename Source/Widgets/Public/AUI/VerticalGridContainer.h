#pragma once

#include "AUI/Container.h"

namespace AUI
{
/**
 * Lays out widgets in a grid that grows vertically.
 *
 * TODO: After scrolling the list, our elements still have hover states based
 *       on their pre-scroll positions. We need to find a way to update them.
 */
class VerticalGridContainer : public Container
{
public:
    //-------------------------------------------------------------------------
    // Public interface
    //-------------------------------------------------------------------------
    VerticalGridContainer(const SDL_Rect& inLogicalExtent,
                          const std::string& inDebugName = "VerticalGridContainer");

    virtual ~VerticalGridContainer() = default;

    /**
     * The number of columns to render widgets in. Rendering occurs across
     * the columns, then down to the next row.
     */
    void setNumColumns(unsigned int inNumColumns);

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

    //-------------------------------------------------------------------------
    // Base class overrides
    //-------------------------------------------------------------------------
    EventResult onMouseWheel(int amountScrolled) override;

    void updateLayout(const SDL_Rect& parentExtent, WidgetLocator* widgetLocator) override;

protected:
    /**
     * Overridden to properly scale cell size.
     */
    bool refreshScaling() override;

private:
    /** The default logical pixel width of this container's cells. */
    static constexpr int LOGICAL_DEFAULT_CELL_WIDTH = 100;

    /**
     * Scrolls the visible elements in the container up or down, bringing
     * offscreen elements onto the screen.
     *
     * If there aren't any offscreen elements in the selected direction, does
     * nothing.
     *
     * @param scrollUp  If true, scrolls up 1 element. If false, scrolls down 1
     *                  element.
     */
    void scrollElements(bool scrollUp);

    /** The number of columns to render widgets in. */
    unsigned int numColumns;

    /** The width in logical space of a grid cell. */
    int logicalCellWidth;
    /** The scaled width in actual space of a grid cell. */
    int scaledCellWidth;

    /** The height in logical space of a grid cell. */
    int logicalCellHeight;
    /** The scaled height in actual space of a grid cell. */
    int scaledCellHeight;

    /** How many rows downwards we're currently scrolled. */
    unsigned int rowScroll;
};

} // namespace AUI
