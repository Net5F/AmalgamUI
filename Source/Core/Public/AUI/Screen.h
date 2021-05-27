#pragma once

#include "AUI/Component.h"
#include "AUI/EventTypes.h"
#include "AUI/Internal/Timer.h"
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
    /** The amount of time between Tick events, in seconds. */
    static constexpr double TICK_TIMESTEP_S = 1 / static_cast<double>(30);

    Screen(const std::string& inDebugName);

    virtual ~Screen() = default;

    /**
     * Adds the given component to the componentMap, registering it as a valid
     * component to get().
     *
     * Errors if the component is anonymous (key is empty).
     *
     * @param component  A component with a non-empty key. Components with
     *                   empty keys are considered anonymous and cannot be
     *                   registered.
     * @post The given component's key is now a valid input to get().
     */
    void registerComponent(Component& component);

    /**
     * Removes the component with the given key from this screen's
     * componentMap, unregistering it as a valid component to get().
     *
     * Errors if the key doesn't exist in this screen's map.
     *
     * @param key  A non-empty key that identifies a component.
     * @post The given key is no longer a valid input to get().
     */
    void unregisterComponent(const entt::hashed_string& key);

    /**
     * Returns a reference to the component with the given key.
     *
     * Errors if the key doesn't exist in this screen's map.
     */
    template <typename T>
    T& get(const entt::hashed_string& key)
    {
        // If we don't have a component with that key, fail.
        auto pairIt = componentMap.find(key.value());
        if (pairIt == componentMap.end()) {
            AUI_LOG_ERROR("No component with given key exists. Screen: %s, Key: %s", debugName.c_str(), key.data());
        }

        // Check if it actually casts to the desired type.
        T* derived = dynamic_cast<T*>(pairIt->second);
        if (derived == nullptr) {
            AUI_LOG_ERROR("Component did not cast to given type. Screen: %s, Key: %s", debugName.c_str(), key.data());
        }

        return *derived;
    }

    /**
     * Registers the given object as listening for the given event type.
     *
     * @param eventType  An event type, corresponding to SDL_Event.type.
     * @param listener  The listening object. Must implement an appropriate
     *                  function to handle the given eventType.
     */
    void registerListener(EventType eventType, Component* listener);

    /**
     * Unregisters the given listener object from receiving the given event.
     *
     * @param eventType  An event type, corresponding to SDL_Event.type.
     * @param listener  The listening object. Must be derived from Component
     *                  and implement an appropriate function to handle the
     *                  given eventType.
     */
    void unregisterListener(EventType eventType, Component* listener);

    /**
     * Offers the given event to this screen to be handled.
     *
     * The event will be passed to any relevant registered listeners.
     *
     * @return true if the event was handled, else false.
     */
    bool handleEvent(SDL_Event& event);

    /**
     * Updates accumulatedTime. If greater than the tick timestep, calls all
     * Tick event listeners.
     */
    void tick();

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
    std::unordered_map<EventType, std::vector<Component*>> listenerMap;

    /** Used to time when we should fire Tick events. */
    Timer timer;

    /** The accumulated time since we last fired a Tick event. */
    double accumulatedTime;
};

} // namespace AUI
