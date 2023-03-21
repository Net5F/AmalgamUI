#include "AUI/FlowContainer.h"
#include "AUI/ScalingHelpers.h"
#include "AUI/WidgetLocator.h"
#include "AUI/Internal/Log.h"
#include <cmath>
#include <algorithm>

namespace AUI
{
FlowContainer::FlowContainer(const SDL_Rect& inLogicalExtent,
                                             const std::string& inDebugName)
: Container(inLogicalExtent, inDebugName)
, logicalScrollHeight{LOGICAL_DEFAULT_SCROLL_HEIGHT}
, scaledScrollHeight{ScalingHelpers::logicalToActual(logicalScrollHeight)}
, logicalGapSize{0}
, scaledGapSize{0}
, scrollDistance{0}
{
}

void FlowContainer::setGapSize(int inLogicalGapSize)
{
    logicalGapSize = inLogicalGapSize;
    scaledGapSize = ScalingHelpers::logicalToActual(logicalGapSize);
}

void FlowContainer::setScrollHeight(int inLogicalScrollHeight)
{
    logicalScrollHeight = inLogicalScrollHeight;
    scaledScrollHeight = ScalingHelpers::logicalToActual(logicalScrollHeight);
}

EventResult FlowContainer::onMouseWheel(int amountScrolled)
{
    // Calc the content height by summing our element's heights and adding the 
    // gaps.
    int contentHeight{0};
    for (const std::unique_ptr<Widget>& widget : elements) {
        contentHeight += widget->getScaledExtent().h;
        contentHeight += scaledGapSize;
    }

    // Subtract 1 gap size, so we don't have a gap on the bottom.
    contentHeight -= scaledGapSize;

    // If the content isn't taller than this widget, don't scroll.
    if (contentHeight < scaledExtent.h) {
        return EventResult{.wasHandled{true}};
    }

    // Calc how far we would need to scroll for the last widget to be fully on
    // screen.
    int maxScrollDistance{contentHeight - scaledExtent.h};

    // Calc the updated scroll distance.
    scrollDistance -= (amountScrolled * scaledScrollHeight);

    // Clamp the scroll distance so we don't go too far.
    scrollDistance = std::clamp(scrollDistance, 0, maxScrollDistance);

    return EventResult{.wasHandled{true}};
}

void FlowContainer::updateLayout(const SDL_Rect& parentExtent,
                                 WidgetLocator* widgetLocator)
{
    // Run the normal layout step (will update us, but won't process any of
    // our elements).
    Widget::updateLayout(parentExtent, widgetLocator);

    // We'll use this to track how far the next element should be vertically 
    // offset.
    int nextYOffset{0};

    // Lay out our elements in a vertical flow.
    AUI_LOG_INFO("ScrollDistance: %d", scrollDistance);
    for (std::size_t i = 0; i < elements.size(); ++i) {
        // Figure out where the element should be placed.
        SDL_Rect elementExtent{elements[i]->getScaledExtent()};
        elementExtent.x += renderExtent.x;
        elementExtent.y += renderExtent.y;
        elementExtent.y += nextYOffset;
        elementExtent.y -= scrollDistance;

        // Update nextYOffset for the next element.
        nextYOffset += (elements[i]->getScaledExtent().h + scaledGapSize);

        // If the element is at least partially inside of this widget, make 
        // sure it's visible.
        if (SDL_HasIntersection(&elementExtent, &renderExtent)) {
            elements[i]->setIsVisible(true);
        }
        else {
            // The element is fully outside of this widget. Make it invisible 
            // (to ignore events) and continue to the next.
            elements[i]->setIsVisible(false);
            continue;
        }

        // Clip the element's extent to this widget's bounds and update the 
        // element.
        SDL_Rect clippedExtent{};
        SDL_IntersectRect(&elementExtent, &renderExtent, &clippedExtent);
        elements[i]->updateLayout(elementExtent, widgetLocator);
    }
}

bool FlowContainer::refreshScaling()
{
    // If actualScreenExtent was refreshed, do our specialized refreshing.
    if (Widget::refreshScaling()) {
        // Refresh the scroll height and gap size.
        scaledScrollHeight
            = ScalingHelpers::logicalToActual(logicalScrollHeight);
        scaledGapSize 
            = ScalingHelpers::logicalToActual(logicalGapSize);

        return true;
    }

    return false;
}

} // namespace AUI
