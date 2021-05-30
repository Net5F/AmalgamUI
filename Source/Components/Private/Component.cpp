#include "AUI/Component.h"
#include "AUI/Screen.h"
#include "AUI/Image.h"
#include "AUI/Core.h"
#include "AUI/Internal/ScalingHelpers.h"
#include "AUI/Internal/Ignore.h"

namespace AUI {

Component::Component(Screen& inScreen, const char* inKey, const SDL_Rect& inExtent)
: screen(inScreen)
, logicalExtent{inExtent}
, scaledExtent{ScalingHelpers::extentToActual(logicalExtent)}
, lastRenderedExtent{}
, lastUsedScreenSize{Core::GetActualScreenSize()}
, isVisible{true}
{
    // If we were given a nullptr, replace it with an empty string while
    // constructing the key. This keeps us from having to nullptr check later.
    if (inKey == nullptr) {
        key = entt::hashed_string{""};
    }
    else {
        key = entt::hashed_string{inKey};
    }

    // If this is not an anonymous component, register it with the screen.
    if (key != entt::hashed_string{""}) {
        screen.registerComponent(*this);
    }

    Core::IncComponentCount();
}

Component::~Component()
{
    // If this is not an anonymous component, unregister it with the screen.
    if (key != entt::hashed_string{""}) {
        screen.unregisterComponent(key);
    }

    Core::DecComponentCount();
}

bool Component::containsPoint(const SDL_Point& actualPoint)
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

bool Component::containsExtent(const SDL_Rect& actualExtent)
{
    // Test if 2 diagonal corners of the extent are within our extent.
    if (containsPoint({actualExtent.x, actualExtent.y})
        && containsPoint({(actualExtent.x + actualExtent.w)
                        , (actualExtent.y + actualExtent.h)})) {
        return true;
    }
    else {
        return false;
    }
}

void Component::setExtent(const SDL_Rect& inExtent)
{
    // Set our logical screen extent.
    logicalExtent = inExtent;

    // Re-calculate our scaled screen extent.
    scaledExtent = ScalingHelpers::extentToActual(logicalExtent);
}

const entt::hashed_string& Component::getKey()
{
    return key;
}

void Component::setIsVisible(bool inIsVisible)
{
    isVisible = inIsVisible;
}

bool Component::getIsVisible()
{
    return isVisible;
}

void Component::render(const SDL_Point& parentOffset)
{
    ignore(parentOffset);
    AUI_LOG_ERROR("Base class render called. Please override render() "
    "in your derived class.");
}

bool Component::onMouseButtonDown(SDL_MouseButtonEvent& event)
{
    ignore(event);
    AUI_LOG_ERROR("Base class callback called. Please override"
    " onMouseButtonDown() in your derived class.");

    return false;
}

bool Component::onMouseButtonUp(SDL_MouseButtonEvent& event)
{
    ignore(event);
    AUI_LOG_ERROR("Base class callback called. Please override"
    " onMouseButtonUp() in your derived class.");

    return false;
}

void Component::onMouseMove(SDL_MouseMotionEvent& event)
{
    ignore(event);
    AUI_LOG_ERROR("Base class callback called. Please override onMouseMove() "
    "in your derived class.");
}

bool Component::onKeyDown(SDL_KeyboardEvent& event)
{
    ignore(event);
    AUI_LOG_ERROR("Base class callback called. Please override onKeyDown() "
    "in your derived class.");

    return false;
}

bool Component::onTextInput(SDL_TextInputEvent& event)
{
    ignore(event);
    AUI_LOG_ERROR("Base class callback called. Please override onTextInput() "
    "in your derived class.");

    return false;
}

void Component::onTick()
{
    AUI_LOG_ERROR("Base class callback called. Please override onTick() "
    "in your derived class.");
}

bool Component::refreshScaling()
{
    // If the screen size has changed.
    if (lastUsedScreenSize != Core::GetActualScreenSize()) {
        // Re-calculate our scaled extent.
        scaledExtent = ScalingHelpers::extentToActual(logicalExtent);

        // Save the new size.
        lastUsedScreenSize = Core::GetActualScreenSize();

        return true;
    }

    return false;
}

SDL_Rect Component::calcClippedExtent(const SDL_Rect& sourceExtent, const SDL_Rect& clipExtent)
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
    int rightDiff = (clippedExtent.x + clippedExtent.w) - (clipExtent.x + clipExtent.w);
    if (rightDiff > 0) {
        clippedExtent.w -= rightDiff;
    }

    // If we're beyond the top bound of clipExtent, set it as our y.
    int topDiff = clipExtent.y - sourceExtent.y;
    if (topDiff > 0) {
        clippedExtent.y = clipExtent.y;
    }

    // If we're beyond the bottom bound of clipExtent, decrease height to fit.
    int bottomDiff = (clippedExtent.y + clippedExtent.h) - (clipExtent.y + clipExtent.h);
    if (bottomDiff > 0) {
        clippedExtent.h -= bottomDiff;
    }

    return clippedExtent;
}

} // namespace AUI
