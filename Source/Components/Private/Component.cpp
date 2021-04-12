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

void Component::render(int offsetX, int offsetY)
{
    ignore(offsetX);
    ignore(offsetY);
}

void Component::setScreenExtent(const SDL_Rect& inScreenExtent)
{
    screenExtent = inScreenExtent;
}

const entt::hashed_string& Component::getKey()
{
    return key;
}

} // namespace AUI
