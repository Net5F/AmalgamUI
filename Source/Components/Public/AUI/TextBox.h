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

    /**
     * The distance between the text and the edge of the text box on each side.
     */
    struct Margin {
        int left{0};
        int top{0};
        int right{0};
        int bottom{0};
    };

    TextBox(Screen& screen, const char* key, const SDL_Rect& inLogicalExtent);

    virtual ~TextBox() = default;

    //-------------------------------------------------------------------------
    // Public interface
    //-------------------------------------------------------------------------
    /**
     * Sets the distance between the text and the edge of the text box on each
     * side.
     */
    void setMargin(Margin inMargin);

    /**
     * Sets the color of the text cursor.
     */
    void setCursorColor(const SDL_Color& inCursorColor);

    /**
     * Sets the width of the text cursor in pixels.
     */
    void setCursorWidth(unsigned int inCursorWidth);

    /**
     * See Text::setText().
     * Keeps our cursor in sync with the newly set text.
     */
    void setText(std::string_view inText);

    /**
     * See Text::setFont().
     */
    void setTextFont(const std::string& relPath, int size);

    /**
     * See Text::setColor().
     */
    void setTextColor(const SDL_Color& inColor);

    State getCurrentState();

    /** Background image, normal state. */
    Image normalImage;
    /** Background image, hovered state. */
    Image hoveredImage;
    /** Background image, selected state. */
    Image selectedImage;
    /** Background image, disabled state. */
    Image disabledImage;

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

    void onTick() override;

    void render(const SDL_Point& parentOffset = {}) override;

private:
    //-------------------------------------------------------------------------
    // Private definitions
    //-------------------------------------------------------------------------
    /** The text cursor's blink rate. Windows seems to default to 530, so
        it should work fine for us. */
    static constexpr double CURSOR_BLINK_RATE_S = 530 / static_cast<double>(1000);

    //-------------------------------------------------------------------------
    // Private members
    //-------------------------------------------------------------------------
    // Event handlers for key press events.
    void handleBackspaceEvent();
    void handleDeleteEvent();
    void handleCopyEvent();
    void handleCutEvent();
    void handlePasteEvent();
    void handleLeftEvent();
    void handleRightEvent();
    void handleHomeEvent();
    void handleEndEvent();

    /**
     * Renders the appropriate background image, based on the current State.
     */
    void renderAppropriateImage(const SDL_Point& childOffset);

    /**
     * Calcs where the text cursor should be and renders it.
     */
    void renderTextCursor(const SDL_Point& childOffset);

    /** The text that this box contains. Private since we must keep the cursor
        in sync with the text. */
    Text text;

    std::function<void(void)> onTextChanged;

    /** Tracks this button's current visual and logical state. */
    State currentState;

    /** The distance between the text and the edge of the text box on each
        side. */
    Margin margin;

    /** The accumulated time since we last toggled the text cursor's
        visibility. */
    double accumulatedBlinkTime;

    /** The color of the text cursor. */
    SDL_Color cursorColor;

    /** The width of the text cursor in pixels. */
    unsigned int cursorWidth;

    /** The character index in our text that the cursor is currently at. */
    unsigned int cursorIndex;

    /** Tracks whether the text cursor should be drawn or not. */
    bool cursorIsVisible;

    /** The current offset that we're adding to our text to account for
        scrolling. Does not include parent offsets or margins.
        Used in calculating whether the cursor index is within our bounds, or
        if the text needs to be scrolled. */
    int textScrollOffset;
};

} // namespace AUI
