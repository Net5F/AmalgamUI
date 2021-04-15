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
, currentState{State::Normal}
{
    // Default to centering the text within the button. The user can set it
    // otherwise if they care to.
    text.setVerticalAlignment(AUI::Text::VerticalAlignment::Middle);
    text.setHorizontalAlignment(AUI::Text::HorizontalAlignment::Middle);
}

void Button::enable()
{
    currentState = State::Normal;
}

void Button::disable()
{
    currentState = State::Disabled;
}

Button::State Button::getCurrentState()
{
    return currentState;
}

void Button::render(int offsetX, int offsetY)
{
    // Render the appropriate background image for our current state.
    switch (currentState) {
        case State::Normal: {
            normalImage.render(offsetX, offsetY);
            break;
        }
        case State::Hovered: {
            hoveredImage.render(offsetX, offsetY);
            break;
        }
        case State::Pressed: {
            pressedImage.render(offsetX, offsetY);
            break;
        }
        case State::Disabled: {
            disabledImage.render(offsetX, offsetY);
            break;
        }
    }

    // Render the text.
    text.render(offsetX, offsetY);
}

} // namespace AUI
