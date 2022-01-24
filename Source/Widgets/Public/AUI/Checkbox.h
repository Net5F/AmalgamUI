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
    enum class State { Unchecked, Checked };

    /**
     * Sets this checkbox's state to the given state.
     *
     * Intended to be used for updating this widget to match the underlying
     * data. As such, calling this doesn't trigger the associated callback.
     */
    void setCurrentState(State inState);

    //-------------------------------------------------------------------------
    // Public interface
    //-------------------------------------------------------------------------
    Checkbox(Screen& inScreen, const SDL_Rect& inLogicalExtent,
             const std::string& inDebugName = "");

    virtual ~Checkbox() = default;

    State getCurrentState();

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
    bool onMouseButtonDown(SDL_MouseButtonEvent& event) override;

    void render(const SDL_Point& parentOffset = {}) override;

private:
    //-------------------------------------------------------------------------
    // Private members
    //-------------------------------------------------------------------------
    std::function<void(void)> onChecked;

    std::function<void(void)> onUnchecked;

    /** Tracks this button's current visual and logical state. */
    State currentState;
};

} // namespace AUI