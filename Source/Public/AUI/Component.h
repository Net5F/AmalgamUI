#pragma once

// Forward declarations.
struct SDL_Renderer;

namespace AUI {

/**
 * This is an abstract class, providing a way to interface with a UI component.
 */
class Component
{
public:
    virtual void renderCopy(const SDL_Renderer* renderer);
};

} // namespace AUI
