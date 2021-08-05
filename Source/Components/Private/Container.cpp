#include "AUI/Container.h"
#include "AUI/Internal/Log.h"
#include <algorithm>

namespace AUI
{
Container::Container(Screen& inScreen, const SDL_Rect& inLogicalExtent,
                     const std::string& inDebugName)
: Component(inScreen, inLogicalExtent, inDebugName)
{
}

void Container::push_back(std::unique_ptr<Component> newElement)
{
    elements.push_back(std::move(newElement));
}

void Container::erase(std::size_t index)
{
    if (elements.size() <= index) {
        AUI_LOG_ERROR("Tried to remove element that doesn't exist in "
                      "container. Index: %u, Size: %u",
                      index, elements.size());
        return;
    }

    elements.erase(elements.begin() + index);
}

void Container::erase(Component* component)
{
    // Try to find the given component.
    auto componentIt
        = std::find_if(elements.begin(), elements.end(),
                       [&component](const std::unique_ptr<Component>& other) {
                           return (component == other.get());
                       });

    // If we found it, erase it.
    if (componentIt != elements.end()) {
        elements.erase(componentIt);
    }
    else {
        // We didn't find it, error.
        AUI_LOG_ERROR("Tried to remove element that doesn't exist in "
                      "container. Container name: %s, element name: %s",
                      debugName.c_str(), component->getDebugName().c_str());
    }
}

void Container::clear()
{
    elements.clear();
}

Component& Container::operator[](std::size_t index)
{
    if (elements.size() <= index) {
        AUI_LOG_ERROR("Given index is out of bounds. Index: %u, Size: %u",
                      index, elements.size());
    }

    return *(elements[index]);
}

std::size_t Container::size()
{
    return elements.size();
}

} // namespace AUI
