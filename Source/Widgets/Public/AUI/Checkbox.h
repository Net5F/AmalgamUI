#pragma once

#include "AUI/Image.h"
#include "AUI/Text.h"
#include <functional>

namespace AUI
{
/**
 * A simple checkbox.
 */
class Checkbox : public Widget
{
public:
    //-------------------------------------------------------------------------
    // Public definitions
    //-------------------------------------------------------------------------
    /**
     * Used to track the button's visual and logical state.
     */
    enum class State
    {
        Unchecked,
        Checked
    };

    //-------------------------------------------------------------------------
    // Public interface
    //-------------------------------------------------------------------------
    Checkbox(const SDL_Rect& inLogicalExtent,
             const std::string& inDebugName = "Checkbox");

    virtual ~Checkbox() = default;

    /**
     * Sets this checkbox's state to the given state.
     *
     * Intended to be used for updating this widget to match the underlying
     * data. As such, calling this doesn't trigger the associated callback.
     */
    void setCurrentState(State inState);

    State getCurrentState();

    //-------------------------------------------------------------------------
    // Public child widgets
    //-------------------------------------------------------------------------
    /** Image, unchecked state. */
    Image uncheckedImage;
    /** Image, checked state. */
    Image checkedImage;

    //-------------------------------------------------------------------------
    // Callback registration
    //-------------------------------------------------------------------------
    void setOnUnchecked(std::function<void(void)> inOnUnchecked);

    void setOnChecked(std::function<void(void)> inOnChecked);

    //-------------------------------------------------------------------------
    // Base class overrides
    //-------------------------------------------------------------------------
    EventResult onMouseDown(MouseButtonType buttonType,
                            const SDL_Point& cursorPosition) override;

private:
    std::function<void(void)> onChecked;

    std::function<void(void)> onUnchecked;

    /** Tracks this button's current visual and logical state. */
    State currentState;
};

} // namespace AUI
