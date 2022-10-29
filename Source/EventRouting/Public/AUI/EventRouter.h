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
 *   must have keyboard focus.
 *   If a widget is marked as isFocusable, it will receive focus when clicked.
 *   It can also be explicitly given focus using the setFocus field in
 *   EventResult.
 *
 *   Focus is lost when either the escape key is pressed, or a click occurs
 *   outside of the focused widget. Focus can also be dropped explicitly by
 *   setting the dropFocus field in EventResult, or implicitly by using the
 *   setFocus field to switch focus to a new widget.
 *
 *   KeyDown events can also be received by registering as a listener. If the
 *   focus target doesn't handle the KeyDown, it will then be routed to any
 *   registered listeners. TODO: Explain how to register
 *   KeyDown listeners are necessary in certain cases, e.g. to open a menu when
 *   a button is pressed. In such cases, the widget in charge of handling the
 *   event likely won't be the focus target, so it needs to explicitly register
 *   to receive it.
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

private:
    /**
     * Used for passing data from a specific event handler function back up to
     * a general event handler function.
     */
    struct HandlerReturn {
        EventResult eventResult{};
        /** If eventResult.wasHandled == true, this points to the widget that
            handled the event. */
        WidgetPath::iterator handlerWidget;
    };

    /**
     * Translates the given SDL button type to our MouseButtonType.
     */
    MouseButtonType translateSDLButtonType(Uint8 sdlButtonType);

    /**
     * Returns a widget path containing all widgets that are underneath the
     * given cursor position.
     *
     * If no widgets are under the cursor, returns an empty path.
     */
    WidgetPath getPathUnderCursor(const SDL_Point& cursorPosition);

    /**
     * Returns a path that traces from the root-most tracked relative of the
     * given widget, down to the given widget (inclusive).
     *
     * If all relevant widgets are being tracked by this locator, this will
     * return the path from the widget's parent Window down to the widget.
     *
     * Note: This relies on our rules: parent widgets must fully overlap their
     *       children, and it's invalid for sibling widgets to overlap.
     */
    WidgetPath getPathUnderWidget(const Widget* widget);

    /**
     * Routes a MouseDown to the given widget path.
     */
    HandlerReturn handleMouseDown(MouseButtonType buttonType,
                                  const SDL_Point& cursorPosition,
                                  WidgetPath& clickPath);

    /**
     * Routes a MouseDoubleClick to the given widget path.
     */
    HandlerReturn handleMouseDoubleClick(MouseButtonType buttonType,
                                         const SDL_Point& cursorPosition,
                                         WidgetPath& clickPath);

    /**
     * Compares hoverPath to lastHoveredWidgetPath and routes MouseEnter and
     * MouseLeave events appropriately.
     *
     * Note: This returns void because MouseEnter and MouseLeave don't return
     *       EventResult.
     */
    void handleMouseEnterAndLeave(WidgetPath& hoverPath);

    /**
     * Routes a MouseMove to the widgets in hoverPath.
     * Diffs hoverPath with lastHoveredWidgetPath. Widgets that are newly
     * hovered get routed a MouseEnter, while widgets that left are routed
     * a MouseLeave.
     */
    EventResult handleUncapturedMouseMove(const SDL_Point& cursorPosition,
                                          WidgetPath& hoverPath);

    /**
     * Processes the given event result. May update mouse capture, etc.
     *
     * @param eventResult  The event result to process.
     */
    void processEventResult(const EventResult& eventResult);

    /**
     * If eventPath has any focusable widgets, sets focus to the path from
     * eventPath's root to its leafmost focusable widget.
     * @return true if a focus was set, else false.
     */
    bool setFocusIfFocusable(WidgetPath& eventPath);

    /**
     * Sets up newFocusPath as the new focus path.
     * If focus is set, routes a FocusLost to the leafmost widget in focusPath.
     * Routes a FocusGained to the leafmost widget in newFocusPath.
     */
    void handleSetFocus(WidgetPath& newFocusPath);

    /**
     * If focusPath is set, clears it and routes a FocusLost to the leafmost
     * widget.
     */
    void handleDropFocus(FocusLostType focusLostType);

    /**
     * Routes a KeyDown through the current focus path.
     *
     * If the Escape key was pressed and the KeyDown event wasn't handled,
     * handles the event and drops focus (if we had a focused widget).
     *
     * @pre focusPath.back() must be a valid widget reference.
     * @return true if the event was handled, else false.
     */
    bool handleFocusedKeyDown(SDL_Keycode keyCode);

    /**
     * Routes a KeyUp through the current focus path.
     * @pre focusPath.back() must be a valid widget reference.
     * @return true if the event was handled, else false.
     */
    bool handleKeyUp(SDL_Keycode keyCode);

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
};

} // End namespace AUI
