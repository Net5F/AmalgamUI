#pragma once

#include "AUI/Widget.h"
#include <vector>
#include <memory>

namespace AUI
{
/**
 * Base class for container widgets.
 *
 * Containers are used to lay out widgets according to some defined logic.
 * For example, a VerticalGridContainer will lay out widgets in a grid that
 * grows vertically.
 */
class Container : public Widget
{
public:
    using container_type = std::vector<std::unique_ptr<Widget>>;
    using iterator = container_type::iterator;
    using const_iterator = container_type::const_iterator;

    virtual ~Container() = default;

    /**
     * Pushes the given widget to the back of the container.
     */
    void push_back(std::unique_ptr<Widget> newElement);

    /**
     * Erases the widget at the given index.
     *
     * Errors in debug if the given index doesn't exist.
     * Does nothing in release if the given index doesn't exist.
     */
    void erase(std::size_t index);

    /**
     * Erases the given widget.
     *
     * Errors in debug if the given widget doesn't exist in this container.
     * Does nothing in release if the given widget doesn't exist in this
     * container.
     */
    void erase(Widget* widget);

    /**
     * Clears the container, removing all elements.
     */
    void clear();

    /**
     * Returns the element at the given index.
     *
     * Errors in debug if the given index doesn't exist.
     * Accessing a nonexistant element through this operator is undefined
     * behavior.
     */
    Widget& operator[](std::size_t index);

    /**
     * Returns the number of elements in this container.
     */
    std::size_t size();

    iterator begin() { return elements.begin(); };
    const_iterator begin() const { return elements.begin(); };
    iterator end() { return elements.end(); };
    const_iterator end() const { return elements.end(); };

protected:
    Container(Screen& screen, const SDL_Rect& inLogicalExtent,
              const std::string& inDebugName = "");

    /** This container's child elements. This container owns the elements in
        this vector and must render them according to its layout logic. */
    std::vector<std::unique_ptr<Widget>> elements;
};

} // namespace AUI