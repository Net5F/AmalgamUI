#include "AUI/Widget.h"
#include "AUI/Core.h"
#include "AUI/ScalingHelpers.h"
#include "AUI/SDLHelpers.h"
#include "AUI/WidgetWeakRef.h"
#include "AUI/WidgetLocator.h"
#include "AUI/Image.h"
#include "AUI/DragDropData.h"
#include "AUI/Internal/Log.h"
#include "AUI/Internal/AUIAssert.h"
#include <algorithm>

namespace AUI
{
Widget::Widget(const SDL_Rect& inLogicalExtent, const std::string& inDebugName)
: debugName{inDebugName}
, logicalExtent{inLogicalExtent}
, scaledExtent{ScalingHelpers::logicalToActual(logicalExtent)}
, fullExtent{scaledExtent}
, clippedExtent{fullExtent}
, isVisible{true}
, isFocusable{false}
, dragDropData{nullptr}
, children{}
, trackedRefs{}
{
    Core::incWidgetCount();
}

Widget::~Widget()
{
    for (WidgetWeakRef* ref : trackedRefs) {
        ref->invalidate();
    }

    Core::decWidgetCount();
}

bool Widget::containsPoint(const SDL_Point& windowPoint)
{
    return SDLHelpers::pointInRect(windowPoint, clippedExtent);
}

void Widget::setLogicalExtent(const SDL_Rect& inLogicalExtent)
{
    // Set our logical screen extent.
    logicalExtent = inLogicalExtent;

    // Update our scaled extent.
    scaledExtent = ScalingHelpers::logicalToActual(logicalExtent);

    // TODO: Invalidate the layout
}

SDL_Rect Widget::getLogicalExtent() const
{
    return logicalExtent;
}

SDL_Rect Widget::getScaledExtent() const
{
    return scaledExtent;
}

SDL_Rect Widget::getFullExtent() const
{
    return fullExtent;
}

SDL_Rect Widget::getClippedExtent() const
{
    return clippedExtent;
}

const std::string& Widget::getDebugName() const
{
    return debugName;
}

void Widget::setIsVisible(bool inIsVisible)
{
    isVisible = inIsVisible;
}

bool Widget::getIsVisible() const
{
    return isVisible;
}

void Widget::setIsFocusable(bool inIsFocusable)
{
    isFocusable = inIsFocusable;
}

bool Widget::getIsFocusable() const
{
    return isFocusable;
}

Image* Widget::getDragDropImage()
{
    return nullptr;
}

void Widget::setDragDropData(std::unique_ptr<DragDropData> inDragDropData)
{
    dragDropData = std::move(inDragDropData);
}

const DragDropData* Widget::getDragDropData() const
{
    return dragDropData.get();
}

bool Widget::getIsDragDroppable()
{
    return (getDragDropImage() && getDragDropData());
}

EventResult Widget::onPreviewMouseDown(MouseButtonType, const SDL_Point&)
{
    return EventResult{.wasHandled{false}};
}

EventResult Widget::onMouseDown(MouseButtonType, const SDL_Point&)
{
    return EventResult{.wasHandled{false}};
}

EventResult Widget::onMouseUp(MouseButtonType, const SDL_Point&)
{
    return EventResult{.wasHandled{false}};
}

EventResult Widget::onMouseDoubleClick(MouseButtonType, const SDL_Point&)
{
    return EventResult{.wasHandled{false}};
}

EventResult Widget::onMouseWheel(int)
{
    return EventResult{.wasHandled{false}};
}

EventResult Widget::onMouseMove(const SDL_Point&)
{
    return EventResult{.wasHandled{false}};
}

void Widget::onMouseEnter() {}

void Widget::onMouseLeave() {}

EventResult Widget::onFocusGained()
{
    return EventResult{.wasHandled{false}};
}

void Widget::onFocusLost(FocusLostType)
{
}

EventResult Widget::onPreviewKeyDown(SDL_Keycode)
{
    return EventResult{.wasHandled{false}};
}

EventResult Widget::onKeyDown(SDL_Keycode)
{
    return EventResult{.wasHandled{false}};
}

EventResult Widget::onKeyUp(SDL_Keycode)
{
    return EventResult{.wasHandled{false}};
}

EventResult Widget::onTextInput(const std::string&)
{
    return EventResult{.wasHandled{false}};
}

void Widget::onDragStart() {}

void Widget::onDragEnd() {}

EventResult Widget::onDragMove(const SDL_Point&)
{
    return EventResult{.wasHandled{false}};
}

void Widget::onDragEnter() {}

void Widget::onDragLeave() {}

EventResult Widget::onDrop(const DragDropData&)
{
    return EventResult{.wasHandled{false}};
}

void Widget::onTick(double timestepS)
{
    // Call every visible child's onTick().
    for (Widget& child : children) {
        if (child.getIsVisible()) {
            child.onTick(timestepS);
        }
    }
}

void Widget::updateLayout(const SDL_Point& startPosition,
                          const SDL_Rect& availableExtent,
                          WidgetLocator* widgetLocator)
{
    // Scale our logicalExtent to get our scaledExtent.
    scaledExtent = ScalingHelpers::logicalToActual(logicalExtent);

    // Offset our scaledExtent to get our fullExtent.
    fullExtent = scaledExtent;
    fullExtent.x += startPosition.x;
    fullExtent.y += startPosition.y;

    // Clip fullExtent to the available space to get our clippedExtent.
    SDL_Rect intersectionResult{};
    if (SDL_IntersectRect(&fullExtent, &availableExtent, &intersectionResult)) {
        clippedExtent = intersectionResult;
    }
    else {
        // fullExtent does not intersect availableExtent (e.g. this widget 
        // is fully clipped). Zero-out clippedExtent and return early.
        clippedExtent = {0, 0, 0, 0};
        return;
    }

    // If we were given a valid locator, add ourselves to it.
    if (widgetLocator != nullptr) {
        widgetLocator->addWidget(this);
    }

    // Update our visible children's layouts and let them add themselves to
    // the locator.
    // Note: We skip invisible children since they won't be rendered or receive
    //       events.
    for (Widget& child : children) {
        if (child.getIsVisible()) {
            child.updateLayout({fullExtent.x, fullExtent.y},
                               clippedExtent, widgetLocator);
        }
    }
}

void Widget::render(const SDL_Point& windowTopLeft)
{
    // If this widget is fully clipped, don't render it.
    if (SDL_RectEmpty(&clippedExtent)) {
        return;
    }

    // Render all visible children.
    for (Widget& child : children) {
        if (child.getIsVisible()) {
            child.render(windowTopLeft);
        }
    }
}

void Widget::trackRef(WidgetWeakRef* ref)
{
    trackedRefs.push_back(ref);
}

void Widget::untrackRef(WidgetWeakRef* ref)
{
    auto it{std::find(trackedRefs.begin(), trackedRefs.end(), ref)};
    AUI_ASSERT(
        it != trackedRefs.end(),
        "Tried to untrack ref that didn't exist in list of widget: %s - %p",
        debugName.c_str(), ref);
    trackedRefs.erase(it);
}

std::size_t Widget::getRefCount()
{
    return trackedRefs.size();
}

} // namespace AUI
