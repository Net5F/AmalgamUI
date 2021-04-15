#include "AUI/Button.h"
#include "AUI/Core.h"

namespace AUI {

Button::Button(Screen& screen, const char* key, const SDL_Rect& screenExtent)
: Component(screen, key, screenExtent)
, normalImage(screen, "", screenExtent)
, hoveredImage(screen, "", screenExtent)
, pressedImage(screen, "", screenExtent)
, disabledImage(screen, "", screenExtent)
, text(screen, "", screenExtent)
{
    // Default to centering the text within the button. The user can set it
    // otherwise if they care to.
    text.setVerticalAlignment(AUI::Text::VerticalAlignment::Middle);
    text.setHorizontalAlignment(AUI::Text::HorizontalAlignment::Middle);
}

void Button::render(int offsetX, int offsetY)
{
    normalImage.render(offsetX, offsetY);

    text.render(offsetX, offsetY);
}

} // namespace AUI
