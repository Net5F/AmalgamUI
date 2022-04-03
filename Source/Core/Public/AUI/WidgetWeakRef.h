#pragma once

namespace AUI
{

class Widget;

/**
 * A weak reference to a Widget object.
 *
 * Registers itself with the associated widget.
 * If the widget is destructed first, it invalidates this reference.
 * If this reference is destructed first, it unregisters itself from the widget.
 *
 * Make sure to check isValid() before accessing the widget.
 *
 * Note: This class is not threadsafe. If you e.g. destruct a widget on
 *       one thread while calling isValid() on another, the result will
 *       not be synchronized. This shouldn't be a problem, since widget
 *       operations (events, layout, rendering) all logically have to
 *       occur on the same thread anyway (rendering while processing
 *       events doesn't make sense).
 */
class WidgetWeakRef
{
public:
    WidgetWeakRef(Widget& inWidget);

    WidgetWeakRef(const WidgetWeakRef& other);

    WidgetWeakRef(WidgetWeakRef&& other);

    ~WidgetWeakRef();

    WidgetWeakRef& operator=(const WidgetWeakRef& other);

    WidgetWeakRef& operator=(WidgetWeakRef&& other);

    /**
     * Returns true if the associated widget is still valid (i.e. if it's
     * still alive).
     */
    bool isValid() const;

    /**
     * Returns a reference to the associated widget.
     *
     * Note: Always call isValid() first to check if the widget is still alive.
     */
    Widget& get() const;

    /**
     * Marks the associated widget as invalid.
     *
     * Used by the Widget class. You likely don't want to call this directly.
     */
    void invalidate();

private:
    Widget* widget;

    /** If true, the widget reference is still valid. */
    bool widgetIsValid;
};

} // End namespace AUI
