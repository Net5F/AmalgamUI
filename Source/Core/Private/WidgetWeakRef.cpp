#include "AUI/WidgetWeakRef.h"
#include "AUI/Widget.h"
#include "AUI/Core.h"

namespace AUI
{

WidgetWeakRef::WidgetWeakRef(Widget& inWidget)
: widget{&inWidget}
, widgetIsValid{true}
{
    widget->trackRef(this);
}

WidgetWeakRef::WidgetWeakRef(const WidgetWeakRef& other)
: widget{other.widget}
, widgetIsValid{other.widgetIsValid}
{
    if (widgetIsValid) {
        widget->trackRef(this);
    }
}

WidgetWeakRef::WidgetWeakRef(WidgetWeakRef&& other)
: widget{other.widget}
, widgetIsValid{other.widgetIsValid}
{
    if (widgetIsValid) {
        widget->untrackRef(&other);
        other.widgetIsValid = false;

        widget->trackRef(this);
    }
}

WidgetWeakRef::~WidgetWeakRef()
{
    if (widgetIsValid) {
        widget->untrackRef(this);
    }
}

WidgetWeakRef& WidgetWeakRef::operator=(const WidgetWeakRef& other)
{
    if (widgetIsValid) {
        widget->untrackRef(this);
    }

    widget = other.widget;
    widgetIsValid = other.widgetIsValid;
    if (widgetIsValid) {
        widget->trackRef(this);
    }
    return *this;
}

WidgetWeakRef& WidgetWeakRef::operator=(WidgetWeakRef&& other)
{
    if (widgetIsValid) {
        widget->untrackRef(this);
    }

    widget = other.widget;
    widgetIsValid = other.widgetIsValid;
    if (widgetIsValid) {
        widget->untrackRef(&other);
        other.widgetIsValid = false;

        widget->trackRef(this);
    }
    return *this;
}

bool WidgetWeakRef::isValid() const
{
    return widgetIsValid;
}

Widget& WidgetWeakRef::get() const
{
    return *widget;
}

void WidgetWeakRef::invalidate()
{
    widgetIsValid = false;
}

} // End namespace AUI
