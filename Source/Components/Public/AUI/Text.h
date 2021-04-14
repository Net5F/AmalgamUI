#pragma once

#include "AUI/Component.h"
#include "AUI/Internal/ResourceManager.h" // TextureHandle
#include <SDL_ttf.h>

namespace AUI {

/**
 * Displays a line or block of text.
 */
class Text : public Component
{
public:
    /**
     * Text render mode, affects the quality of the rendered image.
     * See SDL_ttf documentation for more.
     */
    enum class RenderMode {
        /** Fastest, lowest quality. */
        Solid,
        /** Better quality, but has a box around it. */
        Shaded,
        /** Slowest, best quality, no box. */
        Blended
    };

    /**
     * Vertical text alignment. See setVerticalAlignment().
     */
    enum class VerticalAlignment {
        Top,
        Middle,
        Bottom
    };

    /**
     * Horizontal text alignment. See setHorizontalAlignment().
     */
    enum class HorizontalAlignment {
        Left,
        Middle,
        Right
    };

    Text(Screen& screen, const char* key, const SDL_Rect& screenExtent);

    /**
     * Sets the font and size. Uses the internal ID format "font_size".
     *
     * @param relPath  The path to the font to use, relative to
     *                 Core::resourcePath.
     * @param size  The size of the font.
     */
    void setFont(const std::string& relPath, int size);

    /**
     * Sets the font color to use.
     */
    void setColor(const SDL_Color& inColor);

    /**
     * Sets the background color. Only used when renderMode == Shaded.
     */
    void setBackgroundColor(const SDL_Color& inBackgroundColor);

    /**
     * Sets the font render mode, affecting the quality of the rendered image.
     */
    void setRenderMode(RenderMode inRenderMode);

    /**
     * Renders the given text to a texture, using the already set font, color,
     * and renderMode.
     */
    void setText(const std::string& inText);

    /**
     * Sets the vertical alignment of the text texture within this component's
     * screenExtent.
     */
    void setVerticalAlignment(VerticalAlignment inVerticalAlignment);

    /**
     * Sets the horizontal alignment of the text texture within this
     * component's screenExtent.
     */
    void setHorizontalAlignment(HorizontalAlignment inHorizontalAlignment);

    void render(int offsetX = 0, int offsetY = 0) override;

private:
    /**
     * Re-calculates alignedExtent based on the current verticalAlignment,
     * horizontalAlignment, texExtent, and screenExtent.
     */
    void refreshAlignment();

    /** The handle to our font object. */
    FontHandle fontHandle;

    /** The color of our text. */
    SDL_Color color;

    /** The color of the background. Only used when renderMode == Shaded. */
    SDL_Color backgroundColor;

    /** The render mode. Affects the quality of the rendered image. */
    RenderMode renderMode;

    /** A deleter to use with fontTexture. */
    struct TextureDeleter {
        void operator() (SDL_Texture* p) {
            SDL_DestroyTexture(p);
        }
    };

    /** The current texture; shows our text in the desired font.
        We manage the texture ourselves instead of passing it to the resource
        manager because it'll only ever be used by this component. */
    std::unique_ptr<SDL_Texture, TextureDeleter> textTexture;

    /** The extent of the image within the text texture.
        Since we use the whole texture, this is effectively the size of the
        texture. */
    SDL_Rect texExtent;

    /** Our current vertical alignment. See setVerticalAlignment(). */
    VerticalAlignment verticalAlignment;

    /** Our current horizontal alignment. See setHorizontalAlignment(). */
    HorizontalAlignment horizontalAlignment;

    /** Our texExtent, centered on our screenExtent.
        Centers the text texture since our screenExtent may be larger or
        smaller than texExtent. */
    SDL_Rect alignedExtent;
};

} // namespace AUI