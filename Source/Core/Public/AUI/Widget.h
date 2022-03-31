#pragma once

#include "AUI/ScreenResolution.h"
#include "AUI/MouseButtonType.h"
#include "AUI/FocusLostType.h"
#include "AUI/EventResult.h"
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_events.h>
#include <string>
#include <array>
#include <vector>
#include <functional>

namespace AUI
{
class Screen;
class WidgetWeakRef;
class WidgetLocator;

/**
 * The base class for all UI widgets.
 *
 * Widgets are the third layer of UI construct used by this library. Screens
 * own a stack of Windows, Windows own a list of Widgets (and Widgets can
 * own child Widgets).
 *
 * Users can initialize a widget by following the pattern:
 *   - In the constructor, pass through the data necessary for Widget's
 *     constructor.
 *   - Use setters for all other data.
 *
 * Note: Widgets must not overlap their siblings (extents must not intersect).
 *       Parents must fully overlap their children.
 * Note: Widgets have no concept of reordering their children based on events.
 *       For example, if you want a widget to come to the front of the screen
 *       when it's clicked, you should instead put it in a separate Window.
 */
class Widget
{
public:
    /** Widget is not independently constructible. */
    Widget() = delete;

    virtual ~Widget();

    /**
     * If this widget's screen extent contains the given point, returns
     * true. Else, returns false.
     *
     * @param actualPoint  A point in actual screen space.
     */
    bool containsPoint(const SDL_Point& actualPoint);

    /**
     * Sets the widget's logical extent to the given extent and
     * re-calculates its scaled extent.
     */
    virtual void setLogicalExtent(const SDL_Rect& inLogicalExtent);

    /** See Widget::logicalExtent. */
    SDL_Rect getLogicalExtent() const;
    /** See Widget::scaledExtent. */
    SDL_Rect getScaledExtent() const;
    /** See Widget::renderExtent. */
    SDL_Rect getRenderExtent() const;

    const std::string& getDebugName() const;

    /** See Widget::isVisible. */
    virtual void setIsVisible(bool inIsVisible);
    bool getIsVisible() const;

    /** See Widget::isFocusable. */
    bool getIsFocusable() const;

    /**
     * Called during the tunneling preview pass for a MouseDown event.
     *
     * Note: If this event is handled, it will also stop the MouseDown event
     *       from bubbling afterwards.
     *
     * @param cursorPosition  The cursor's position, relative to the top left
     *                        of the screen.
     */
    virtual EventResult onPreviewMouseDown(MouseButtonType buttonType, const SDL_Point& cursorPosition);

    /**
     * Called when a mouse click occurs on this widget.
     *
     * This event is bubbled to widgets under the mouse.
     *
     * @param cursorPosition  The cursor's position, relative to the top left
     *                        of the screen.
     */
    virtual EventResult onMouseDown(MouseButtonType buttonType, const SDL_Point& cursorPosition);

    /**
     * Called when a mouse click is released.
     *
     * This event is only routed to the widget that is capturing the mouse.
     *
     * @param cursorPosition  The cursor's position, relative to the top left
     *                        of the screen.
     */
    virtual EventResult onMouseUp(MouseButtonType buttonType, const SDL_Point& cursorPosition);

    /**
     * Called when a mouse double click (or triple click, or more) occurs on
     * this widget.
     *
     * This event is bubbled to widgets under the mouse.
     *
     * @param cursorPosition  The cursor's position, relative to the top left
     *                        of the screen.
     */
    virtual EventResult onMouseDoubleClick(MouseButtonType buttonType, const SDL_Point& cursorPosition);

    /**
     * Called when the mouse wheel is scrolled while the cursor is over this
     * widget.
     *
     * This event is routed to the widget that is capturing the mouse. If
     * there's no mouse captor, it's bubbled to widgets under the mouse.
     *
     * @param amountScrolled  The amount that the wheel was scrolled. Movements
     *                        up (scroll forward) generate positive values,
     *                        movements down (scroll backward) generate
     *                        negative values.
     */
    virtual EventResult onMouseWheel(int amountScrolled);

    /**
     * Called when the mouse cursor moves within this widget's bounds.
     *
     * This event is routed to the widget that is capturing the mouse. If
     * there's no mouse captor, it's bubbled to widgets under the mouse.
     *
     * @param cursorPosition  The cursor's position, relative to the top left
     *                        of the screen.
     */
    virtual EventResult onMouseMove(const SDL_Point& cursorPosition);

    /**
     * Called when the mouse cursor first enters this widget's bounds.
     *
     * This event is routed to widgets that are newly under the mouse.
     */
    virtual void onMouseEnter();

    /**
     * Called when the mouse cursor leaves this widget's bounds.
     *
     * This event is routed to widgets that were previously under the mouse.
     */
    virtual void onMouseLeave();

    /**
     * Called when this widget is focused.
     *
     * This event is routed to the newly focused widget when focus is set.
     *
     * See Widget::isFocusable.
     */
    virtual EventResult onFocusGained();

    /**
     * Called when focus is lost on this widget.
     *
     * This event is routed to the previously focused widget when focus is
     * cleared or changed.
     */
    virtual void onFocusLost(FocusLostType focusLostType);

    /**
     * Called during the tunneling preview pass for a KeyDown event.
     *
     * Note: If this event is handled, it will also stop the KeyDown event
     *       from bubbling afterwards.
     */
    virtual EventResult onPreviewKeyDown(SDL_Keycode keyCode);

