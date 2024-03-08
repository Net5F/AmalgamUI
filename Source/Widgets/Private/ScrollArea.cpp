#include "AUI/VerticalListContainer.h"
#include "AUI/ScalingHelpers.h"
#include "AUI/WidgetLocator.h"
#include "AUI/Internal/Log.h"
#include <cmath>
#include <algorithm>

namespace AUI
{
VerticalListContainer::VerticalListContainer(const SDL_Rect& inLogicalExtent,
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

void VerticalListContainer::setGapSize(int inLogicalGapSize)
{
    logicalGapSize = inLogicalGapSize;
    scaledGapSize = ScalingHelpers::logicalToActual(logicalGapSize);
}

void VerticalListContainer::setScrollHeight(int inLogicalScrollHeight)
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

EventResult VerticalListContainer::onMouseWheel(int amountScrolled)
{
    // If the content isn't taller than this widget, don't scroll.
    int contentHeight{calcContentHeight()};
    if (contentHeight < scaledExtent.h) {
        return EventResult{.wasHandled{true}};
    }

    // Calc how far we would need to scroll for the last widget to be fully on
    // screen.
    int maxScrollDistance{contentHeight - scaledExtent.h};

    // Calc the updated scroll distance.
    if (flowDirection == FlowDirection::TopToBottom) {
        scrollDistance -= (amountScrolled * scaledScrollHeight);
    }
    else if (flowDirection == FlowDirection::BottomToTop) {
        scrollDistance += (amountScrolled * scaledScrollHeight);
    }

    // Clamp the scroll distance so we don't go too far.
    scrollDistance = std::clamp(scrollDistance, 0, maxScrollDistance);

    return EventResult{.wasHandled{true}};
}

void VerticalListContainer::updateLayout(const SDL_Point& startPosition,
                                         const SDL_Rect& availableExtent,
                                         WidgetLocator* widgetLocator)
{
    // Run the normal layout step (will update us, but won't process any of
    // our elements).
    Widget::updateLayout(startPosition, availableExtent, widgetLocator);

    // If this widget is fully clipped, return early.
    if (SDL_RectEmpty(&clippedExtent)) {
        return;
    }

    // If our content changed and is now shorter than this widget, reset the
    // scroll distance.
    int contentHeight{calcContentHeight()};
    if (contentHeight < scaledExtent.h) {
        // Content is shorter than widget, reset scroll distance.
        scrollDistance = 0;
    }
    else if (int maxScrollDistance{contentHeight - scaledExtent.h};
             scrollDistance > maxScrollDistance) {
        // Scroll distance is too far (element was erased), clamp it back.
        scrollDistance = std::clamp(scrollDistance, 0, maxScrollDistance);
    }

    // Refresh the scroll height and gap size.
    scaledScrollHeight = ScalingHelpers::logicalToActual(logicalScrollHeight);
    scaledGapSize = ScalingHelpers::logicalToActual(logicalGapSize);

    // Lay out our elements in the appropriate direction.
    if (flowDirection == FlowDirection::TopToBottom) {
        arrangeElementsTopToBottom(widgetLocator);
    }
    else {
        arrangeElementsBottomToTop(widgetLocator);
    }
}

int VerticalListContainer::calcContentHeight()
{
    // TODO: We've had to do some hokey stuff in CollapsibleContainer and Text
    //       to get this to work. If we hit a third special case, we should just
    //       refactor updateLayout() into measure()/arrange().

    // Calc the content height by summing our element's heights and adding the
    // gaps.
    int contentHeight{0};
    for (const std::unique_ptr<Widget>& widget : elements) {
        contentHeight += scaledGapSize;
        // Note: We scale manually since there's no guarantee updateLayout()
        //       has ran already to update this widget's scaledExtent.
        contentHeight
            += ScalingHelpers::logicalToActual(widget->getLogicalExtent().h);
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
    int nextYOffset{0};

    // Lay out our elements in a vertical flow.
    for (std::size_t i = 0; i < elements.size(); ++i) {
        // Figure out where the element should be placed.
        SDL_Rect elementExtent{elements[i]->getScaledExtent()};
        elementExtent.x += fullExtent.x;
        elementExtent.y += fullExtent.y;
        elementExtent.y += nextYOffset;
        elementExtent.y -= scrollDistance;

        // Update the element, passing it the calculated start position.
        elements[i]->updateLayout({elementExtent.x, elementExtent.y},
                                  clippedExtent, widgetLocator);

        // Update nextYOffset for the next element.
        nextYOffset += (elements[i]->getScaledExtent().h + scaledGapSize);
    }
}

void VerticalListContainer::arrangeElementsBottomToTop(
    WidgetLocator* widgetLocator)
{
    // We'll use this to track how far the next element should be vertically
    // offset.
    int nextYOffset{0};

    // Lay out our elements in a vertical flow.
    for (std::size_t i = 0; i < elements.size(); ++i) {
        // Update nextYOffset for this element.
        nextYOffset += elements[i]->getScaledExtent().h;

        // Figure out where the element should be placed.
        SDL_Rect elementExtent{elements[i]->getScaledExtent()};
        elementExtent.x += fullExtent.x;
        elementExtent.y += (fullExtent.y + fullExtent.h);
        elementExtent.y -= nextYOffset;
        elementExtent.y += scrollDistance;

        // Update the element, passing it the calculated start position.
        elements[i]->updateLayout({elementExtent.x, elementExtent.y},
                                  clippedExtent, widgetLocator);

        // Add a gap for the next element.
        nextYOffset += scaledGapSize;
    }
}

} // namespace AUI
