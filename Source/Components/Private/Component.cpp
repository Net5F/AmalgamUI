#include "AUI/Image.h"
#include "AUI/Internal/Ignore.h"
#include "AUI/Core.h"

namespace AUI {

Component::Component(entt::hashed_string inKey, const SDL_Rect& inScreenExtent)
: key(inKey)
, screenExtent(inScreenExtent)
{
    Core::IncComponentCount();
}

Component::~Component()
{
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
