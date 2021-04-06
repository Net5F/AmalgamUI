#pragma once

#include <string>

// Forward declarations.
struct SDL_Renderer;

namespace AUI {

/**
 * Helper class for calling Core::Initialize() and Core::Quit() at the
 * appropriate times.
 *
 * Put this class first in your initialization order so that its constructor is
 * called before any screens or components are constructed, and its destructor
 * is called after all of the screens and components are destructed.
 */
class Initializer
{
public:
    /**
     * @param resourcePath  The base path to use when opening resource files.
     *                      Provided for convenience, can be whatever is
     *                      useful for you.
     * @param sdlRenderer  The renderer to use for constructing textures and
     *                     rendering.
     */
    Initializer(const std::string& resourcePath, SDL_Renderer* sdlRenderer);

    ~Initializer();
};

} // namespace AUI
