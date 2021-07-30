#pragma once

#include "AUI/Image.h"
#include "AUI/Text.h"
#include "AUI/Button.h"
#include <functional>

namespace AUI {

/**
 * A simple confirmation dialog.
 */
class ConfirmationDialog : public Component
{
public:
    //-------------------------------------------------------------------------
    // Public interface
    //-------------------------------------------------------------------------
    ConfirmationDialog(Screen& screen, const SDL_Rect& inLogicalExtent, const std::string& inDebugName = "");

    virtual ~ConfirmationDialog() = default;

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
    void render(const SDL_Point& parentOffset = {}) override;
};

} // namespace AUI
