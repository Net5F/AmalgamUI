#include "AUI/Component.h"
#include "AUI/Screen.h"
#include "AUI/Image.h"
#include "AUI/Core.h"
#include "AUI/Internal/ScalingHelpers.h"
#include "AUI/Internal/Ignore.h"

namespace AUI {

Component::Component(Screen& inScreen, const char* inKey, const SDL_Rect& inScreenExtent)
: screen(inScreen)
, logicalScreenExtent{inScreenExtent}
, actualScreenExtent{ScalingHelpers::extentToActual(logicalScreenExtent)}
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

void Component::setScreenExtent(const SDL_Rect& inScreenExtent)
{
    // Set our logical screen extent.
    logicalScreenExtent = inScreenExtent;

    // Re-calculate our actual screen extent.
    actualScreenExtent = ScalingHelpers::extentToActual(logicalScreenExtent);
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

bool Component::refreshScaling()
{
    // If the screen size has changed.
    if (lastUsedScreenSize != Core::GetActualScreenSize()) {
        // Re-calculate our actual extent.
        actualScreenExtent = ScalingHelpers::extentToActual(logicalScreenExtent);

        // Save the new size.
        lastUsedScreenSize = Core::GetActualScreenSize();

        return true;
    }

    return false;
}

} // namespace AUI
