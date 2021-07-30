#pragma once

#include "AUI/Component.h"
#include "AUI/InternalEvent.h"
#include "AUI/Internal/Log.h"
#include <SDL_events.h>
#include "entt/core/hashed_string.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace AUI {

/**
 * This class represents a UI screen.
 *
 * Screens facilitate the organization of UI components, and provide an easy way
 * to switch between sets of components.
 *
 * An appropriate screen may be a title screen, settings screen, or a world
 * screen that displays UI elements while allowing the user to see the world
 * behind it.
 */
class Screen
{
public:
    Screen(const std::string& inDebugName);

    virtual ~Screen() = default;

    /**
     * Registers the given object as listening for the given event type.
     *
     * @param eventType  An event type, corresponding to SDL_Event.type.
     * @param listener  The listening object. Must implement an appropriate
     *                  function to handle the given eventType.
     */
    void registerListener(InternalEvent::Type eventType, Component* listener);

    /**
     * Unregisters the given listener object from receiving the given event.
     *
     * References and iterators to the listenerMap are not invalidated.
     *
     * @param eventType  An event type, corresponding to SDL_Event.type.
     * @param listener  The listening object. Must be derived from Component
     *                  and implement an appropriate function to handle the
     *                  given eventType.
     */
    void unregisterListener(InternalEvent::Type eventType, Component* listener);

    /**
     * Offers the given event to this screen to be handled.
     *
     * The event will be passed to any relevant registered listeners.
     *
     * @return true if the event was handled, else false.
     */
    bool handleEvent(SDL_Event& event);

    /**
     * Calls all Tick event listeners.
     *
     * @param timestepS  The amount of time that has passed since the last
     *                   tick() call, in seconds.
     */
    void tick(double timestepS);

    /**
     * Renders all graphics for this screen to the current rendering target.
     */
    virtual void render();

private:
    /**
     * Passes the event to all MouseButtonDown listeners.
     */
    bool handleMouseButtonDown(SDL_MouseButtonEvent& event);

    /**
     * Passes the event to all MouseButtonUp listeners.
     */
    bool handleMouseButtonUp(SDL_MouseButtonEvent& event);

    /**
     * Passes the event to all MouseWheel listeners.
     */
    bool handleMouseWheel(SDL_MouseWheelEvent& event);

    /**
     * Passes the event to all MouseMove listeners.
     */
    void handleMouseMove(SDL_MouseMotionEvent& event);

    /**
     * Passes the event to all KeyDown listeners.
     */
    bool handleKeyDown(SDL_KeyboardEvent& event);

    /**
     * Passes the event to all TextInput listeners.
     */
    bool handleTextInput(SDL_TextInputEvent& event);

    /** The user-assigned name associated with this screen.
        Only useful for debugging. For performance reasons, avoid using it
        in real logic. */
    std::string debugName;

    /** A key->component map for convenient access. */
    std::unordered_map<entt::hashed_string::hash_type, Component*> componentMap;

    /** A map containing all of this screen's components that care to listen
        for particular system events. */
    std::unordered_map<InternalEvent::Type, std::vector<Component*>> listenerMap;
};

} // namespace AUI
