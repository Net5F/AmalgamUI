#pragma once

#include "WidgetWeakRef.h"
#include <vector>

namespace AUI
{

class Widget;

/**
 * A path of widgets progressing down the widget tree, from root-most to leaf-
 * most (i.e. farthest back in the final rendered screen to farthest forward).
 *
 * The front of a path will typically be a Window.
 */
class WidgetPath
{
public:
    using iterator = std::vector<WidgetWeakRef>::iterator;
    using const_iterator = std::vector<WidgetWeakRef>::const_iterator;
    using reverse_iterator = std::vector<WidgetWeakRef>::reverse_iterator;
    using const_reverse_iterator = std::vector<WidgetWeakRef>::const_reverse_iterator;

    //-------------------------------------------------------------------------
    // Constructors, Assignment
    //-------------------------------------------------------------------------
    /**
     * Constructs an empty path.
     */
    WidgetPath();

    /**
     * Copy constructor. Constructs the path with the contents of other.
     */
    WidgetPath(const WidgetPath& other);

    /**
     * Move constructor. Constructs the path with the contents of other using
     * move semantics.
     */
    WidgetPath(WidgetPath&& other);

    /**
     * Constructs the path with the contents of the range [first, last).
     */
    WidgetPath(iterator first, iterator last);

    /**
     * Copy assignment operator. Replaces the contents of the path with the
     * contents of other.
     */
    WidgetPath& operator=(const WidgetPath& other);

    /**
     * Move assignment operator. Replaces the contents of the path with the
     * contents of other using move semantics.
     */
    WidgetPath& operator=(WidgetPath&& other);

    //-------------------------------------------------------------------------
    // Modifiers
    //-------------------------------------------------------------------------
    /**
     * Pushes a reference to the given widget to the back of the path.
     *
     * Note: Don't confuse what "back" means. Widget paths progress down the
     *       widget tree, so "back" means farthest from the root.
     */
    void push_back(Widget& widget);

    iterator insert(iterator pos, Widget& widget);
    iterator insert(const_iterator pos, Widget& widget);

    /**
     * Removes the element at pos.
     */
    iterator erase(iterator pos);
    iterator erase(const_iterator pos);

    /**
     * Erases all widgets from the path.
     */
    void clear();

    //-------------------------------------------------------------------------
    // Element access
    //-------------------------------------------------------------------------
    WidgetWeakRef& at(std::size_t pos);
    const WidgetWeakRef& at(std::size_t pos) const;

    WidgetWeakRef& operator[](std::size_t pos);
    const WidgetWeakRef& operator[](std::size_t pos) const;

    /**
     * Returns the widget at the front of the path.
     *
     * Note: Don't confuse what "front" means. Widget paths progress down the
     *       widget tree, so "front" means closest to the root.
     */
    WidgetWeakRef& front();
    const WidgetWeakRef& front() const;

    /**
     * Returns the widget at the back of the path.
     *
     * Note: Don't confuse what "back" means. Widget paths progress down the
     *       widget tree, so "back" means farthest from the root.
     */
    WidgetWeakRef& back();
    const WidgetWeakRef& back() const;

    //-------------------------------------------------------------------------
    // Capacity
    //-------------------------------------------------------------------------
    /**
     * Returns true if the path is empty.
     */
    bool empty() const;

    /**
     * Returns the number of widgets in this path.
     */
    std::size_t size();

    //-------------------------------------------------------------------------
    // Lookup
    //-------------------------------------------------------------------------
    /**
     * Iterator to the first element referencing the given widget. If no such
     * element is found, past-the-end (see end()) iterator is returned.
     *
     * Note: Doesn't check if the widget reference is still valid.
     */
    iterator find(const Widget* widget);
    const_iterator find(const Widget* widget) const;

    /**
     * true if the given widget was found in this path, otherwise false.
     *
     * Note: Doesn't check if the widget reference is still valid.
     */
    bool contains(const Widget* widget) const;

    //-------------------------------------------------------------------------
    // Iterators
    //-------------------------------------------------------------------------
    iterator begin();
    const_iterator begin() const;
    const_iterator cbegin() const;

    iterator end();
    const_iterator end() const;
    const_iterator cend() const;

    reverse_iterator rbegin();
    const_reverse_iterator rbegin() const;
    const_reverse_iterator crbegin() const;

    reverse_iterator rend();
    const_reverse_iterator rend() const;
    const_reverse_iterator crend() const;

private:
    std::vector<WidgetWeakRef> widgetRefs;
};

} // End namespace AUI
