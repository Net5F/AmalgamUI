#include "AUI/Scrollbar.h"
#include "AUI/ScalingHelpers.h"

namespace AUI
{
//-------------------------------------------------------------------------
// ScrollBar::Bar
//-------------------------------------------------------------------------
ScrollBar::Bar::Bar(const SDL_Rect& inLogicalExtent,
                    const std::string& inDebugName)
: Button(inLogicalExtent, inDebugName)
{
}

EventResult ScrollBar::Bar::onMouseMove(const SDL_Point& cursorPosition)
{
    // If we're pressed and the user set a callback, give them the cursor's
    // new position.
    if ((currentState == Button::State::Pressed) && onDragged) {
        onDragged(cursorPosition);
    }
}

void ScrollBar::Bar::setOnDragged(
    std::function<void(const SDL_Point&)> inOnDragged)
{
    onDragged = std::move(inOnDragged);
}

//-------------------------------------------------------------------------
// ScrollBar
//-------------------------------------------------------------------------
Scrollbar::Scrollbar(const SDL_Rect& inLogicalExtent,
                     const std::string& inDebugName)
: Widget(inLogicalExtent, inDebugName)
, trackImage({0, 0, logicalExtent.w, logicalExtent.h})
, bar({0, 0, logicalExtent.w, logicalExtent.h})
, currentOrientation{Orientation::Vertical}
, logicalContentSize{0}
, currentPosition{0}
, barPressLogicalOrigin{}
{
    // Add our children so they're included in rendering, etc.
    children.push_back(trackImage);
    children.push_back(bar);

    // Handle the bar being pressed and dragged.
    bar.setOnPressed([this](const SDL_Point& cursorPosition) {
        barPressLogicalOrigin = ScalingHelpers::actualToLogical(cursorPosition);
    });
    bar.setOnDragged([this](const SDL_Point& cursorPosition) {
        onBarDragged(cursorPosition);
    });
}

void Scrollbar::setOrientation(Orientation newOrientation)
{
    currentOrientation = newOrientation;

    // Reset the bar's position.
    if (currentPosition != 0) {
        currentPosition = 0;
        SDL_Rect barLogicalExtent{bar.getLogicalExtent()};
        barLogicalExtent.x = 0;
        barLogicalExtent.y = 0;
        bar.setLogicalExtent(barLogicalExtent);

        if (onPositionChanged) {
            onPositionChanged(currentPosition);
        }
    }

    // Note: We don't handle the edge case where orientation is changed while 
    //       dragging the bar.
}

void Scrollbar::setContentSize(int newLogicalContentSize)
{
    int oldContentOffset{getAvailableSpace() * currentPosition};
    logicalContentSize = newLogicalContentSize;

    // Resize the bar to match the new content size.
    if (currentOrientation == Orientation::Vertical) {
        // Make the bar : track height ratio match the track : content ratio.
        float heightFactor{logicalExtent.h
                           / static_cast<float>(newLogicalContentSize)};
        SDL_Rect barLogicalExtent{bar.getLogicalExtent()};
        barLogicalExtent.h = logicalExtent.h * heightFactor;
        bar.setLogicalExtent(barLogicalExtent);

        // Attempt to keep the content at the same place on screen, but move 
        // it if the content has shrunk too much.
        int availableSpace{getAvailableSpace()};
        int newContentOffset{std::min(oldContentOffset, availableSpace)};
        currentPosition = newContentOffset / static_cast<float>(availableSpace);
        refreshBarPosition();
    }
}

void Scrollbar::setOnPositionChanged(std::function<void(int)> inOnPositionChanged)
{
    onPositionChanged = std::move(inOnPositionChanged);
}

void Scrollbar::onBarDragged(const SDL_Point& cursorPosition)
{
    // Calc how far the cursor has moved.
    int dragLength{0};
    SDL_Point cursorLogicalPosition{
        ScalingHelpers::actualToLogical(cursorPosition)};
    if (orientation == Orientation::Vertical) {
        dragLength = cursorLogicalPosition.y - barPressLogicalOrigin.y;
    }
    else {
        // Horizontal
        dragLength = cursorLogicalPosition.x - barPressLogicalOrigin.x;
    }

    // Calc where the bar should be moved to.
    int availableSpace{getAvailableSpace()};
    int currentLogicalPosition{currentPosition * availableSpace};
    int desiredLogicalPosition{currentLogicalPosition + dragLength};
    int finalLogicalPosition{std::min(desiredLogicalPosition, availableSpace)};
    finalLogicalPosition = std::max(finalLogicalPosition, 0);

    // Move the bar.
    currentPosition = finalLogicalPosition / static_cast<float>(availableSpace);
    refreshBarPosition();

    // Save the new origin.
    barPressLogicalOrigin = cursorLogicalPosition;

    // If the user set a callback, call it.
    if (onPositionChanged) {
        onPositionChanged(currentPosition);
    }
}

void Scrollbar::refreshBarPosition() 
{
    SDL_Rect barLogicalExtent{bar.getLogicalExtent()};
    if (orientation == Orientation::Vertical) {
        barLogicalExtent.x = 0;
        barLogicalExtent.y = getAvailableSpace() * currentPosition;
    }
    else {
        // Horizontal
        barLogicalExtent.x = getAvailableSpace() * currentPosition;
        barLogicalExtent.y = 0;
    }
}

int Scrollbar::getAvailableSpace()
{
    const SDL_Rect& barExtent{bar.getLogicalExtent()};
    if (orientation == Orientation::Vertical) {
        return {(logicalExtent.y + logicalExtent.h)
                - (barExtent.y + barExtent.h)};
    }
    else {
        // Horizontal
        return {(logicalExtent.x + logicalExtent.w)
                - (barExtent.x + barExtent.w)};
    }
}

} // namespace AUI
