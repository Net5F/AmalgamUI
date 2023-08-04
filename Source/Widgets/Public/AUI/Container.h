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
     * Returns the element at the given index.
     *
     * Errors in debug if the given index doesn't exist.
     * Accessing a nonexistant element through this operator is undefined
     * behavior.
     */
    std::unique_ptr<Widget>& operator[](std::size_t index);

    std::unique_ptr<Widget>& front();
    std::unique_ptr<Widget>& back();

    iterator begin() { return elements.begin(); };
    const_iterator begin() const { return elements.begin(); };
    iterator end() { return elements.end(); };
    const_iterator end() const { return elements.end(); };

    /**
     * Returns the number of elements in this container.
     */
    std::size_t size();

    /**
     * Clears the container, removing all elements.
     */
    void clear();

    /**
     * Erases the widget at the given index.
     *
     * Errors in debug if the given index doesn't exist.
     * Does nothing in release if the given index doesn't exist.
     */
    void erase(std::size_t index);

    /**
     * Erases the elements in the range [first, last).
     *
     * Doesn't check the iterators before using them. Make sure they're valid.
     */
    void erase(const_iterator first, const_iterator last);

    /**
     * Erases the given widget.
     *
     * Errors if the given widget doesn't exist in this container.
     */
    void erase(Widget* widget);

    /**
     * Pushes the given widget to the back of the container.
     */
    void push_back(std::unique_ptr<Widget> newElement);

    // Note: We intentionally exclude resize() since it's easy to misuse. 
    //       (The new elements must immediately be set, or else the next 
    //       layout update will cause a crash.)

    //-------------------------------------------------------------------------
    // Base class overrides
    //-------------------------------------------------------------------------
    void onTick(double timestepS) override;

    void render(const SDL_Point& windowTopLeft) override;

protected:
    Container(const SDL_Rect& inLogicalExtent,
              const std::string& inDebugName = "Container");

    /** This container's child elements. This container owns the elements in
        this vector and must render them according to its layout logic.

        We can't reuse Widget::children because it only contains references.
        Containers must actually own their children. */
    std::vector<std::unique_ptr<Widget>> elements;
};

} // namespace AUI
