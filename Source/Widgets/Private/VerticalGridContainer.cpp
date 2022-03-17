#include "AUI/VerticalGridContainer.h"
#include "AUI/ScalingHelpers.h"
#include "AUI/WidgetLocator.h"
#include "AUI/Internal/Log.h"
#include <cmath>

namespace AUI
{
VerticalGridContainer::VerticalGridContainer(Screen& screen,
                                             const SDL_Rect& inLogicalExtent,
                                             const std::string& inDebugName)
: Container(screen, inLogicalExtent, inDebugName)
, numColumns{1}
, logicalCellWidth{LOGICAL_DEFAULT_CELL_WIDTH}
, scaledCellWidth{ScalingHelpers::logicalToActual(logicalCellWidth)}
, logicalCellHeight{LOGICAL_DEFAULT_CELL_WIDTH}
, scaledCellHeight{ScalingHelpers::logicalToActual(logicalCellHeight)}
, rowScroll{0}
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

EventResult VerticalGridContainer::onMouseWheel(int amountScrolled)
{
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

void VerticalGridContainer::updateLayout(const SDL_Rect& parentExtent, WidgetLocator* widgetLocator)
{
    // Run the normal layout step (will update us, but won't process any of
    // our elements).
    Widget::updateLayout(parentExtent, widgetLocator);

    // Calc how many rows can fit onscreen at once.
    int maxVisibleRows{logicalExtent.h / logicalCellHeight};

    // Lay out our elements in a vertical grid.
    for (unsigned int i = 0; i < elements.size(); ++i) {
        // Get the cell coordinates for this element.
        unsigned int cellColumn{i % numColumns};
        unsigned int cellRow{i / numColumns};

        // If this element is offscreen, make it invisible (to ignore events)
        // and continue to the next.
        if ((cellRow < rowScroll)
            || (cellRow >= (maxVisibleRows + rowScroll))) {
            elements[i]->setIsVisible(false);
            continue;
        }
        else {
            // Element is on screen, make sure it's visible.
            elements[i]->setIsVisible(true);
        }

        // Get the offsets for the cell at the calculated coordinates.
        int cellXOffset{static_cast<int>(cellColumn * scaledCellWidth)};
        int cellYOffset{static_cast<int>(cellRow * scaledCellHeight)};

        // Move the Y offset based on our current scroll position.
        cellYOffset -= (rowScroll * scaledCellHeight);

        // Add this widget's offset to get our final offset.
        int finalX{renderExtent.x + cellXOffset};
        int finalY{renderExtent.y + cellYOffset};
        elements[i]->updateLayout({finalX, finalY
            , (finalX + scaledCellWidth), (finalY + scaledCellHeight)}, widgetLocator);
    }
}

bool VerticalGridContainer::refreshScaling()
{
    // If actualScreenExtent was refreshed, do our specialized refreshing.
    if (Widget::refreshScaling()) {
        // Refresh the cell width and height.
        scaledCellWidth = ScalingHelpers::logicalToActual(logicalCellWidth);
        scaledCellHeight = ScalingHelpers::logicalToActual(logicalCellHeight);

        return true;
    }

    return false;
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
        // Else if we've being asked to scroll down, calculate if there are
        // any rows below to scroll to.
        int rowsBelow{currentRows - maxVisibleRows - static_cast<int>(rowScroll)};

        // If there are any elements offscreen below, scroll down 1 row.
        if (rowsBelow > 0) {
            rowScroll++;
        }
    }

}

} // namespace AUI
