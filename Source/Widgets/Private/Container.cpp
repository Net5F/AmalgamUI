#include "AUI/Container.h"
#include "AUI/Internal/Log.h"
#include <algorithm>

namespace AUI
{
Container::Container(Screen& inScreen, const SDL_Rect& inLogicalExtent,
                     const std::string& inDebugName)
: Widget(inScreen, inLogicalExtent, inDebugName)
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

Widget& Container::operator[](std::size_t index)
{
    if (elements.size() <= index) {
        AUI_LOG_FATAL("Given index is out of bounds. Index: %u, Size: %u",
                      index, elements.size());
    }

    return *(elements[index]);
}

std::size_t Container::size()
{
    return elements.size();
}

Widget* Container::handleOSEvent(SDL_Event& event)
{
    // Propagate the event through our visible elements.
    for (auto it = elements.rbegin(); it != elements.rend(); ++it)
    {
        // If the element isn't visible, skip it.
        std::unique_ptr<Widget>& element{*it};
        if (!(element->getIsVisible())) {
            continue;
        }

        // If the element consumed the event, return early.
        Widget* consumer{element->handleOSEvent(event)};
        if (consumer != nullptr) {
            return element.get();
        }
    }

    // None of our children handled the event. Try to handle it ourselves.
    switch (event.type) {
        case SDL_MOUSEBUTTONDOWN: {
            return onMouseButtonDown(event.button);
        }
        case SDL_MOUSEBUTTONUP: {
            return onMouseButtonUp(event.button);
        }
        case SDL_MOUSEMOTION: {
            return onMouseMove(event.motion);
        }
        case SDL_MOUSEWHEEL: {
            return onMouseWheel(event.wheel);
        }
        case SDL_KEYDOWN: {
            return onKeyDown(event.key);
        }
        case SDL_TEXTINPUT: {
            return onTextInput(event.text);
        }
        default:
            break;
    }

    return nullptr;
}

void Container::render()
{
    // Render all visible elements.
    for (std::unique_ptr<Widget>& element : elements)
    {
        if (element->getIsVisible()) {
            element->render();
        }
    }
}

} // namespace AUI
