#include "AUI/Text.h"
#include "AUI/Core.h"
#include "AUI/ScalingHelpers.h"
#include "AUI/Internal/Log.h"
#include <SDL_render.h>

namespace AUI {
Text::Text(const SDL_Rect& inLogicalExtent, const std::string& inDebugName)
: Widget(inLogicalExtent, inDebugName)
, fontPath{ "" }
, logicalFontSize{ 10 }
, logicalFontOutlineSize{ 0 }
, font{}
, outlinedFont{}
, color{ 0, 0, 0, 255 }
, backgroundColor{ 0, 0, 0, 0 }
, renderMode{ RenderMode::Blended }
, wordWrapEnabled{ true }
, autoHeightEnabled{ false }
, text{ "Initialized" }
, verticalAlignment{ VerticalAlignment::Top }
, horizontalAlignment{ HorizontalAlignment::Left }
, lastUsedScreenSize{ 0, 0 }
, textureIsDirty{ true }
, alignmentIsDirty{ true }
, textTexture{ nullptr }
, textureExtent{}
, textExtent{}
, textOffset{ 0 }
, lastStartPosition{ 0, 0 }
, offsetClippedTextExtent{}
, offsetClippedTextureExtent{}
{
}

void Text::setFont(std::string_view inFontPath, int inLogicalFontSize,
                   int inLogicalFontOutlineSize)
{
    // Save the data for later scaling.
    fontPath = inFontPath;
    logicalFontSize = inLogicalFontSize;
    logicalFontOutlineSize = inLogicalFontOutlineSize;

    // Load the new font object.
    refreshFontObject();

    invalidateTexture();
}

void Text::setColor(const SDL_Color& inColor)
{
    color = inColor;
    invalidateTexture();
}

void Text::setBackgroundColor(const SDL_Color& inBackgroundColor)
{
    backgroundColor = inBackgroundColor;
    invalidateTexture();
}

void Text::setRenderMode(RenderMode inRenderMode)
{
    renderMode = inRenderMode;
    invalidateTexture();
}

void Text::setText(std::string_view inText)
{
    if (text != inText) {
        text = inText;
        invalidateTexture();
    }
}

void Text::setVerticalAlignment(VerticalAlignment inVerticalAlignment)
{
    verticalAlignment = inVerticalAlignment;
    invalidateAlignment();
}

void Text::setHorizontalAlignment(HorizontalAlignment inHorizontalAlignment)
{
    horizontalAlignment = inHorizontalAlignment;
    invalidateAlignment();
}

void Text::setWordWrapEnabled(bool inWordWrapEnabled)
{
    if (inWordWrapEnabled == wordWrapEnabled) {
        return;
    }

    wordWrapEnabled = inWordWrapEnabled;

    // Wrapping changes the size of the rendered text.
    invalidateTexture();
}

void Text::setAutoHeightEnabled(bool inAutoHeightEnabled)
{
    if (inAutoHeightEnabled == autoHeightEnabled) {
        return;
    }

    autoHeightEnabled = inAutoHeightEnabled;

    // Our height now follows (or stops following) our texture, which only
    // measure() can apply.
    Core::markLayoutDirty();
}

void Text::setTextOffset(int inTextOffset)
{
    if (inTextOffset == textOffset) {
        return;
    }

    textOffset = inTextOffset;

    // This shifts where our text sits within our extent, which refreshTextExtents()
    // applies during render().
    invalidateAlignment();
}

void Text::insertText(std::string_view inText, std::size_t index)
{
    // Insert the given text at the given index.
    text.insert(index, inText);
    invalidateTexture();
}

bool Text::eraseCharacter(std::size_t index)
{
    // If there's a character to remove, pop it.
    if (text.length() > index) {
        text.erase(text.begin() + index);
        invalidateTexture();
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

    // Create a temporary surface on the cpu and render our text image using the
    // current renderMode.
    SDL_Surface* surface{ getSurface(font.get(), color, backgroundColor) };

    // If we have an outline, create an outlined background surface and blit
    // the text image onto it.
    if (logicalFontOutlineSize > 0) {
        // Create a temporary surface using the outlined background text.
        SDL_Color blackColor{ 0, 0, 0, 0 };
        SDL_Surface* backgroundSurface{ getSurface(outlinedFont.get(),
                                                   blackColor, blackColor) };

        // Calculate the foreground text's offset to center it on the
        // outlined background text.
        int actualOutlineSize{ ScalingHelpers::logicalToActual(
            logicalFontOutlineSize) };
        SDL_Rect foregroundExtent{ actualOutlineSize, actualOutlineSize,
                                   surface->w - actualOutlineSize,
                                   surface->h - actualOutlineSize };

        // Blit the foreground text onto the background outlined text.
        SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);
        SDL_BlitSurface(surface, nullptr, backgroundSurface, &foregroundExtent);

        // Free the old foreground surface and set the new combined surface
        // as the one to use.
        SDL_FreeSurface(surface);
        surface = backgroundSurface;
    }

    // Move the image to a texture on the gpu.
    SDL_Texture* texture{ SDL_CreateTextureFromSurface(Core::getRenderer(),
                                                       surface) };
    SDL_FreeSurface(surface);
    if (texture == nullptr) {
        AUI_LOG_FATAL("Failed to create texture.");
    }

    // Give ownership of the texture pointer to our smart pointer.
    textTexture = std::unique_ptr<SDL_Texture, TextureDeleter>(texture);

    // Save the width and height of the new texture.
    SDL_QueryTexture(textTexture.get(), nullptr, nullptr, &(textureExtent.w),
                     &(textureExtent.h));
    textExtent = { 0, 0, textureExtent.w, textureExtent.h };

    textureIsDirty = false;
    alignmentIsDirty = true;
}

const std::string& Text::asString()
{
    return text;
}

SDL_Rect Text::calcCharacterOffset(std::size_t index)
{
    // Get a null-terminated substring containing all characters up to index.
    std::string relevantChars{ text, 0, index };

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
    int stringWidth{ 0 };
    TTF_SizeUTF8(font.get(), string.c_str(), &(stringWidth), nullptr);

    return stringWidth;
}

SDL_Rect Text::getLogicalTextureExtent()
{
    return ScalingHelpers::actualToLogical(textureExtent);
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
    // Note: This marks the layout dirty for us if the extent actually changed.
    Widget::setLogicalExtent(inLogicalExtent);

    // Refresh our alignment, since we align against our own extent.
    invalidateAlignment();
}

void Text::measure(const SDL_Rect& availableExtent)
{
    // If the UI scaling has changed, refresh everything.
    if (lastUsedScreenSize != Core::getActualScreenSize()) {
        refreshScaling();
        lastUsedScreenSize = Core::getActualScreenSize();
    }
    // Else if a property has been changed, just re-render our text texture.
    else if (textureIsDirty) {
        refreshTexture();
    }

    // If auto-height is enabled, set this widget's height to match the texture.
    // Note: We don't adjust to fit availableExtent because we want to match
    //       the texture's size, not the parent's size. We'll clip in arrange()
    //       if necessary.
    if (autoHeightEnabled) {
        logicalExtent.h = ScalingHelpers::actualToLogical(textExtent.h);
        alignmentIsDirty = true;
    }

    // Run the normal measure step (sets our scaledExtent).
    // Note: This must be done after setting logicalExtent above.
    Widget::measure(availableExtent);

    // If the text alignment is dirty, refresh it.
    // Note: This must be done after measure(), since it uses scaledExtent.
    if (alignmentIsDirty) {
        refreshAlignment();
    }
}

void Text::arrange(const SDL_Point& startPosition,
                   const SDL_Rect& availableExtent,
                   WidgetLocator* widgetLocator)
{
    // Run the normal arrange step.
    Widget::arrange(startPosition, availableExtent, widgetLocator);

    // If this widget is fully clipped, return early.
    if (SDL_RectEmpty(&clippedExtent)) {
        return;
    }

    // Save our start position, so render() can redo the calc below if our
    // texture changes without another layout pass.
    lastStartPosition = startPosition;

    refreshTextExtents();
}

void Text::render(const SDL_Point& windowTopLeft)
{
    // If this widget is fully clipped, don't render it.
    if (SDL_RectEmpty(&clippedExtent)) {
        return;
    }

    // If a property has been changed, re-render our text texture.
    // Note: This is done here (as well as in measure()) so that text can be
    //       updated without running a full layout pass.
    if (textureIsDirty) {
        refreshTexture();
    }

    // If our alignment is dirty, refresh it and our clipped extents.
    // Note: refreshTexture() dirties the alignment, since the new texture may
    //       be a different size than the old one. Without this, we'd render
    //       the new texture into the extents that were calc'd for the old one.
    if (alignmentIsDirty) {
        refreshAlignment();
        refreshTextExtents();
    }

    if (textTexture == nullptr) {
        AUI_LOG_FATAL("Tried to render Font with no texture. DebugName: %s",
                      debugName.c_str());
    }

    // Render the text texture.
    SDL_Rect finalExtent{ offsetClippedTextExtent };
    finalExtent.x += windowTopLeft.x;
    finalExtent.y += windowTopLeft.y;
    SDL_RenderCopy(Core::getRenderer(), textTexture.get(),
                   &offsetClippedTextureExtent, &finalExtent);
}

void Text::refreshScaling()
{
    // Refresh our font object to match the new scale.
    refreshFontObject();

    // Re-render the text texture.
    refreshTexture();
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

    alignmentIsDirty = false;
}

void Text::invalidateTexture()
{
    textureIsDirty = true;

    // Note: refreshTexture() runs during render(), so a new texture normally
    //       only needs a redraw. The exception is auto-height, where our height
    //       follows the texture's height, which only measure() can apply.
    if (autoHeightEnabled) {
        Core::markLayoutDirty();
    }
    else {
        Core::markRenderDirty();
    }
}

void Text::invalidateAlignment()
{
    alignmentIsDirty = true;

    // Note: Alignment only moves our text within our own extent, so it can't
    //       affect any other widget. render() refreshes it for us.
    Core::markRenderDirty();
}

void Text::refreshTextExtents()
{
    // Offset our textExtent to start at our last arrange()'s startPosition.
    SDL_Rect offsetTextExtent{ textExtent };
    offsetTextExtent.x += (lastStartPosition.x + textOffset);
    offsetTextExtent.y += lastStartPosition.y;

    // Clip the text image's extent to not go beyond this widget's extent.
    SDL_IntersectRect(&offsetTextExtent, &clippedExtent,
                      &offsetClippedTextExtent);

    // Pull offsetClippedTextExtent back into texture space ((0, 0) origin).
    // This tells us what part of the text image texture to actually render.
    offsetClippedTextureExtent = offsetClippedTextExtent;
    offsetClippedTextureExtent.x -= offsetTextExtent.x;
    offsetClippedTextureExtent.y -= offsetTextExtent.y;
}

void Text::refreshFontObject()
{
    // Scale the font size to the current actual size.
    int actualFontSize{ ScalingHelpers::logicalToActual(logicalFontSize) };

    // Attempt to load the desired font (errors on failure).
    AssetCache& assetCache{ Core::getAssetCache() };
    font = assetCache.requestFont(fontPath, actualFontSize, 0);

    // If we have an outline, load the outlined font as well.
    if (logicalFontOutlineSize > 0) {
        int actualFontOutlineSize{ ScalingHelpers::logicalToActual(
            logicalFontOutlineSize) };
        outlinedFont = assetCache.requestFont(fontPath, actualFontSize,
                                              actualFontOutlineSize);
    }
}

SDL_Surface* Text::getSurface(TTF_Font* font, const SDL_Color& fontColor,
                              const SDL_Color& fontBackgroundColor)
{
    // If the text string is empty, render a space instead.
    std::string spaceText{ " " };
    std::string_view textToRender{ text };
    if (text == "") {
        textToRender = " ";
    }

    // Create a temporary surface on the cpu and render our image using the
    // set renderMode.
    SDL_Surface* surface{ nullptr };
    if (wordWrapEnabled) {
        // Note: We need to manually scale our width since it may not yet have
        //       been updated.
        int scaledWidth{ ScalingHelpers::logicalToActual(logicalExtent.w) };
        switch (renderMode) {
            case RenderMode::Solid:
                surface = TTF_RenderUTF8_Solid_Wrapped(
                    font, textToRender.data(), fontColor, scaledWidth);
                break;
            case RenderMode::Shaded:
                surface = TTF_RenderUTF8_Shaded_Wrapped(
                    font, textToRender.data(), fontColor, fontBackgroundColor,
                    scaledWidth);
                break;
            case RenderMode::Blended:
                surface = TTF_RenderUTF8_Blended_Wrapped(
                    font, textToRender.data(), fontColor, scaledWidth);
                break;
                // Note: Removed because SDL_ttf on 22.04 doesn't support it.
                // case RenderMode::LCD:
                //    surface = TTF_RenderUTF8_LCD_Wrapped(
                //        font, textToRender.data(), fontColor,
                //        fontBackgroundColor, scaledWidth);
                //    break;
        }
    }
    else {
        switch (renderMode) {
            case RenderMode::Solid:
                surface = TTF_RenderUTF8_Solid(font, textToRender.data(),
                                               fontColor);
                break;
            case RenderMode::Shaded:
                surface = TTF_RenderUTF8_Shaded(font, textToRender.data(),
                                                fontColor, fontBackgroundColor);
                break;
            case RenderMode::Blended:
                surface = TTF_RenderUTF8_Blended(font, textToRender.data(),
                                                 fontColor);
                break;
                // Note: Removed because SDL_ttf on 22.04 doesn't support it.
                // case RenderMode::LCD:
                //    surface = TTF_RenderUTF8_LCD(font,
                //        textToRender.data(), fontColor, fontBackgroundColor);
                //    break;
        }
    }
    if (surface == nullptr) {
        AUI_LOG_FATAL("Failed to create surface.");
    }

    return surface;
}

} // namespace AUI
