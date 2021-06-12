#pragma once

#include "AUI/Component.h"
#include "AUI/Internal/Log.h"
#include <vector>
#include <memory>

namespace AUI {

/**
 * Base class for container components.
 */
class Container : public Component
{
public:
    Container(Screen& screen, const char* key, const SDL_Rect& logicalExtent)
    : Component(screen, key, logicalExtent)
    {
    }

    virtual ~Container() = default;

    /**
     * Pushes the given component to the back of the container.
     */
    void push_back(std::unique_ptr<Component> newElement)
    {
        elements.push_back(std::move(newElement));
    }

    /**
     * Erases the component at the given index.
     *
     * Errors in debug if the given index doesn't exist.
     * Does nothing in release if the given index doesn't exist.
     */
    void erase(std::size_t index) {
        if (elements.size() <= index) {
            AUI_LOG_ERROR("Tried to remove element that doesn't exist in "
            "container. Index: %u, Size: %u", index, elements.size());
            return;
        }

        elements.erase(elements.begin() + index);
    }

    /**
     * Erases the given component.
     *
     * Errors in debug if the given component doesn't exist in this container.
     * Does nothing in release if the given component doesn't exist in this
     * container.
     */
    void erase(Component* component) {
        // Try to find the given component.
        auto componentIt = std::find_if(elements.begin(), elements.end()
                       , [&component](const std::unique_ptr<Component>& other) {
                           return (component == other.get());
                       });

        // If we found it, erase it.
        if (componentIt != elements.end()) {
            elements.erase(componentIt);
        }
        else {
            // We didn't find it, error.
            AUI_LOG_ERROR("Tried to remove element that doesn't exist in "
            "container. Container name: %s, element name: %s", key.data(), component->getKey().data());
        }
    }

    /**
     * Returns the element at the given index.
     *
     * Errors in debug if the given index doesn't exist.
     * Accessing a nonexistant element through this operator is undefined
     * behavior.
     */
    Component& operator[](std::size_t index) {
        if (elements.size() <= index) {
            AUI_LOG_ERROR("Given index is out of bounds. Index: %u, Size: %u"
                , index, elements.size());
        }

        return *(elements[index]);
    }

    /**
     * Returns the number of elements in this container.
     */
    std::size_t size() {
        return elements.size();
    }

protected:
    std::vector<std::unique_ptr<Component>> elements;
};

} // namespace AUI
