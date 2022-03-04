#pragma once

#include "AUI/WidgetPath.h"
#include "AUI/WidgetWeakRef.h"
#include "AUI/MouseButtonType.h"
#include <SDL2/SDL_events.h>

namespace AUI
{

class Screen;
struct EventResult;

/**
 * Translates SDL events to AUI events and handles their routing.
 *
 * Holds state relevant to event routing (current focused widget path, etc).
 */
class EventRouter
{
public:
    // TODO: Fully comment how tunneling/bubbling and preview and events
    //       work in here

    EventRouter(Screen& inScreen);

    /**
     * Called when a SDL_MOUSEBUTTONDOWN event occurs.
     * @return true if the event was consumed, else false.
     */
    bool handleMouseButtonDown(SDL_MouseButtonEvent& event);

    /**
     * Called when a SDL_MOUSEBUTTONUP event occurs.
     * @return true if the event was consumed, else false.
     */
    bool handleMouseButtonUp(SDL_MouseButtonEvent& event);

    /**
     * Called when a SDL_MOUSEWHEEL event occurs.
     * @return true if the event was consumed, else false.
     */
    bool handleMouseWheel(SDL_MouseWheelEvent& event);

    /**
     * Called when a SDL_MOUSEMOTION event occurs.
     * @return true if the event was consumed, else false.
     */
    bool handleMouseMove(SDL_MouseMotionEvent& event);

    /**
     * Called when a SDL_KEYDOWN event occurs.
     * @return true if the event was consumed, else false.
     */
    bool handleKeyDown(SDL_KeyboardEvent& event);

    /**
     * Called when a SDL_TEXTINPUT event occurs.
     * @return true if the event was consumed, else false.
     */
    bool handleTextInput(SDL_TextInputEvent& event);

private:
    /**
     * Translates the given SDL button type to our MouseButtonType.
     */
    MouseButtonType translateSDLButtonType(Uint8 sdlButtonType);

    /**
     * @return A widget path containing all widgets that are underneath the
     *         given cursorPosition.
     *         If no widgets are under the cursor, returns an empty path.
     */
    WidgetPath getPathUnderCursor(const SDL_Point& cursorPosition);

    /**
     * Routes a MouseDown to the given widget path.
     */
    EventResult handleMouseDown(MouseButtonType buttonType, const SDL_Point& cursorPosition,
                         WidgetPath& clickPath);

    /**
     * Routes a MouseDoubleClick to the given widget path.
     */
    EventResult handleMouseDoubleClick(MouseButtonType buttonType,
                                  const SDL_Point& cursorPosition, WidgetPath& clickPath);

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
    EventResult handleUncapturedMouseMove(const SDL_Point& cursorPosition, WidgetPath& hoverPath);

    /**
     * Processes the given event result. May update mouse capture, etc.
     */
    void processEventResult(const EventResult& eventResult);

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
};

} // End namespace AUI
