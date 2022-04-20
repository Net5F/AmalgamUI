#include "catch2/catch_all.hpp"
#include "AUI/Core.h"
#include "AUI/Internal/Log.h"
#include <SDL.h>

int SCREEN_WIDTH = 1920;
int SCREEN_HEIGHT = 1080;

int main(int argc, char* argv[])
{
    // Initialize SDL.
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        AUI_LOG_FATAL("Failed to initialize SDL: %s", SDL_GetError());
    }

    // Create our hidden window.
    // (We need to create a window to get a renderer to pass to Core, but
    //  we aren't going to draw anything in our unit tests.)
    SDL_Window* sdlWindow{SDL_CreateWindow(
        "AUI Unit Tests", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_HIDDEN)};
    if (sdlWindow == nullptr) {
        AUI_LOG_FATAL("Failed to create SDL_Window: %s", SDL_GetError());
    }

    // Create our renderer.
    SDL_Renderer* sdlRenderer{
        SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED)};
    if (sdlRenderer == nullptr) {
        AUI_LOG_FATAL("Failed to create SDL_Renderer: %s", SDL_GetError());
    }

    // Initialize AUI.
    AUI::Core::initialize(sdlRenderer, {1920, 1080});

    /* Run Tests */
    int result = Catch::Session().run(argc, argv);

    // Uninitalize AUI.
    AUI::Core::quit();

    // Uninitialize SDL.
    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();

    return result;
}
