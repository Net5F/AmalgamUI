#pragma once

#include "WidgetWeakRef.h"
#include <vector>

namespace AUI
{

class Widget;

/**
 * A path through the widget tree, in order from bottom-most (typically a
 * Window) to leaf-most.
 */
class WidgetPath
{
public:
    // TODO: Figure out the real interface for this
    void add(Widget* widget);

    void insert(Widget* widget, unsigned int index);

    void remove(Widget* widget);

private:
    std::vector<WidgetWeakRef> widgetRefs;
};

} // End namespace AUI
