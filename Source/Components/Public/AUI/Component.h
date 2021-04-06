#pragma once

#include <SDL_Rect.h>
#include <string>

namespace AUI {

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
    Component(const std::string& inDebugKey, const SDL_Rect& inScreenExtent);

    virtual ~Component();

    void setScreenExtent(const SDL_Rect& inScreenExtent);

    /**
     * Handles all drawing needs of a particular component.
     * Directly calls SDL_RenderCopy().
     */
    virtual void renderCopy(int offsetX = 0, int offsetY = 0);

protected:
    /** The user-assigned key associated with this component. Corresponds to
        this element's key in the Screen's componentMap.
        In this context, the key is only useful for debugging. For performance
        reasons, avoid using it in real logic. */
    const std::string debugKey;

    /** The component's screen extent, e.g. the size and position of the
        component on the screen. Used in hit testing and rendering. */
    SDL_Rect screenExtent;
};

} // namespace AUI
