#pragma once

#include "AUI/Window.h"
#include "AUI/Image.h"
#include "AUI/Text.h"
#include "AUI/Button.h"
#include <functional>

namespace AUI
{
/**
 * A simple confirmation dialog window.
 */
class ConfirmationDialog : public Window
{
public:
    ConfirmationDialog(const SDL_Rect& inLogicalExtent,
                       const std::string& inDebugName = "ConfirmationDialog");

    virtual ~ConfirmationDialog() = default;

    //-------------------------------------------------------------------------
    // Public child widgets
    //-------------------------------------------------------------------------
    /** Semi-transparent shadow image to obscure things that are behind the
        dialog. */
    Image shadowImage;

    /** The dialog's background image. */
    Image backgroundImage;

    /** Body text. Typically will prompt the user with a question that
        describes the decision they're making. */
    Text bodyText;

    /** Right-side confirmation button. */
    Button confirmButton;

    /** Left-side cancel button. By default, closes the dialog without
        performing any action. */
    Button cancelButton;
};

} // namespace AUI
