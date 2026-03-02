#pragma once

#include <SDL3/SDL_rect.h>

namespace AUI
{
/**
 * Static functions for scaling between logical and actual sizes.
 *
 * Note: UI widgets exist in two spaces: logical and actual.
 *       Logical screen space is used for all developer-given widget
 *       measurements, and is relative to the value of Core::logicalScreenSize.
 *       Actual screen space is what's used when actually rendering to the
 *       screen, and is relative to the value of Core::actualScreenSize.
 *
 *       The intent is that the developer will set a logical screen size and
 *       give all width, height, position, etc values in reference to that
 *       logical size. Then, the user can select a new actual size and the UI
 *       will intelligently scale to match it.
 */
class ScalingHelpers
{
public:
    /**
     * Scales the given logical-space value to match the current UI scaling,
     * returning its actual-space equivalent.
     * Uses Core::logicalScreenSize and Core::actualScreenSize.
     */
    static int logicalToActual(int logicalInt);

    /** Overload for unsigned int. See logicalToActual(int). */
    static unsigned int logicalToActual(unsigned int logicalInt);

    /** Overload for float. See logicalToActual(int). */
    static float logicalToActual(float logicalFloat);

    /** Overload for SDL_FRect. See logicalToActual(int). */
    static SDL_FRect logicalToActual(const SDL_FRect& logicalExtent);

    /** Overload for SDL_FPoint. See logicalToActual(int). */
    static SDL_FPoint logicalToActual(const SDL_FPoint& logicalPoint);

    /**
     * Applies the inverse of the current UI scaling to the given actual-space
     * value, returning its logical-space equivalent.
     * Uses Core::logicalScreenSize and Core::actualScreenSize.
     */
    static int actualToLogical(int actualInt);

    /** Overload for unsigned int. See actualToLogical(int). */
    static unsigned int actualToLogical(unsigned int actualInt);

    /** Overload for float. See actualToLogical(int). */
    static float actualToLogical(float actualFloat);

    /** Overload for SDL_FRect. See actualToLogical(int). */
    static SDL_FRect actualToLogical(const SDL_FRect& actualExtent);

    /** Overload for SDL_FPoint. See actualToLogical(int). */
    static SDL_FPoint actualToLogical(const SDL_FPoint& actualPoint);

    /**
     * Scales logicalExtent to actual space, offsets it to start at 
     * startPosition, then clips it to fit within availableExtent.
     *
     * Returns an extent that's ready for rendering.
     */
    static SDL_FRect logicalToClipped(const SDL_FRect& logicalExtent,
                                     const SDL_FPoint& startPosition,
                                     const SDL_FRect& availableExtent);
};

} // namespace AUI
