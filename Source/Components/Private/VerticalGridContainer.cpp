#include "AUI/VerticalGridContainer.h"
#include "AUI/ScalingHelpers.h"
#include "AUI/Internal/Log.h"
#include <cmath>

namespace AUI {

VerticalGridContainer::VerticalGridContainer(Screen& screen, const char* key, const SDL_Rect& logicalExtent)
: Container(screen, key, logicalExtent)
, numColumns{1}
, logicalCellWidth{100}
, scaledCellWidth{ScalingHelpers::logicalToActual(logicalCellWidth)}
, logicalCellHeight{100}
, scaledCellHeight{ScalingHelpers::logicalToActual(logicalCellHeight)}
, rowScroll{0}
{
    // Register for the events that we want to listen for.
    registerListener(InternalEvent::MouseWheel);
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

bool VerticalGridContainer::onMouseWheel(SDL_MouseWheelEvent& event)
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

        return true;
    }

    return false;
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

    // Calc how many rows can fit onscreen at once.
    int maxVisibleRows = logicalExtent.h / logicalCellHeight;

    // Render our elements in a vertical grid.
    for (unsigned int i = 0; i < elements.size(); ++i) {
        // Get the cell coordinates for this element.
        unsigned int cellColumn = i % numColumns;
        unsigned int cellRow = i / numColumns;

        // If this element is offscreen, make it invisible (to ignore events)
        // and continue to the next.
        if ((cellRow < rowScroll) || (cellRow >= (maxVisibleRows + rowScroll))) {
            elements[i]->setIsVisible(false);
            continue;
        }
        else {
            // Element is on screen, make sure it's visible.
            elements[i]->setIsVisible(true);
        }

        // Get the offsets for the cell at the calculated coordinates.
        int cellXOffset = cellColumn * scaledCellWidth;
        int cellYOffset = cellRow * scaledCellHeight;

        // Move the Y offset based on our current scroll position.
        cellYOffset -= (rowScroll * scaledCellHeight);

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

void VerticalGridContainer::scrollElements(bool scrollUp)
{
    // Calc how many rows are currently present.
    int currentRows = std::ceil(elements.size() / static_cast<float>(numColumns));

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
