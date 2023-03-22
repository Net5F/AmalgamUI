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

void Window::updateLayout(const SDL_Rect& parentExtent)
{
    // Keep our extent up to date.
    refreshScaling();

    // Calculate our new extent to render at.
    renderExtent = scaledExtent;

    // TODO: Can we get rid of this since windows don't have a parent?
    renderExtent.x += parentExtent.x;
    renderExtent.y += parentExtent.y;
    // TODO: Should we clip here to fit parentExtent?

    // Clear the stale widgets from the locator.
    widgetLocator.clear();

    // Update the locator to match our new screen extent.
    widgetLocator.setExtent(renderExtent);

    // Add ourself to the locator.
    widgetLocator.addWidget(this);

    // Update our visible children's layouts and add them to the locator.
    // Note: We skip invisible children since they won't be rendered. If we
    //       need to process invisible children (for the widget locator's use,
    //       perhaps), we can change this.
    for (Widget& child : children) {
        if (child.getIsVisible()) {
            child.updateLayout(renderExtent, &widgetLocator);
        }
    }
}

} // namespace AUI
