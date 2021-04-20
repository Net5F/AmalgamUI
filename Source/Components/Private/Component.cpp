#include "AUI/Component.h"
#include "AUI/Screen.h"
#include "AUI/Image.h"
#include "AUI/Core.h"
#include "AUI/Internal/Ignore.h"

namespace AUI {

Component::Component(Screen& inScreen, const char* inKey, const SDL_Rect& inScreenExtent)
: screen(inScreen)
, screenExtent(inScreenExtent)
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

bool Component::containsPoint(const SDL_Point& point)
{
    if ((point.x > screenExtent.x)
       && (point.x < (screenExtent.x + screenExtent.w))
       && (point.y > screenExtent.y)
       && (point.y < (screenExtent.y + screenExtent.h))) {
        return true;
    }
    else {
        return false;
    }
}

void Component::setScreenExtent(const SDL_Rect& inScreenExtent)
{
    screenExtent = inScreenExtent;
}

const entt::hashed_string& Component::getKey()
{
    return key;
}

void Component::render(int offsetX, int offsetY)
{
    ignore(offsetX);
    ignore(offsetY);
}

void Component::onMouseButtonDown(SDL_MouseButtonEvent& event)
{
    ignore(event);
    AUI_LOG_ERROR("Base class callback called. Please override onMouseDown() "
    "in your derived class.");
}

void Component::onMouseButtonUp(SDL_MouseButtonEvent& event, bool isHovered)
{
    ignore(event);
    ignore(isHovered);
    AUI_LOG_ERROR("Base class callback called. Please override onMouseUp() "
    "in your derived class.");
}

void Component::onMouseMove(SDL_MouseMotionEvent& event)
{
    ignore(event);
    AUI_LOG_ERROR("Base class callback called. Please override onMouseMoved() "
    "in your derived class.");
}

void Component::onMouseEnter(SDL_MouseMotionEvent& event)
{
    ignore(event);
    AUI_LOG_ERROR("Base class callback called. Please override onHovered() "
    "in your derived class.");
}

void Component::onMouseLeave(SDL_MouseMotionEvent& event)
{
    ignore(event);
    AUI_LOG_ERROR("Base class callback called. Please override onUnhovered() "
    "in your derived class.");
}

} // namespace AUI
