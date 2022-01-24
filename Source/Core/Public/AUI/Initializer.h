#pragma once

#include "AUI/ScreenResolution.h"

// Forward declarations.
struct SDL_Renderer;

namespace AUI
{
/**
 * Helper class for calling Core::Initialize() and Core::Quit() at the
 * appropriate times.
 *
 * Put this class first in your initialization order so that its constructor is
 * called before any screens or widgets are constructed, and its destructor
 * is called after all of the screens and widgets are destructed.
 */
class Initializer
{
public:
    /**
     * @param sdlRenderer  The renderer to use for constructing textures and
     *                     rendering.
     * @param logicalScreenSize  See Core::setLogicalScreenSize().
     */
    Initializer(SDL_Renderer* sdlRenderer, ScreenResolution logicalScreenSize);

    ~Initializer();
};

} // namespace AUI
