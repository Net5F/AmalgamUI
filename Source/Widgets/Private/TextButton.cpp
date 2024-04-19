#include "AUI/TextButton.h"
#include "AUI/Screen.h"
#include "AUI/Core.h"
#include "AUI/ScalingHelpers.h"

namespace AUI
{
TextButton::TextButton(const SDL_Rect& inLogicalExtent, const std::string& inDebugName)
: Widget(inLogicalExtent, inDebugName)
, text({0, 0, logicalExtent.w, logicalExtent.h})
, normalColor{0, 0, 0, 255}
, hoveredColor{255, 255, 255, 255}
, pressedColor{0, 0, 0, 255}
, disabledColor{0, 0, 0, 255}
, autoHeightEnabled{false}
, currentState{Button::State::Normal}
{
    // Add our children so they're included in rendering, etc.
    children.push_back(text);
}

void TextButton::setNormalColor(const SDL_Color& color)
{
    // Set the color and refresh our text widget.
    normalColor = color;
    setCurrentState(currentState);
}

void TextButton::setHoveredColor(const SDL_Color& color)
{
    hoveredColor = color;
    setCurrentState(currentState);
}

void TextButton::setPressedColor(const SDL_Color& color)
{
    pressedColor = color;
    setCurrentState(currentState);
}

void TextButton::setDisabledColor(const SDL_Color& color)
{
    disabledColor = color;
    setCurrentState(currentState);
}

void TextButton::setAutoHeightEnabled(bool inAutoHeightEnabled)
{
    autoHeightEnabled = inAutoHeightEnabled;
}

void TextButton::enable()
{
    SDL_Point cursorPosition{};
    SDL_GetMouseState(&(cursorPosition.x), &(cursorPosition.y));
    cursorPosition.x -= clippedExtent.x;
    cursorPosition.y -= clippedExtent.y;

    // Check if we're currently hovered.
    if (containsPoint(cursorPosition)) {
        setCurrentState(Button::State::Hovered);
    }
    else {
        setCurrentState(Button::State::Normal);
    }
}

void TextButton::disable()
{
    setCurrentState(Button::State::Disabled);
}

Button::State TextButton::getCurrentState()
{
    return currentState;
}

void TextButton::setOnPressed(std::function<void(void)> inOnPressed)
{
    onPressed = std::move(inOnPressed);
}

void TextButton::setOnReleased(std::function<void(void)> inOnReleased)
{
    onReleased = std::move(inOnReleased);
}

void TextButton::setIsVisible(bool inIsVisible)
{
    // If we're being made invisible, set our state back to normal.
    if (!inIsVisible) {
        setCurrentState(Button::Button::State::Normal);
    }

    Widget::setIsVisible(inIsVisible);
}

EventResult TextButton::onMouseDown(MouseButtonType buttonType, const SDL_Point&)
{
    // Only respond to the left mouse button.
    if (buttonType != MouseButtonType::Left) {
        return EventResult{.wasHandled{false}};
    }
    // If we're disabled, ignore the event.
    else if (currentState == Button::State::Disabled) {
        return EventResult{.wasHandled{false}};
    }

    // Check if the user set a callback.
    if (onPressed == nullptr) {
        AUI_LOG_FATAL("Button tried to call empty onPressed() callback.");
    }

    // Set our state to pressed.
    setCurrentState(Button::State::Pressed);

    // Call the user's onPressed callback.
    onPressed();

    // Set mouse capture so we get the associated MouseUp.
    return EventResult{.wasHandled{true}, .setMouseCapture{this}};
}

EventResult TextButton::onMouseUp(MouseButtonType buttonType,
                              const SDL_Point& cursorPosition)
{
    // Only respond to the left mouse button.
    if (buttonType != MouseButtonType::Left) {
        return EventResult{.wasHandled{false}};
    }
    // If we're disabled, ignore the event.
    else if (currentState == Button::State::Disabled) {
        // Note: We need to release mouse capture in case we were disabled
        //       while a click was being held.
        return EventResult{.wasHandled{false}, .releaseMouseCapture{true}};
    }

    // If we were being pressed.
    if (currentState == Button::State::Pressed) {
        // If the mouse is still over this widget, go to hovered.
        if (containsPoint(cursorPosition)) {
            setCurrentState(Button::State::Hovered);
        }
        else {
            // Mouse is gone, go to normal.
            setCurrentState(Button::State::Normal);
        }

        // If the user set a callback, call it.
        if (onReleased) {
            onReleased();
        }
    }

    return EventResult{.wasHandled{true}, .releaseMouseCapture{true}};
}

EventResult TextButton::onMouseDoubleClick(MouseButtonType buttonType,
                                       const SDL_Point& cursorPosition)
{
    // We treat additional clicks as regular MouseDown events.
    return onMouseDown(buttonType, cursorPosition);
}

void TextButton::onMouseEnter()
{
    // If we're disabled, ignore the event.
    if (currentState == Button::State::Disabled) {
        return;
    }

    // If we're normal, change to hovered.
    if (currentState == Button::State::Normal) {
        setCurrentState(Button::State::Hovered);
    }
}

void TextButton::onMouseLeave()
{
    // If we're disabled, ignore the event.
    if (currentState == Button::State::Disabled) {
        return;
    }

    // We won't get a MouseLeave while Pressed because we capture the mouse, 
    // and we know we aren't disabled. This must be an unhover or a release, 
    // so go to normal.
    setCurrentState(Button::State::Normal);
}

void TextButton::measure(const SDL_Rect& availableExtent)
{
    // Run the normal measure step (measures our children and sets our 
    // scaledExtent).
    Widget::measure(availableExtent);

    // If auto-height is enabled, set this widget's height to match the text.
    if (autoHeightEnabled) {
        logicalExtent.h = text.getLogicalExtent().h;
        scaledExtent = ScalingHelpers::logicalToActual(logicalExtent);
    }
}

void TextButton::setCurrentState(Button::State inState)
{
    // Set the new state.
    currentState = inState;

    // Set the text color to match the current state.
    switch (currentState) {
        case Button::State::Normal: {
            text.setColor(normalColor);
            break;
        }
        case Button::State::Hovered: {
            text.setColor(hoveredColor);
            break;
        }
        case Button::State::Pressed: {
            text.setColor(pressedColor);
            break;
        }
        case Button::State::Disabled: {
            text.setColor(disabledColor);
            break;
        }
    }
}

} // namespace AUI
