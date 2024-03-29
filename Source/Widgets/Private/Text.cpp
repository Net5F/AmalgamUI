#include "AUI/Text.h"
#include "AUI/Core.h"
#include "AUI/ScalingHelpers.h"
#include <SDL_render.h>

namespace AUI
{
Text::Text(const SDL_Rect& inLogicalExtent, const std::string& inDebugName)
: Widget(inLogicalExtent, inDebugName)
, fontPath{""}
, logicalFontSize{10}
, font{}
, color{0, 0, 0, 255}
, backgroundColor{0, 0, 0, 0}
, renderMode{RenderMode::Blended}
, wordWrapEnabled{true}
, autoHeightEnabled{false}
, text("Initialized")
, verticalAlignment{VerticalAlignment::Top}
, horizontalAlignment{HorizontalAlignment::Left}
, lastUsedScreenSize{0, 0}
, textureIsDirty{true}
, textTexture{nullptr}
, textureExtent{}
, textExtent{}
, textOffset{0}
, offsetClippedTextExtent{}
, offsetClippedTextureExtent{}
{
}

void Text::setFont(std::string_view inFontPath, int size)
{
    // Save the data for later scaling.
    fontPath = inFontPath;
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
    if (text != inText) {
        text = inText;
        textureIsDirty = true;
    }
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

void Text::setWordWrapEnabled(bool inWordWrapEnabled)
{
    wordWrapEnabled = inWordWrapEnabled;
}

void Text::setAutoHeightEnabled(bool inAutoHeightEnabled)
{
    autoHeightEnabled = inAutoHeightEnabled;
}

void Text::setTextOffset(int inTextOffset)
{
    textOffset = inTextOffset;
}

void Text::insertText(std::string_view inText, std::size_t index)
{
    // Insert the given text at the given index.
    text.insert(index, inText);
    textureIsDirty = true;
}

bool Text::eraseCharacter(std::size_t index)
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

void Text::refreshTexture()
{
    if (!textureIsDirty) {
        // Nothing to refresh.
        return;
    }
    else if (!font) {
        AUI_LOG_FATAL("Please call setFont() before refreshTexture(), so"
                      " that a valid font object can be used for texture "
                      "generation. DebugName: %s",
                      debugName.c_str());
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
    if (wordWrapEnabled) {
        switch (renderMode) {
            case RenderMode::Solid:
                surface = TTF_RenderUTF8_Solid_Wrapped(
                    font.get(), textToRender->c_str(), color, scaledExtent.w);
                break;
            case RenderMode::Shaded:
                surface = TTF_RenderUTF8_Shaded_Wrapped(
                    font.get(), textToRender->c_str(), color, backgroundColor,
                    scaledExtent.w);
                break;
            case RenderMode::Blended:
                surface = TTF_RenderUTF8_Blended_Wrapped(
                    font.get(), textToRender->c_str(), color, scaledExtent.w);
                break;
                // Note: Removed because SDL_ttf on 22.04 doesn't support it.
                // case RenderMode::LCD:
                //    surface = TTF_RenderUTF8_LCD_Wrapped(
                //        font.get(), textToRender->c_str(), color,
                //        backgroundColor, scaledExtent.w);
                //    break;
        }
    }
    else {
        switch (renderMode) {
            case RenderMode::Solid:
                surface = TTF_RenderUTF8_Solid(font.get(),
                                               textToRender->c_str(), color);
                break;
            case RenderMode::Shaded:
                surface = TTF_RenderUTF8_Shaded(
                    font.get(), textToRender->c_str(), color, backgroundColor);
                break;
            case RenderMode::Blended:
                surface = TTF_RenderUTF8_Blended(font.get(),
                                                 textToRender->c_str(), color);
                break;
                // Note: Removed because SDL_ttf on 22.04 doesn't support it.
                // case RenderMode::LCD:
                //    surface = TTF_RenderUTF8_LCD(font.get(),
                //    textToRender->c_str(),
                //                                 color, backgroundColor);
                //    break;
        }
    }
    if (surface == nullptr) {
        AUI_LOG_FATAL("Failed to create surface.");
    }

    // Move the image to a texture on the gpu.
    SDL_Texture* texture{
        SDL_CreateTextureFromSurface(Core::getRenderer(), surface)};
    SDL_FreeSurface(surface);
    if (texture == nullptr) {
        AUI_LOG_FATAL("Failed to create texture.");
    }

    // Give ownership of the texture pointer to our smart pointer.
    textTexture = std::unique_ptr<SDL_Texture, TextureDeleter>(texture);

    // Save the width and height of the new texture.
    SDL_QueryTexture(textTexture.get(), nullptr, nullptr, &(textureExtent.w),
                     &(textureExtent.h));
    textExtent = {0, 0, textureExtent.w, textureExtent.h};

    // If auto-height is enabled, set this widget's height to match the texture.
    if (autoHeightEnabled) {
        logicalExtent.h = ScalingHelpers::actualToLogical(textExtent.h);
    }

    // Calc our new aligned position.
    refreshAlignment();

    textureIsDirty = false;
}

const std::string& Text::asString()
{
    return text;
}

SDL_Rect Text::calcCharacterOffset(std::size_t index)
{
    // Get a null-terminated substring containing all characters up to index.
    std::string relevantChars{text, 0, index};

    // Get the x offset and height from the relevant characters.
    SDL_Rect offsetExtent{};
    TTF_SizeUTF8(font.get(), relevantChars.c_str(), &(offsetExtent.x),
                 &(offsetExtent.h));

    // Account for our alignment/position by adding the text extent's offset.
    offsetExtent.x += textExtent.x;
    offsetExtent.y += textExtent.y;

    // Account for our current text offset.
    offsetExtent.x += textOffset;

    return offsetExtent;
}

int Text::calcStringWidth(const std::string& string)
{
    // Calculate the width that the given string would have if rendered using
    // the current font.
    int stringWidth{0};
    TTF_SizeUTF8(font.get(), string.c_str(), &(stringWidth), nullptr);

    return stringWidth;
}

Text::VerticalAlignment Text::getVerticalAlignment()
{
    return verticalAlignment;
}

Text::HorizontalAlignment Text::getHorizontalAlignment()
{
    return horizontalAlignment;
}

int Text::getTextOffset()
{
    return textOffset;
}

void Text::setLogicalExtent(const SDL_Rect& inLogicalExtent)
{
    // Scale and set the extent.
    Widget::setLogicalExtent(inLogicalExtent);

    // Refresh our alignment.
    refreshAlignment();
}

void Text::updateLayout(const SDL_Point& startPosition,
                        const SDL_Rect& availableExtent,
                        WidgetLocator* widgetLocator)
{
    // Do the normal layout updating.
    Widget::updateLayout(startPosition, availableExtent, widgetLocator);

    // If the UI scaling has changed, refresh everything.
    if (lastUsedScreenSize != Core::getActualScreenSize()) {
        refreshScaling();
        lastUsedScreenSize = Core::getActualScreenSize();
    }
    // Else if a property has been changed, just re-render our text texture.
    else if (textureIsDirty) {
        refreshTexture();
    }

    // If this widget is fully clipped, return early.
    if (SDL_RectEmpty(&clippedExtent)) {
        return;
    }

    // Offset our textExtent to start at startPosition.
    SDL_Rect offsetTextExtent{textExtent};
    offsetTextExtent.x += (startPosition.x + textOffset);
    offsetTextExtent.y += startPosition.y;

    // Clip the text image's extent to not go beyond this widget's extent.
    SDL_IntersectRect(&offsetTextExtent, &clippedExtent,
                      &offsetClippedTextExtent);

    // Pull offsetClippedTextExtent back into texture space ((0, 0) origin).
    // This tells us what part of the text image texture to actually render.
    offsetClippedTextureExtent = offsetClippedTextExtent;
    offsetClippedTextureExtent.x -= offsetTextExtent.x;
    offsetClippedTextureExtent.y -= offsetTextExtent.y;
}

void Text::render(const SDL_Point& windowTopLeft)
{
    // If this widget is fully clipped, don't render it.
    if (SDL_RectEmpty(&clippedExtent)) {
        return;
    }

    if (textTexture == nullptr) {
        AUI_LOG_FATAL("Tried to render Font with no texture. DebugName: %s",
                      debugName.c_str());
    }

    // Render the text texture.
    SDL_Rect finalExtent{offsetClippedTextExtent};
    finalExtent.x += windowTopLeft.x;
    finalExtent.y += windowTopLeft.y;
    SDL_RenderCopy(Core::getRenderer(), textTexture.get(),
                   &offsetClippedTextureExtent, &finalExtent);
}

void Text::refreshScaling()
{
    // Refresh our alignment since the extent has moved.
    refreshAlignment();

    // Refresh our font object to match the new scale.
    refreshFontObject();

    // Re-render the text texture.
    refreshTexture();
    textureIsDirty = false;
}

void Text::refreshAlignment()
{
    // Calc the appropriate vertical alignment.
    switch (verticalAlignment) {
        case VerticalAlignment::Top: {
            textExtent.y = scaledExtent.y;
            break;
        }
        case VerticalAlignment::Center: {
            textExtent.y
                = scaledExtent.y + ((scaledExtent.h - textureExtent.h) / 2);
            break;
        }
        case VerticalAlignment::Bottom: {
            textExtent.y = (scaledExtent.y + scaledExtent.h) - textureExtent.h;
            break;
        }
    }

    // Calc the appropriate horizontal alignment.
    switch (horizontalAlignment) {
        case HorizontalAlignment::Left: {
            textExtent.x = scaledExtent.x;
            break;
        }
        case HorizontalAlignment::Center: {
            textExtent.x
                = scaledExtent.x + ((scaledExtent.w - textureExtent.w) / 2);
            break;
        }
        case HorizontalAlignment::Right: {
            textExtent.x = (scaledExtent.x + scaledExtent.w) - textureExtent.w;
            break;
        }
    }
}

void Text::refreshFontObject()
{
    // Scale the font size to the current actual size.
    int actualFontSize{ScalingHelpers::logicalToActual(logicalFontSize)};

    // Attempt to load the given font (errors on failure).
    AssetCache& assetCache{Core::getAssetCache()};
    font = assetCache.requestFont(fontPath, actualFontSize);
}

} // namespace AUI
