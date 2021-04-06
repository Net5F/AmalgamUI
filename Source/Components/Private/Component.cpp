#include "AUI/Image.h"
#include "AUI/Internal/Ignore.h"
#include "AUI/Core.h"

namespace AUI {

Component::Component(const std::string& inDebugKey, const SDL_Rect& inScreenExtent)
: debugKey(inDebugKey)
, screenExtent(inScreenExtent)
{
    Core::IncComponentCount();
}

Component::~Component()
{
    Core::DecComponentCount();
}

void Component::setScreenExtent(const SDL_Rect& inScreenExtent)
{
    screenExtent = inScreenExtent;
}

void Component::renderCopy(int offsetX, int offsetY)
{
    ignore(offsetX);
    ignore(offsetY);
}

} // namespace AUI
