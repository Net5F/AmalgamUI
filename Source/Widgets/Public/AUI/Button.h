#pragma once

#include "AUI/Image.h"
#include "AUI/Text.h"
#include <functional>

namespace AUI
{
/**
 * A simple button with text.
 */
class Button : public Widget
{
public:
    //-------------------------------------------------------------------------
    // Public definitions
    //-------------------------------------------------------------------------
    /**
     * Used to track the button's visual and logical state.
     */
    enum class State { Normal, Hovered, Pressed, Disabled };

    //-------------------------------------------------------------------------
    // Public interface
    //-------------------------------------------------------------------------
    Button(Screen& inScreen, const SDL_Rect& inLogicalExtent,
           const std::string& inDebugName = "");

    virtual ~Button() = default;

    /**
     * Enables this button.
     *
     * @post The button will visually be in the Normal state and will respond
     *       to hover and click events.
     */
    void enable();

    /**
     * Disables this button.
     *
     * @post The button will visually be in the Disabled state and will
     *       ignore all events.
     */
    void disable();

    State getCurrentState();

    /** Background image, normal state. */
    Image normalImage;
    /** Background image, hovered state. */
    Image hoveredImage;
    /** Background image, pressed state. */
    Image pressedImage;
    /** Background image, disabled state. */
    Image disabledImage;

    /** Button text. */
    Text text;

    //-------------------------------------------------------------------------
    // Callback registration
    //-------------------------------------------------------------------------
    void setOnPressed(std::function<void(void)> inOnPressed);

    //-------------------------------------------------------------------------
    // Base class overrides
    //-------------------------------------------------------------------------
    bool onMouseButtonDown(SDL_MouseButtonEvent& event) override;

    bool onMouseButtonUp(SDL_MouseButtonEvent& event) override;

    void onMouseMove(SDL_MouseMotionEvent& event) override;

    void render(const SDL_Point& parentOffset = {}) override;

private:
    //-------------------------------------------------------------------------
    // Private members
    //-------------------------------------------------------------------------
    std::function<void(void)> onPressed;

    /** Tracks this button's current visual and logical state. */
    State currentState;
};

} // namespace AUI