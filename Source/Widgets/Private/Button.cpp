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
    // Default to centering the text within the button. The user can set it
    // otherwise if they care to.
    text.setVerticalAlignment(Text::VerticalAlignment::Center);
    text.setHorizontalAlignment(Text::HorizontalAlignment::Center);

    // Register for the events that we want to listen for.
    registerListener(InternalEvent::MouseButtonDown);
    registerListener(InternalEvent::MouseButtonUp);
    registerListener(InternalEvent::MouseMove);
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
        currentState = State::Pressed;

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
            currentState = State::Hovered;
        }
        else {
            currentState = State::Normal;
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
            currentState = State::Hovered;
        }
    }
    else {
        // Else, the mouse isn't in our extent. If we're hovered, unhover.
        if (currentState == State::Hovered) {
            currentState = State::Normal;
        }
    }
}

void Button::render(const SDL_Point& parentOffset)
{
    // Keep our extent up to date.
    refreshScaling();

    // Children should render at the parent's offset + this widget's offset.
    SDL_Point childOffset{parentOffset};
    childOffset.x += scaledExtent.x;
    childOffset.y += scaledExtent.y;

    // Save the extent that we're going to render at.
    lastRenderedExtent = scaledExtent;
    lastRenderedExtent.x += parentOffset.x;
    lastRenderedExtent.y += parentOffset.y;

    // If the widget isn't visible, return without rendering.
    if (!isVisible) {
        return;
    }

    // Render the appropriate background image for our current state.
    switch (currentState) {
        case State::Normal: {
            normalImage.render(childOffset);
            break;
        }
        case State::Hovered: {
            hoveredImage.render(childOffset);
            break;
        }
        case State::Pressed: {
            pressedImage.render(childOffset);
            break;
        }
        case State::Disabled: {
            disabledImage.render(childOffset);
            break;
        }
    }

    // Render the text.
    text.render(childOffset);
}

} // namespace AUI