#include "AUI/Checkbox.h"
#include "AUI/Screen.h"
#include "AUI/Core.h"
#include "AUI/Internal/Ignore.h"

namespace AUI
{
Checkbox::Checkbox(const SDL_Rect& inLogicalExtent,
                   const std::string& inDebugName)
: Widget(inLogicalExtent, inDebugName)
, uncheckedImage({0, 0, logicalExtent.w, logicalExtent.h})
, checkedImage({0, 0, logicalExtent.w, logicalExtent.h})
, currentState{State::Unchecked}
{
    // Add our children so they're included in rendering, etc.
    children.push_back(uncheckedImage);
    children.push_back(checkedImage);

    // Make the images we aren't using invisible.
    checkedImage.setIsVisible(false);
}

void Checkbox::setCurrentState(State inState)
{
    // Set the new state.
    currentState = inState;

    // Make the associated image visible and make the rest invisible.
    uncheckedImage.setIsVisible(false);
    checkedImage.setIsVisible(false);

    switch (currentState) {
        case State::Unchecked: {
            uncheckedImage.setIsVisible(true);
            break;
        }
        case State::Checked: {
            checkedImage.setIsVisible(true);
            break;
        }
    }
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

EventResult Checkbox::onMouseDown(MouseButtonType buttonType,
                                  const SDL_Point& cursorPosition)
{
    ignore(cursorPosition);

    // Only respond to the left mouse button.
    if (buttonType != MouseButtonType::Left) {
        return EventResult{.wasHandled{false}};
    }

    // If we're unchecked.
    if (currentState == State::Unchecked) {
        // Check if the user set a callback.
        if (onChecked == nullptr) {
            AUI_LOG_FATAL("Checkbox tried to call empty onChecked() callback.");
        }

        // Set our state to checked.
        setCurrentState(State::Checked);

        // Call the user's onChecked callback.
        onChecked();
    }
    else if (currentState == State::Checked) {
        // We're checked, check if the user set a callback.
        if (onUnchecked == nullptr) {
            AUI_LOG_FATAL(
                "Checkbox tried to call empty onUnchecked() callback.");
        }

        // Set our state to unchecked.
        setCurrentState(State::Unchecked);

        // Call the user's onUnchecked callback.
        onUnchecked();
    }

    return EventResult{.wasHandled{true}};
}

EventResult Checkbox::onMouseDoubleClick(MouseButtonType buttonType,
                                         const SDL_Point& cursorPosition)
{
    // We treat additional clicks as regular MouseDown events.
    return onMouseDown(buttonType, cursorPosition);
}

} // namespace AUI
