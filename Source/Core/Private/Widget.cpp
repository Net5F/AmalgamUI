#include "AUI/Widget.h"
#include "AUI/Screen.h"
#include "AUI/Image.h"
#include "AUI/Core.h"
#include "AUI/ScalingHelpers.h"
#include "AUI/SDLHelpers.h"
#include "AUI/WidgetWeakRef.h"
#include "AUI/WidgetLocator.h"
#include "AUI/Internal/Ignore.h"
#include "AUI/Internal/Log.h"
#include "AUI/Internal/AUIAssert.h"
#include <algorithm>

namespace AUI
{
Widget::Widget(const SDL_Rect& inLogicalExtent, const std::string& inDebugName)
: debugName{inDebugName}
, logicalExtent{inLogicalExtent}
, scaledExtent{ScalingHelpers::logicalToActual(logicalExtent)}
, renderExtent{}
, lastUsedScreenSize{Core::getActualScreenSize()}
, isVisible{true}
, isFocusable{false}
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

bool Widget::containsPoint(const SDL_Point& actualPoint)
{
    return SDLHelpers::pointInRect(actualPoint, renderExtent);
}

void Widget::setLogicalExtent(const SDL_Rect& inLogicalExtent)
{
    // Set our logical screen extent.
    logicalExtent = inLogicalExtent;

    // Re-calculate our scaled screen extent.
    scaledExtent = ScalingHelpers::logicalToActual(logicalExtent);
}

SDL_Rect Widget::getLogicalExtent() const
{
    return logicalExtent;
}

SDL_Rect Widget::getScaledExtent() const
{
    return scaledExtent;
}

SDL_Rect Widget::getRenderExtent() const
{
    return renderExtent;
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

bool Widget::getIsFocusable() const
{
    return isFocusable;
}

EventResult Widget::onPreviewMouseDown(MouseButtonType buttonType,
                                       const SDL_Point& cursorPosition)
{
    ignore(buttonType);
    ignore(cursorPosition);
    return EventResult{.wasHandled{false}};
}

EventResult Widget::onMouseDown(MouseButtonType buttonType,
                                const SDL_Point& cursorPosition)
{
    ignore(buttonType);
    ignore(cursorPosition);
    return EventResult{.wasHandled{false}};
}

EventResult Widget::onMouseUp(MouseButtonType buttonType,
                              const SDL_Point& cursorPosition)
{
    ignore(buttonType);
    ignore(cursorPosition);
    return EventResult{.wasHandled{false}};
}

EventResult Widget::onMouseDoubleClick(MouseButtonType buttonType,
                                       const SDL_Point& cursorPosition)
{
    ignore(buttonType);
    ignore(cursorPosition);
    return EventResult{.wasHandled{false}};
}

EventResult Widget::onMouseWheel(int amountScrolled)
{
    ignore(amountScrolled);
    return EventResult{.wasHandled{false}};
}

EventResult Widget::onMouseMove(const SDL_Point& cursorPosition)
{
    ignore(cursorPosition);
    return EventResult{.wasHandled{false}};
}

void Widget::onMouseEnter() {}

void Widget::onMouseLeave() {}

EventResult Widget::onFocusGained()
{
    return EventResult{.wasHandled{false}};
}

void Widget::onFocusLost(FocusLostType focusLostType)
{
    ignore(focusLostType);
}

EventResult Widget::onPreviewKeyDown(SDL_Keycode keyCode)
{
    ignore(keyCode);
    return EventResult{.wasHandled{false}};
}

EventResult Widget::onKeyDown(SDL_Keycode keyCode)
{
    ignore(keyCode);
    return EventResult{.wasHandled{false}};
}

EventResult Widget::onKeyUp(SDL_Keycode keyCode)
{
    ignore(keyCode);
    return EventResult{.wasHandled{false}};
}

EventResult Widget::onTextInput(const std::string& inputText)
{
    ignore(inputText);
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

void Widget::updateLayout(const SDL_Rect& parentExtent,
                          WidgetLocator* widgetLocator)
{
    // Keep our extent up to date.
    refreshScaling();

    // Calculate our new extent to render at.
    renderExtent = scaledExtent;
    renderExtent.x += parentExtent.x;
    renderExtent.y += parentExtent.y;
    // TODO: Should we clip here to fit parentExtent?

    // If we were given a valid locator, add ourselves to it.
    if (widgetLocator != nullptr) {
        widgetLocator->addWidget(this);
    }

    // Update our visible children's layouts and add them to the locator.
    // Note: We skip invisible children since they won't be rendered. If we
    //       need to process invisible children (for the widget locator's use,
    //       perhaps), we can change this.
    for (Widget& child : children) {
        if (child.getIsVisible()) {
            child.updateLayout(renderExtent, widgetLocator);
        }
    }
}

void Widget::render()
{
    // Render all visible children.
    for (Widget& child : children) {
        if (child.getIsVisible()) {
            child.render();
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

bool Widget::refreshScaling()
{
    // If the screen size has changed.
    if (lastUsedScreenSize != Core::getActualScreenSize()) {
        // Re-calculate our scaled extent.
        scaledExtent = ScalingHelpers::logicalToActual(logicalExtent);

        // Save the new size.
        lastUsedScreenSize = Core::getActualScreenSize();

        return true;
    }

    return false;
}

SDL_Rect Widget::calcClippedExtent(const SDL_Rect& sourceExtent,
                                   const SDL_Rect& clipExtent)
{
    // If the clipping extent has no width or height, don't clip.
    if ((clipExtent.w == 0) || (clipExtent.h == 0)) {
        AUI_LOG_INFO("Tried to clip using a clipExtent with either no width or"
                     " no height.");
        return sourceExtent;
    }

    // If we're beyond the left bound of clipExtent, set it as our x.
    SDL_Rect clippedExtent{sourceExtent};
    int leftDiff = clipExtent.x - sourceExtent.x;
    if (leftDiff > 0) {
        clippedExtent.x = clipExtent.x;
    }

    // If we're beyond the right bound of clipExtent, decrease width to fit.
    int rightDiff
        = (clippedExtent.x + clippedExtent.w) - (clipExtent.x + clipExtent.w);
    if (rightDiff > 0) {
        clippedExtent.w -= rightDiff;
    }

    // If we're beyond the top bound of clipExtent, set it as our y.
    int topDiff = clipExtent.y - sourceExtent.y;
    if (topDiff > 0) {
        clippedExtent.y = clipExtent.y;
    }

    // If we're beyond the bottom bound of clipExtent, decrease height to fit.
    int bottomDiff
        = (clippedExtent.y + clippedExtent.h) - (clipExtent.y + clipExtent.h);
    if (bottomDiff > 0) {
        clippedExtent.h -= bottomDiff;
    }

    return clippedExtent;
}

} // namespace AUI
