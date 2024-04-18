#include "AUI/ScrollArea.h"
#include "AUI/ScalingHelpers.h"
#include "AUI/WidgetLocator.h"
#include "AUI/Internal/Log.h"
#include <cmath>
#include <algorithm>

namespace AUI
{
ScrollArea::ScrollArea(const SDL_Rect& inLogicalExtent,
                                             const std::string& inDebugName)
: Widget(inLogicalExtent, inDebugName)
, content{nullptr}
, logicalScrollStepX{LOGICAL_DEFAULT_SCROLL_STEP_X}
, scaledScrollStepX{ScalingHelpers::logicalToActual(logicalScrollStepX)}
, logicalScrollStepY{LOGICAL_DEFAULT_SCROLL_STEP_Y}
, scaledScrollStepY{ScalingHelpers::logicalToActual(logicalScrollStepY)}
, scrollOrientation{Orientation::Vertical}
, scrollOrigin{ScrollOrigin::TopLeft}
, scrollDistanceX{0}
, scrollDistanceY{0}
{
}

void ScrollArea::setScrollStepX(int inLogicalScrollStepX)
{
    logicalScrollStepX = inLogicalScrollStepX;
    scaledScrollStepX = ScalingHelpers::logicalToActual(logicalScrollStepX);
}

void ScrollArea::setScrollStepY(int inLogicalScrollStepY)
{
    logicalScrollStepY = inLogicalScrollStepY;
    scaledScrollStepY = ScalingHelpers::logicalToActual(logicalScrollStepY);
}

void ScrollArea::setScrollOrientation(Orientation inScrollOrientation)
{
    scrollOrientation = inScrollOrientation;
}

void ScrollArea::setScrollOrigin(ScrollOrigin inScrollOrigin)
{
    scrollOrigin = inScrollOrigin;
}

int ScrollArea::getScrollDistanceX()
{
    return scrollDistanceX;
}

int ScrollArea::getScrollDistanceY()
{
    return scrollDistanceY;
}

EventResult ScrollArea::onMouseWheel(int amountScrolled)
{
    if (!content) {
        return EventResult{.wasHandled{false}};
    }

    if (scrollOrientation == Orientation::Horizontal) {
        handleMouseScrollHorizontal(amountScrolled);
    }
    else {
        handleMouseScrollVertical(amountScrolled);
    }

    return EventResult{.wasHandled{true}};
}

void ScrollArea::onTick(double timestepS)
{
    // Call the content's onTick().
    if (content) {
        content->onTick(timestepS);
    }
}

void ScrollArea::measure(const SDL_Rect& availableExtent)
{
    // Run the normal measure step (doesn't affect us since we don't use the 
    // children vector, but good to do in case of extension).
    Widget::measure(availableExtent);

    // Give our content widget a chance to update its logical extent.
    content->measure(logicalExtent);
}

void ScrollArea::arrange(const SDL_Point& startPosition,
                         const SDL_Rect& availableExtent,
                         WidgetLocator* widgetLocator)
{
    // Run the normal arrange step (will arrange us, but won't arrange our 
    // content).
    Widget::arrange(startPosition, availableExtent, widgetLocator);

    // If this widget is fully clipped, return early.
    if (SDL_RectEmpty(&clippedExtent)) {
        return;
    }

    // If our content changed and is now smaller than this widget, reset the
    // scroll distance.
    SDL_Rect contentExtent{calcContentExtent()};
    if (contentExtent.w < scaledExtent.w) {
        // Content is more narrow than widget, reset scroll distance.
        scrollDistanceY = 0;
    }
    else if (int maxScrollDistance{contentExtent.w - scaledExtent.w};
             scrollDistanceX > maxScrollDistance) {
        // Scroll distance is too far (element was erased), clamp it back.
        scrollDistanceX = std::clamp(scrollDistanceX, 0, maxScrollDistance);
    }

    if (contentExtent.h < scaledExtent.h) {
        // Content is shorter than widget, reset scroll distance.
        scrollDistanceY = 0;
    }
    else if (int maxScrollDistance{contentExtent.h - scaledExtent.h};
             scrollDistanceY > maxScrollDistance) {
        // Scroll distance is too far (element was erased), clamp it back.
        scrollDistanceY = std::clamp(scrollDistanceY, 0, maxScrollDistance);
    }

    // Refresh the scroll step.
    scaledScrollStepX = ScalingHelpers::logicalToActual(logicalScrollStepX);
    scaledScrollStepY = ScalingHelpers::logicalToActual(logicalScrollStepY);

    // Lay out our child content widget.
    if (content) {
        // Figure out where our content should be placed.
        SDL_Rect contentExtent{calcContentExtent()};
        contentExtent.x += fullExtent.x;
        contentExtent.x -= scrollDistanceX;
        contentExtent.y += fullExtent.y;
        if (scrollOrigin == ScrollOrigin::TopLeft) {
            contentExtent.y -= scrollDistanceY;
        }
        else if (scrollOrigin == ScrollOrigin::BottomLeft) {
            contentExtent.y += scrollDistanceY;
        }

        // Arrange the content, passing it the calculated start position.
        content->arrange({contentExtent.x, contentExtent.y}, clippedExtent,
                         widgetLocator);
    }
}

void ScrollArea::render(const SDL_Point& windowTopLeft)
{
    // If this widget is fully clipped, don't render it.
    if (SDL_RectEmpty(&clippedExtent)) {
        return;
    }

    // Render our content.
    content->render(windowTopLeft);
}

void ScrollArea::handleMouseScrollHorizontal(int amountScrolled)
{
    if (!content) {
        return;
    }

    // If the content isn't wider than this widget, don't scroll.
    int contentWidth{calcContentExtent().w};
    if (contentWidth < scaledExtent.w) {
        return;
    }

    // Calc how far we would need to scroll for the last widget to be fully on
    // screen.
    int maxScrollDistance{contentWidth - scaledExtent.w};

    // Calc the updated scroll distance.
    scrollDistanceX += (amountScrolled * scaledScrollStepX);

    // Clamp the scroll distance so we don't go too far.
    scrollDistanceX = std::clamp(scrollDistanceX, 0, maxScrollDistance);
}

void ScrollArea::handleMouseScrollVertical(int amountScrolled)
{
    if (!content) {
        return;
    }

    // If the content isn't taller than this widget, don't scroll.
    int contentHeight{calcContentExtent().h};
    if (contentHeight < scaledExtent.h) {
        return;
    }

    // Calc how far we would need to scroll for the last widget to be fully on
    // screen.
    int maxScrollDistance{contentHeight - scaledExtent.h};

    // Calc the updated scroll distance.
    if (scrollOrigin == ScrollOrigin::TopLeft) {
        scrollDistanceY -= (amountScrolled * scaledScrollStepY);
    }
    else if (scrollOrigin == ScrollOrigin::BottomLeft) {
        scrollDistanceY += (amountScrolled * scaledScrollStepY);
    }

    // Clamp the scroll distance so we don't go too far.
    scrollDistanceY = std::clamp(scrollDistanceY, 0, maxScrollDistance);
}

SDL_Rect ScrollArea::calcContentExtent()
{
    if (!content) {
        return {};
    }

    // Note: We scale manually since there's no guarantee updateLayout()
    //       has ran already to update the content's scaledExtent.
    return ScalingHelpers::logicalToActual(content->getLogicalExtent());
}

} // namespace AUI
