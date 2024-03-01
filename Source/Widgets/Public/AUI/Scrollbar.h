#pragma once

#include "AUI/Image.h"
#include "AUI/Button.h"
#include <functional>

namespace AUI
{
/**
 * A scrollbar for use in scrollable containers.
 *
 * Contains a track (aka trough) and a bar (aka thumb).
 */
class Scrollbar : public Widget
{
public:
    /**
     * A minimal class to implement the bar's behavior.
     * 
     * The bar is basically a button, but we need to override MouseMove since 
     * buttons don't handle it.
     */
    class Bar : public Button
    {
    public:
        Bar(const SDL_Rect& inLogicalExtent,
            const std::string& inDebugName = "Bar");

        virtual ~Bar() = default;

        EventResult onMouseMove(const SDL_Point& cursorPosition) override;

        /**
         * Called when the mouse is moved while clicking this bar.
         * @param inOnDragged A callback that expects the cursor's new position.
         */
        void setOnDragged(std::function<void(const SDL_Point&)> inOnDragged);

    private:
        std::function<void(const SDL_Point&)> onDragged;
    };

    //-------------------------------------------------------------------------
    // Public definitions
    //-------------------------------------------------------------------------
    enum class Orientation { Vertical, Horizontal };

    //-------------------------------------------------------------------------
    // Public interface
    //-------------------------------------------------------------------------
    Scrollbar(const SDL_Rect& inLogicalExtent,
              const std::string& inDebugName = "Scrollbar");

    virtual ~Scrollbar() = default;

    /**
     * Sets this scrollbar's orientation.
     */
    void setOrientation(Orientation newOrientation);

    /**
     * Sets the height (if vertical) or width (if horizontal) of the content that
     * this scrollbar is controlling.
     * 
     * Determines the size of the scrollbar in relation to the track, and what 
     * values will be given by OnPositionChanged.
     */
    void setContentSize(int newLogicalContentSize);

    //-------------------------------------------------------------------------
    // Public child widgets
    //-------------------------------------------------------------------------
    /** The track (aka trough) background image. */
    Image trackImage;

    /** The bar (the actual thing you click and drag). */
    Bar bar;

    //-------------------------------------------------------------------------
    // Callback registration
    //-------------------------------------------------------------------------
    /**
     * @param inOnPositionChanged A callback that takes the scrollbar's new 
     *                            position (a logical-space int representing 
     *                            how far the content should be offset).
     */
    void setOnPositionChanged(std::function<void(int)> inOnPositionChanged);

private:
    /**
     * Tracks where the bar was initially pressed.
     */
    void onBarPressed(const SDL_Point& cursorPosition);

    /**
     * Moves the bar if appropriate and calls onPositionChanged.
     */
    void onBarDragged(const SDL_Point& cursorPosition);

    /**
     * Moves the bar to match currentPosition.
     */
    void refreshBarPosition();

    /**
     * Returns the amount of space (in logical units) there is for the bar to 
     * move within the track.
     */
    int getAvailableSpace();

    std::function<void(float)> onPositionChanged;

    /** This scrollbar's current orientation. */
    Orientation currentOrientation;

    /** The logical-space height (if vertical) or width (if horizontal) of the 
        content that this scrollbar is controlling. */
    int logicalContentSize;

    /** The bar's current position within the track.
        This is a percentage of the available room to move [0, 1]. */
    float currentPosition;

    /** The logical-space cursor position where the bar was last pressed or 
        dragged. Used to calculate how far it was dragged. */
    SDL_Point barPressLogicalOrigin;
};

} // namespace AUI
