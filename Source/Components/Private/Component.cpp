#include "AUI/Image.h"
#include "AUI/Internal/Ignore.h"

namespace AUI {

Component::Component(const std::string& inDebugKey, const SDL_Rect& inScreenExtent)
: debugKey(inDebugKey)
, screenExtent(inScreenExtent)
{
}

void Component::renderCopy(int offsetX, int offsetY)
{
    ignore(offsetX);
    ignore(offsetY);
}

} // namespace AUI
