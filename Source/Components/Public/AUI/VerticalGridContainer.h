#pragma once

#include "AUI/Container.h"

namespace AUI {

/**
 * Lays out components in a grid that grows vertically.
 */
class VerticalGridContainer : public Container
{
public:
    VerticalGridContainer(Screen& screen, const char* key, const SDL_Rect& logicalExtent);

    virtual ~VerticalGridContainer() = default;

    /**
     * The number of columns to render components in. When all columns are
     * used, rendering continues down to the next row.
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

    void render(const SDL_Point& parentOffset = {}) override;

protected:
    /**
     * Overridden to properly scale cell size.
     */
    bool refreshScaling() override;

private:
    unsigned int numColumns;

    int logicalCellWidth;
    int scaledCellWidth;

    int logicalCellHeight;
    int scaledCellHeight;
};

} // namespace AUI
