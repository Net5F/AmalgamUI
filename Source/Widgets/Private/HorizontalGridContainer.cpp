#include "AUI/HorizontalGridContainer.h"
#include "AUI/ScalingHelpers.h"
#include "AUI/WidgetLocator.h"
#include "AUI/Internal/Log.h"
#include <cmath>
#include "AUI/Core.h"

namespace AUI
{
HorizontalGridContainer::HorizontalGridContainer(const SDL_Rect& inLogicalExtent,
                                             const std::string& inDebugName)
: Container(inLogicalExtent, inDebugName)
, numRows{1}
, logicalCellWidth{LOGICAL_DEFAULT_CELL_WIDTH}
, scaledCellWidth{ScalingHelpers::logicalToActual(logicalCellWidth)}
, logicalCellHeight{LOGICAL_DEFAULT_CELL_WIDTH}
, scaledCellHeight{ScalingHelpers::logicalToActual(logicalCellHeight)}
, columnScroll{0}
, isScrollingEnabled{true}
{
}

void HorizontalGridContainer::setNumRows(unsigned int inNumRows)
{
    numRows = inNumRows;
}

void HorizontalGridContainer::setCellWidth(unsigned int inLogicalCellWidth)
{
    logicalCellWidth = static_cast<int>(inLogicalCellWidth);
    scaledCellWidth = ScalingHelpers::logicalToActual(logicalCellWidth);
}

void HorizontalGridContainer::setCellHeight(unsigned int inLogicalCellHeight)
{
    logicalCellHeight = static_cast<int>(inLogicalCellHeight);
    scaledCellHeight = ScalingHelpers::logicalToActual(logicalCellHeight);
}

void HorizontalGridContainer::setScrollingEnabled(bool isEnabled)
{
    isScrollingEnabled = isEnabled;
}

EventResult HorizontalGridContainer::onMouseWheel(int amountScrolled)
{
    if (!isScrollingEnabled) {
        return EventResult{.wasHandled{false}};
    }

    if (amountScrolled > 0) {
        // Scroll right.
        scrollElements(true);
    }
    else {
        // Scroll left.
        scrollElements(false);
    }

    return EventResult{.wasHandled{true}};
}

void HorizontalGridContainer::measure(const SDL_Rect& availableExtent)
{
    // Run the normal measure step (doesn't affect us since we don't use the 
    // children vector, but good to do in case of extension).
    Widget::measure(availableExtent);

    // Give our elements a chance to update their logical extent.
    for (auto& element : elements) {
        // Note: We measure/arrange all elements, even if they're invisible, 
        //       so we can get the rest of the elements offsets correct.
        element->measure(logicalExtent);
    }
}

void HorizontalGridContainer::arrange(const SDL_Point& startPosition,
                                      const SDL_Rect& availableExtent,
                                      WidgetLocator* widgetLocator)
{
    // Run the normal arrange step (will arrange us, but won't arrange any of
    // our elements).
    Widget::arrange(startPosition, availableExtent, widgetLocator);

    // If this widget is fully clipped, return early.
    if (SDL_RectEmpty(&clippedExtent)) {
        return;
    }

    // Refresh the cell width and height.
    scaledCellWidth = ScalingHelpers::logicalToActual(logicalCellWidth);
    scaledCellHeight = ScalingHelpers::logicalToActual(logicalCellHeight);

    // Lay out our elements in a vertical grid.
    for (std::size_t i = 0; i < elements.size(); ++i) {
        // Get the cell coordinates for this element.
        std::size_t cellRow{i % numRows};
        std::size_t cellColumn{i / numRows};

        // Get the offsets for the cell at the calculated coordinates.
        int cellXOffset{static_cast<int>(cellColumn * scaledCellWidth)};
        int cellYOffset{static_cast<int>(cellRow * scaledCellHeight)};

        // Move the X offset based on our current scroll position.
        cellXOffset -= (columnScroll * scaledCellWidth);

        // Add this widget's offset to get our final offset.
        int finalX{fullExtent.x + cellXOffset};
        int finalY{fullExtent.y + cellYOffset};
        elements[i]->arrange({finalX, finalY}, clippedExtent, widgetLocator);
    }
}

void HorizontalGridContainer::scrollElements(bool scrollLeft)
{
    // Calc how many columns are currently present.
    int currentColumns{static_cast<int>(
        std::ceil(elements.size() / static_cast<float>(numRows)))};

    // Calc how many columns can fit onscreen at once.
    int maxVisibleColumns{logicalExtent.w / logicalCellWidth};

    // If we're being asked to scroll left and we've scrolled right previously.
    if (scrollLeft && (columnScroll > 0)) {
        // Scroll left 1 row.
        columnScroll--;
    }
    else if (!scrollLeft) {
        // Else if we're being asked to scroll right, calculate if there are
        // any columns to the right to scroll to.
        int columnsRight{currentColumns - maxVisibleColumns
                         - static_cast<int>(columnScroll)};

        // If there are any elements offscreen, scroll to the right 1 column.
        if (columnsRight > 0) {
            columnScroll++;
        }
    }
}

} // namespace AUI
