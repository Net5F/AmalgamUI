#include "AUI/Text.h"
#include "AUI/Core.h"
#include <SDL_Render.h>

namespace AUI {

Text::Text(Screen& screen, const char* key, const SDL_Rect& screenExtent)
: Component(screen, key, screenExtent)
, fontHandle()
, color{255, 255, 255, 255}
, renderMode{RenderMode::Blended}
, textTexture{nullptr}
, texExtent{}
, verticalAlignment{VerticalAlignment::Top}
, horizontalAlignment{HorizontalAlignment::Left}
, alignedExtent{}
{
}

void Text::setFont(const std::string& relPath, int size)
{
    // Attempt to load the given font (errors on failure).
    ResourceManager& resourceManager = Core::GetResourceManager();
    fontHandle = resourceManager.loadFont(relPath, size);
}

void Text::setColor(const SDL_Color& inColor)
{
    color = inColor;
}

void Text::setBackgroundColor(const SDL_Color& inBackgroundColor)
{
    backgroundColor = inBackgroundColor;
}

void Text::setRenderMode(RenderMode inRenderMode)
{
    renderMode = inRenderMode;
}

void Text::setText(const std::string& inText)
{
    if (!fontHandle) {
        AUI_LOG_ERROR("Please call setFont() before setText(), so that a valid "
        "font object can be used for texture generation.")
    }

    // Create a temporary surface on the cpu and render our image using the
    // set renderMode.
    SDL_Surface* surface{nullptr};
    switch (renderMode) {
        case RenderMode::Solid: {
            surface = TTF_RenderText_Solid(&(*fontHandle), inText.c_str(), color);
            break;
        }
        case RenderMode::Shaded: {
            surface = TTF_RenderText_Shaded(&(*fontHandle), inText.c_str(), color, backgroundColor);
            break;
        }
        case RenderMode::Blended: {
            surface = TTF_RenderText_Blended(&(*fontHandle), inText.c_str(), color);
            break;
        }
    }
    if (surface == nullptr) {
        AUI_LOG_ERROR("Failed to create surface.");
    }

    // Move the image to a texture on the gpu.
    SDL_Texture* texture = SDL_CreateTextureFromSurface(Core::GetRenderer(), surface);
    SDL_FreeSurface(surface);
    if (texture == nullptr) {
        AUI_LOG_ERROR("Failed to create texture.");
    }

    // Give ownership of the texture pointer to our smart pointer.
    textTexture = std::unique_ptr<SDL_Texture, TextureDeleter>(texture);

    // Save the width and height of the new texture.
    SDL_QueryTexture(textTexture.get(), nullptr, nullptr, &(texExtent.w), &(texExtent.h));
    alignedExtent = {0, 0, texExtent.w, texExtent.h};

    // Calc our new aligned position.
    refreshAlignment();
}

void Text::setVerticalAlignment(VerticalAlignment inVerticalAlignment)
{
    verticalAlignment = inVerticalAlignment;
    refreshAlignment();
}

void Text::setHorizontalAlignment(HorizontalAlignment inHorizontalAlignment)
{
    horizontalAlignment = inHorizontalAlignment;
    refreshAlignment();
}

void Text::render(int offsetX, int offsetY)
{
    if (textTexture == nullptr) {
        AUI_LOG_ERROR("Tried to render Font with no texture. Key: %s", key.data());
    }

    // Account for the given offset.
    SDL_Rect offsetExtent{alignedExtent};
    offsetExtent.x += offsetX;
    offsetExtent.y += offsetY;

    // Render the text texture.
    SDL_RenderCopy(Core::GetRenderer(), textTexture.get(), &texExtent, &offsetExtent);
}

void Text::refreshAlignment()
{
    // Calc the appropriate vertical alignment.
    switch (verticalAlignment) {
        case VerticalAlignment::Top: {
            alignedExtent.y = screenExtent.y;
            break;
        }
        case VerticalAlignment::Middle: {
            alignedExtent.y = screenExtent.y + ((screenExtent.h - texExtent.h) / 2);
            break;
        }
        case VerticalAlignment::Bottom: {
            alignedExtent.y = (screenExtent.y + screenExtent.h) - texExtent.h;
            break;
        }
    }

    // Calc the appropriate horizontal alignment.
    switch (horizontalAlignment) {
        case HorizontalAlignment::Left: {
            alignedExtent.x = screenExtent.x;
            break;
        }
        case HorizontalAlignment::Middle: {
            alignedExtent.x = screenExtent.x + ((screenExtent.w - texExtent.w) / 2);
            break;
        }
        case HorizontalAlignment::Right: {
            alignedExtent.x = (screenExtent.x + screenExtent.w) - texExtent.w;
            break;
        }
    }
}

} // namespace AUI
