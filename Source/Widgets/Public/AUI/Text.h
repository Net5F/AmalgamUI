#pragma once

#include "AUI/Widget.h"
#include "AUI/AssetCache.h" // FontHandle
#include <SDL_ttf.h>
#include <string_view>
#include <string>

namespace AUI
{
/**
 * Displays a line of text.
 *
 * The Text widget consists of two concepts: the widget extent, and the
 * text extent.
 *
 * The widget extent is defined by logicalExtent/scaledExtent. This extent
 * defines the area that the widget ultimately takes up. You can think of it
 * as the area that the text goes in.
 *
 * The text extent reflects the size and placement of the text.
 * This is placed within the widget extent, offset through the alignment
 * and textOffset parameters, and is finally clipped by the widget extent
 * before rendering.

 * Note: Font assets are managed in an internal cache.
 */
class Text : public Widget
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
        /** Slower, high quality, no box. */
        Blended,
        // Note: Removed because SDL_ttf on 22.04 doesn't support it.
        ///** Slowest, LCD subpixel quality, but has a box around it.
        //    Useful for small font sizes. */
        // LCD
    };

    /**
     * Vertical text alignment. See setVerticalAlignment().
     */
    enum class VerticalAlignment { Top, Center, Bottom };

    /**
     * Horizontal text alignment. See setHorizontalAlignment().
     */
    enum class HorizontalAlignment { Left, Center, Right };

    //-------------------------------------------------------------------------
    // Public interface
    //-------------------------------------------------------------------------
    Text(const SDL_Rect& inLogicalExtent,
         const std::string& inDebugName = "Text");

    /**
     * Sets the font and size. Uses the internal ID format "font_size".
     *
     * @param fontPath  The full path to the font file.
     * @param size  The size of the font.
     */
    void setFont(std::string_view fontPath, int size);

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
     * Sets the text that this widget will display.
     */
    void setText(std::string_view inText);

    /**
     * Sets the vertical alignment of the text texture within this widget's
     * screenExtent.
     */
    void setVerticalAlignment(VerticalAlignment inVerticalAlignment);

    /**
     * Sets the horizontal alignment of the text texture within this
     * widget's screenExtent.
     */
    void setHorizontalAlignment(HorizontalAlignment inHorizontalAlignment);

    /**
     * If true, text that is longer than this widget's extent will be wrapped 
     * at word boundaries.
     */
    void setWordWrapEnabled(bool inWordWrapEnabled);

    /**
     * If true, this widget's height will automatically grow or shrink to fit 
     * its text.
     * Note: If you need the height adjusted immediately (such as for content 
     *       size calculations), see refreshTexture().
     */
    void setAutoHeightEnabled(bool inAutoHeightEnabled);

    /**
     * Sets the text texture's x-axis offset. Effectively, this moves the text
     * in relation to this widget's scaledExtent.
     *
     * This is done after all other offsets but pre-clipping, allowing you to
     * scroll the text and have it be clipped appropriately.
     */
    void setTextOffset(int inTextOffset);

    /**
     * Inserts the given text into the given position in the underlying string.
     */
    void insertText(std::string_view inText, std::size_t index);

    /**
     * Erases the character at the given index in the underlying string.
     *
     * @return true if a character was erased, else false (empty string).
     */
    bool eraseCharacter(std::size_t index);

    /**
     * Re-renders the text texture, using all current property values.
     *
     * Note: If you're relying on auto height and need this widget's height 
     *       to be adjusted immediately, you can do so by calling this.
     *       Otherwise, don't worry about it. It'll be handled automatically.
     */
    void refreshTexture();

    /** Returns a const reference to the underlying std::string. */
    const std::string& asString();

    /**
     * Used to tell where within the widget a particular character starts.
     *
     * @param index  The index of the desired character in the underlying
     *               string.
     * @return An extent containing the offset of the top left of the desired
     *         character, and the character's height. This extent is relative
     *         to scaledExtent.
     */
    SDL_Rect calcCharacterOffset(std::size_t index);

    /**
     * Calculates the width that the given string would have if rendered using
     * this widget's current font.
     */
    int calcStringWidth(const std::string& string);

    VerticalAlignment getVerticalAlignment();
    HorizontalAlignment getHorizontalAlignment();
    int getTextOffset();

    //-------------------------------------------------------------------------
    // Base class overrides
    //-------------------------------------------------------------------------
    /**
     * Calls Widget::setLogicalExtent(), then calls refreshAlignment().
     */
    void setLogicalExtent(const SDL_Rect& inLogicalExtent) override;

    /**
     * Calls Widget::updateLayout() and updates our special extents.
     */
    void updateLayout(const SDL_Point& startPosition,
                      const SDL_Rect& availableExtent,
                      WidgetLocator* widgetLocator) override;

    void render(const SDL_Point& windowTopLeft) override;

private:
    /**
     * Refreshes our alignment, font object, and text texture to match the
     * current UI scaling.
     */
    void refreshScaling();

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

    /** Full path to the font file. */
    std::string fontPath;

    /** Logical font size in point, i.e. font size relative to Core's
        logicalScreenSize. */
    int logicalFontSize;

    /** The handle to our font object. */
    std::shared_ptr<TTF_Font> font;

    /** The color of our text. */
    SDL_Color color;

    /** The color of the background. Only used when renderMode == Shaded. */
    SDL_Color backgroundColor;

    /** The render mode. Affects the quality of the rendered image. */
    RenderMode renderMode;

    /** If true, text that is longer than this widget's extent will be wrapped 
        at word boundaries. */
    bool wordWrapEnabled;

    /** If true, this widget's height will automatically grow or shrink to fit 
        its text. */
    bool autoHeightEnabled;

    /** The text that this widget will display. */
    std::string text;

    /** Our current vertical alignment. See setVerticalAlignment(). */
    VerticalAlignment verticalAlignment;

    /** Our current horizontal alignment. See setHorizontalAlignment(). */
    HorizontalAlignment horizontalAlignment;

    /** The value of Core::actualScreenSize that was used the last time this
        widget updated its layout. Used to detect when the UI scale changes,
        so we can re-render the text object. */
    ScreenResolution lastUsedScreenSize;

    /** If true, a property has been changed and the font texture must be
        re-rendered. */
    bool textureIsDirty;

    /** A deleter to use with fontTexture. */
    struct TextureDeleter {
        void operator()(SDL_Texture* p) { SDL_DestroyTexture(p); }
    };

    /** The current texture. Shows our text in the desired font.
        We manage the texture ourselves instead of passing it to the resource
        manager because it'll only ever be used by this widget. */
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

    /** Our textExtent, offset to match the parentExtent given during
        updateLayout() and clipped to renderExtent's bounds.
        Calc'd during updateLayout() and only valid for that frame. */
    SDL_Rect offsetClippedTextExtent;

    /** Our offsetClippedTextExtent, pulled back into texture space
        ((0, 0) origin). Tells us what part of the texture to render.
        Calc'd during updateLayout() and only valid for that frame. */
    SDL_Rect offsetClippedTextureExtent;
};

} // namespace AUI
