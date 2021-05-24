#pragma once

#include "AUI/Image.h"
#include "AUI/Text.h"
#include <functional>

namespace AUI {

/**
 * A box for displaying or inputting text.
 */
class TextBox : public Component
{
public:
    /**
     * Used to track the button's visual and logical state.
     */
    enum class State {
        Normal,
        Hovered,
        Selected,
        Disabled
    };

    TextBox(Screen& screen, const char* key, const SDL_Rect& screenExtent);

    virtual ~TextBox() = default;

    /**
     * Sets the distance between the left side of the box and the start of the
     * text.
     */
    void setLeftMargin(int inMargin);

    // Set cursor blink rate

    State getCurrentState();

    //-------------------------------------------------------------------------
    // Callback registration
    //-------------------------------------------------------------------------
    void setOnTextChanged(std::function<void(void)> inOnTextChanged);

    //-------------------------------------------------------------------------
    // Base class overrides
    //-------------------------------------------------------------------------
    bool onMouseButtonDown(SDL_MouseButtonEvent& event) override;

    void onMouseMove(SDL_MouseMotionEvent& event) override;

    bool onKeyDown(SDL_KeyboardEvent& event) override;

    bool onTextInput(SDL_TextInputEvent& event) override;

    void render(const SDL_Point& parentOffset = {}) override;

    //-------------------------------------------------------------------------
    // Public child components
    //-------------------------------------------------------------------------
    /** Background image, normal state. */
    Image normalImage;
    /** Background image, hovered state. */
    Image hoveredImage;
    /** Background image, selected state. */
    Image selectedImage;
    /** Background image, disabled state. */
    Image disabledImage;
    /** Text cursor image. */
    Image cursorImage;

    /** The text that this box contains. */
    Text text;

private:
    //-------------------------------------------------------------------------
    // Private members
    //-------------------------------------------------------------------------
    std::function<void(void)> onTextChanged;

    /** Tracks this button's current visual and logical state. */
    State currentState;

    /** The distance between the left side of the box and the start of the
        text. */
    int leftMargin;
};

} // namespace AUI
