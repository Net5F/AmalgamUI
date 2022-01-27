#pragma once

#include "AUI/ScreenResolution.h"
#include "AUI/InternalEvent.h"
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_events.h>
#include <string>
#include <array>
#include <vector>
#include <functional>

namespace AUI
{
class Screen;

/**
 * The base class for all UI widgets.
 *
 * Widget initialization follows the pattern:
 *   - In the constructor, pass through the data necessary for Widget's
 *     constructor.
 *   - Use setters for all other data.
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
     * If this widget's screen extent fully contains the given extent,
     * returns true. Else, returns false.
     *
     * @param actualExtent  An extent in actual screen space.
     */
    bool containsExtent(const SDL_Rect& actualExtent);

    /**
     * Sets the widget's logical extent to the given extent and
     * re-calculates its scaled extent.
     */
    virtual void setLogicalExtent(const SDL_Rect& inLogicalExtent);

    /** See Widget::logicalExtent. */
    SDL_Rect getLogicalExtent();
    /** See Widget::scaledExtent. */
    SDL_Rect getScaledExtent();
    /** See Widget::renderExtent. */
    SDL_Rect getRenderExtent();

    const std::string& getDebugName();

    virtual void setIsVisible(bool inIsVisible);
    bool getIsVisible();

    /**
     * Called when a SDL_MOUSEBUTTONDOWN event occurs.
     */
    virtual bool onMouseButtonDown(SDL_MouseButtonEvent& event);

    /**
     * Called when a SDL_MOUSEBUTTONUP event occurs.
     */
    virtual bool onMouseButtonUp(SDL_MouseButtonEvent& event);

    /**
     * Called when a SDL_MOUSEWHEEL event occurs.
     */
    virtual bool onMouseWheel(SDL_MouseWheelEvent& event);

    /**
     * Called when a SDL_MOUSEMOTION event occurs.
     */
    virtual void onMouseMove(SDL_MouseMotionEvent& event);

    /**
     * Called when a SDL_KEYDOWN event occurs.
     */
    virtual bool onKeyDown(SDL_KeyboardEvent& event);

    /**
     * Called when a SDL_TEXTINPUT event occurs.
     */
    virtual bool onTextInput(SDL_TextInputEvent& event);

    /**
     * Called when the current screen's tick() is called.
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
     * @post renderExtent is properly positioned for use in rendering and
     *       hit testing.
     */
    virtual void updateLayout(const SDL_Rect& parentExtent);

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

protected:
    Widget(Screen& inScreen, const SDL_Rect& inLogicalExtent,
              const std::string& inDebugName = "");

    /**
     * Registers this widget as a listener for the given event type.
     *
     * This widget will internally track that the given event type is being
     * listened to, and will unregister itself during destruction.
     */
    void registerListener(InternalEvent::Type eventType);

    /**
     * Unregisters this widget as a listener for the given event type.
     */
    void unregisterListener(InternalEvent::Type eventType);

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

    /** The actual screen extent of this widget after last updateLayout()
        call. Generally equal to scaledExtent + any offsets added by parent
        widgets. Used in rendering and hit testing for events. */
    SDL_Rect renderExtent;

    /** The value of Core::actualScreenSize that was used the last time this
        widget calculated its actualScreenExtent.
        Used to detect when to re-calculate actualScreenExtent. */
    ScreenResolution lastUsedScreenSize;

    /** If true, this widget will be rendered and will respond to events. */
    bool isVisible;

    /** Tracks the events that this widget is currently listening for. */
    std::array<bool, InternalEvent::NUM_TYPES> listeningEventTypes;

    /** An ordered list of references to this widget's children.
        Widgets must be added to this list to be involved in layout, rendering,
        and event propagation.
        Child widgets must be separate members of the class. This list only
        holds references to those members.
        This list's elements are in rendering order (rendering happens from
        front -> back, events propagate from back -> front). */
    std::vector<std::reference_wrapper<Widget>> children;
};

} // namespace AUI
