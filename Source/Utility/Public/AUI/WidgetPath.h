#pragma once

#include "WidgetWeakRef.h"
#include <vector>

namespace AUI
{

class Widget;

/**
 * A path through the widget tree, in order from root-most (typically a
 * Window) to leaf-most (i.e. bottom-most to top-most when rendered).
 */
class WidgetPath
{
public:
    // TODO: Figure out the real interface for this
    void add(Widget* widget);

    void add(const WidgetWeakRef& widget);

    void insert(Widget* widget, unsigned int index);

    void remove(Widget* widget);

    /**
     * Returns the number of widgets in this path.
     */
    std::size_t size();

private:
    std::vector<WidgetWeakRef> widgetRefs;
};

} // End namespace AUI
