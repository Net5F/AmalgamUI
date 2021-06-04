#pragma once

#include "AUI/Image.h"
#include "AUI/Text.h"
#include "AUI/Margins.h"
#include <functional>

namespace AUI {

/**
 * A box for displaying or inputting text.
 */
class TextInput : public Component
{
public:
    // TODO: Mouse/ctrl+arrow text selection should be added. It should be
    //       fairly straightforward using positioning similar to the text
    //       scroll offset calcs. The graphic itself can just be a blue
    //       box drawn behind the text, or a semi-transparent box drawn
    //       in front of it.
    // TODO: Scaling is currently broken. When moving the cursor to the right,
    //       the text image blows up instead of the destination extent scaling
    //       properly.

    /**
     * Used to track the button's visual and logical state.
     */
    enum class State {
        Normal, /*!< Normal state. Only mouse events are handled. */
        Hovered, /*!< The mouse is within our extent. */
        Focused, /*!< We were clicked on. Key press events are handled. */
        Disabled /*!< Disabled state. No events are handled. */
    };

    TextInput(Screen& screen, const char* key, const SDL_Rect& inLogicalExtent);

    virtual ~TextInput() = default;

    //-------------------------------------------------------------------------
    // Public interface
    //-------------------------------------------------------------------------
    /**
     * Sets the distance between the text and the edge of the text box on each
     * side.
     */
    void setMargins(Margins inLogicalMargins);

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
    /**
     * Sets a callback to be called when text is entered or deleted.
     */
    void setOnTextChanged(std::function<void(void)> inOnTextChanged);

    /**
     * Sets a callback to be called when either the enter key is pressed, or
     * this component loses focus (the user clicks outside the box).
     */
    void setOnTextCommitted(std::function<void(void)> inOnTextChanged);

    //-------------------------------------------------------------------------
    // Base class overrides
    //-------------------------------------------------------------------------
    bool onMouseButtonDown(SDL_MouseButtonEvent& event) override;

    void onMouseMove(SDL_MouseMotionEvent& event) override;

    bool onKeyDown(SDL_KeyboardEvent& event) override;

    bool onTextInput(SDL_TextInputEvent& event) override;

    void onTick(double timestepS) override;

    void render(const SDL_Point& parentOffset = {}) override;

private:
    //-------------------------------------------------------------------------
    // Private definitions
    //-------------------------------------------------------------------------
    /** The text cursor's blink rate. Windows seems to default to 530ms, so
        it should work fine for us. */
    static constexpr double CURSOR_BLINK_RATE_S = 530 / static_cast<double>(1000);

    //-------------------------------------------------------------------------
    // Private members
    //-------------------------------------------------------------------------
    // Event handlers for key press events.
    bool handleBackspaceEvent();
    bool handleDeleteEvent();
    bool handleCopyEvent();
    bool handleCutEvent();
    bool handlePasteEvent();
    bool handleLeftEvent();
    bool handleRightEvent();
    bool handleHomeEvent();
    bool handleEndEvent();
    bool handleEnterEvent();

    /**
     * Puts this component in a focused state. Changes the image displayed,
     * activates the text cursor, and causes this component to start responding
     * to and generating additional.
     */
    void assumeFocus();

    /**
     * Puts this component back into a normal state. Changes the image
     * displayed, de-activates the text cursor, and causes this component to
     * stop responding to and generating events, other than those that can
     * result in re-assuming focus.
     * The onTextCommitted callback will be called, since losing focus is
     * counted as an implicit commit.
     */
    void removeFocus();

    /**
     * Re-calculates where the text should be scrolled to, based on the current
     * cursor index.
     */
    void refreshTextScrollOffset();

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

    /** See setOnTextChanged(). */
    std::function<void(void)> onTextChanged;

    /** See setOnTextCommitted(). */
    std::function<void(void)> onTextCommitted;

    /** Tracks this button's current visual and logical state. */
    State currentState;

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
};

} // namespace AUI
