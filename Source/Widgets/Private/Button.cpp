#include "AUI/Button.h"
#include "AUI/Screen.h"
#include "AUI/Core.h"

namespace AUI
{
Button::Button(const SDL_Rect& inLogicalExtent, const std::string& inDebugName)
: Widget(inLogicalExtent, inDebugName)
, normalImage({0, 0, logicalExtent.w, logicalExtent.h})
, hoveredImage({0, 0, logicalExtent.w, logicalExtent.h})
, pressedImage({0, 0, logicalExtent.w, logicalExtent.h})
, disabledImage({0, 0, logicalExtent.w, logicalExtent.h})
, text({0, 0, logicalExtent.w, logicalExtent.h})
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
    SDL_Point cursorPosition{};
    SDL_GetMouseState(&(cursorPosition.x), &(cursorPosition.y));

    // Check if we're currently hovered.
    if (containsPoint(cursorPosition)) {
        setCurrentState(State::Hovered);
    }
    else {
        setCurrentState(State::Normal);
    }
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

void Button::setIsVisible(bool inIsVisible)
{
    // If we're being made invisible, set our state back to normal.
    if (!inIsVisible) {
        setCurrentState(State::Normal);
    }

    Widget::setIsVisible(inIsVisible);
}

EventResult Button::onMouseDown(MouseButtonType buttonType, const SDL_Point&)
{
    // Only respond to the left mouse button.
    if (buttonType != MouseButtonType::Left) {
        return EventResult{.wasHandled{false}};
    }
    // If we're disabled, ignore the event.
    else if (currentState == State::Disabled) {
        return EventResult{.wasHandled{false}};
    }

    // Check if the user set a callback.
    if (onPressed == nullptr) {
        AUI_LOG_FATAL("Button tried to call empty onPressed() callback.");
    }

    // Set our state to pressed.
    setCurrentState(State::Pressed);

    // Call the user's onPressed callback.
    onPressed();

    // Set mouse capture so we get the associated MouseUp.
    return EventResult{.wasHandled{true}, .setMouseCapture{this}};
}

EventResult Button::onMouseUp(MouseButtonType buttonType,
                              const SDL_Point& cursorPosition)
{
    // Only respond to the left mouse button.
    if (buttonType != MouseButtonType::Left) {
        return EventResult{.wasHandled{false}};
    }
    // If we're disabled, ignore the event.
    else if (currentState == State::Disabled) {
        // Note: We need to release mouse capture in case we were disabled
        //       while a click was being held.
        return EventResult{.wasHandled{false}, .releaseMouseCapture{true}};
    }

    // If we were being pressed.
    if (currentState == State::Pressed) {
        // If the mouse is still over this widget, go to hovered.
        if (containsPoint(cursorPosition)) {
            setCurrentState(State::Hovered);
        }
        else {
            // Mouse is gone, go to normal.
            setCurrentState(State::Normal);
        }
    }

    return EventResult{.wasHandled{true}, .releaseMouseCapture{true}};
}

EventResult Button::onMouseDoubleClick(MouseButtonType buttonType,
                                       const SDL_Point& cursorPosition)
{
    // We treat additional clicks as regular MouseDown events.
    return onMouseDown(buttonType, cursorPosition);
}

void Button::onMouseEnter()
{
    // If we're disabled, ignore the event.
    if (currentState == State::Disabled) {
        return;
    }

    // If we're normal, change to hovered.
    if (currentState == State::Normal) {
        setCurrentState(State::Hovered);
    }
}

void Button::onMouseLeave()
{
    // If we're disabled, ignore the event.
    if (currentState == State::Disabled) {
        return;
    }

    // If we're hovered, unhover.
    if (currentState == State::Hovered) {
        setCurrentState(State::Normal);
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
