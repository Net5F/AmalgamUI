#pragma once

#include "AUI/Component.h"
#include <vector>
#include <memory>

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
    template <typename T>
    T& addComponent(const char* key, const SDL_Rect& screenExtent)
    {
        std::unique_ptr<T> component = std::make_unique<T>(key, screenExtent);
        components.push_back(std::move(component));

        return static_cast<T&>(*(components.back()));
    }

    std::vector<std::unique_ptr<Component>> components;
};

} // namespace AUI
