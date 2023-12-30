#include "AUI/ConfirmationDialog.h"
#include "AUI/Screen.h"
#include "AUI/Core.h"

namespace AUI
{
ConfirmationDialog::ConfirmationDialog(const SDL_Rect& inLogicalExtent,
                                       const std::string& inDebugName)
: Window(inLogicalExtent, inDebugName)
, shadowImage({0, 0, logicalExtent.w, logicalExtent.h})
, backgroundImage({0, 0, logicalExtent.w, logicalExtent.h})
, bodyText({0, 0, logicalExtent.w, logicalExtent.h})
, confirmButton({0, 0, logicalExtent.w, logicalExtent.h})
, cancelButton({0, 0, logicalExtent.w, logicalExtent.h})
{
    // Add our children so they're included in rendering, etc.
    children.push_back(shadowImage);
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
