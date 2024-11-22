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
    Button(const SDL_Rect& inLogicalExtent,
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

    State getCurrentState() const;

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
     * @param inOnPressed A callback for when this button is pressed.
     */
    void setOnPressed(std::function<void(void)> inOnPressed);

    /**
     * @param inOnReleased A callback for when this button is released.
     */
    void setOnReleased(std::function<void(void)> inOnReleased);

    //-------------------------------------------------------------------------
    // Base class overrides
    //-------------------------------------------------------------------------
    void setIsVisible(bool inIsVisible) override;

    EventResult onMouseDown(MouseButtonType buttonType,
                            const SDL_Point& cursorPosition) override;

    EventResult onMouseUp(MouseButtonType buttonType,
                          const SDL_Point& cursorPosition) override;

    EventResult onMouseDoubleClick(MouseButtonType buttonType,
                                   const SDL_Point& cursorPosition) override;

    void onMouseEnter() override;

    void onMouseLeave() override;

protected:
    /**
     * Sets currentState and updates child widget visibility.
     */
    void setCurrentState(State inState);

    std::function<void(void)> onPressed;
    std::function<void(void)> onReleased;

    /** Tracks this button's current visual and logical state. */
    State currentState;
};

} // namespace AUI
