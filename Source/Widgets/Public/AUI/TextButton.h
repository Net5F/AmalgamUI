#pragma once

#include "AUI/Button.h"
#include "AUI/Image.h"
#include "AUI/Text.h"
#include <functional>

namespace AUI
{
/**
 * A line of text that can be hovered and clicked.
 *
 * This widget could be accomplished by exposing all mouse events on Button 
 * or Text, but it's more convenient to have it as its own widget with a 
 * minimal, convenient interface.
 */
class TextButton : public Widget
{
public:
    //-------------------------------------------------------------------------
    // Public interface
    //-------------------------------------------------------------------------
    TextButton(const SDL_Rect& inLogicalExtent,
           const std::string& inDebugName = "TextButton");

    virtual ~TextButton() = default;

    /**
     * Sets the text color for this widget's normal state.
     */
    void setNormalColor(const SDL_Color& color);

    /**
     * Sets the text color for when the mouse is hovering over this widget.
     */
    void setHoveredColor(const SDL_Color& color);

    /**
     * Sets the text color for when the mouse is pressing this widget.
     */
    void setPressedColor(const SDL_Color& color);

    /**
     * Sets the text color for when this widget is disabled.
     */
    void setDisabledColor(const SDL_Color& color);

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

    Button::State getCurrentState();

    //-------------------------------------------------------------------------
    // Public child widgets
    //-------------------------------------------------------------------------
    /** The main text widget. */
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
    void setCurrentState(Button::State inState);

    /** Colors for each button state. */
    SDL_Color normalColor;
    SDL_Color hoveredColor;
    SDL_Color pressedColor;
    SDL_Color disabledColor;

    std::function<void(void)> onPressed;
    std::function<void(void)> onReleased;

    /** Tracks this button's current visual and logical state. */
    Button::State currentState;
};

} // namespace AUI
