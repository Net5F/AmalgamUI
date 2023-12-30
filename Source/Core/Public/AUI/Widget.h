#pragma once

#include "AUI/MouseButtonType.h"
#include "AUI/FocusLostType.h"
#include "AUI/EventResult.h"
#include <SDL_rect.h>
#include <SDL_events.h>
#include <string>
#include <memory>
#include <vector>

namespace AUI
{
class Image;
class DragDropData;
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
     * @param windowPoint  A point on the screen, relative to the top left of
     *                     this widget's parent window.
     */
    bool containsPoint(const SDL_Point& windowPoint);

    /**
     * Sets the widget's logical extent to the given extent and
     * re-calculates its scaled extent.
     */
    virtual void setLogicalExtent(const SDL_Rect& inLogicalExtent);

    /** See Widget::logicalExtent. */
    const SDL_Rect& getLogicalExtent() const;
    /** See Widget::scaledExtent. */
    const SDL_Rect& getScaledExtent() const;
    /** See Widget::fullExtent. */
    const SDL_Rect& getFullExtent() const;
    /** See Widget::clippedExtent. */
    const SDL_Rect& getClippedExtent() const;

    const std::string& getDebugName() const;

    /** See Widget::isVisible. */
    virtual void setIsVisible(bool inIsVisible);
    bool getIsVisible() const;

    /** See Widget::isFocusable. */
    virtual void setIsFocusable(bool inIsFocusable);
    bool getIsFocusable() const;

    // Note: We'd prefer to return a const Image*, but render() is non-const.
    /** Returns the image that should follow the mouse while this widget is
        being dragged.
        Widgets that want to support drag and drop must override this.
        This may be called frequently, so try to keep it lightweight.
        Note: If this widget is e.g. in a disabled state, you can return nullptr
              here to disable drag and drop. */
    virtual Image* getDragDropImage();

    /** See Widget::dragDropData. */
    void setDragDropData(std::unique_ptr<DragDropData> inDragDropData);
    const DragDropData* getDragDropData() const;

    /** Returns true if getDragDropImage() and getDragDropData() both return
        non-nullptr. */
    bool getIsDragDroppable();

    /**
     * Called during the tunneling preview pass for a MouseDown event.
     *
     * Note: If this event is handled, it will also stop the MouseDown event
     *       from bubbling afterwards.
     *
     * @param cursorPosition  The cursor's position, relative to this widget's
     *                        parent window.
     */
    virtual EventResult onPreviewMouseDown(MouseButtonType buttonType,
                                           const SDL_Point& cursorPosition);

    /**
     * Called when a mouse click occurs on this widget.
     *
     * This event is bubbled to widgets under the mouse.
     *
     * @param cursorPosition  The cursor's position, relative to this widget's
     *                        parent window.
     */
    virtual EventResult onMouseDown(MouseButtonType buttonType,
                                    const SDL_Point& cursorPosition);

    /**
     * Called when a mouse click is released.
     *
     * This event is only routed to the widget that is capturing the mouse.
     *
     * @param cursorPosition  The cursor's position, relative to this widget's
     *                        parent window.
     */
    virtual EventResult onMouseUp(MouseButtonType buttonType,
                                  const SDL_Point& cursorPosition);

    /**
     * Called when a mouse double click (or triple click, or more) occurs on
     * this widget.
     *
     * This event is bubbled to widgets under the mouse.
     *
     * @param cursorPosition  The cursor's position, relative to this widget's
     *                        parent window.
     */
    virtual EventResult onMouseDoubleClick(MouseButtonType buttonType,
                                           const SDL_Point& cursorPosition);

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
     * @param cursorPosition  The cursor's position, relative to this widget's
     *                        parent window.
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
     * Called when this widget starts being dragged.
     */
    virtual void onDragStart();

    /**
     * Called when this widget is released after being dragged.
     */
    virtual void onDragEnd();

