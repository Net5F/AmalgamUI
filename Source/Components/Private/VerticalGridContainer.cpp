#include "AUI/VerticalGridContainer.h"
#include "AUI/ScalingHelpers.h"

namespace AUI {

VerticalGridContainer::VerticalGridContainer(Screen& screen, const char* key, const SDL_Rect& logicalExtent)
: Container(screen, key, logicalExtent)
, numColumns{1}
, logicalCellWidth{100}
, scaledCellWidth{ScalingHelpers::logicalToActual(logicalCellWidth)}
, logicalCellHeight{100}
, scaledCellHeight{ScalingHelpers::logicalToActual(logicalCellHeight)}
{
}

void VerticalGridContainer::setNumColumns(unsigned int inNumColumns)
{
    numColumns = inNumColumns;
}

void VerticalGridContainer::setCellWidth(unsigned int inLogicalCellWidth)
{
    logicalCellWidth = static_cast<int>(inLogicalCellWidth);
    scaledCellWidth = ScalingHelpers::logicalToActual(logicalCellWidth);
}

void VerticalGridContainer::setCellHeight(unsigned int inLogicalCellHeight)
{
    logicalCellHeight = static_cast<int>(inLogicalCellHeight);
    scaledCellHeight = ScalingHelpers::logicalToActual(logicalCellHeight);
}

void VerticalGridContainer::render(const SDL_Point& parentOffset)
{
    // Keep our scaling up to date.
    refreshScaling();

    // Account for the given offset.
    SDL_Rect offsetExtent{scaledExtent};
    offsetExtent.x += parentOffset.x;
    offsetExtent.y += parentOffset.y;

    // Save the extent that we're going to render at.
    lastRenderedExtent = offsetExtent;

    // If the component isn't visible, return without rendering.
    if (!isVisible) {
        return;
    }

    // Render our elements in a vertical grid.
    for (unsigned int i = 0; i < elements.size(); ++i) {
        // Get the cell coordinates for this element.
        unsigned int cellXPos = i % numColumns;
        unsigned int cellYPos = i / numColumns;

        // Get the offsets for the cell at the calculated coordinates.
        int cellXOffset = cellXPos * scaledCellWidth;
        int cellYOffset = cellYPos * scaledCellHeight;

        // Add this component's offset to get our final offset.
        int finalX = offsetExtent.x + cellXOffset;
        int finalY = offsetExtent.y + cellYOffset;
        elements[i]->render({finalX, finalY});
    }
}

bool VerticalGridContainer::refreshScaling()
{
    // If actualScreenExtent was refreshed, do our specialized refreshing.
    if (Component::refreshScaling()) {
        // Refresh the cell width and height.
        scaledCellWidth = ScalingHelpers::logicalToActual(logicalCellWidth);
        scaledCellHeight = ScalingHelpers::logicalToActual(logicalCellHeight);

        return true;
    }

    return false;
}

} // namespace AUI
