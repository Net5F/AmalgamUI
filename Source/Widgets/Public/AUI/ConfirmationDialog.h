#pragma once

#include "AUI/Image.h"
#include "AUI/Text.h"
#include "AUI/Button.h"
#include <functional>

namespace AUI
{
/**
 * A simple confirmation dialog.
 */
class ConfirmationDialog : public Widget
{
public:
    ConfirmationDialog(Screen& screen, const SDL_Rect& inLogicalExtent,
                       const std::string& inDebugName = "");

    virtual ~ConfirmationDialog() = default;

    //-------------------------------------------------------------------------
    // Public child widgets
    //-------------------------------------------------------------------------
    /** Dialog background image. */
    AUI::Image backgroundImage;

    /** Body text. Typically will prompt the user with a question that
        describes the decision they're making. */
    AUI::Text bodyText;

    /** Right-side confirmation button. */
    AUI::Button confirmButton;

    /** Left-side cancel button. By default, closes the dialog without
        performing any action. */
    AUI::Button cancelButton;

    //-------------------------------------------------------------------------
    // Base class overrides
    //-------------------------------------------------------------------------
    // This dialog blocks all events.
    Widget* onMouseButtonDown(SDL_MouseButtonEvent& event) override;
    Widget* onMouseButtonUp(SDL_MouseButtonEvent& event) override;
    Widget* onMouseWheel(SDL_MouseWheelEvent& event) override;
    Widget* onMouseMove(SDL_MouseMotionEvent& event) override;
    Widget* onKeyDown(SDL_KeyboardEvent& event) override;
    Widget* onTextInput(SDL_TextInputEvent& event) override;
};

} // namespace AUI