    /**
     * Called when a key is pressed while this widget has focus.
     *
     * This event is routed only to the currently focused widget.
     */
    virtual EventResult onKeyDown(SDL_Keycode keyCode);

    /**
     * Called when a key is released while this widget has focus.
     *
     * This event is routed only to the currently focused widget.
     */
    virtual EventResult onKeyUp(SDL_Keycode keyCode);

    /**
     * Called when a text character is committed.
     *
     * Note: This currently doesn't support text composition systems (such as
     *       for Japanese text), but we will eventually add such support.
     *
     * @param inputText  The input text in UTF-8 encoding.
     */
    virtual EventResult onTextInput(const std::string& inputText);

    /**
     * Called when the current screen's tick() is called.
     *
     * Note: If you override this function, you must call onTick() on all of
     *       your children. You can do this by calling Widget::onTick().
     *
     * @param timestepS  The amount of time that has passed since the last
     *                   tick() call, in seconds.
     */
    virtual void onTick(double timestepS);

    /**
     * Refreshes this widget's scaling, and updates its renderExtent.
     *
     * @param parentExtent  The parent widget's renderExtent.
     *                      Child widgets should position themselves relative
     *                      to their parent's position and clip themselves to
     *                      their parent's bounds (unless intentionally
     *                      overflowing).
     * @param widgetLocator  (If non-nullptr) The widget locator for this
     *                       widget to add itself to after updating.
     * @post renderExtent is properly positioned for use in rendering and
     *       hit testing.
     */
    virtual void updateLayout(const SDL_Rect& parentExtent, WidgetLocator* widgetLocator);

    /**
     * Renders this widget to the current rendering target.
     *
     * The default implementation simply calls render() on all widgets in our
     * children list. Some overrides may directly call SDL functions like
     * SDL_RenderCopy().
     *
     * @param parentOffset  The offset that should be added to this widget's
     *                      position before rendering. Used by parent classes
     *                      to control the layout of their children.
     */
    virtual void render();

    /**
     * Internal library function.
     * Used by WidgetWeakRef to register a reference.
     */
    void trackRef(WidgetWeakRef* ref);

    /**
     * Internal library function.
     * Used by WidgetWeakRef to unregister a reference.
     */
    void untrackRef(WidgetWeakRef* ref);

    /**
     * Internal library function.
     * Used by tests to verify that refs are being tracked correctly.
     */
    std::size_t getRefCount();

protected:
    Widget(Screen& inScreen, const SDL_Rect& inLogicalExtent,
              const std::string& inDebugName);

    /**
     * Checks if Core::actualScreenSize has changed since the last time this
     * widget's scaledExtent was calculated. If so, re-calculates
     * scaledExtent, scaling it to the new actualScreenSize.
     *
     * This implementation is sufficient for refreshing actualExtent, but must
     * be overridden if your widget has other scaling needs.
     */
    virtual bool refreshScaling();

    /**
     * Returns an extent equal to sourceExtent, adjusted to not go beyond the
     * bounds of clipExtent.
     */
    SDL_Rect calcClippedExtent(const SDL_Rect& sourceExtent,
                               const SDL_Rect& clipExtent);

    /** A reference to the screen that this widget is a part of. Used for
        registering/unregistering named widgets, and accessing other
        widgets. */
    Screen& screen;

    /** An optional user-assigned name associated with this widget.
        Only useful for debugging. For performance reasons, avoid using it
        in real logic. */
    std::string debugName;

    /** The widget's logical screen extent, i.e. the position/size of the
        widget relative to the UI's logical size. */
    SDL_Rect logicalExtent;

    /** The widget's logical screen extent, scaled to match the current UI
        scaling. The position of this extent does not account for any offsets,
        such as those passed by parents. */
    SDL_Rect scaledExtent;

    /** The actual screen extent of this widget after the last updateLayout()
        call. This is the actual location that this widget will be rendered at.
        Generally equal to scaledExtent + any offsets added by parent widgets.
        Used in rendering and hit testing for events. */
    SDL_Rect renderExtent;

    /** The value of Core::actualScreenSize that was used the last time this
        widget calculated its actualScreenExtent.
        Used to detect when to re-calculate actualScreenExtent. */
    ScreenResolution lastUsedScreenSize;

    /** If true, this widget will be rendered and will respond to events. */
    bool isVisible;

    /** If true, this widget is keyboard focusable.
        Focusable widgets can be focused by left clicking on them, or by
        explicitly setting focus through an EventResult. Focus can be removed
        by clicking elsewhere, or by hitting the escape key.
        When a widget is focused, it will receive key press and character
        events. */
    bool isFocusable;

    /** An ordered list of references to this widget's children.
        Widgets must be added to this list to be involved in layout, rendering,
        and event propagation.
        Child widgets must be separate members of the class. This list only
        holds references to those members.
        This list's elements are in rendering order (rendering happens from
        front -> back, events propagate from back -> front). */
    std::vector<std::reference_wrapper<Widget>> children;

    /** The weak references to this widget.
        When this widget is destructed, it will invalidate itself in these
        refs. When one of these refs is destructed, it will tell us to stop
        tracking it. */
    std::vector<WidgetWeakRef*> trackedRefs;
};

} // namespace AUI
