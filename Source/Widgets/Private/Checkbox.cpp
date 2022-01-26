#include "AUI/Checkbox.h"
#include "AUI/Screen.h"
#include "AUI/Core.h"

namespace AUI
{
Checkbox::Checkbox(Screen& inScreen, const SDL_Rect& inLogicalExtent,
                   const std::string& inDebugName)
: Widget(inScreen, inLogicalExtent, inDebugName)
, uncheckedImage(inScreen, {0, 0, logicalExtent.w, logicalExtent.h})
, checkedImage(inScreen, {0, 0, logicalExtent.w, logicalExtent.h})
, currentState{State::Unchecked}
{
    // Add our children so they're included in rendering, etc.
    children.push_back(uncheckedImage);
    children.push_back(checkedImage);

    // Register for the events that we want to listen for.
    registerListener(InternalEvent::MouseButtonDown);

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

bool Checkbox::onMouseButtonDown(SDL_MouseButtonEvent& event)
{
    // If the mouse press was inside our extent.
    if (containsPoint({event.x, event.y})) {
        // If we're unchecked.
        if (currentState == State::Unchecked) {
            // Check if the user set a callback.
            if (onChecked == nullptr) {
                AUI_LOG_FATAL(
                    "Checkbox tried to call empty onChecked() callback.");
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

    // Render our children.
    for (Widget& child : children)
    {
        child.render(childOffset);
    }
}

} // namespace AUI
