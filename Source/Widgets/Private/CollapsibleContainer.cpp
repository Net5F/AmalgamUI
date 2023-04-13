#include "AUI/CollapsibleContainer.h"
#include "AUI/ScalingHelpers.h"
#include "AUI/SDLHelpers.h"
#include "AUI/WidgetLocator.h"
#include "AUI/Internal/Log.h"
#include <cmath>
#include <algorithm>

namespace AUI
{
CollapsibleContainer::CollapsibleContainer(const SDL_Rect& inLogicalExtent,
                                           const std::string& inDebugName)
: Container(inLogicalExtent, inDebugName)
, expandedImage{{0, 0, logicalExtent.w, logicalExtent.h}}
, collapsedImage{{0, 0, logicalExtent.w, logicalExtent.h}}
, headerText{{0, 0, logicalExtent.w, logicalExtent.h}}
, headerLogicalExtent{inLogicalExtent}
, isCollapsed{true}
, logicalGapSize{0}
, scaledGapSize{0}
{
    // Add our children so they're included in rendering, etc.
    children.push_back(expandedImage);
    children.push_back(collapsedImage);
    children.push_back(headerText);

    // Default to the collapsed state.
    setIsCollapsed(true);

    // Note: Remember, this is a container so it also has elements.
}

void CollapsibleContainer::setIsCollapsed(bool inIsCollapsed)
{
    isCollapsed = inIsCollapsed;

    // Update our visible children and extent to match the new state.
    if (isCollapsed) {
        expandedImage.setIsVisible(false);
        collapsedImage.setIsVisible(true);

        logicalExtent = headerLogicalExtent;
    }
    else {
        expandedImage.setIsVisible(true);
        collapsedImage.setIsVisible(false);

        // Note: We need to immediately update our height so that our parent 
        //       can get an accurate height during the next updateLayout().
        logicalExtent.h = calcExpandedHeight();
    }

    // TODO: Invalidate layout
}

void CollapsibleContainer::setGapSize(int inLogicalGapSize)
{
    logicalGapSize = inLogicalGapSize;
    scaledGapSize = ScalingHelpers::logicalToActual(logicalGapSize);
}

void CollapsibleContainer::setLogicalExtent(const SDL_Rect& inLogicalExtent)
{
    Widget::setLogicalExtent(inLogicalExtent);
    headerLogicalExtent = inLogicalExtent;

    // TODO: Invalidate layout
}

EventResult CollapsibleContainer::onMouseDown(MouseButtonType buttonType,
                                   const SDL_Point& cursorPosition)
{
    // Calculate the header's extent (we can't just use clippedExtent because 
    // when we're expanded it accounts for the whole container).
    SDL_Rect headerExtent{
        ScalingHelpers::logicalToActual(headerLogicalExtent)};
    headerExtent.x = clippedExtent.x;
    headerExtent.y = clippedExtent.y;

    // If the header was clicked, toggle the collapsed state.
    if (SDLHelpers::pointInRect(cursorPosition, headerExtent)) {
        setIsCollapsed(!isCollapsed);

        return EventResult{.wasHandled{true}};
    }

    return EventResult{.wasHandled{false}};
}

EventResult CollapsibleContainer::onMouseDoubleClick(MouseButtonType buttonType,
                                          const SDL_Point& cursorPosition)
{
    // We treat additional clicks as regular MouseDown events.
    return onMouseDown(buttonType, cursorPosition);
}

void CollapsibleContainer::updateLayout(const SDL_Point& startPosition,
                                 const SDL_Rect& availableExtent,
                                 WidgetLocator* widgetLocator)
{
    // If we're in the expanded state, refresh our height.
    // Note: We need to do this in case any of our elements changed their 
    //       extents.
    if (!isCollapsed) {
        logicalExtent.h = calcExpandedHeight();
    }

    // Run the normal layout step (will update us and our children, but won't 
    // process any of our elements).
    Widget::updateLayout(startPosition, availableExtent, widgetLocator);

    // If this widget is fully clipped, return early.
    if (SDL_RectEmpty(&clippedExtent)) {
        return;
    }

    // If we're collapsed, return without updating our elements.
    if (isCollapsed) {
        // Make sure all of our elements are invisible.
        for (std::size_t i = 0; i < elements.size(); ++i) {
            elements[i]->setIsVisible(false);
        }
        return;
    }

    // We use this to track how far the next element should be vertically
    // positioned.
    int scaledHeaderHeight{
        ScalingHelpers::logicalToActual(headerLogicalExtent.h)};
    int nextYPosition{fullExtent.y + scaledHeaderHeight};

    // Lay out our elements.
    for (std::size_t i = 0; i < elements.size(); ++i) {
        // Make sure the element is visible.
        elements[i]->setIsVisible(true);

        // Update the element, passing it the calculated start position.
        elements[i]->updateLayout({fullExtent.x, nextYPosition},
                                  clippedExtent, widgetLocator);

        // Update nextYPosition for the next element.
        nextYPosition += (elements[i]->getScaledExtent().h + scaledGapSize);
    }
}

int CollapsibleContainer::calcExpandedHeight()
{
    // Sum our element y-offsets, element heights, and gaps.
    int newHeight{headerLogicalExtent.h};
    for (std::unique_ptr<Widget>& element : elements) {
        SDL_Rect elementLogicalExtent{element->getLogicalExtent()};
        newHeight += elementLogicalExtent.y + elementLogicalExtent.h;
        newHeight += logicalGapSize;
    }

    // Subtract 1 gap size, so we don't have a gap on the bottom.
    newHeight -= logicalGapSize;

    return newHeight;
}

} // namespace AUI
