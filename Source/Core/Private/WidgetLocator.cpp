#include "AUI/WidgetLocator.h"
#include "AUI/Widget.h"
#include "AUI/ScalingHelpers.h"
#include "AUI/SDLHelpers.h"
#include "AUI/Internal/Log.h"
#include "AUI/Internal/AUIAssert.h"
#include <cmath>
#include <algorithm>

namespace AUI
{

WidgetLocator::WidgetLocator(const SDL_Rect& inScreenExtent)
: cellWidth{ScalingHelpers::logicalToActual(LOGICAL_DEFAULT_CELL_WIDTH)}
, gridScreenExtent{}
, gridRelativeExtent{}
, gridCellExtent{}
{
    setExtent(inScreenExtent);
}

void WidgetLocator::addWidget(Widget* widget)
{
    // Note: This is relative to the parent window's extent (which matches
    //       this locator's extent).
    SDL_Rect widgetRelativeExtent{widget->getClippedExtent()};
    AUI_ASSERT(SDL_HasIntersection(&widgetRelativeExtent, &gridRelativeExtent),
               "Tried to add a widget that is outside this locator's bounds. "
               "Widget name: %s",
               widget->getDebugName().c_str());

    // Find the cells that the widget intersects.
    SDL_Rect widgetCellExtent{screenToCellExtent(widgetRelativeExtent)};

    // Add the widget to the map, or update it if it already exists for some
    // reason.
    widgetMap.insert_or_assign(widget, widgetCellExtent);

    // Add the widget to all the cells that it occupies.
    int xMax{widgetCellExtent.x + widgetCellExtent.w};
    int yMax{widgetCellExtent.y + widgetCellExtent.h};
    for (int x = widgetCellExtent.x; x < xMax; ++x) {
        for (int y = widgetCellExtent.y; y < yMax; ++y) {
            // Add the widget to this cell's widget array.
            std::size_t linearizedIndex{linearizeCellIndex(x, y)};
            std::vector<WidgetWeakRef>& widgetVec{widgetGrid[linearizedIndex]};

            widgetVec.emplace_back(*widget);
        }
    }
}

void WidgetLocator::removeWidget(Widget* widget)
{
    // If the given widget is in the widget map, remove it from the map and
    // from the grid.
    auto widgetIt{widgetMap.find(widget)};
    if (widgetIt != widgetMap.end()) {
        // Remove the widget from each cell that it's located in.
        clearWidgetLocation(widget, widgetIt->second);

        // Remove the widget from the map.
        widgetMap.erase(widgetIt);
    }
}

WidgetPath WidgetLocator::getPathUnderPoint(const SDL_Point& actualPoint)
{
    AUI_ASSERT(
        SDLHelpers::pointInRect(actualPoint, gridScreenExtent),
        "Tried to get path for a point that is outside this locator's bounds.");

    // Convert the actual screen-space point to a window-relative point.
    SDL_Point relativePoint{(actualPoint.x - gridScreenExtent.x),
                            (actualPoint.y - gridScreenExtent.y)};

    // Get the cell that contains the given point.
    float hitCellX{relativePoint.x / cellWidth};
    float hitCellY{relativePoint.y / cellWidth};
    std::size_t hitCellIndex{linearizeCellIndex(static_cast<int>(hitCellX),
                                                static_cast<int>(hitCellY))};
    std::vector<WidgetWeakRef>& widgetVec{widgetGrid[hitCellIndex]};

    // Iterate the widgets in the cell, adding them to the path if they're
    // still valid and contain the given point.
    WidgetPath returnPath;
    for (WidgetWeakRef& widgetWeakRef : widgetVec) {
        // If the widget isn't valid, skip it.
        if (!(widgetWeakRef.isValid())) {
            continue;
        }
        Widget& widget{widgetWeakRef.get()};

        // If the widget contains the point, add it to the path.
        if (widget.containsPoint(relativePoint)) {
            returnPath.push_back(widget);
        }
    }

    return returnPath;
}

void WidgetLocator::clear()
{
    widgetMap.clear();
    for (auto& widgetVector : widgetGrid) {
        widgetVector.clear();
    }
}

void WidgetLocator::setExtent(const SDL_Rect& inScreenExtent)
{
    gridScreenExtent = inScreenExtent;
    gridRelativeExtent = gridScreenExtent;
    gridRelativeExtent.x = 0;
    gridRelativeExtent.y = 0;

    // Set our grid size to match the extent.
    gridCellExtent = screenToCellExtent(gridRelativeExtent);

    // Resize the grid to fit our new extent.
    widgetGrid.resize(gridCellExtent.w * gridCellExtent.h);
}

void WidgetLocator::setCellWidth(float inCellWidth)
{
    cellWidth = inCellWidth;
}

SDL_Rect WidgetLocator::getGridCellExtent()
{
    return gridCellExtent;
}

void WidgetLocator::clearWidgetLocation(Widget* widget,
                                        const SDL_Rect& cellClearExtent)
{
    // Iterate through all the cells that the widget occupies.
    int xMax{cellClearExtent.x + cellClearExtent.w};
    int yMax{cellClearExtent.y + cellClearExtent.h};
    for (int x = cellClearExtent.x; x < xMax; ++x) {
        for (int y = cellClearExtent.y; y < yMax; ++y) {
            // Find the widget's location in this cell's widget vector.
            std::size_t linearizedIndex{linearizeCellIndex(x, y)};
            std::vector<WidgetWeakRef>& widgetVec{widgetGrid[linearizedIndex]};

            auto widgetEquals{[&widget](const WidgetWeakRef& other) {
                // Note: We don't need to check other's validity here since
                //       we're just comparing addresses.
                return (widget == &(other.get()));
            }};
            auto widgetIt{
                std::find_if(widgetVec.begin(), widgetVec.end(), widgetEquals)};

            // Remove the entity from this cell's widget vector.
            if (widgetIt != widgetVec.end()) {
                widgetVec.erase(widgetIt);
            }
        }
    }
}

SDL_Rect WidgetLocator::screenToCellExtent(const SDL_Rect& screenExtent)
{
    // Find the top left and bottom right cell coordinates for the screen
    // extent.
    SDL_Point topLeft{};
    topLeft.x = static_cast<int>(std::floor(screenExtent.x / cellWidth));
    topLeft.y = static_cast<int>(std::floor(screenExtent.y / cellWidth));

    SDL_Point bottomRight{};
    bottomRight.x = static_cast<int>(
        std::ceil((screenExtent.x + screenExtent.w) / cellWidth));
    bottomRight.y = static_cast<int>(
        std::ceil((screenExtent.y + screenExtent.h) / cellWidth));

    // Use the top left and bottom right to build the total cell extent.
    return {topLeft.x, topLeft.y, (bottomRight.x - topLeft.x),
            (bottomRight.y - topLeft.y)};
}

} // End namespace AUI
