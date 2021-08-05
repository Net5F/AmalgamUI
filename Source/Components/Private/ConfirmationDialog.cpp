#include "AUI/ConfirmationDialog.h"
#include "AUI/Screen.h"
#include "AUI/Core.h"

namespace AUI
{
ConfirmationDialog::ConfirmationDialog(Screen& inScreen,
                                       const SDL_Rect& inLogicalExtent,
                                       const std::string& inDebugName)
: Component(inScreen, inLogicalExtent, inDebugName)
, backgroundImage(inScreen, {0, 0, logicalExtent.w, logicalExtent.h})
, bodyText(inScreen, {0, 0, logicalExtent.w, logicalExtent.h})
, confirmButton(inScreen, {0, 0, logicalExtent.w, logicalExtent.h})
, cancelButton(inScreen, {0, 0, logicalExtent.w, logicalExtent.h})
{
    // Set the default cancel button behavior.
    cancelButton.setOnPressed([&]() {
        // Remove the dialog.
        setIsVisible(false);
    });
}

void ConfirmationDialog::render(const SDL_Point& parentOffset)
{
    // Keep our extent up to date.
    refreshScaling();

    // Children should render at the parent's offset + this component's offset.
    SDL_Point childOffset{parentOffset};
    childOffset.x += scaledExtent.x;
    childOffset.y += scaledExtent.y;

    // Save the extent that we're going to render at.
    lastRenderedExtent = scaledExtent;
    lastRenderedExtent.x += parentOffset.x;
    lastRenderedExtent.y += parentOffset.y;

    // If the component isn't visible, return without rendering.
    if (!isVisible) {
        return;
    }

    // Render our children.
    backgroundImage.render(childOffset);
    bodyText.render(childOffset);
    confirmButton.render(childOffset);
    cancelButton.render(childOffset);
}

} // namespace AUI
