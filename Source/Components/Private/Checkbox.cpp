#include "AUI/Checkbox.h"
#include "AUI/Screen.h"
#include "AUI/Core.h"

namespace AUI
{
Checkbox::Checkbox(Screen& inScreen, const SDL_Rect& inLogicalExtent,
                   const std::string& inDebugName)
: Component(inScreen, inLogicalExtent, inDebugName)
, uncheckedImage(inScreen, {0, 0, logicalExtent.w, logicalExtent.h})
, checkedImage(inScreen, {0, 0, logicalExtent.w, logicalExtent.h})
, currentState{State::Unchecked}
{
    // Register for the events that we want to listen for.
    registerListener(InternalEvent::MouseButtonDown);
}

void Checkbox::setCurrentState(State inState)
{
    currentState = inState;
}

Checkbox::State Checkbox::getCurrentState()
{
    return currentState;
}

void Checkbox::setOnChecked(std::function<void(void)> inOnChecked)
{
    onChecked = std::move(inOnChecked);
}

void Checkbox::setOnUnchecked(std::function<void(void)> inOnUnchecked)
{
    onUnchecked = std::move(inOnUnchecked);
}

bool Checkbox::onMouseButtonDown(SDL_MouseButtonEvent& event)
{
    // If the mouse press was inside our extent.
    if (containsPoint({event.x, event.y})) {
        // If we're unchecked.
        if (currentState == State::Unchecked) {
            // Check if the user set a callback.
            if (onChecked == nullptr) {
                AUI_LOG_ERROR(
                    "Checkbox tried to call empty onChecked() callback.");
            }

            // Set our state to checked.
            currentState = State::Checked;

            // Call the user's onChecked callback.
            onChecked();
        }
        else if (currentState == State::Checked) {
            // We're checked, check if the user set a callback.
            if (onUnchecked == nullptr) {
                AUI_LOG_ERROR(
                    "Checkbox tried to call empty onUnchecked() callback.");
            }

            // Set our state to unchecked.
            currentState = State::Unchecked;

            // Call the user's onUnchecked callback.
            onUnchecked();
        }

        return true;
    }
    else {
        // Else, the mouse press missed us.
        return false;
    }
}

void Checkbox::render(const SDL_Point& parentOffset)
{
    // Keep our extent up to date.
    refreshScaling();

    // Children should render at the parent's offset + this component's offset.
    SDL_Point childOffset{parentOffset};
    childOffset.x += scaledExtent.x;
    childOffset.y += scaledExtent.y;

    // Save the extent that we're going to render at.
    lastRenderedExtent = scaledExtent;
    lastRenderedExtent.x += parentOffset.x;
    lastRenderedExtent.y += parentOffset.y;

    // If the component isn't visible, return without rendering.
    if (!isVisible) {
        return;
    }

    // Render the appropriate background image for our current state.
    switch (currentState) {
        case State::Unchecked: {
            uncheckedImage.render(childOffset);
            break;
        }
        case State::Checked: {
            checkedImage.render(childOffset);
            break;
        }
    }
}

} // namespace AUI
