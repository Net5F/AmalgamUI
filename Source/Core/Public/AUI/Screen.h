#pragma once

#include "AUI/Component.h"
#include "AUI/Internal/Log.h"
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

    /**
     * Constructs a component of the templated type and adds it to the
     * screen.
     *
     * Errors if the given key is already used in the componentMap.
     *
     * @param key  The key to use when inserting into the component map.
     * @param screenExtent  The screenExtent of the component.
     * @return A reference to the added component.
     */
    template <typename T>
    T& add(const entt::hashed_string& key, const SDL_Rect& screenExtent)
    {
        // If we already have a component with that key, fail.
        if (componentMap.find(key) != componentMap.end()) {
            AUI_LOG_ERROR("Tried to add component with key that is already in use. Screen: %s, Key: %s", debugName.c_str(), key.data());
        }

        // Add it to the map.
        std::unique_ptr<T> component = std::make_unique<T>(key, screenExtent);
        componentMap[key] = component.get();

        // Add it to the vector.
        components.push_back(std::move(component));

        return static_cast<T&>(*(components.back()));
    }

    /**
     * Returns a reference to the component with the given key.
     *
     * Errors if the key doesn't exist in this screen.
     */
    template <typename T>
    T& get(entt::hashed_string key)
    {
        // If we don't have a component with that key, fail.
        auto pairIt = componentMap.find(key);
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
     * Removes the component with the given key from this screen, destructing
     * it.
     *
     * Errors if the key doesn't exist in this screen.
     */
    void remove(entt::hashed_string key);

    std::vector<std::unique_ptr<Component>>& getComponents();

private:
    /** The user-assigned name associated with this screen.
        Only useful for debugging. For performance reasons, avoid using it
        in real logic. */
    std::string debugName;

    /** The components that this screen owns. */
    std::vector<std::unique_ptr<Component>> components;

    /** A key->component map for convenient access. */
    std::unordered_map<entt::id_type, Component*> componentMap;
};

} // namespace AUI
