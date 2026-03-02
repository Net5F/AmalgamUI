#include "AUI/Image.h"
#include "AUI/Core.h"
#include "AUI/AssetCache.h"
#include "AUI/ImageType/SimpleImage.h"
#include "AUI/ImageType/MultiResImage.h"
#include "AUI/ImageType/TiledImage.h"
#include "AUI/Internal/AUIAssert.h"

namespace AUI
{
Image::Image(const SDL_FRect& inLogicalExtent, const std::string& inDebugName)
: Widget(inLogicalExtent, inDebugName)
, imageType{nullptr}
, lastScaledExtent{scaledExtent}
, alphaMod{1.0}
{
}

void Image::setSimpleImage(const std::string& textureID,
                           SDL_ScaleMode scaleMode)
{
    imageType = std::make_unique<SimpleImage>();
    SimpleImage* simpleImage{static_cast<SimpleImage*>(imageType.get())};
    simpleImage->set(textureID, scaleMode);
}

void Image::setSimpleImage(const std::string& textureID, SDL_FRect texExtent,
                           SDL_ScaleMode scaleMode)
{
    imageType = std::make_unique<SimpleImage>();
    SimpleImage* simpleImage{static_cast<SimpleImage*>(imageType.get())};
    simpleImage->set(textureID, texExtent, scaleMode);
}

void Image::setNineSliceImage(const std::string& textureID,
                              NineSliceImage::SliceSizes sliceSizes)
{
    imageType = std::make_unique<NineSliceImage>();
    NineSliceImage* nineSliceImage{
        static_cast<NineSliceImage*>(imageType.get())};
    nineSliceImage->set(textureID, sliceSizes, scaledExtent);
}

void Image::setMultiResImage(
    const std::vector<MultiResImagePathInfo>& imageInfo)
{
    imageType = std::make_unique<MultiResImage>();
    MultiResImage* multiResImage{static_cast<MultiResImage*>(imageType.get())};

    for (const MultiResImagePathInfo& info : imageInfo) {
        if ((info.texExtent.x == 0) && (info.texExtent.y == 0)
            && (info.texExtent.w == 0) && (info.texExtent.h == 0)) {
            multiResImage->addResolution(info.resolution, info.textureID,
                                         info.scaleMode);
        }
        else {
            multiResImage->addResolution(info.resolution, info.textureID,
                                         info.texExtent, info.scaleMode);
        }
    }
}

void Image::setTiledImage(const std::string& imagePath)
{
    imageType = std::make_unique<TiledImage>();
    TiledImage* tiledImage{static_cast<TiledImage*>(imageType.get())};
    tiledImage->set(imagePath, scaledExtent);
}

void Image::setCustomImage(std::unique_ptr<ImageType> inImageType)
{
    imageType = std::move(inImageType);
}

void Image::setSimpleImage(SDL_Texture* texture, const std::string& textureID,
                           SDL_ScaleMode scaleMode)
{
    Core::getAssetCache().addTexture(texture, textureID);

    imageType = std::make_unique<SimpleImage>();
    SimpleImage* simpleImage{static_cast<SimpleImage*>(imageType.get())};
    simpleImage->set(textureID, scaleMode);
}

void Image::setSimpleImage(SDL_Texture* texture, const std::string& textureID,
                           SDL_FRect texExtent, SDL_ScaleMode scaleMode)
{
    Core::getAssetCache().addTexture(texture, textureID);

    imageType = std::make_unique<SimpleImage>();
    SimpleImage* simpleImage{static_cast<SimpleImage*>(imageType.get())};
    simpleImage->set(textureID, texExtent, scaleMode);
}

void Image::setNineSliceImage(SDL_Texture* texture,
                              const std::string& textureID,
                              NineSliceImage::SliceSizes inSliceSizes)
{
    Core::getAssetCache().addTexture(texture, textureID);

    imageType = std::make_unique<NineSliceImage>();
    NineSliceImage* nineSliceImage{
        static_cast<NineSliceImage*>(imageType.get())};
    nineSliceImage->set(textureID, inSliceSizes, scaledExtent);
}

void Image::setMultiResImage(
    const std::vector<MultiResImageTextureInfo>& imageInfo)
{
    imageType = std::make_unique<MultiResImage>();
    MultiResImage* multiResImage{static_cast<MultiResImage*>(imageType.get())};

    for (const MultiResImageTextureInfo& info : imageInfo) {
        Core::getAssetCache().addTexture(info.texture, info.textureID);
        if ((info.texExtent.x == 0) && (info.texExtent.y == 0)
            && (info.texExtent.w == 0) && (info.texExtent.h == 0)) {
            multiResImage->addResolution(info.resolution, info.textureID,
                                         info.scaleMode);
        }
        else {
            multiResImage->addResolution(info.resolution, info.textureID,
                                         info.texExtent, info.scaleMode);
        }
    }
}

void Image::setTiledImage(SDL_Texture* texture, const std::string& textureID)
{
    Core::getAssetCache().addTexture(texture, textureID);

    imageType = std::make_unique<TiledImage>();
    TiledImage* tiledImage{static_cast<TiledImage*>(imageType.get())};
    tiledImage->set(textureID, scaledExtent);
}

void Image::setAlphaMod(float newAlphaMod)
{
    alphaMod = newAlphaMod;
}

SDL_FRect Image::getCurrentImageTextureExtent() const
{
    AUI_ASSERT(imageType, "Tried to get extent while image did not exist.");
    return imageType->currentTexExtent;
}

void Image::measure(const SDL_FRect& availableExtent)
{
    // Run the normal measure step (sets our scaledExtent).
    Widget::measure(availableExtent);

    // If this widget's size has changed, refresh the image.
    if (!SDL_RectsEqualFloat(&scaledExtent, &lastScaledExtent)) {
        if (imageType != nullptr) {
            // We do this in case it needs to regenerate to match the new size.
            imageType->refresh(scaledExtent);
        }

        lastScaledExtent = scaledExtent;
    }
}

void Image::arrange(const SDL_FPoint& startPosition,
                    const SDL_FRect& availableExtent,
                    WidgetLocator* widgetLocator)
{
    // Run the normal arrange step.
    Widget::arrange(startPosition, availableExtent, widgetLocator);

    // If this widget is fully clipped, return early.
    if (SDL_RectEmptyFloat(&clippedExtent)) {
        return;
    }
}

void Image::render(const SDL_FPoint& windowTopLeft)
{
    // If this widget is fully clipped, don't render it.
    if (SDL_RectEmptyFloat(&clippedExtent)) {
        return;
    }

    // If we don't have an ImageType to render, return early.
    if (imageType == nullptr) {
        return;
    }

    // If this widget is partially clipped, calculate a matching clipped
    // extent for the texture.
    SDL_FRect clippedTexExtent{imageType->currentTexExtent};
    if (!SDL_RectsEqualFloat(&fullExtent, &clippedExtent)) {
        // Calc the size difference factor between the texture's extent and
        // this widget's full extent.
        float widthDiffFactor{imageType->currentTexExtent.w / fullExtent.w};
        float heightDiffFactor{imageType->currentTexExtent.h / fullExtent.h};

        // Use the difference factor to calc the clipped texture extent.
        // The idea here is that clippedTexExtent/clippedExtent should have
        // the same scale relationship as currentTexExtent/fullExtent.
        // Note: We need to subtract fullExtent's origin to make clippedExtent
        //       relative to (0, 0) like currentTexExtent is.
        clippedTexExtent.x
            = (clippedExtent.x - fullExtent.x) * widthDiffFactor;
        clippedTexExtent.y
            = (clippedExtent.y - fullExtent.y) * heightDiffFactor;
        clippedTexExtent.w = clippedExtent.w * widthDiffFactor;
        clippedTexExtent.h = clippedExtent.h * heightDiffFactor;
    }

    // Apply the current alpha mod.
    SDL_SetTextureAlphaModFloat(imageType->currentTexture.get(), alphaMod);

    // Render the image.
    SDL_FRect finalExtent{clippedExtent};
    finalExtent.x += windowTopLeft.x;
    finalExtent.y += windowTopLeft.y;
    SDL_RenderTexture(Core::getRenderer(), imageType->currentTexture.get(),
                   &clippedTexExtent, &finalExtent);
}

} // namespace AUI
