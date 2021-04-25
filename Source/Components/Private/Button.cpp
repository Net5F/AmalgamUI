#include "AUI/Button.h"
#include "AUI/Screen.h"
#include "AUI/Core.h"
#include "AUI/Internal/Ignore.h"

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

    screen.registerListener(EventType::MouseButtonDown, this);
    screen.registerListener(EventType::MouseButtonUp, this);
    screen.registerListener(EventType::MouseEnter, this);
    screen.registerListener(EventType::MouseLeave, this);
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

void Button::setOnPressed(std::function<void(void)> inOnPressed)
{
    onPressed = std::move(inOnPressed);
}

void Button::onMouseButtonDown(SDL_MouseButtonEvent& event)
{
    // We don't care about where exactly the click happened.
    ignore(event);

    // Check if the user set a callback.
    if (onPressed == nullptr) {
        AUI_LOG_ERROR("Tried to call empty onMouseButtonDown() callback.");
    }

    // Set our state to pressed.
    currentState = State::Pressed;

    // Call the user's onPressed callback.
    onPressed();
}

void Button::onMouseButtonUp(SDL_MouseButtonEvent& event, bool isHovered)
{
    // We don't care about where exactly the click happened.
    ignore(event);

    // Check if the user set a callback.
    if (onPressed == nullptr) {
        AUI_LOG_ERROR("Tried to call empty onMouseButtonUp() callback.");
    }

    // If we were being pressed.
    if (currentState == State::Pressed) {
        // If the mouse is still over this component, go to hovered.
        if (isHovered) {
            currentState = State::Hovered;
        }
        else {
            currentState = State::Normal;
        }
    }
}

void Button::onMouseEnter(SDL_MouseMotionEvent& event)
{
    // We don't care about where exactly the hover happened.
    ignore(event);

    // If we were in a normal state, go to hovered.
    if (currentState == State::Normal) {
        currentState = State::Hovered;
    }
}

void Button::onMouseLeave(SDL_MouseMotionEvent& event)
{
    // We don't care about where exactly the hover happened.
    ignore(event);

    // If we were in a hovered state, go to normal.
    if (currentState == State::Hovered) {
        currentState = State::Normal;
    }
}

void Button::render(int offsetX, int offsetY)
{
    // Keep our extent up to date.
    refreshScaling();

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
