#include "AUI/Window.h"
#include "AUI/ScalingHelpers.h"
#include "AUI/Internal/Log.h"

namespace AUI
{
Window::Window(const SDL_Rect& inLogicalExtent, const std::string& inDebugName)
: Widget(inLogicalExtent, inDebugName)
, widgetLocator{ScalingHelpers::logicalToActual(inLogicalExtent)}
{
}

WidgetPath Window::getPathUnderPoint(const SDL_Point& actualPoint)
{
    return widgetLocator.getPathUnderPoint(actualPoint);
}

void Window::tick(double timestepS)
{
    // Call every visible child's onTick().
    for (Widget& child : children) {
        if (!(child.getIsVisible())) {
            continue;
        }

        child.onTick(timestepS);
    }
}

void Window::updateLayout()
{
    // Scale our logicalExtent to get our scaledExtent.
    scaledExtent = ScalingHelpers::logicalToActual(logicalExtent);

    // Windows don't have a parent, so scaledExtent is their final extent in 
    // the layout.
    fullExtent = scaledExtent;
    clippedExtent = scaledExtent;

    // Clear the locator, to prepare for widgets re-adding themselves.
    widgetLocator.clear();

    // Update the locator to match our current screen extent.
    widgetLocator.setExtent(scaledExtent);

    // Add ourself to the locator.
    widgetLocator.addWidget(this);

    // Update our visible children's layouts and let them add themselves to 
    // the locator.
    // Note: We skip invisible children since they won't be rendered. If we
    //       need to process invisible children (for the widget locator's use,
    //       perhaps), we can change this.
    for (Widget& child : children) {
        if (child.getIsVisible()) {
            child.updateLayout({scaledExtent.x, scaledExtent.y}, scaledExtent,
                               &widgetLocator);
        }
    }
}

} // namespace AUI
