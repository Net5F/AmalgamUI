#include "AUI/Initializer.h"
#include "AUI/Core.h"
#include <SDL_image.h>
#include <SDL_ttf.h>

namespace AUI {

Initializer::Initializer(const std::string& resourcePath, SDL_Renderer* sdlRenderer, ScreenResolution logicalScreenSize)
{
    Core::Initialize(resourcePath, sdlRenderer, logicalScreenSize);
}

Initializer::~Initializer()
{
    Core::Quit();
}

} // namespace AUI
