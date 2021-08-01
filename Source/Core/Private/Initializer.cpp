#include "AUI/Initializer.h"
#include "AUI/Core.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

namespace AUI {

Initializer::Initializer(SDL_Renderer* sdlRenderer, ScreenResolution logicalScreenSize)
{
    // Initialize AUI.
    Core::initialize(sdlRenderer, logicalScreenSize);
}

Initializer::~Initializer()
{
    Core::quit();
}

} // namespace AUI
