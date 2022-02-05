#include "AUI/ConfirmationDialog.h"
#include "AUI/Screen.h"
#include "AUI/Core.h"
#include "AUI/Internal/Ignore.h"

namespace AUI
{
ConfirmationDialog::ConfirmationDialog(Screen& inScreen,
                                       const SDL_Rect& inLogicalExtent,
                                       const std::string& inDebugName)
: Widget(inScreen, inLogicalExtent, inDebugName)
, backgroundImage(inScreen, {0, 0, logicalExtent.w, logicalExtent.h})
, bodyText(inScreen, {0, 0, logicalExtent.w, logicalExtent.h})
, confirmButton(inScreen, {0, 0, logicalExtent.w, logicalExtent.h})
, cancelButton(inScreen, {0, 0, logicalExtent.w, logicalExtent.h})
{
    // Add our children so they're included in rendering, etc.
    children.push_back(backgroundImage);
    children.push_back(bodyText);
    children.push_back(confirmButton);
    children.push_back(cancelButton);

    // Set the default cancel button behavior.
    cancelButton.setOnPressed([&]() {
        // Remove the dialog.
        setIsVisible(false);
    });
}

Widget* ConfirmationDialog::onMouseButtonDown(SDL_MouseButtonEvent& event)
{
    ignore(event);
    return this;
}

Widget* ConfirmationDialog::onMouseButtonUp(SDL_MouseButtonEvent& event)
{
    ignore(event);
    return this;
}

Widget* ConfirmationDialog::onMouseWheel(SDL_MouseWheelEvent& event)
{
    ignore(event);
    return this;
}

Widget* ConfirmationDialog::onMouseMove(SDL_MouseMotionEvent& event)
{
    ignore(event);
    return this;
}

Widget* ConfirmationDialog::onKeyDown(SDL_KeyboardEvent& event)
{
    ignore(event);
    return this;
}

Widget* ConfirmationDialog::onTextInput(SDL_TextInputEvent& event)
{
    ignore(event);
    return this;
}

} // namespace AUI
