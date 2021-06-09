#include "AUI/Thumbnail.h"
#include "AUI/Screen.h"
#include "AUI/Core.h"

namespace AUI {

Thumbnail::Thumbnail(Screen& screen, const char* key, const SDL_Rect& logicalExtent)
: Component(screen, key, logicalExtent)
, normalImage(screen, "", {0, 0, logicalExtent.w, logicalExtent.h})
, hoveredImage(screen, "", {0, 0, logicalExtent.w, logicalExtent.h})
, activeImage(screen, "", {0, 0, logicalExtent.w, logicalExtent.h})
, thumbnailImage(screen, "", {0, 0, logicalExtent.w, logicalExtent.h})
, text(screen, "", {0, 0, logicalExtent.w, logicalExtent.h})
, currentState{State::Normal}
, savedTextAlignment{Text::HorizontalAlignment::Center}
{
    // Default to centering the text. The user can set it otherwise if they
    // care to.
    text.setHorizontalAlignment(AUI::Text::HorizontalAlignment::Center);

    // Register for the events that we want to listen for.
    screen.registerListener(EventType::MouseButtonDown, this);
    screen.registerListener(EventType::MouseMove, this);
}

void Thumbnail::activate()
{
    // If we're already active, do nothing.
    if (currentState == State::Active) {
        return;
    }

    // Check if the user set a callback for this event.
    // Note: We require the user to set this callback because this component
    //       doesn't make much sense if you're ignoring this interaction.
    if (onActivated == nullptr) {
        AUI_LOG_ERROR("Thumbnail tried to call empty onActivated() callback.");
    }

    // Set our state to active.
    currentState = State::Active;

    // Call the user's onActivated callback.
    // Signal that we've been activated.
    onActivated(this);
}

void Thumbnail::deactivate()
{
    // Set our state back to normal.
    currentState = State::Normal;

    // If the user set a callback, signal that we've been deactivated.
    if (onDeactivated == nullptr) {
        onDeactivated(this);
    }
}

Thumbnail::State Thumbnail::getCurrentState()
{
    return currentState;
}

void Thumbnail::setTextLogicalExtent(const SDL_Rect& inLogicalExtent)
{
    text.setLogicalExtent(inLogicalExtent);
}

void Thumbnail::setTextFont(const std::string& relPath, int size)
{
    text.setFont(relPath, size);
}

void Thumbnail::setTextColor(const SDL_Color& inColor)
{
    text.setColor(inColor);
}

void Thumbnail::setTextVerticalAlignment(Text::VerticalAlignment inVerticalAlignment)
{
    text.setVerticalAlignment(inVerticalAlignment);
}

void Thumbnail::setText(std::string_view inText)
{
    // Set the text.
    text.setText(inText);

    // Re-set the horizontal alignment, in case the text has changed width
    // and needs to be re-aligned.
    setTextHorizontalAlignment(savedTextAlignment);
}

void Thumbnail::setTextHorizontalAlignment(Text::HorizontalAlignment inHorizontalAlignment)
{
    // Save the given extent.
    savedTextAlignment = inHorizontalAlignment;

    // If the text fits within its extent, use the given alignment.
    int textWidth = text.calcStringWidth(text.asString());
    if (textWidth <= text.getScaledExtent().w) {
        text.setHorizontalAlignment(inHorizontalAlignment);
    }
    else {
        // Else the text is too big to fit, right align it.
        text.setHorizontalAlignment(Text::HorizontalAlignment::Right);
    }
}

void Thumbnail::setOnActivated(std::function<void(Thumbnail*)> inOnActivated)
{
    onActivated = std::move(inOnActivated);
}

void Thumbnail::setOnDeactivated(std::function<void(Thumbnail*)> inOnDeactivated)
{
    onDeactivated = std::move(inOnDeactivated);
}

bool Thumbnail::onMouseButtonDown(SDL_MouseButtonEvent& event)
{
    // If we're already active, do nothing.
    if (currentState == State::Active) {
        return false;
    }

    // If this was a double click and was inside our extent.
    if ((event.clicks == 2) && containsPoint({event.x, event.y})) {
        // Activate this component.
        activate();

        return true;
    }
    else {
        // Else, the mouse press missed us.
        return false;
    }
}

void Thumbnail::onMouseMove(SDL_MouseMotionEvent& event)
{
    // If we're active, don't change to hovered.
    if (currentState == State::Active) {
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

void Thumbnail::render(const SDL_Point& parentOffset)
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
        case State::Normal: {
            normalImage.render(childOffset);
            break;
        }
        case State::Hovered: {
            hoveredImage.render(childOffset);
            break;
        }
        case State::Active: {
            activeImage.render(childOffset);
            break;
        }
    }

    // Render the thumbnail image.
    thumbnailImage.render(childOffset);

    // Render the text.
    text.render(childOffset);
}

} // namespace AUI
