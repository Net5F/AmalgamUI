#pragma once

#include "AUI/Image.h"
#include "AUI/Text.h"
#include <functional>

namespace AUI {

/**
 * Displays a simple static image.
 */
class Button : public Component
{
public:
    /**
     * Used to track the button's visual and logical state.
     */
    enum class State {
        Normal,
        Hovered,
        Pressed,
        Disabled
    };

    Button(Screen& screen, const char* key, const SDL_Rect& screenExtent);

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
    // Callback registration
    //-------------------------------------------------------------------------
    void setOnPressed(std::function<void(void)> inOnPressed);

    //-------------------------------------------------------------------------
    // Base class overrides
    //-------------------------------------------------------------------------
    void onMouseButtonDown(SDL_MouseButtonEvent& event) override;

    void onMouseButtonUp(SDL_MouseButtonEvent& event, bool isHovered) override;

    void onMouseEnter(SDL_MouseMotionEvent& event) override;

    void onMouseLeave(SDL_MouseMotionEvent& event) override;

    void render(int offsetX = 0, int offsetY = 0) override;

    //-------------------------------------------------------------------------
    // Public child components
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

private:
    //-------------------------------------------------------------------------
    // Private members
    //-------------------------------------------------------------------------
    std::function<void(void)> onPressed;

    /** Tracks this button's current visual and logical state. */
    State currentState;
};

} // namespace AUI
