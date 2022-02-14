#include "AUI/WidgetPath.h"
#include "AUI/Widget.h"
#include "AUI/Internal/Assert.h"

namespace AUI
{

void WidgetPath::add(Widget* widget)
{
    widgetRefs.emplace_back(*widget);
}

void WidgetPath::insert(Widget* widget, unsigned int index)
{
    AUI_ASSERT(widgetRefs.size() > index, "Index is out of bounds");

    widgetRefs.emplace((widgetRefs.begin() + index), *widget);
}

void WidgetPath::remove(Widget* widget)
{
    for (auto it = widgetRefs.begin(); it != widgetRefs.end(); ++it) {
        if (&(it->get()) == widget) {
            widgetRefs.erase(it);
            break;
        }
    }
}

} // End namespace AUI
