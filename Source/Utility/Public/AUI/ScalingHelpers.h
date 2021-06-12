#pragma once

#include "AUI/Margins.h"
#include <SDL_Rect.h>

namespace AUI
{

/**
 * Static functions for scaling between logical and actual sizes.
 *
 * Note: UI components exist in two spaces: logical and actual.
 *       Logical screen space is used for all developer-given component
 *       measurements, and is relative to the value of Core::logicalScreenSize.
 *       Actual screen space is what's actually used in rendering to the
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

    /** Overload for SDL_Rect. See logicalToActual(int). */
    static SDL_Rect logicalToActual(const SDL_Rect& logicalExtent);

    /** Overload for SDL_Point. See logicalToActual(int). */
    static SDL_Point logicalToActual(const SDL_Point& logicalPoint);

    /** Overload for Margins. See logicalToActual(int). */
    static Margins logicalToActual(Margins logicalMargins);

    /**
     * Applies the inverse of the current UI scaling to the given actual-space
     * value, returning its logical-space equivalent.
     * Uses Core::logicalScreenSize and Core::actualScreenSize.
     */
    static SDL_Point actualToLogical(const SDL_Point& actualPoint);
};

} // namespace AUI
