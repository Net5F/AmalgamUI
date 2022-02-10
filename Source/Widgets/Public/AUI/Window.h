#pragma once

#include "AUI/Widget.h"
#include <SDL2/SDL_events.h>
#include <vector>

namespace AUI
{
/**
 * This class represents a UI window.
 *
 * Windows are the second layer of UI construct used by this library. Screens
 * own a stack of Windows, Windows own a list of Widgets (and Widgets can
 * own child Widgets).
 *
 * Windows facilitate the organization of UI widgets into spatial groups, and
 * can be dragged around and overlapped. Widgets themselves don't support
 * overlapping, so things like dialog boxes must be built as separate windows.
 *
 * Windows can be:
 *   Dragged around the screen (not yet implemented)
 *   Resized (not yet implemented)
 *   Moved in front/behind each other (not yet implemented)
 */
class Window : public Widget
{
public:
    Window(Screen& screen, const SDL_Rect& inLogicalExtent,
              const std::string& inDebugName);

    virtual ~Window() = default;

    //-------------------------------------------------------------------------
    // Base class overrides
    //-------------------------------------------------------------------------
    Widget* handleOSEvent(SDL_Event& event) override;

    void tick(double timestepS);

private:
    /**
     * Propagates the given event through our child widgets.
     * If a child handles the event, updates our internal tracking.
     */
    Widget* passOSEventToChildren(SDL_Event& event);

    /** The last child widget that consumed a MouseMove event. */
    Widget* lastHoveredChild;
    /** The last child widget that consumed a MouseButtonDown event.
        Cleared when the corresponding MouseButtonUp occurs. */
    Widget* lastClickedChild;
};

} // namespace AUI
