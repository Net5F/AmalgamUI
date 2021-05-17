#include "AUI/Text.h"
#include "AUI/Core.h"
#include "AUI/Internal/ScalingHelpers.h"
#include <SDL_Render.h>

namespace AUI {

Text::Text(Screen& screen, const char* key, const SDL_Rect& screenExtent)
: Component(screen, key, screenExtent)
, fontPath("")
, logicalFontSize{10}
, fontHandle()
, color{255, 255, 255, 255}
, backgroundColor{0, 0, 0, 0}
, renderMode{RenderMode::Blended}
, text("Initialized")
, verticalAlignment{VerticalAlignment::Top}
, horizontalAlignment{HorizontalAlignment::Left}
, textureIsDirty{true}
, textTexture{nullptr}
, texExtent{}
, alignedExtent{}
{
}

void Text::setFont(const std::string& relPath, int size)
{
    // Save the data for later scaling.
    fontPath = relPath;
    logicalFontSize = size;

    // Load the new font object.
    refreshFontObject();

    textureIsDirty = true;
}

void Text::setColor(const SDL_Color& inColor)
{
    color = inColor;
    textureIsDirty = true;
}

void Text::setBackgroundColor(const SDL_Color& inBackgroundColor)
{
    backgroundColor = inBackgroundColor;
    textureIsDirty = true;
}

void Text::setRenderMode(RenderMode inRenderMode)
{
    renderMode = inRenderMode;
    textureIsDirty = true;
}

void Text::setText(const std::string& inText)
{
    text = inText;
    textureIsDirty = true;
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

void Text::render(const SDL_Point& parentOffset)
{
    // Keep our scaling up to date.
    bool isRefreshed = refreshScaling();

    // If we didn't already refresh our texture.
    if (!isRefreshed) {
        // If a property has been changed, re-render our text texture.
        if (textureIsDirty) {
            refreshTexture();
        }
    }

    if (textTexture == nullptr) {
        AUI_LOG_ERROR("Tried to render Font with no texture. Key: %s", key.data());
    }

    // Account for the given offset.
    SDL_Rect offsetExtent{alignedExtent};
    offsetExtent.x += parentOffset.x;
    offsetExtent.y += parentOffset.y;

    // Save the extent that we should render at.
    lastRenderedExtent = offsetExtent;

    // If the component isn't visible, return without rendering.
    if (!isVisible) {
        return;
    }

    // Render the text texture.
    SDL_RenderCopy(Core::GetRenderer(), textTexture.get()
        , &texExtent, &offsetExtent);
}

bool Text::refreshScaling()
{
    // If actualScreenExtent was refreshed, do our specialized refreshing.
    if (Component::refreshScaling()) {
        // Refresh our alignment since the extent has moved.
        refreshAlignment();

        // Refresh our font object to match the new scale.
        refreshFontObject();

        // Re-render the text texture.
        refreshTexture();

        return true;
    }

    return false;
}

void Text::refreshAlignment()
{
    // Calc the appropriate vertical alignment.
    switch (verticalAlignment) {
        case VerticalAlignment::Top: {
            alignedExtent.y = actualScreenExtent.y;
            break;
        }
        case VerticalAlignment::Middle: {
            alignedExtent.y = actualScreenExtent.y + ((actualScreenExtent.h - texExtent.h) / 2);
            break;
        }
        case VerticalAlignment::Bottom: {
            alignedExtent.y = (actualScreenExtent.y + actualScreenExtent.h) - texExtent.h;
            break;
        }
    }

    // Calc the appropriate horizontal alignment.
    switch (horizontalAlignment) {
        case HorizontalAlignment::Left: {
            alignedExtent.x = actualScreenExtent.x;
            break;
        }
        case HorizontalAlignment::Middle: {
            alignedExtent.x = actualScreenExtent.x + ((actualScreenExtent.w - texExtent.w) / 2);
            break;
        }
        case HorizontalAlignment::Right: {
            alignedExtent.x = (actualScreenExtent.x + actualScreenExtent.w) - texExtent.w;
            break;
        }
    }
}

void Text::refreshFontObject()
{
    // Scale the font size to the current actual size.
    int actualFontSize = ScalingHelpers::fontSizeToActual(logicalFontSize);

    // Attempt to load the given font (errors on failure).
    ResourceManager& resourceManager = Core::GetResourceManager();
    fontHandle = resourceManager.loadFont(fontPath, actualFontSize);
}

void Text::refreshTexture()
{
    if (!fontHandle) {
        AUI_LOG_ERROR("Please call setFont() before renderTextTexture(), so"
        " that a valid font object can be used for texture generation.");
    }

    // Create a temporary surface on the cpu and render our image using the
    // set renderMode.
    SDL_Surface* surface{nullptr};
    switch (renderMode) {
        case RenderMode::Solid: {
            surface = TTF_RenderText_Solid(&(*fontHandle), text.c_str(), color);
            break;
        }
        case RenderMode::Shaded: {
            surface = TTF_RenderText_Shaded(&(*fontHandle), text.c_str(), color, backgroundColor);
            break;
        }
        case RenderMode::Blended: {
            surface = TTF_RenderText_Blended(&(*fontHandle), text.c_str(), color);
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

    textureIsDirty = false;
}

} // namespace AUI
