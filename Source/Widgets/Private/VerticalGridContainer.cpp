#include "AUI/VerticalGridContainer.h"
#include "AUI/ScalingHelpers.h"
#include "AUI/Internal/Log.h"
#include <cmath>

namespace AUI
{
VerticalGridContainer::VerticalGridContainer(Screen& screen,
                                             const SDL_Rect& inLogicalExtent,
                                             const std::string& inDebugName)
: Container(screen, inLogicalExtent, inDebugName)
, numColumns{1}
, logicalCellWidth{100}
, scaledCellWidth{ScalingHelpers::logicalToActual(logicalCellWidth)}
, logicalCellHeight{100}
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

Widget* VerticalGridContainer::onMouseWheel(SDL_MouseWheelEvent& event)
{
    // Get the mouse position since the event doesn't report it.
    SDL_Point mousePosition{};
    SDL_GetMouseState(&(mousePosition.x), &(mousePosition.y));

    // If the mouse is inside our extent.
    if (containsPoint(mousePosition)) {
        if (event.y > 0) {
            // Scroll up.
            scrollElements(true);
        }
        else if (event.y < 0) {
            // Scroll down.
            scrollElements(false);
        }

        return this;
    }

    return nullptr;
}

void VerticalGridContainer::updateLayout(const SDL_Rect& parentExtent)
{
    // Keep our extent up to date.
    refreshScaling();

    // Calculate our new extent to render at.
    renderExtent = scaledExtent;
    renderExtent.x += parentExtent.x;
    renderExtent.y += parentExtent.y;
    // TODO: Should we clip here to fit parentExtent?

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
            , (finalX + scaledCellWidth), (finalY + scaledCellHeight)});
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
    int currentRows
        = std::ceil(elements.size() / static_cast<float>(numColumns));

    // Calc how many rows can fit onscreen at once.
    int maxVisibleRows = logicalExtent.h / logicalCellHeight;

    // If we're being asked to scroll up and we've scrolled down previously.
    if (scrollUp && (rowScroll > 0)) {
        // Scroll up 1 row.
        rowScroll--;
    }
    else if (!scrollUp) {
        // Else if we've being asked to scroll down, calculate if there are
        // any rows below to scroll to.
        int rowsBelow = currentRows - maxVisibleRows - rowScroll;

        // If there are any elements offscreen below, scroll down 1 row.
        if (rowsBelow > 0) {
            rowScroll++;
        }
    }
}

} // namespace AUI
