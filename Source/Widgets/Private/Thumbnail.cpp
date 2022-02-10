#include "AUI/Thumbnail.h"
#include "AUI/Screen.h"
#include "AUI/Core.h"
#include "AUI/Internal/Ignore.h"

namespace AUI
{
Thumbnail::Thumbnail(Screen& inScreen, const SDL_Rect& inLogicalExtent,
                     const std::string& inDebugName)
: Widget(inScreen, inLogicalExtent, inDebugName)
, hoveredImage(screen, {0, 0, logicalExtent.w, logicalExtent.h})
, activeImage(screen, {0, 0, logicalExtent.w, logicalExtent.h})
, backdropImage(screen, {0, 0, logicalExtent.w, logicalExtent.h})
, selectedImage(screen, {0, 0, logicalExtent.w, logicalExtent.h})
, thumbnailImage(screen, {0, 0, logicalExtent.w, logicalExtent.h})
, isHoverable{true}
, isSelectable{true}
, isActivateable{true}
, isHovered{false}
, isSelected{false}
, isActive{false}
, savedTextAlignment{Text::HorizontalAlignment::Center}
, text(screen, {0, 0, logicalExtent.w, logicalExtent.h})
{
    // Add our children so they're included in rendering, etc.
    children.push_back(hoveredImage);
    children.push_back(activeImage);
    children.push_back(backdropImage);
    children.push_back(selectedImage);
    children.push_back(thumbnailImage);
    children.push_back(text);

    // Default to centering the text. The user can set it otherwise if they
    // care to.
    text.setHorizontalAlignment(AUI::Text::HorizontalAlignment::Center);

    // Make the images we aren't using invisible.
    hoveredImage.setIsVisible(false);
    activeImage.setIsVisible(false);
    selectedImage.setIsVisible(false);
}

void Thumbnail::select()
{
    // If we shouldn't become selected or are already selected, do nothing.
    if (!isSelectable || isSelected) {
        return;
    }

    // Flag that we're now selected.
    setIsSelected(true);

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
    setIsSelected(false);

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
    setIsActive(true);

    // Flag that we aren't hovered (can't be hovered while active.)
    setIsHovered(false);

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
    setIsActive(false);

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

void Thumbnail::setTextVerticalAlignment(
    Text::VerticalAlignment inVerticalAlignment)
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

void Thumbnail::setTextHorizontalAlignment(
    Text::HorizontalAlignment inHorizontalAlignment)
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

void Thumbnail::setOnDeactivated(
    std::function<void(Thumbnail*)> inOnDeactivated)
{
    onDeactivated = std::move(inOnDeactivated);
}

Widget* Thumbnail::onMouseButtonDown(SDL_MouseButtonEvent& event)
{
    // If we're already selected and active, do nothing.
    if (isSelected && isActive) {
        return nullptr;
    }

    // If the click event was inside our extent.
    if (containsPoint({event.x, event.y})) {
        // If this was a double click (or more) and we aren't already active,
        // activate this widget.
        if (isActivateable && !isActive && (event.clicks >= 2)) {
            activate();

            // If we were selected, clear the selection.
            // Note: We don't call the deselected callback since this wasn't
            //       a normal deselect event.
            if (isSelected) {
                setIsSelected(false);
            }
        }
        else if (!isSelected) {
            // This was a single click, if we aren't already selected, select
            // this widget.
            select();
        }

        // The click event was inside our widget, so flag it as handled.
        return this;
    }
    else {
        // Else, the mouse press missed us.
        // Note: It would make sense to deselect ourselves here, but it seems
        //       like every use case for "select a thumbnail" prefers leaving
        //       the thumbnail selected and controlling it from the outside.
        //       E.g. for both build mode and multi-select scenarios, we want
        //       the thumbnail to stay selected until the parent tells it to
        //       deselect.
        return nullptr;
    }
}

Widget* Thumbnail::onMouseMove(SDL_MouseMotionEvent& event)
{
    // Update our hovered state if necessary.
    return updateHovered({event.x, event.y});
}

void Thumbnail::setIsHovered(bool inIsHovered)
{
    isHovered = inIsHovered;
    hoveredImage.setIsVisible(isHovered);
}

void Thumbnail::setIsSelected(bool inIsSelected)
{
    isSelected = inIsSelected;
    selectedImage.setIsVisible(isSelected);
}

void Thumbnail::setIsActive(bool inIsActive)
{
    isActive = inIsActive;
    activeImage.setIsVisible(isActive);
}

Widget* Thumbnail::updateHovered(SDL_Point actualMousePoint)
{
    // If we're active, don't change to hovered.
    if (isActive) {
        return nullptr;
    }

    // If the mouse is inside our extent.
    if (containsPoint({actualMousePoint.x, actualMousePoint.y})) {
        // If we're not hovered, become hovered.
        if (!isHovered) {
            setIsHovered(true);
        }

        return this;
    }
    else {
        // Else, the mouse isn't in our extent. If we're hovered, unhover.
        if (isHovered) {
            setIsHovered(false);
        }

        return nullptr;
    }
}

} // namespace AUI
