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
, alignedTexExtent{}
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

void Text::setText(std::string_view inText)
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

void Text::insertText(std::string_view inText, unsigned int index)
{
    // Insert the given text at the given index.
    text.insert(index, inText);
    textureIsDirty = true;
}

bool Text::eraseCharacter(unsigned int index)
{
    // If there's a character to remove, pop it.
    if (text.length() > index) {
        text.erase(text.begin() + index);
        textureIsDirty = true;
        return true;
    }
    else {
        // Else, the string was empty.
        return false;
    }
}

const std::string& Text::asString()
{
    return text;
}

SDL_Rect Text::calcCharacterOffset(unsigned int index)
{
    // Get a null-terminated substring containing all characters up to index.
    std::string relevantChars{text, 0, index};

    // Get the x offset and height from the relevant characters.
    SDL_Rect offsetExtent{};
    TTF_SizeText(&(*(fontHandle)), relevantChars.c_str(), &(offsetExtent.x)
                 , &(offsetExtent.h));

    // Account for our alignment by adding the aligned extent's offset.
    offsetExtent.x += alignedTexExtent.x;
    offsetExtent.y += alignedTexExtent.y;

    return offsetExtent;
}

Text::VerticalAlignment Text::getVerticalAlignment()
{
    return verticalAlignment;
}

Text::HorizontalAlignment Text::getHorizontalAlignment()
{
    return horizontalAlignment;
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
            textureIsDirty = false;
        }
    }

    if (textTexture == nullptr) {
        AUI_LOG_ERROR("Tried to render Font with no texture. Key: %s", key.data());
    }

    // Account for the given offset.
    SDL_Rect offsetExtent{alignedTexExtent};
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
        textureIsDirty = false;

        return true;
    }

    return false;
}

void Text::refreshAlignment()
{
    // Calc the appropriate vertical alignment.
    switch (verticalAlignment) {
        case VerticalAlignment::Top: {
            alignedTexExtent.y = scaledExtent.y;
            break;
        }
        case VerticalAlignment::Middle: {
            alignedTexExtent.y = scaledExtent.y + ((scaledExtent.h - texExtent.h) / 2);
            break;
        }
        case VerticalAlignment::Bottom: {
            alignedTexExtent.y = (scaledExtent.y + scaledExtent.h) - texExtent.h;
            break;
        }
    }

    // Calc the appropriate horizontal alignment.
    switch (horizontalAlignment) {
        case HorizontalAlignment::Left: {
            alignedTexExtent.x = scaledExtent.x;
            break;
        }
        case HorizontalAlignment::Middle: {
            alignedTexExtent.x = scaledExtent.x + ((scaledExtent.w - texExtent.w) / 2);
            break;
        }
        case HorizontalAlignment::Right: {
            alignedTexExtent.x = (scaledExtent.x + scaledExtent.w) - texExtent.w;
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
        AUI_LOG_ERROR("Please call setFont() before refreshTexture(), so"
        " that a valid font object can be used for texture generation.");
    }

    // If the text string is empty, render a space instead.
    std::string spaceText{" "};
    std::string* textToRender{&text};
    if (text == "") {
        textToRender = &spaceText;
    }

    // Create a temporary surface on the cpu and render our image using the
    // set renderMode.
    SDL_Surface* surface{nullptr};
    switch (renderMode) {
        case RenderMode::Solid: {
            surface = TTF_RenderText_Solid(&(*fontHandle), textToRender->c_str(), color);
            break;
        }
        case RenderMode::Shaded: {
            surface = TTF_RenderText_Shaded(&(*fontHandle), textToRender->c_str(), color, backgroundColor);
            break;
        }
        case RenderMode::Blended: {
            surface = TTF_RenderText_Blended(&(*fontHandle), textToRender->c_str(), color);
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
    alignedTexExtent = {0, 0, texExtent.w, texExtent.h};

    // Calc our new aligned position.
    refreshAlignment();

    textureIsDirty = false;
}

} // namespace AUI
