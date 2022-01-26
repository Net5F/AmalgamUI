#include "AUI/Widget.h"
#include "AUI/Screen.h"
#include "AUI/Image.h"
#include "AUI/Core.h"
#include "AUI/ScalingHelpers.h"
#include "AUI/Internal/Ignore.h"

namespace AUI
{
Widget::Widget(Screen& inScreen, const SDL_Rect& inLogicalExtent,
                     const std::string& inDebugName)
: screen(inScreen)
, debugName{inDebugName}
, logicalExtent{inLogicalExtent}
, scaledExtent{ScalingHelpers::logicalToActual(logicalExtent)}
, lastRenderedExtent{}
, lastUsedScreenSize{Core::getActualScreenSize()}
, isVisible{true}
, listeningEventTypes{}
{
    Core::incWidgetCount();
}

Widget::~Widget()
{
    // Unregister from any events that we were listening for.
    for (unsigned int i = 0; i < InternalEvent::NUM_TYPES; ++i) {
        if (listeningEventTypes[i]) {
            screen.unregisterListener(static_cast<InternalEvent::Type>(i),
                                      this);
        }
    }

    Core::decWidgetCount();
}

bool Widget::containsPoint(const SDL_Point& actualPoint)
{
    // Test if the point is within all 4 sides of our extent.
    if ((actualPoint.x > lastRenderedExtent.x)
        && (actualPoint.x < (lastRenderedExtent.x + lastRenderedExtent.w))
        && (actualPoint.y > lastRenderedExtent.y)
        && (actualPoint.y < (lastRenderedExtent.y + lastRenderedExtent.h))) {
        return true;
    }
    else {
        return false;
    }
}

bool Widget::containsExtent(const SDL_Rect& actualExtent)
{
    // Test if 2 diagonal corners of the extent are within our extent.
    if (containsPoint({actualExtent.x, actualExtent.y})
        && containsPoint({(actualExtent.x + actualExtent.w),
                          (actualExtent.y + actualExtent.h)})) {
        return true;
    }
    else {
        return false;
    }
}

void Widget::setLogicalExtent(const SDL_Rect& inLogicalExtent)
{
    // Set our logical screen extent.
    logicalExtent = inLogicalExtent;

    // Re-calculate our scaled screen extent.
    scaledExtent = ScalingHelpers::logicalToActual(logicalExtent);
}

SDL_Rect Widget::getLogicalExtent()
{
    return logicalExtent;
}

SDL_Rect Widget::getScaledExtent()
{
    return scaledExtent;
}

SDL_Rect Widget::getLastRenderedExtent()
{
    return lastRenderedExtent;
}

const std::string& Widget::getDebugName()
{
    return debugName;
}

void Widget::setIsVisible(bool inIsVisible)
{
    isVisible = inIsVisible;
}

bool Widget::getIsVisible()
{
    return isVisible;
}

void Widget::render(const SDL_Point& parentOffset)
{
    ignore(parentOffset);
    AUI_LOG_FATAL("Base class render called. Please override render() "
                  "in your derived class.");
}

bool Widget::onMouseButtonDown(SDL_MouseButtonEvent& event)
{
    ignore(event);
    AUI_LOG_FATAL("Base class callback called. Please override"
                  " onMouseButtonDown() in your derived class.");

    return false;
}

bool Widget::onMouseButtonUp(SDL_MouseButtonEvent& event)
{
    ignore(event);
    AUI_LOG_FATAL("Base class callback called. Please override"
                  " onMouseButtonUp() in your derived class.");

    return false;
}

bool Widget::onMouseWheel(SDL_MouseWheelEvent& event)
{
    ignore(event);
    AUI_LOG_FATAL("Base class callback called. Please override onMouseWheel() "
                  "in your derived class.");

    return false;
}

void Widget::onMouseMove(SDL_MouseMotionEvent& event)
{
    ignore(event);
    AUI_LOG_FATAL("Base class callback called. Please override onMouseMove() "
                  "in your derived class.");
}

bool Widget::onKeyDown(SDL_KeyboardEvent& event)
{
    ignore(event);
    AUI_LOG_FATAL("Base class callback called. Please override onKeyDown() "
                  "in your derived class.");

    return false;
}

bool Widget::onTextInput(SDL_TextInputEvent& event)
{
    ignore(event);
    AUI_LOG_FATAL("Base class callback called. Please override onTextInput() "
                  "in your derived class.");

    return false;
}

void Widget::onTick(double timestepS)
{
    ignore(timestepS);
    AUI_LOG_FATAL("Base class callback called. Please override onTick() "
                  "in your derived class.");
}

void Widget::registerListener(InternalEvent::Type eventType)
{
    // Register with the screen as a listener for the given type.
    screen.registerListener(eventType, this);

    // Track that we're now listening to the given type.
    listeningEventTypes[static_cast<unsigned int>(eventType)] = true;
}

void Widget::unregisterListener(InternalEvent::Type eventType)
{
    // Unregister with the screen as a listener for the given type.
    // Note: Errors if we aren't listening to the given type.
    screen.unregisterListener(eventType, this);

    // Track that we're no longer listening to the given type.
    listeningEventTypes[eventType] = false;
}

bool Widget::refreshScaling()
{
    // If the screen size has changed.
    if (lastUsedScreenSize != Core::getActualScreenSize()) {
        // Re-calculate our scaled extent.
        scaledExtent = ScalingHelpers::logicalToActual(logicalExtent);

        // Save the new size.
        lastUsedScreenSize = Core::getActualScreenSize();

        return true;
    }

    return false;
}

SDL_Rect Widget::calcClippedExtent(const SDL_Rect& sourceExtent,
                                   const SDL_Rect& clipExtent)
{
    // If the clipping extent has no width or height, don't clip.
    if ((clipExtent.w == 0) || (clipExtent.h == 0)) {
        AUI_LOG_INFO("Tried to clip using a clipExtent with either no width or"
                     " no height.");
        return sourceExtent;
    }

    // If we're beyond the left bound of clipExtent, set it as our x.
    SDL_Rect clippedExtent{sourceExtent};
    int leftDiff = clipExtent.x - sourceExtent.x;
    if (leftDiff > 0) {
        clippedExtent.x = clipExtent.x;
    }

    // If we're beyond the right bound of clipExtent, decrease width to fit.
    int rightDiff
        = (clippedExtent.x + clippedExtent.w) - (clipExtent.x + clipExtent.w);
    if (rightDiff > 0) {
        clippedExtent.w -= rightDiff;
    }

    // If we're beyond the top bound of clipExtent, set it as our y.
    int topDiff = clipExtent.y - sourceExtent.y;
    if (topDiff > 0) {
        clippedExtent.y = clipExtent.y;
    }

    // If we're beyond the bottom bound of clipExtent, decrease height to fit.
    int bottomDiff
        = (clippedExtent.y + clippedExtent.h) - (clipExtent.y + clipExtent.h);
    if (bottomDiff > 0) {
        clippedExtent.h -= bottomDiff;
    }

    return clippedExtent;
}

} // namespace AUI
