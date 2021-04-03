#pragma once

#include "Component.h"
#include <memory>

// Forward declarations.
struct SDL_Texture;
struct SDL_Renderer;

namespace AUI {

/**
 * This component displays a simple static image.
 */
class Image : public Component
{
public:
    Image(int inX, int inY, int inWidth, int inHeight, const std::shared_ptr<SDL_Texture>& inTexture);

    void renderCopy(const SDL_Renderer* renderer) override;

private:
    int x;
    int y;

    int width;
    int height;

    std::shared_ptr<SDL_Texture> texture;
};

} // namespace AUI
