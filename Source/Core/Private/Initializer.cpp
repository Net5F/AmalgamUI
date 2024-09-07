#include "AUI/Initializer.h"
#include "AUI/Core.h"
#include <SDL_image.h>
#include <SDL_ttf.h>

namespace AUI
{
Initializer::Initializer(SDL_Renderer* sdlRenderer,
                         ScreenResolution logicalScreenSize,
                         ScreenResolution actualScreenSize)
{
    // Initialize AUI.
    Core::initialize(sdlRenderer, logicalScreenSize, actualScreenSize);
}

Initializer::~Initializer()
{
    Core::quit();
}

} // namespace AUI
