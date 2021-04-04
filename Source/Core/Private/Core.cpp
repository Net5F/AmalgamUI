#include "AUI/Core.h"
#include <SDL_Render.h>

namespace AUI {

std::string Core::resourcePath{""};
SDL_Renderer* Core::sdlRenderer{nullptr};
ResourceManager Core::resourceManager;

void Core::Initialize(const std::string& inResourcePath, SDL_Renderer* inSdlRenderer)
{
    resourcePath = inResourcePath;
    sdlRenderer = inSdlRenderer;
}

const std::string& Core::GetResourcePath()
{
    return resourcePath;
}

SDL_Renderer* Core::GetRenderer()
{
    return sdlRenderer;
}

ResourceManager& Core::GetResourceManager()
{
    return resourceManager;
}

} // namespace AUI
