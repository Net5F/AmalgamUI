#include "AUI/Initializer.h"
#include "AUI/Core.h"
#include <SDL_image.h>
#include <SDL_ttf.h>

namespace AUI {

Initializer::Initializer(const std::string& resourcePath, SDL_Renderer* sdlRenderer)
{
    Core::Initialize(resourcePath, sdlRenderer);
}

Initializer::~Initializer()
{
    Core::Quit();
}

} // namespace AUI
