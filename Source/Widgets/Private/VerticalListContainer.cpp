#include "AUI/VerticalListContainer.h"
#include "AUI/ScalingHelpers.h"
#include "AUI/WidgetLocator.h"
#include "AUI/Internal/Log.h"
#include <cmath>
#include "AUI/SDLHelpers.h"
#include <algorithm>

namespace AUI
{
VerticalListContainer::VerticalListContainer(const SDL_FRect& inLogicalExtent,
                                             const std::string& inDebugName)
: Container(inLogicalExtent, inDebugName)
, logicalScrollHeight{LOGICAL_DEFAULT_SCROLL_DISTANCE}
, scaledScrollHeight{ScalingHelpers::logicalToActual(logicalScrollHeight)}
, logicalGapSize{0}
, scaledGapSize{0}
, flowDirection{FlowDirection::TopToBottom}
, scrollDistance{0}
{
}

void VerticalListContainer::setGapSize(float inLogicalGapSize)
{
    logicalGapSize = inLogicalGapSize;
    scaledGapSize = ScalingHelpers::logicalToActual(logicalGapSize);
}

void VerticalListContainer::setScrollHeight(float inLogicalScrollHeight)
{
    logicalScrollHeight = inLogicalScrollHeight;
    scaledScrollHeight = ScalingHelpers::logicalToActual(logicalScrollHeight);
}

void VerticalListContainer::setFlowDirection(FlowDirection inFlowDirection)
{
    flowDirection = inFlowDirection;

    // Reset the scroll distance since it's going in the other direction now.
    scrollDistance = 0;
}

EventResult VerticalListContainer::onMouseWheel(float amountScrolled)
{
    // If the content isn't taller than this widget, don't scroll.
    // Note: We aren't worried about this function being called before measure()
    //       is able to run, because if measure/arrange/render hasn't ran, the 
    //       user can't see any of this anyway.
    float contentHeight{calcContentHeight()};
    if (contentHeight < scaledExtent.h) {
        return EventResult{.wasHandled{true}};
    }

    // Calc how far we would need to scroll for the last widget to be fully on
    // screen.
    float maxScrollDistance{contentHeight - scaledExtent.h};

    // Calc the updated scroll distance.
    if (flowDirection == FlowDirection::TopToBottom) {
        scrollDistance -= (amountScrolled * scaledScrollHeight);
    }
    else if (flowDirection == FlowDirection::BottomToTop) {
        scrollDistance += (amountScrolled * scaledScrollHeight);
    }

    // Clamp the scroll distance so we don't go too far.
    scrollDistance = std::clamp(scrollDistance, 0.f, maxScrollDistance);

    return EventResult{.wasHandled{true}};
}

void VerticalListContainer::measure(const SDL_FRect& availableExtent)
{
    // Run the normal measure step (sets our scaledExtent).
    Widget::measure(availableExtent);

    // Give our elements a chance to update their logical extent.
    for (auto& element : elements) {
        // Note: We measure/arrange all elements, even if they're invisible,
        //       so we can get the rest of the elements offsets correct.
        element->measure(logicalExtent);
    }

    // Refresh the scroll height and gap size.
    scaledScrollHeight = ScalingHelpers::logicalToActual(logicalScrollHeight);
    scaledGapSize = ScalingHelpers::logicalToActual(logicalGapSize);

    // If our content changed and is now shorter than this widget, reset the
    // scroll distance.
    float contentHeight{calcContentHeight()};
    if (contentHeight < scaledExtent.h) {
        // Content is shorter than widget, reset scroll distance.
        scrollDistance = 0;
    }
    else if (float maxScrollDistance{contentHeight - scaledExtent.h};
             scrollDistance > maxScrollDistance) {
        // Scroll distance is too far (element was erased), clamp it back.
        scrollDistance = std::clamp(scrollDistance, 0.f, maxScrollDistance);
    }
}

void VerticalListContainer::arrange(const SDL_FPoint& startPosition,
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

    // Lay out our elements in the appropriate direction.
    if (flowDirection == FlowDirection::TopToBottom) {
        arrangeElementsTopToBottom(widgetLocator);
    }
    else {
        arrangeElementsBottomToTop(widgetLocator);
    }
}

float VerticalListContainer::calcContentHeight()
{
    // Calc the content height by summing our element's heights and adding the
    // gaps.
    float contentHeight{0};
    for (const std::unique_ptr<Widget>& widget : elements) {
        contentHeight += widget->getScaledExtent().h;
        contentHeight += scaledGapSize;
    }

    // Subtract 1 gap size, so we don't have a gap on the bottom.
    contentHeight -= scaledGapSize;

    return contentHeight;
}

void VerticalListContainer::arrangeElementsTopToBottom(
    WidgetLocator* widgetLocator)
{
    // We'll use this to track how far the next element should be vertically
    // offset.
    float nextYOffset{0};

    // Lay out our elements in a vertical flow.
    for (std::size_t i = 0; i < elements.size(); ++i) {
        // Figure out where the element should be placed.
        SDL_FRect elementExtent{elements[i]->getScaledExtent()};
        elementExtent.x += fullExtent.x;
        elementExtent.y += fullExtent.y;
        elementExtent.y += nextYOffset;
        elementExtent.y -= scrollDistance;

        // Arrange the element, passing it the calculated start position.
        elements[i]->arrange({elementExtent.x, elementExtent.y}, clippedExtent,
                             widgetLocator);

        // Update nextYOffset for the next element.
        nextYOffset += (elements[i]->getScaledExtent().h + scaledGapSize);
    }
}

void VerticalListContainer::arrangeElementsBottomToTop(
    WidgetLocator* widgetLocator)
{
    // We'll use this to track how far the next element should be vertically
    // offset.
    float nextYOffset{0};

    // Lay out our elements in a vertical flow.
    for (std::size_t i = 0; i < elements.size(); ++i) {
        // Update nextYOffset for this element.
        nextYOffset += elements[i]->getScaledExtent().h;

        // Figure out where the element should be placed.
        SDL_FRect elementExtent{elements[i]->getScaledExtent()};
        elementExtent.x += fullExtent.x;
        elementExtent.y += (fullExtent.y + fullExtent.h);
        elementExtent.y -= nextYOffset;
        elementExtent.y += scrollDistance;

        // Arrange the element, passing it the calculated start position.
        elements[i]->arrange({elementExtent.x, elementExtent.y}, clippedExtent,
                             widgetLocator);

        // Add a gap for the next element.
        nextYOffset += scaledGapSize;
    }
}

} // namespace AUI
