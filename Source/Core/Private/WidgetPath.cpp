#include "AUI/WidgetPath.h"
#include "AUI/Widget.h"
#include "AUI/Internal/Assert.h"

namespace AUI
{

void WidgetPath::push_back(Widget& widget)
{
    widgetRefs.emplace_back(widget);
}

WidgetPath::iterator WidgetPath::insert(iterator pos, Widget& widget)
{
    return widgetRefs.emplace(pos, widget);
}

WidgetPath::iterator WidgetPath::insert(const_iterator pos, Widget& widget)
{
    return widgetRefs.emplace(pos, widget);
}

WidgetPath::iterator WidgetPath::erase(iterator pos)
{
    return widgetRefs.erase(pos);
}

WidgetPath::iterator WidgetPath::erase(const_iterator pos)
{
    return widgetRefs.erase(pos);
}

void WidgetPath::clear()
{
    widgetRefs.clear();
}

WidgetWeakRef& WidgetPath::at(std::size_t pos)
{
    return widgetRefs.at(pos);
}

const WidgetWeakRef& WidgetPath::at(std::size_t pos) const
{
    return widgetRefs.at(pos);
}

WidgetWeakRef& WidgetPath::operator[](std::size_t pos)
{
    return widgetRefs[pos];
}

const WidgetWeakRef& WidgetPath::operator[](std::size_t pos) const
{
    return widgetRefs[pos];
}

WidgetWeakRef& WidgetPath::front()
{
    return widgetRefs.front();
}

const WidgetWeakRef& WidgetPath::front() const
{
    return widgetRefs.front();
}

WidgetWeakRef& WidgetPath::back()
{
    return widgetRefs.back();
}

const WidgetWeakRef& WidgetPath::back() const
{
    return widgetRefs.back();
}

bool WidgetPath::empty() const
{
    return widgetRefs.empty();
}

std::size_t WidgetPath::size()
{
    return widgetRefs.size();
}

WidgetPath::iterator WidgetPath::begin()
{
    return widgetRefs.begin();
}

WidgetPath::const_iterator WidgetPath::begin() const
{
    return widgetRefs.begin();
}

WidgetPath::const_iterator WidgetPath::cbegin() const
{
    return widgetRefs.begin();
}

WidgetPath::iterator WidgetPath::end()
{
    return widgetRefs.end();
}

WidgetPath::const_iterator WidgetPath::end() const
{
    return widgetRefs.end();
}

WidgetPath::const_iterator WidgetPath::cend() const
{
    return widgetRefs.end();
}

WidgetPath::reverse_iterator WidgetPath::rbegin()
{
    return widgetRefs.rbegin();
}

WidgetPath::const_reverse_iterator WidgetPath::rbegin() const
{
    return widgetRefs.rbegin();
}

WidgetPath::const_reverse_iterator WidgetPath::crbegin() const
{
    return widgetRefs.rbegin();
}

WidgetPath::reverse_iterator WidgetPath::rend()
{
    return widgetRefs.rend();
}

WidgetPath::const_reverse_iterator WidgetPath::rend() const
{
    return widgetRefs.rend();
}

WidgetPath::const_reverse_iterator WidgetPath::crend() const
{
    return widgetRefs.rend();
}

} // End namespace AUI
