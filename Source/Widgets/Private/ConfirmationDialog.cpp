#include "AUI/ConfirmationDialog.h"
#include "AUI/Screen.h"
#include "AUI/Core.h"

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

} // namespace AUI
