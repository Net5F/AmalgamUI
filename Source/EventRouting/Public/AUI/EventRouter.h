#pragma once

#include "AUI/WidgetPath.h"
#include "AUI/WidgetWeakRef.h"
#include "AUI/MouseButtonType.h"
#include "AUI/FocusLostType.h"
#include "AUI/EventResult.h"
#include <SDL_events.h>

namespace AUI
{

class Screen;
class Image;

/**
 * Translates SDL events to AUI events and handles their routing.
 *
 * Holds state relevant to event routing (current focused widget path, etc).
 *
 * Routing:
 *   Events are routed in various ways:
 *     Tunneling (Rootmost -> Leafmost) (Tunneled events are called Preview)
 *     Bubbling (Leafmost -> Rootmost)
 *     Direct (Leafmost)
 *
 *   See the handler comments in Widget.h for specifics on which routing
 *   strategy is used for each event.
 *
 * EventResult:
 *   In certain event handlers, Widgets can direct the EventRouter by setting
 *   fields in the EventResult return type. This can be used for things like
 *   setting mouse capture, explicitly setting focus, or marking the event as
 *   handled. See EventResult.h for more info.
 *
 * Key Events and Focus:
 *   In order for a widget to receive KeyDown, KeyUp, or TextInput events, it
 *   must have focus.
 *   If a widget is marked as isFocusable, it will receive focus when clicked.
 *   It can also be explicitly given focus using the setFocus field in
 *   EventResult.
 *
 *   Focus is lost when either the escape key is pressed, or a click occurs
 *   outside of the focused widget. Focus can also be dropped explicitly by
 *   setting the dropFocus field in EventResult, or implicitly by using the
 *   setFocus field to switch focus to a new widget.
 *
 *   Sometimes you want to receive key events without having focus, e.g.
 *   to open a menu. To do this, use Screen::onKeyDown(). It will receive
 *   any key events that aren't handled by a focused widget.
 */
class EventRouter
{
public:
    EventRouter(Screen& inScreen);

    /**
     * Call when a SDL_MOUSEBUTTONDOWN event occurs.
     * @return true if the event was consumed, else false.
     */
    bool handleMouseButtonDown(SDL_MouseButtonEvent& event);

    /**
     * Call when a SDL_MOUSEBUTTONUP event occurs.
     * @return true if the event was consumed, else false.
     */
    bool handleMouseButtonUp(SDL_MouseButtonEvent& event);

    /**
     * Call when a SDL_MOUSEWHEEL event occurs.
     * @return true if the event was consumed, else false.
     */
    bool handleMouseWheel(SDL_MouseWheelEvent& event);

    /**
     * Call when a SDL_MOUSEMOTION event occurs.
     * @return true if the event was consumed, else false.
     */
    bool handleMouseMove(SDL_MouseMotionEvent& event);

    /**
     * Call when a SDL_KEYDOWN event occurs.
     * @return true if the event was consumed, else false.
     */
    bool handleKeyDown(SDL_KeyboardEvent& event);

    /**
     * Call when a SDL_TEXTINPUT event occurs.
     * @return true if the event was consumed, else false.
     */
    bool handleTextInput(SDL_TextInputEvent& event);

    /**
     * Attempts to set focus to the given widget.
     */
    void setFocus(const Widget* widget);

    /**
     * If a widget is currently focused, drops it.
     */
    void dropFocus();

    /**
     * If a widget is currently being dragged, returns
     * widget.getDragDropImage(). Else, returns nullptr.
     */
    Image* getDragDropImage();

private:
    /**
     * Used for passing data from an event router function back up to an event
     * event handler function.
     */
    struct RouterReturnData {
        /** If true, a widget handled the event. */
        bool eventWasHandled{false};
        /** If true, a handler explicitly set focus while the event was
            propagating. */
        bool focusWasSet{false};
        /** If eventResult.wasHandled == true, this points to the widget that
            handled the event. */
        WidgetPath::iterator handlerWidget;
    };

    /**
     * Translates the given SDL button type to our MouseButtonType.
     */
    MouseButtonType translateSDLButtonType(Uint8 sdlButtonType);

    /**
     * Translates a screen-relative cursor position to be relative to the
     * window that it's hovering over.
     * If cursorPosition is not over a window, does nothing.
     */
    SDL_Point screenToWindowRelative(const SDL_Point& cursorPosition);

    /**
     * Returns a path that traces from the first hit window, up to its top-most
     * hit child widget (inclusive).
     *
     * If no widgets are under the cursor, returns an empty path.
     */
    WidgetPath getPathUnderCursor(const SDL_Point& cursorPosition);

    /**
     * Returns a path that traces from the given widget's parent Window, up to
     * the given widget (inclusive).
     *
     * If the widget was not found in the current layout, returns an empty path.
     */
    WidgetPath getPathUnderWidget(const Widget* widget);

    /**
     * Routes a MouseDown to the given widget path.
     */
    RouterReturnData routeMouseDown(MouseButtonType buttonType,
                                    const SDL_Point& cursorPosition,
                                    WidgetPath& clickPath);

