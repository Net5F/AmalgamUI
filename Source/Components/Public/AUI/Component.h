#pragma once

#include "AUI/ScreenResolution.h"
#include "AUI/InternalEvent.h"
#include <SDL_Rect.h>
#include <SDL_events.h>
#include "entt/core/hashed_string.hpp"
#include <string>
#include <array>

namespace AUI {

class Screen;

/**
 * The base class for all UI components.
 *
 * Component data setting follows the pattern of:
 *   - In the constructor, pass through the data necessary for Component's
 *     constructor.
 *   - Use setters for all other data, which return a reference to the
 *     component to facilitate chaining if desired.
 */
class Component
{
public:
    /** Component is not independently constructible. */
    Component() = delete;

    virtual ~Component();

    /**
     * If this component's screen extent contains the given point, returns
     * true. Else, returns false.
     *
     * @param actualPoint  A point in actual screen space.
     */
    bool containsPoint(const SDL_Point& actualPoint);

    /**
     * If this component's screen extent fully contains the given extent,
     * returns true. Else, returns false.
     *
     * @param actualExtent  An extent in actual screen space.
     */
    bool containsExtent(const SDL_Rect& actualExtent);

    /**
     * Sets the component's logical extent to the given extent and
     * re-calculates its scaled extent.
     */
    virtual void setLogicalExtent(const SDL_Rect& inLogicalExtent);

    /** See Component::logicalExtent. */
    SDL_Rect getLogicalExtent();
    /** See Component::scaledExtent. */
    SDL_Rect getScaledExtent();
    /** See Component::lastRenderedExtent. */
    SDL_Rect getLastRenderedExtent();

    const entt::hashed_string& getKey();

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
     * Renders this component to the current rendering target.
     * Directly calls SDL functions like SDL_RenderCopy().
     *
     * @param parentOffset  The offset that should be added to this component's
     *                      position before rendering. Used by parent classes
     *                      to control the layout of their children.
     */
    virtual void render(const SDL_Point& parentOffset = {});

protected:
    Component(Screen& inScreen, const char* inKey, const SDL_Rect& inLogicalExtent);

    /**
     * Registers this component as a listener for the given event type.
     *
     * This component will internally track that the given event type is being
     * listened to, and will unregister itself during destruction.
     */
    void registerListener(InternalEvent::Type eventType);

    /**
     * Unregisters this component as a listener for the given event type.
     */
    void unregisterListener(InternalEvent::Type eventType);

    /**
     * Checks if Core::actualScreenSize has changed since the last time this
     * component's scaledExtent was calculated. If so, re-calculates
     * scaledExtent, scaling it to the new actualScreenSize.
     *
     * This implementation is sufficient for refreshing actualExtent, but must
     * be overridden if your component has other scaling needs.
     */
    virtual bool refreshScaling();

    /**
     * Returns an extent equal to sourceExtent, adjusted to not go beyond the
     * bounds of clipExtent.
     */
    SDL_Rect calcClippedExtent(const SDL_Rect& sourceExtent, const SDL_Rect& clipExtent);

    /** A reference to the screen that this component is a part of. Used for
        registering/unregistering named components, and accessing other
        components. */
    Screen& screen;

    /** The unique, user-assigned key. Used to identify the component for
        removal from the Screen's vector and map. */
    entt::hashed_string key;

    /** The component's logical screen extent, i.e. the position/size of the
        component relative to the UI's logical size. */
    SDL_Rect logicalExtent;

    /** The component's logical screen extent, scaled to match the current UI
        scaling. The position of this extent does not account for any offsets,
        such as those passed by parents. */
    SDL_Rect scaledExtent;

    /** The actual screen extent of this component during the last render()
        call. Generally equal to scaledScreenExtent + any offsets added by
        parent components. Used in hit testing for event handling. */
    SDL_Rect lastRenderedExtent;

    /** The value of Core::actualScreenSize that was used the last time this
        component calculated its actualScreenExtent.
        Used to detect when to re-calculate actualScreenExtent. */
    ScreenResolution lastUsedScreenSize;

    /** If true, this component will be rendered and will respond to events. */
    bool isVisible;

    /** Tracks the events that this component is currently listening for. */
    std::array<bool, InternalEvent::NUM_TYPES> listeningEventTypes;
};

} // namespace AUI
