#include "AUI/Container.h"
#include "AUI/Internal/Log.h"
#include <algorithm>

namespace AUI
{
Container::Container(const SDL_Rect& inLogicalExtent,
                     const std::string& inDebugName)
: Widget(inLogicalExtent, inDebugName)
{
}

void Container::push_back(std::unique_ptr<Widget> newElement)
{
    elements.push_back(std::move(newElement));
}

void Container::erase(std::size_t index)
{
    if (elements.size() <= index) {
        AUI_LOG_FATAL("Tried to remove element that doesn't exist in "
                      "container. Index: %u, Size: %u",
                      index, elements.size());
        return;
    }

    elements.erase(elements.begin() + index);
}

void Container::erase(Widget* widget)
{
    // Try to find the given widget.
    auto widgetIt
        = std::find_if(elements.begin(), elements.end(),
                       [&widget](const std::unique_ptr<Widget>& other) {
                           return (widget == other.get());
                       });

    // If we found it, erase it.
    if (widgetIt != elements.end()) {
        elements.erase(widgetIt);
    }
    else {
        // We didn't find it, error.
        AUI_LOG_FATAL("Tried to remove element that doesn't exist in "
                      "container. Container name: %s, element name: %s",
                      debugName.c_str(), widget->getDebugName().c_str());
    }
}

void Container::clear()
{
    elements.clear();
}

std::unique_ptr<Widget>& Container::operator[](std::size_t index)
{
    if (elements.size() <= index) {
        AUI_LOG_FATAL("Given index is out of bounds. Index: %u, Size: %u",
                      index, elements.size());
    }

    return elements[index];
}

std::size_t Container::size()
{
    return elements.size();
}

void Container::onTick(double timestepS)
{
    // Call every visible element's onTick().
    for (std::unique_ptr<Widget>& element : elements) {
        if (element->getIsVisible()) {
            element->onTick(timestepS);
        }
    }
}

void Container::render(const SDL_Point& windowTopLeft)
{
    // If this widget is fully clipped, don't render it.
    if (SDL_RectEmpty(&clippedExtent)) {
        return;
    }

    // Run the normal render step (will render our children, but won't render 
    // any of our elements).
    Widget::render(windowTopLeft);

    // Render all visible elements.
    // Note: We skip invisible elements since they won't be rendered or receive
    //       events.
    for (std::unique_ptr<Widget>& element : elements) {
        if (element->getIsVisible()) {
            element->render(windowTopLeft);
        }
    }
}

} // namespace AUI
