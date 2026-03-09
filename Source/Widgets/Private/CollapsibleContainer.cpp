#include "AUI/CollapsibleContainer.h"
#include "AUI/ScalingHelpers.h"
#include "AUI/WidgetLocator.h"
#include "AUI/Internal/Log.h"
#include "AUI/SDLHelpers.h"
#include <cmath>
#include <algorithm>
#include <SDL3/SDL_rect.h>

namespace AUI
{
CollapsibleContainer::CollapsibleContainer(const SDL_FRect& inLogicalExtent,
                                           const std::string& inDebugName)
: Container(inLogicalExtent, inDebugName)
, expandedImage{{0, 0, logicalExtent.w, logicalExtent.h}}
, collapsedImage{{0, 0, logicalExtent.w, logicalExtent.h}}
, headerText{{0, 0, logicalExtent.w, logicalExtent.h}}
, headerLogicalExtent{inLogicalExtent}
, clickRegionLogicalExtent{0, 0, logicalExtent.w, logicalExtent.h}
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

void CollapsibleContainer::setClickRegionLogicalExtent(
    const SDL_FRect& inLogicalExtent)
{
    clickRegionLogicalExtent = inLogicalExtent;
}

void CollapsibleContainer::setIsCollapsed(bool inIsCollapsed)
{
    isCollapsed = inIsCollapsed;

    // Update our visible children to match the new state.
    if (isCollapsed) {
        expandedImage.setIsVisible(false);
        collapsedImage.setIsVisible(true);
    }
    else {
        expandedImage.setIsVisible(true);
        collapsedImage.setIsVisible(false);
    }

    // TODO: Invalidate layout
}

void CollapsibleContainer::setGapSize(float inLogicalGapSize)
{
    logicalGapSize = inLogicalGapSize;
    scaledGapSize = ScalingHelpers::logicalToActual(logicalGapSize);
}

SDL_FRect CollapsibleContainer::getHeaderExtent()
{
    // Calculate the header's extent (we can't just use clippedExtent because
    // when we're expanded it accounts for the whole container).
    SDL_FRect headerExtent{
        AUI::ScalingHelpers::logicalToActual(headerLogicalExtent)};
    headerExtent.x = clippedExtent.x;
    headerExtent.y = clippedExtent.y;

    return headerExtent;
}

SDL_FRect CollapsibleContainer::getClickRegionExtent()
{
    // Calculate the region's extent.
    SDL_FRect clickRegionExtent{
        AUI::ScalingHelpers::logicalToActual(clickRegionLogicalExtent)};
    clickRegionExtent.x = clippedExtent.x;
    clickRegionExtent.y = clippedExtent.y;

    return clickRegionExtent;
}

void CollapsibleContainer::setLogicalExtent(const SDL_FRect& inLogicalExtent)
{
    Widget::setLogicalExtent(inLogicalExtent);
    headerLogicalExtent = inLogicalExtent;

    // TODO: Invalidate layout
}

EventResult CollapsibleContainer::onMouseDown(MouseButtonType,
                                              const SDL_FPoint& cursorPosition)
{
    // If the click region was clicked, toggle the collapsed state.
    SDL_FRect clickRegionExtent{getClickRegionExtent()};
    if (SDL_PointInRectFloat(&cursorPosition, &clickRegionExtent)) {
        setIsCollapsed(!isCollapsed);

        return EventResult{.wasHandled{true}};
    }

    return EventResult{.wasHandled{false}};
}

EventResult
    CollapsibleContainer::onMouseDoubleClick(MouseButtonType buttonType,
                                             const SDL_FPoint& cursorPosition)
{
    // We treat additional clicks as regular MouseDown events.
    return onMouseDown(buttonType, cursorPosition);
}

void CollapsibleContainer::measure(const SDL_FRect& availableExtent)
{
    // Run the normal measure step (sets our scaledExtent).
    Widget::measure(availableExtent);

    // If we're expanded, set our height to fit our elements.
    if (!isCollapsed) {
        // Measure our elements, giving them infinite available height.
        SDL_FRect elementAvailableExtent{0, 0, logicalExtent.w, -1};
        for (auto& element : elements) {
            element->measure(elementAvailableExtent);
        }

        // Calc our new height based on our elements.
        logicalExtent.h = calcExpandedHeight();
    }
    else {
        // We're collapsed. Set our height to the header's height.
        logicalExtent.h = headerLogicalExtent.h;
    }
}

void CollapsibleContainer::arrange(const SDL_FPoint& startPosition,
                                   const SDL_FRect& availableExtent,
                                   WidgetLocator* widgetLocator)
{
    // Run the normal arrange step (will arrange us and our children, but won't
    // arrange any of our elements).
    Widget::arrange(startPosition, availableExtent, widgetLocator);

    // If this widget is fully clipped, return early.
    if (SDL_RectEmptyFloat(&clippedExtent)) {
        return;
    }

    // If we're collapsed, return without updating our elements.
    if (isCollapsed) {
        // Make sure all of our elements are invisible.
        for (auto& element : elements) {
            element->setIsVisible(false);
        }
        return;
    }

    // We use this to track how far the next element should be vertically
    // positioned.
    float scaledHeaderHeight{
        ScalingHelpers::logicalToActual(headerLogicalExtent.h)};
    float nextYPosition{fullExtent.y + scaledHeaderHeight};

    // Lay out our elements.
    for (auto& element : elements) {
        // Make sure the element is visible.
        element->setIsVisible(true);

        // Arrange the element, passing it the calculated start position.
        element->arrange({fullExtent.x, nextYPosition}, clippedExtent,
                         widgetLocator);

        // Update nextYPosition for the next element.
        nextYPosition += (element->getScaledExtent().h + scaledGapSize);
    }
}

float CollapsibleContainer::calcExpandedHeight()
{
    // Sum our element y-offsets, element heights, and gaps.
    float newHeight{headerLogicalExtent.h};
    for (std::unique_ptr<Widget>& element : elements) {
        SDL_FRect elementLogicalExtent{element->getLogicalExtent()};
        newHeight += elementLogicalExtent.y + elementLogicalExtent.h;
        newHeight += logicalGapSize;
    }

    // Subtract 1 gap size, so we don't have a gap on the bottom.
    newHeight -= logicalGapSize;

    return newHeight;
}

} // namespace AUI
