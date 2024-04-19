#include "AUI/VerticalGridContainer.h"
#include "AUI/ScalingHelpers.h"
#include "AUI/WidgetLocator.h"
#include "AUI/Internal/Log.h"
#include <cmath>

namespace AUI
{
VerticalGridContainer::VerticalGridContainer(const SDL_Rect& inLogicalExtent,
                                             const std::string& inDebugName)
: Container(inLogicalExtent, inDebugName)
, numColumns{1}
, logicalCellWidth{LOGICAL_DEFAULT_CELL_WIDTH}
, scaledCellWidth{ScalingHelpers::logicalToActual(logicalCellWidth)}
, logicalCellHeight{LOGICAL_DEFAULT_CELL_WIDTH}
, scaledCellHeight{ScalingHelpers::logicalToActual(logicalCellHeight)}
, rowScroll{0}
, isScrollingEnabled{true}
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

void VerticalGridContainer::setScrollingEnabled(bool isEnabled)
{
    isScrollingEnabled = isEnabled;
}

EventResult VerticalGridContainer::onMouseWheel(int amountScrolled)
{
    if (!isScrollingEnabled) {
        return EventResult{.wasHandled{false}};
    }

    if (amountScrolled > 0) {
        // Scroll up.
        scrollElements(true);
    }
    else {
        // Scroll down.
        scrollElements(false);
    }

    return EventResult{.wasHandled{true}};
}

void VerticalGridContainer::measure(const SDL_Rect& availableExtent)
{
    // Run the normal measure step (sets our scaledExtent).
    Widget::measure(availableExtent);

    // Refresh the cell width and height.
    scaledCellWidth = ScalingHelpers::logicalToActual(logicalCellWidth);
    scaledCellHeight = ScalingHelpers::logicalToActual(logicalCellHeight);

    // Give our elements a chance to update their logical extent.
    for (auto& element : elements) {
        // Note: We measure/arrange all elements, even if they're invisible, 
        //       so we can get the rest of the elements offsets correct.
        element->measure(logicalExtent);
    }
}

void VerticalGridContainer::arrange(const SDL_Point& startPosition,
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

    // Lay out our elements in a vertical grid.
    for (std::size_t i = 0; i < elements.size(); ++i) {
        // Get the cell coordinates for this element.
        std::size_t cellColumn{i % numColumns};
        std::size_t cellRow{i / numColumns};

        // Get the offsets for the cell at the calculated coordinates.
        int cellXOffset{static_cast<int>(cellColumn * scaledCellWidth)};
        int cellYOffset{static_cast<int>(cellRow * scaledCellHeight)};

        // Move the Y offset based on our current scroll position.
        cellYOffset -= (rowScroll * scaledCellHeight);

        // Add this widget's offset to get our final offset.
        int finalX{fullExtent.x + cellXOffset};
        int finalY{fullExtent.y + cellYOffset};
        elements[i]->arrange({finalX, finalY}, clippedExtent, widgetLocator);
    }
}

void VerticalGridContainer::scrollElements(bool scrollUp)
{
    // Calc how many rows are currently present.
    int currentRows{static_cast<int>(
        std::ceil(elements.size() / static_cast<float>(numColumns)))};

    // Calc how many rows can fit onscreen at once.
    int maxVisibleRows{logicalExtent.h / logicalCellHeight};

    // If we're being asked to scroll up and we've scrolled down previously.
    if (scrollUp && (rowScroll > 0)) {
        // Scroll up 1 row.
        rowScroll--;
    }
    else if (!scrollUp) {
        // Else if we're being asked to scroll down, calculate if there are
        // any rows below to scroll to.
        int rowsBelow{currentRows - maxVisibleRows
                      - static_cast<int>(rowScroll)};

        // If there are any elements offscreen below, scroll down 1 row.
        if (rowsBelow > 0) {
            rowScroll++;
        }
    }
}

} // namespace AUI
