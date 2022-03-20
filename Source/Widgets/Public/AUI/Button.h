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
           const std::string& inDebugName = "Button");

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

    //-------------------------------------------------------------------------
    // Public child widgets
    //-------------------------------------------------------------------------
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
    /**
     * @param inOnPressed  A callback for when this button is pressed.
     */
    void setOnPressed(std::function<void(void)> inOnPressed);

    //-------------------------------------------------------------------------
    // Base class overrides
    //-------------------------------------------------------------------------
    EventResult onMouseDown(MouseButtonType buttonType, const SDL_Point& cursorPosition) override;

    EventResult onMouseUp(MouseButtonType buttonType, const SDL_Point& cursorPosition) override;

    void onMouseEnter() override;

    void onMouseLeave() override;

private:
    /**
     * Sets currentState and updates child widget visibility.
     */
    void setCurrentState(State inState);

    std::function<void(void)> onPressed;

    /** Tracks this button's current visual and logical state. */
    State currentState;
};

} // namespace AUI
