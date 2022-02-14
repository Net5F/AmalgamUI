#include "AUI/Widget.h"
#include "AUI/Screen.h"
#include "AUI/Image.h"
#include "AUI/Core.h"
#include "AUI/ScalingHelpers.h"
#include "AUI/WidgetWeakRef.h"
#include "AUI/Internal/Ignore.h"
#include "AUI/Internal/Log.h"
#include "AUI/Internal/Assert.h"
#include <algorithm>

namespace AUI
{
Widget::Widget(Screen& inScreen, const SDL_Rect& inLogicalExtent,
                     const std::string& inDebugName)
: screen(inScreen)
, debugName{inDebugName}
, logicalExtent{inLogicalExtent}
, scaledExtent{ScalingHelpers::logicalToActual(logicalExtent)}
, renderExtent{}
, lastUsedScreenSize{Core::getActualScreenSize()}
, isVisible{true}
{
    Core::incWidgetCount();
}

Widget::~Widget()
{
    for (WidgetWeakRef* ref : trackedRefs)
    {
        ref->invalidate();
    }

    Core::decWidgetCount();
}

bool Widget::containsPoint(const SDL_Point& actualPoint)
{
    // Test if the point is within all 4 sides of our extent.
    if ((actualPoint.x > renderExtent.x)
        && (actualPoint.x < (renderExtent.x + renderExtent.w))
        && (actualPoint.y > renderExtent.y)
        && (actualPoint.y < (renderExtent.y + renderExtent.h))) {
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

SDL_Rect Widget::getRenderExtent()
{
    return renderExtent;
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

Widget* Widget::handleOSEvent(SDL_Event& event)
{
    // Propagate the event through our visible children.
    for (auto it = children.rbegin(); it != children.rend(); ++it) {
        // If the child isn't visible, skip it.
        Widget& child{it->get()};
        if (!(child.getIsVisible())) {
            continue;
        }

        // If the child consumed the event, return early.
        Widget* consumer{child.handleOSEvent(event)};
        if (consumer != nullptr) {
            return consumer;
        }
    }

    // None of our children handled the event. Try to handle it ourselves.
    switch (event.type) {
        case SDL_MOUSEBUTTONDOWN: {
            return onMouseButtonDown(event.button);
        }
        case SDL_MOUSEBUTTONUP: {
            return onMouseButtonUp(event.button);
        }
        case SDL_MOUSEMOTION: {
            return onMouseMove(event.motion);
        }
        case SDL_MOUSEWHEEL: {
            return onMouseWheel(event.wheel);
        }
        case SDL_KEYDOWN: {
            return onKeyDown(event.key);
        }
        case SDL_TEXTINPUT: {
            return onTextInput(event.text);
        }
        default:
            break;
    }

    return nullptr;
}

Widget* Widget::onMouseButtonDown(SDL_MouseButtonEvent& event)
{
    ignore(event);
    return nullptr;
}

Widget* Widget::onMouseButtonUp(SDL_MouseButtonEvent& event)
{
    ignore(event);
    return nullptr;
}

Widget* Widget::onMouseWheel(SDL_MouseWheelEvent& event)
{
    ignore(event);
    return nullptr;
}

Widget* Widget::onMouseMove(SDL_MouseMotionEvent& event)
{
    ignore(event);
    return nullptr;
}

Widget* Widget::onKeyDown(SDL_KeyboardEvent& event)
{
    ignore(event);
    return nullptr;
}

Widget* Widget::onTextInput(SDL_TextInputEvent& event)
{
    ignore(event);
    return nullptr;
}

void Widget::onTick(double timestepS)
{
    ignore(timestepS);
}

void Widget::updateLayout(const SDL_Rect& parentExtent)
{
    // Keep our extent up to date.
    refreshScaling();

    // Calculate our new extent to render at.
    renderExtent = scaledExtent;
    renderExtent.x += parentExtent.x;
    renderExtent.y += parentExtent.y;
    // TODO: Should we clip here to fit parentExtent?

    // Update our visible children's layout.
    for (Widget& child : children)
    {
        if (child.getIsVisible()) {
            child.updateLayout(renderExtent);
        }
    }
}

void Widget::render()
{
    // Render all visible children.
    for (Widget& child : children)
    {
        if (child.getIsVisible()) {
            child.render();
        }
    }
}

void Widget::trackRef(WidgetWeakRef* ref)
{
    trackedRefs.push_back(ref);
}

void Widget::untrackRef(WidgetWeakRef* ref)
{
    auto it{std::find(trackedRefs.begin(), trackedRefs.end(), ref)};
    AUI_ASSERT(it != trackedRefs.end(), "Tried to untrack ref that didn't exist in list.");
    trackedRefs.erase(it);
}

std::size_t Widget::getRefCount()
{
    return trackedRefs.size();
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
