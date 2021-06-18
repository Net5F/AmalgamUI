#pragma once

#include "AUI/Component.h"
#include <vector>
#include <memory>

namespace AUI {

/**
 * Base class for container components.
 *
 * Containers are used to lay out components according to some defined logic.
 * For example, a VerticalGridContainer will lay out components in a grid that
 * grows vertically.
 */
class Container : public Component
{
public:
    using container_type = std::vector<std::unique_ptr<Component>>;
    using iterator = container_type::iterator;
    using const_iterator = container_type::const_iterator;

    virtual ~Container() = default;

    /**
     * Pushes the given component to the back of the container.
     */
    void push_back(std::unique_ptr<Component> newElement);

    /**
     * Erases the component at the given index.
     *
     * Errors in debug if the given index doesn't exist.
     * Does nothing in release if the given index doesn't exist.
     */
    void erase(std::size_t index);

    /**
     * Erases the given component.
     *
     * Errors in debug if the given component doesn't exist in this container.
     * Does nothing in release if the given component doesn't exist in this
     * container.
     */
    void erase(Component* component);

    /**
     * Returns the element at the given index.
     *
     * Errors in debug if the given index doesn't exist.
     * Accessing a nonexistant element through this operator is undefined
     * behavior.
     */
    Component& operator[](std::size_t index);

    /**
     * Returns the number of elements in this container.
     */
    std::size_t size();

    iterator begin() { return elements.begin(); };
    const_iterator begin() const { return elements.begin(); };
    iterator end() { return elements.end(); };
    const_iterator end() const { return elements.end(); };

protected:
    Container(Screen& screen, const char* key, const SDL_Rect& logicalExtent);

    /** This container's child elements. This container owns the elements in
        this vector and must render them according to its layout logic. */
    std::vector<std::unique_ptr<Component>> elements;
};

} // namespace AUI
