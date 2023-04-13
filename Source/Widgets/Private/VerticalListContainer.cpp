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
, logicalScrollHeight{LOGICAL_DEFAULT_SCROLL_HEIGHT}
, scaledScrollHeight{ScalingHelpers::logicalToActual(logicalScrollHeight)}
, logicalGapSize{0}
, scaledGapSize{0}
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

EventResult VerticalListContainer::onMouseWheel(int amountScrolled)
{
    // Calc the content height by summing our element's heights and adding the
    // gaps.
    int contentHeight{0};
    for (const std::unique_ptr<Widget>& widget : elements) {
        contentHeight += scaledGapSize;
        contentHeight += widget->getScaledExtent().h;
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

    // Refresh the scroll height and gap size.
    scaledScrollHeight = ScalingHelpers::logicalToActual(logicalScrollHeight);
    scaledGapSize = ScalingHelpers::logicalToActual(logicalGapSize);

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

} // namespace AUI
