#pragma once

#include "AUI/Image.h"
#include "AUI/Text.h"
#include "AUI/Padding.h"
#include <functional>

namespace AUI
{
/**
 * A box for displaying or inputting text.
 */
class TextInput : public Widget
{
public:
    // TODO: Mouse/ctrl+arrow text selection should be added. It should be
    //       fairly straightforward using positioning similar to the text
    //       scroll offset calcs. The graphic itself can just be a blue
    //       box drawn behind the text, or a semi-transparent box drawn
    //       in front of it.

    //-------------------------------------------------------------------------
    // Public definitions
    //-------------------------------------------------------------------------
    /**
     * Used to track the button's visual and logical state.
     */
    enum class State {
        Normal,  /*!< Normal state. Only mouse events are handled. */
        Hovered, /*!< The mouse is within our extent. */
        Focused, /*!< We were clicked on. Key press events are handled. */
        Disabled /*!< Disabled state. No events are handled. */
    };

    //-------------------------------------------------------------------------
    // Public interface
    //-------------------------------------------------------------------------
    TextInput(const SDL_Rect& inLogicalExtent,
              const std::string& inDebugName = "TextInput");

    virtual ~TextInput() = default;

    /**
     * Enables this text input.
     *
     * @post The text input will visually be in the Normal state and will
     *       respond to hover and click events.
     */
    void enable();

    /**
     * Disables this text input.
     *
     * @post The text input will visually be in the Disabled state and will
     *       ignore all events.
     */
    void disable();

    /**
     * Sets the distance between the text and the border of the text box on
     * each side.
     */
    void setPadding(Padding inLogicalPadding);

    /**
     * Sets the color of the text cursor.
     */
    void setCursorColor(const SDL_Color& inCursorColor);

    /**
     * Sets the width of the text cursor in pixels.
     */
    void setCursorWidth(unsigned int inCursorWidth);

    State getCurrentState();

    //-------------------------------------------------------------------------
    // Public child widgets
    //-------------------------------------------------------------------------
    /** Background image, normal state. */
    Image normalImage;
    /** Background image, hovered state. */
    Image hoveredImage;
    /** Background image, focused state. */
    Image focusedImage;
    /** Background image, disabled state. */
    Image disabledImage;

    //-------------------------------------------------------------------------
    // Limited public interface of private widgets
    //-------------------------------------------------------------------------
    /**
     * Sets our text to inText and updates the cursor.
     *
     * Note: This doesn't call onTextCommitted.
     */
    void setText(std::string_view inText);

    /** Calls text.asString(). */
    const std::string& getText();
    /** Calls text.setFont(). */
    void setTextFont(const std::string& fontPath, int size);
    /** Calls text.setColor(). */
    void setTextColor(const SDL_Color& inColor);

    //-------------------------------------------------------------------------
    // Callback registration
    //-------------------------------------------------------------------------
    /**
     * Sets a callback to be called when text is entered or deleted.
     */
    void setOnTextChanged(std::function<void(void)> inOnTextChanged);

    /**
     * Sets a callback to be called when either the enter key is pressed, or
     * this widget loses focus (the user clicks outside the box).
     */
    void setOnTextCommitted(std::function<void(void)> inOnTextChanged);

    //-------------------------------------------------------------------------
    // Base class overrides
    //-------------------------------------------------------------------------
    EventResult onMouseDown(MouseButtonType buttonType,
                            const SDL_Point& cursorPosition) override;

    EventResult onMouseDoubleClick(MouseButtonType buttonType,
                                   const SDL_Point& cursorPosition) override;

    void onMouseEnter() override;

    void onMouseLeave() override;

    EventResult onFocusGained() override;

    /**
     * If focus was lost for any reason other than the Escape key being
     * pressed, the onTextCommitted callback will be called (since losing
     * focus is counted as an implicit commit).
     */
    void onFocusLost(FocusLostType focusLostType) override;

    EventResult onKeyDown(SDL_Keycode keyCode) override;

    EventResult onKeyUp(SDL_Keycode keyCode) override;

    EventResult onTextInput(const std::string& inputText) override;

    void onTick(double timestepS) override;

    void render() override;

protected:
    /**
     * Overridden to properly scale our cursor size.
     */
    bool refreshScaling() override;

private:
    //-------------------------------------------------------------------------
    // Private definitions
    //-------------------------------------------------------------------------
    /** The text cursor's blink rate. Windows seems to default to 530ms, so
        it should work fine for us. */
    static constexpr double CURSOR_BLINK_RATE_S
        = 530 / static_cast<double>(1000);

    //-------------------------------------------------------------------------
    // Private members
    //-------------------------------------------------------------------------
    // Event handlers for key press events.
    EventResult handleBackspaceEvent();
    EventResult handleDeleteEvent();
    EventResult handleCopyEvent();
    EventResult handleCutEvent();
    EventResult handlePasteEvent();
    EventResult handleLeftEvent();
    EventResult handleRightEvent();
    EventResult handleHomeEvent();
    EventResult handleEndEvent();
    EventResult handleEnterEvent();

    /**
     * Sets currentState and updates child widget visibility.
     */
    void setCurrentState(State inState);

    /**
     * Re-calculates where the text should be scrolled to, based on the current
     * cursor index.
     */
    void refreshTextScrollOffset();

    /**
     * Calcs where the text cursor should be and renders it.
     */
    void renderTextCursor();

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

    /** The logical width of the text cursor in pixels. */
    unsigned int logicalCursorWidth;

    /** The scaled width of the text cursor in pixels. */
    unsigned int scaledCursorWidth;

    /** The character index in our text that the cursor is currently at. */
    std::size_t cursorIndex;

    /** Tracks whether the text cursor should be drawn or not. */
    bool cursorIsVisible;

    /** The last text string that was committed to this text input.
        Text is committed on Enter key press or focus loss (click away),
        but text is reverted to this string on Escape key press. */
    std::string lastCommittedText;

    //-------------------------------------------------------------------------
    // Private child widgets
    //-------------------------------------------------------------------------
    /** The text that this box contains. Private since we must keep the cursor
        in sync with the text. */
    Text text;
};

} // namespace AUI
