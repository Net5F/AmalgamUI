#pragma once

#include <SDL_Rect.h>
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
    Component(Screen& inScreen, const char* inKey, const SDL_Rect& inScreenExtent);

    virtual ~Component();

    /**
     * Renders this component to the current rendering target.
     * Directly calls SDL functions like SDL_RenderCopy().
     */
    virtual void render(int offsetX = 0, int offsetY = 0);

    /**
     * Used to dynamically change the component's screen extent.
     */
    void setScreenExtent(const SDL_Rect& inScreenExtent);

    const entt::hashed_string& getKey();

protected:
    /** A reference to the screen that this component is a part of. Used for
        registering/unregistering named components, and accessing other
        components. */
    Screen& screen;

    /** The unique, user-assigned key. Used to identify the component for
        removal from the Screen's vector and map. */
    entt::hashed_string key;

    /** The component's screen extent, e.g. the size and position of the
        component on the screen. Used in hit testing and rendering. */
    SDL_Rect screenExtent;
};

} // namespace AUI