    /**
     * Routes a MouseDoubleClick to the given widget path.
     */
    RouterReturnData routeMouseDoubleClick(MouseButtonType buttonType,
                                           const SDL_Point& cursorPosition,
                                           WidgetPath& clickPath);

    /**
     * Compares hoverPath to lastHoveredWidgetPath and routes MouseEnter and
     * MouseLeave (or DragEnter/DragLeave) events appropriately.
     *
     * Note: This returns void because MouseEnter and MouseLeave don't return
     *       EventResult.
     */
    void routeMouseEnterAndLeave(WidgetPath& hoverPath);

    /**
     * Routes a MouseMove (or DragMove) to the widgets in hoverPath.
     * @return true if the event was handled, else false.
     */
    bool routeMouseMove(const SDL_Point& cursorPosition, WidgetPath& hoverPath);

    /**
     * Sets focus to a path from eventPath's root to its leafmost focusable
     * widget.
     * @return true if focus was set, false if eventPath had no focusable
     *         widgets.
     */
    bool setFocusIfFocusable(WidgetPath& eventPath);

    /**
     * Sets up newFocusPath as the new focus path.
     * If focus is set, routes a FocusLost to the leafmost widget in focusPath.
     * Routes a FocusGained to the leafmost widget in newFocusPath.
     */
    void setFocus(WidgetPath& newFocusPath);

    /**
     * If focusPath is set, clears it and routes a FocusLost to the leafmost
     * widget.
     */
    void dropFocus(FocusLostType focusLostType);

    /**
     * Routes a KeyDown through the current focus path.
     *
     * If the Escape key was pressed and the KeyDown event wasn't handled,
     * handles the event and drops focus (if we had a focused widget).
     *
     * @pre focusPath.back() must be a valid widget reference.
     * @return true if the event was handled, else false.
     */
    bool routeFocusedKeyDown(SDL_Keycode keyCode);

    /**
     * Routes a KeyUp through the current focus path.
     * @pre focusPath.back() must be a valid widget reference.
     * @return true if the event was handled, else false.
     */
    bool routeKeyUp(SDL_Keycode keyCode);

    /**
     * If eventPath contains a draggable widget, stages it for drag detection.
     * A drag event won't actually be started until the user drags the mouse
     * beyond Core::dragTriggerDistance.
     */
    void setDragIfDraggable(WidgetPath& eventPath, SDL_Point& cursorPosition);

    /**
     * Routes a MouseLeave through lastHoveredWidgetPath, then routes a
     * DragStart to the currently dragged widget.
     */
    void routeDragStart();

    /**
     * Routes a Drop event through hoverPath, then routes a DragEnd to the
     * currently dragged widget, then routes DragLeave and MouseEnter events
     * to the hovered widgets.
     */
    void routeDrop(WidgetPath& hoverPath);

    /**
     * Processes the given event result. May update mouse capture, etc.
     *
     * @param eventResult  The event result to process.
     */
    void processEventResult(const EventResult& eventResult);

    /**
     * Returns the current focused widget, or nullptr if there is none.
     * Note: This does not check the validity of the widget. This is mostly
     *       useful just to get a pointer for comparisons.
     */
    Widget* getFocusedWidget();

    /** Used to interact with the Window stack. */
    Screen& screen;

    /** The last widget path that our mouse cursor was hovered over.
        Updated when the mouse moves. Cleared when a capture is set. */
    WidgetPath lastHoveredWidgetPath;

    /** If non-empty, holds the widget that is currently capturing the mouse.
        When the mouse is captured, all mouse events will go to this captured
        widget instead of the widgets under the cursor.
        Widgets can request mouse capture during event handling.
        Note: This path just holds the captor widget. We use a path instead
              of using a single ref because the semantics are more clear. */
    WidgetPath mouseCapturePath;

    /** If non-empty, holds the widget that is currently focused.
        Focused widgets will receive key and character events.
        Focus is gained on left mouse click or when requested.
        Focus is dropped if the mouse is clicked on an area that isn't the
        focused widget, or the escape key is pressed, or when requested.
        Note: This path holds all widgets from the Window to the focused
              widget. KeyDown events will be tunneled then bubbled through
              the entire path. */
    WidgetPath focusPath;

    /** If non-empty, holds the currently clicked drag/drop widget.
        When a draggable widget is clicked, drag detection begins. If the
        mouse moves beyond Core::dragTriggerDistance, the drag event starts.
        Check dragUnderway to see if we're actually dragging or not.
        Note: This path just holds the first draggable widget. We use a path
              instead of using a single ref because the semantics are more
              clear. */
    WidgetPath dragPath;

    /** If dragPath is non-empty, this is actual position where the drag
       started. If the mouse moves beyond Core::dragTriggerDistance, a drag
       event will start. */
    SDL_Point dragOrigin;

    /** If true, we're currently dragging a widget. */
    bool dragUnderway;
};

} // End namespace AUI