    /**
     * Called when the mouse cursor moves within this widget's bounds while
     * another widget is being dragged.
     *
     * This event is routed to the widget that is capturing the mouse. If
     * there's no mouse captor, it's bubbled to widgets under the mouse.
     *
     * @param cursorPosition  The cursor's position, relative to this widget's
     *                        parent window.
     */
    virtual EventResult onDragMove(const SDL_Point& cursorPosition);

    /**
     * Called when the mouse cursor first enters this widget's bounds while
     * another widget is being dragged
     *
     * This event is routed to widgets that are newly under the mouse.
     */
    virtual void onDragEnter();

    /**
     * Called when the mouse cursor leaves this widget's bounds while another
     * widget is being dragged.
     *
     * This event is routed to widgets that were previously under the mouse.
     */
    virtual void onDragLeave();

    /**
     * Called when a dragged widget is dropped onto this widget.
     */
    virtual EventResult onDrop(const DragDropData& dragDropData);

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
     * Updates this widget's extents to be properly scaled and positioned
     * within the parent window.
     *
     * @param startPosition  The position where this widget should begin its
     *                       layout. Typically will be equal to the parent's
     *                       top left, though containers may use this to add
     *                       additional offsets while arranging their elements.
     * @param availableExtent  The available space for this widget to take up.
     * @param widgetLocator  (If non-nullptr) The widget locator that this
     *                       widget should add itself to after updating.
     * @post scaledExtent matches the current Core::actualScreenSize.
     *       offsetExtent and clippedExtent are properly positioned.
     *       clippedExtent is clipped to the given availableExtent and is ready
     *       for use in rendering and hit testing.
     */
    virtual void updateLayout(const SDL_Point& startPosition,
                              const SDL_Rect& availableExtent,
                              WidgetLocator* widgetLocator);

    /**
     * Renders this widget to the current rendering target.
     *
     * The default implementation simply calls render() on all widgets in our
     * children list. Some overrides may directly call SDL functions like
     * SDL_RenderCopy().
     *
     * @param windowTopLeft  The top left coordinate of this widget's parent
     *                       window. This is used to translate the widget's
     *                       window-relative extent into a final screen
     *                       position.
     */
    virtual void render(const SDL_Point& windowTopLeft);

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
    Widget(const SDL_Rect& inLogicalExtent, const std::string& inDebugName);

    /** An optional user-assigned name associated with this widget.
        Only useful for debugging. For performance reasons, avoid using it
        in real logic. */
    std::string debugName;

    /** This widget's logical screen extent, i.e. the position/size of the
        widget relative to the UI's logical size. */
    SDL_Rect logicalExtent;

    /** This widget's scaled screen extent. Equal to logicalExtent, but scaled
        to match the current UI scaling. */
    SDL_Rect scaledExtent;

    /** This widget's full window-relative extent within the layout. Equal to
        scaledExtent, but offset to be positioned within the parent.  */
    SDL_Rect fullExtent;

    /** This widget's final window-relative extent within the layout. Equal to
        fullExtent, but clipped to fit within the parent. Ready for use in
        rendering and hit testing for events.
        Note: During updateLayout(), this widget may be found to not fit
              within the availableExtent. If so, this will be {0, 0, 0, 0}.
              You can test for this with SDL_RectEmpty(clippedExtent), and
              should do so before using this extent. */
    SDL_Rect clippedExtent;

    /** If true, this widget will be rendered and will respond to events. */
    bool isVisible;

    /** If true, this widget is focusable.
        Focusable widgets can be focused by left clicking on them, or by
        explicitly setting focus through an EventResult or Screen::setFocus().
        Focus can be removed by clicking elsewhere, or by hitting the escape
        key. When a widget is focused, it will receive key press and character
        events. */
    bool isFocusable;

    /** If non-nullptr, this is the data that should be given to the target
        when this widget is dropped.
        If nullptr, drag and drop will be disabled. */
    std::unique_ptr<DragDropData> dragDropData;

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
