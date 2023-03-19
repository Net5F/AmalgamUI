#include "AUI/Thumbnail.h"
#include "AUI/Screen.h"
#include "AUI/Core.h"
#include "AUI/Internal/Ignore.h"

namespace AUI
{
Thumbnail::Thumbnail(const SDL_Rect& inLogicalExtent,
                     const std::string& inDebugName)
: Widget(inLogicalExtent, inDebugName)
, hoveredImage({0, 0, logicalExtent.w, logicalExtent.h})
, activeImage({0, 0, logicalExtent.w, logicalExtent.h})
, backdropImage({0, 0, logicalExtent.w, logicalExtent.h})
, selectedImage({0, 0, logicalExtent.w, logicalExtent.h})
, thumbnailImage({0, 0, logicalExtent.w, logicalExtent.h})
, isHoverable{true}
, isSelectable{true}
, isActivateable{true}
, isHovered{false}
, isSelected{false}
, isActive{false}
, savedTextAlignment{Text::HorizontalAlignment::Center}
, text({0, 0, logicalExtent.w, logicalExtent.h})
{
    // Add our children so they're included in rendering, etc.
    children.push_back(backdropImage);
    children.push_back(thumbnailImage);
    children.push_back(activeImage);
    children.push_back(hoveredImage);
    children.push_back(selectedImage);
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

void Thumbnail::setTextFont(const std::string& fontPath, int size)
{
    text.setFont(fontPath, size);
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

EventResult Thumbnail::onMouseDown(MouseButtonType buttonType,
                                   const SDL_Point& cursorPosition)
{
    ignore(cursorPosition);

    // Only respond to the left mouse button.
    if (buttonType != MouseButtonType::Left) {
        return EventResult{.wasHandled{false}};
    }
    // If we're already selected and active, do nothing.
    else if (isSelected && isActive) {
        return EventResult{.wasHandled{false}};
    }

    if (isSelectable && !isSelected) {
        // This was a single click, if we aren't already selected, select
        // this widget.
        select();

        // Note: It would make sense to request focus and deselect when we
        //       lose focus, but it seems like every use case for "select a
        //       thumbnail" prefers leaving the thumbnail selected and
        //       controlling it from the outside.
        //       E.g. for build mode, we want the thumbnail to stay selected
        //       until the parent tells it to deselect.

        return EventResult{.wasHandled{true}};
    }

    return EventResult{.wasHandled{false}};
}

EventResult Thumbnail::onMouseDoubleClick(MouseButtonType buttonType,
                                          const SDL_Point& cursorPosition)
{
    ignore(cursorPosition);

    // Only respond to the left mouse button.
    if (buttonType != MouseButtonType::Left) {
        return EventResult{.wasHandled{false}};
    }

    // If we aren't already active, activate.
    if (isActivateable && !isActive) {
        activate();

        // If we were selected, clear the selection.
        // Note: We don't call the deselected callback since this wasn't
        //       a normal deselect event.
        if (isSelected) {
            setIsSelected(false);
        }

        return EventResult{.wasHandled{true}};
    }

    return EventResult{.wasHandled{false}};
}

void Thumbnail::onMouseEnter()
{
    // If we're active, don't change to hovered.
    if (isActive) {
        return;
    }

    // If we're not hovered, become hovered.
    if (!isHovered) {
        setIsHovered(true);
    }
    else {
        // We're hovered, unhover.
        setIsHovered(false);
    }
}

void Thumbnail::onMouseLeave()
{
    // If we're hovered, unhover.
    if (isHovered) {
        setIsHovered(false);
    }
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

} // namespace AUI
