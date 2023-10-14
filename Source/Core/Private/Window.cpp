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

WidgetPath Window::getPathUnderWidget(Widget* widget)
{
    return widgetLocator.getPathUnderWidget(widget);
}

void Window::updateLayout()
{
    // Scale our logicalExtent to get our scaledExtent.
    // Windows don't have a parent, so scaledExtent is their final extent in
    // the layout.
    scaledExtent = ScalingHelpers::logicalToActual(logicalExtent);

    // The locator expects clippedExtent to be window-relative, so we need to
    // 0-out its position.
    fullExtent = scaledExtent;
    fullExtent.x = 0;
    fullExtent.y = 0;
    clippedExtent = fullExtent;

    // Clear the locator, to prepare for widgets re-adding themselves.
    widgetLocator.clear();

    // Update the locator to match our current screen extent.
    widgetLocator.setExtent(scaledExtent);

    // Add ourself to the locator.
    widgetLocator.addWidget(this);

    // Update our visible children's layouts and let them add themselves to
    // the locator.
    // Note: We skip invisible children since they won't be rendered or receive
    //       events.
    SDL_Rect availableExtent{scaledExtent};
    availableExtent.x = 0;
    availableExtent.y = 0;
    for (Widget& child : children) {
        if (child.getIsVisible()) {
            child.updateLayout({0, 0}, availableExtent, &widgetLocator);
        }
    }
}

bool Window::containsWidget(Widget* widget)
{
    return widgetLocator.containsWidget(widget);
}

void Window::render()
{
    // Render all visible children.
    for (Widget& child : children) {
        if (child.getIsVisible()) {
            child.render({scaledExtent.x, scaledExtent.y});
        }
    }
}

} // namespace AUI
