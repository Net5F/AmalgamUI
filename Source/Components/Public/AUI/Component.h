#pragma once

#include "AUI/ScreenResolution.h"
#include <SDL_Rect.h>
#include <SDL_events.h>
#include "entt/core/hashed_string.hpp"
#include <string>

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
     * @param actualPoint  An actual screen position. Will be scaled internally
     *                     to its equivalent logical screen position.
     */
    bool containsPoint(const SDL_Point& actualPoint);

    /**
     * Sets the component's logical screen extent to the given extent and
     * re-calculates its actual screen extent.
     */
    void setScreenExtent(const SDL_Rect& inScreenExtent);

    const entt::hashed_string& getKey();

    void setIsVisible(bool inIsVisible);
    bool getIsVisible();

    /**
     * Called when a MouseButtonDown event happens within this component.
     */
    virtual void onMouseButtonDown(SDL_MouseButtonEvent& event);

    /**
     * Called when a MouseButtonUp event happens after a previous
     * MouseButtonDown event occurred on this component.
     */
    virtual void onMouseButtonUp(SDL_MouseButtonEvent& event, bool isHovered);

    /**
     * Called when a MouseMove event lands inside this component.
     */
    virtual void onMouseMove(SDL_MouseMotionEvent& event);

    /**
     * Called when the mouse first enters this component.
     */
    virtual void onMouseEnter(SDL_MouseMotionEvent& event);

    /**
     * Called when the mouse leaves this component.
     */
    virtual void onMouseLeave(SDL_MouseMotionEvent& event);

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
    Component(Screen& inScreen, const char* inKey, const SDL_Rect& inScreenExtent);

    /**
     * Checks if Core::actualScreenSize has changed since the last time this
     * component's actualScreenExtent was calculated. If so, re-calculates
     * actualScreenExtent, scaling it to the new actualScreenSize.
     *
     * This implementation is sufficient for refreshing actualScreenExtent, but
     * must be overridden if your component has other scaling needs.
     */
    virtual bool refreshScaling();

    /** A reference to the screen that this component is a part of. Used for
        registering/unregistering named components, and accessing other
        components. */
    Screen& screen;

    /** The unique, user-assigned key. Used to identify the component for
        removal from the Screen's vector and map. */
    entt::hashed_string key;

    /** The component's logical screen extent, i.e. the position/size of the
        component relative to the UI's logical size. */
    SDL_Rect logicalScreenExtent;

    /** The component's actual screen extent, i.e. the position/size of the
        component on the screen. Does not account for parent offsets. */
    SDL_Rect actualScreenExtent;

    /** The actual screen extent of this component during the last render()
        call. Generally equal to actualScreenExtent + any offsets added by
        parent components. Used in hit testing for event handling. */
    SDL_Rect lastRenderedExtent;

    /** The value of Core::actualScreenSize that was used the last time this
        component calculated its actualScreenExtent.
        Used to detect when to re-calculate actualScreenExtent. */
    ScreenResolution lastUsedScreenSize;

    /** If true, this component will be rendered and will respond to events. */
    bool isVisible;
};

} // namespace AUI
