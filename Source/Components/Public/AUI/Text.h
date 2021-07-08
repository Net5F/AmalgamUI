#pragma once

#include "AUI/Component.h"
#include "AUI/ResourceManager.h" // TextureHandle
#include <SDL_ttf.h>
#include <string_view>

namespace AUI {

/**
 * Displays a line of text.
 *
 * The Text component consists of two concepts: the component extent, and the
 * text extent.
 *
 * The component extent is defined by logicalExtent/scaledExtent. This extent
 * defines the area that the component ultimately takes up. You can think of it
 * as the area that the text goes in.
 *
 * The text extent reflects the size and placement of the text.
 * This is placed within the component extent, offset through the alignment
 * and textOffset parameters, and is finally clipped by the component extent
 * before rendering.
 */
class Text : public Component
{
public:
    //-------------------------------------------------------------------------
    // Public definitions
    //-------------------------------------------------------------------------
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
        Center,
        Bottom
    };

    /**
     * Horizontal text alignment. See setHorizontalAlignment().
     */
    enum class HorizontalAlignment {
        Left,
        Center,
        Right
    };

    //-------------------------------------------------------------------------
    // Public interface
    //-------------------------------------------------------------------------
    Text(Screen& screen, const char* key, const SDL_Rect& logicalExtent);

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
    void setText(std::string_view inText);

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

    /**
     * Sets the text texture's x-axis offset. Effectively, this moves the text
     * in relation to this component's scaledExtent.
     *
     * This is done after all other offsets but pre-clipping, allowing you to
     * scroll the text and have it be clipped appropriately.
     */
    void setTextOffset(int inTextOffset);

    /**
     * Inserts the given text into the given position in the underlying string.
     */
    void insertText(std::string_view inText, unsigned int index);

    /**
     * Erases the character at the given index in the underlying string.
     *
     * @return true if a character was erased, else false (empty string).
     */
    bool eraseCharacter(unsigned int index);

    /** Returns a const reference to the underlying std::string. */
    const std::string& asString();

    /**
     * Used to tell where within the component a particular character starts.
     *
     * @param index  The index of the desired character in the underlying
     *               string.
     * @return An extent containing the offset of the top left of the desired
     *         character, and the character's height. This extent is relative
     *         to scaledExtent.
     */
    SDL_Rect calcCharacterOffset(unsigned int index);

    /**
     * Calculates the width that the given string would have if rendered using
     * this component's current font.
     */
    int calcStringWidth(const std::string& string);

    VerticalAlignment getVerticalAlignment();
    HorizontalAlignment getHorizontalAlignment();
    int getTextOffset();

    //-------------------------------------------------------------------------
    // Base class overrides
    //-------------------------------------------------------------------------
    /**
     * Calls Component::setExtent(), then calls refreshAlignment().
     */
    void setLogicalExtent(const SDL_Rect& inLogicalExtent) override;

    void render(const SDL_Point& parentOffset = {}) override;

protected:
    /**
     * Overridden to properly scale text.
     */
    bool refreshScaling() override;

private:
    /**
     * Re-calculates alignedExtent based on the current verticalAlignment,
     * horizontalAlignment, texExtent, and scaledExtent.
     */
    void refreshAlignment();

    /**
     * Re-loads the font object, using the current fontPath and scaling
     * logicalFontSize to the appropriate actual font size.
     */
    void refreshFontObject();

    /**
     * Re-renders the text texture, using all current property values.
     */
    void refreshTexture();

    /** Path to the font file, relative to Core::resourcePath. */
    std::string fontPath;

    /** Logical font size in point, i.e. font size relative to Core's
        logicalScreenSize. */
    int logicalFontSize;

    /** The handle to our font object. */
    FontHandle fontHandle;

    /** The color of our text. */
    SDL_Color color;

    /** The color of the background. Only used when renderMode == Shaded. */
    SDL_Color backgroundColor;

    /** The render mode. Affects the quality of the rendered image. */
    RenderMode renderMode;

    /** The text that this component will display. */
    std::string text;

    /** Our current vertical alignment. See setVerticalAlignment(). */
    VerticalAlignment verticalAlignment;

    /** Our current horizontal alignment. See setHorizontalAlignment(). */
    HorizontalAlignment horizontalAlignment;

    /** If true, a property has been changed and the font texture must be
        re-rendered. */
    bool textureIsDirty;

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

    /** The source extent of the image within the text texture.
        Since we use the whole texture, this is effectively the size of the
        texture. */
    SDL_Rect textureExtent;

    /** Our textureExtent, aligned to our scaledExtent according to our
        vertical/horizontal alignment setting. This is the extent in actual
        space that the text texture should be rendered at. */
    SDL_Rect textExtent;

    /** An actual-space x-axis offset applied to the text's position before
        clipping. Effectively moves the text in relation to our scaledExtent.
        Used to scroll the text and have it be clipped appropriately. */
    int textOffset;
};

} // namespace AUI
