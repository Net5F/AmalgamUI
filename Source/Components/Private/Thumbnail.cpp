#include "AUI/Thumbnail.h"
#include "AUI/Screen.h"
#include "AUI/Core.h"
#include "AUI/Internal/Ignore.h"

namespace AUI {

Thumbnail::Thumbnail(Screen& screen, const char* key, const SDL_Rect& logicalExtent)
: Component(screen, key, logicalExtent)
, hoveredImage(screen, "", {0, 0, logicalExtent.w, logicalExtent.h})
, activeImage(screen, "", {0, 0, logicalExtent.w, logicalExtent.h})
, backdropImage(screen, "", {0, 0, logicalExtent.w, logicalExtent.h})
, selectedImage(screen, "", {0, 0, logicalExtent.w, logicalExtent.h})
, thumbnailImage(screen, "", {0, 0, logicalExtent.w, logicalExtent.h})
, text(screen, "", {0, 0, logicalExtent.w, logicalExtent.h})
, isHoverable{true}
, isSelectable{true}
, isActivateable{true}
, isHovered{false}
, isSelected{false}
, isActive{false}
, savedTextAlignment{Text::HorizontalAlignment::Center}
{
    // Default to centering the text. The user can set it otherwise if they
    // care to.
    text.setHorizontalAlignment(AUI::Text::HorizontalAlignment::Center);

    // Register for the events that we want to listen for.
    registerListener(InternalEvent::MouseButtonDown);
    registerListener(InternalEvent::MouseWheel);
    registerListener(InternalEvent::MouseMove);
}

void Thumbnail::select()
{
    // If we shouldn't become selected or are already selected, do nothing.
    if (!isSelectable || isSelected) {
        return;
    }

    // Flag that we're now selected.
    isSelected = true;

    // If the user set a callback for this event, call it.
    if (onSelected != nullptr) {
        onSelected(this);
    }
}

void Thumbnail::deselect()
{
    // If we aren't selected, do nothing.
    if (!isSelected) {
        return;
    }

    // Flag that we're not selected.
    isSelected = false;

    // If the user set a callback for this event, call it.
    if (onDeselected != nullptr) {
        onDeselected(this);
    }
}

void Thumbnail::activate()
{
    // If we shouldn't become active or are already active, do nothing.
    if (!isActivateable || isActive) {
        return;
    }

    // Flag that we're now active.
    isActive = true;

    // If the user set a callback for this event, call it.
    if (onActivated != nullptr) {
        onActivated(this);
    }
}

void Thumbnail::deactivate()
{
    // If we aren't active, do nothing.
    if (!isActive) {
        return;
    }

    // Flag that we're inactive.
    isActive = false;

    // If the user set a callback for this event, call it.
    if (onDeactivated != nullptr) {
        onDeactivated(this);
    }
}

bool Thumbnail::getIsHovered()
{
    return isHovered;
}

bool Thumbnail::getIsSelected()
{
    return isSelected;
}

bool Thumbnail::getIsActive()
{
    return isActive;
}

void Thumbnail::setIsHoverable(bool inIsHoverable)
{
    isHoverable = inIsHoverable;
}

void Thumbnail::setIsSelectable(bool inIsSelectable)
{
    isSelectable = inIsSelectable;
}

void Thumbnail::setIsActivateable(bool inIsActivateable)
{
    isActivateable = inIsActivateable;
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

void Thumbnail::setOnSelected(std::function<void(Thumbnail*)> inOnSelected)
{
    onSelected = std::move(inOnSelected);
}

void Thumbnail::setOnDeselected(std::function<void(Thumbnail*)> inOnDeselected)
{
    onDeselected = std::move(inOnDeselected);
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
    // If we're already selected and active, do nothing.
    if (isSelected && isActive) {
        return false;
    }

    // If the click event was inside our extent.
    if (containsPoint({event.x, event.y})) {
        // If this was a double click (or more) and we aren't already active,
        // activate this component.
        if (isActivateable && !isActive && (event.clicks >= 2)) {
            activate();

            // If we were selected, clear the selection.
            // Note: We don't call the deselected callback since this wasn't
            //       a normal deselect event.
            if (isSelected) {
                isSelected = false;
            }
        }
        else if (!isSelected){
            // This was a single click, if we aren't already selected, select
            // this component.
            select();
        }

        // The click event was inside our component, so flag it as handled.
        return true;
    }
    else {
        // Else, the mouse press missed us.
        return false;
    }
}

bool Thumbnail::onMouseWheel(SDL_MouseWheelEvent& event)
{
    // We don't care about the scroll itself, just about updating our
    // hovered state since we may have moved.
    ignore(event);

    // Get the mouse position since the event doesn't report it.
    SDL_Point mousePosition{};
    SDL_GetMouseState(&(mousePosition.x), &(mousePosition.y));

    // Update our hovered state if necessary.
    return updateHovered(mousePosition);
}

void Thumbnail::onMouseMove(SDL_MouseMotionEvent& event)
{
    // Update our hovered state if necessary.
    updateHovered({event.x, event.y});
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

    // If we're active, render the active image.
    if (isActive) {
        activeImage.render(childOffset);
    }
    else if (isHovered) {
        // We aren't active and are hovered, render the hovered image.
        hoveredImage.render(childOffset);
    }

    // Render the thumbnail's backdrop image.
    backdropImage.render(childOffset);

    // If we're selected, render the selected image.
    if (isSelected) {
        selectedImage.render(childOffset);
    }

    // Render the thumbnail image.
    thumbnailImage.render(childOffset);

    // Render the text.
    text.render(childOffset);
}

bool Thumbnail::updateHovered(SDL_Point actualMousePoint)
{
    // If we're active, don't change to hovered.
    if (isActive) {
        return false;
    }

    // If the mouse is inside our extent.
    if (containsPoint({actualMousePoint.x, actualMousePoint.y})) {
        // If we're not hovered, become hovered.
        if (!isHovered) {
            isHovered = true;
            return true;
        }
    }
    else {
        // Else, the mouse isn't in our extent. If we're hovered, unhover.
        if (isHovered) {
            isHovered = false;
        }
    }

    return false;
}

} // namespace AUI
