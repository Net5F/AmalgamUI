#include "AUI/Button.h"
#include "AUI/Screen.h"
#include "AUI/Core.h"

namespace AUI
{
Button::Button(Screen& inScreen, const SDL_Rect& inLogicalExtent,
               const std::string& inDebugName)
: Widget(inScreen, inLogicalExtent, inDebugName)
, normalImage(inScreen, {0, 0, logicalExtent.w, logicalExtent.h})
, hoveredImage(inScreen, {0, 0, logicalExtent.w, logicalExtent.h})
, pressedImage(inScreen, {0, 0, logicalExtent.w, logicalExtent.h})
, disabledImage(inScreen, {0, 0, logicalExtent.w, logicalExtent.h})
, text(inScreen, {0, 0, logicalExtent.w, logicalExtent.h})
, currentState{State::Normal}
{
    // Add our children so they're included in rendering, etc.
    children.push_back(normalImage);
    children.push_back(hoveredImage);
    children.push_back(pressedImage);
    children.push_back(disabledImage);
    children.push_back(text);

    // Default to centering the text within the button. The user can set it
    // otherwise if they care to.
    text.setVerticalAlignment(Text::VerticalAlignment::Center);
    text.setHorizontalAlignment(Text::HorizontalAlignment::Center);

    // Make the backgrounds we aren't using invisible.
    hoveredImage.setIsVisible(false);
    pressedImage.setIsVisible(false);
    disabledImage.setIsVisible(false);
}

void Button::enable()
{
    setCurrentState(State::Normal);
}

void Button::disable()
{
    setCurrentState(State::Disabled);
}

Button::State Button::getCurrentState()
{
    return currentState;
}

void Button::setOnPressed(std::function<void(void)> inOnPressed)
{
    onPressed = std::move(inOnPressed);
}

bool Button::onMouseButtonDown(SDL_MouseButtonEvent& event)
{
    // If we're disabled, ignore the event.
    if (currentState == State::Disabled) {
        return false;
    }

    // If the mouse press was inside our extent.
    if (containsPoint({event.x, event.y})) {
        // Check if the user set a callback.
        if (onPressed == nullptr) {
            AUI_LOG_FATAL("Button tried to call empty onPressed() callback.");
        }

        // Set our state to pressed.
        setCurrentState(State::Pressed);

        // Call the user's onPressed callback.
        onPressed();

        return true;
    }
    else {
        // Else, the mouse press missed us.
        return false;
    }
}

bool Button::onMouseButtonUp(SDL_MouseButtonEvent& event)
{
    // If we're disabled, ignore the event.
    if (currentState == State::Disabled) {
        return false;
    }

    // If we were being pressed.
    if (currentState == State::Pressed) {
        // If the mouse is still over this widget, go to hovered.
        if (containsPoint({event.x, event.y})) {
            setCurrentState(State::Hovered);
        }
        else {
            setCurrentState(State::Normal);
        }

        return true;
    }
    else {
        // We weren't being pressed.
        return false;
    }
}

void Button::onMouseMove(SDL_MouseMotionEvent& event)
{
    // If we're disabled, ignore the event.
    if (currentState == State::Disabled) {
        return;
    }

    // If the mouse is inside our extent.
    if (containsPoint({event.x, event.y})) {
        // If we're normal, change to hovered.
        if (currentState == State::Normal) {
            setCurrentState(State::Hovered);
        }
    }
    else {
        // Else, the mouse isn't in our extent. If we're hovered, unhover.
        if (currentState == State::Hovered) {
            setCurrentState(State::Normal);
        }
    }
}

void Button::setCurrentState(State inState)
{
    // Set the new state.
    currentState = inState;

    // Make the associated background visible and make the rest invisible.
    normalImage.setIsVisible(false);
    hoveredImage.setIsVisible(false);
    pressedImage.setIsVisible(false);
    disabledImage.setIsVisible(false);

    switch (currentState) {
        case State::Normal: {
            normalImage.setIsVisible(true);
            break;
        }
        case State::Hovered: {
            hoveredImage.setIsVisible(true);
            break;
        }
        case State::Pressed: {
            pressedImage.setIsVisible(true);
            break;
        }
        case State::Disabled: {
            disabledImage.setIsVisible(true);
            break;
        }
    }
}

} // namespace AUI
