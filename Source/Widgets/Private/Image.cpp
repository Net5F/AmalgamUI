#include "AUI/Image.h"
#include "AUI/Core.h"
#include "AUI/AssetCache.h"
#include "AUI/ImageType/SimpleImage.h"
#include "AUI/ImageType/MultiResImage.h"
#include "AUI/ImageType/TiledImage.h"

namespace AUI
{
Image::Image(const SDL_Rect& inLogicalExtent, const std::string& inDebugName)
: Widget(inLogicalExtent, inDebugName)
, imageType{nullptr}
{
}

void Image::setSimpleImage(const std::string& imagePath)
{
    imageType = std::make_unique<SimpleImage>();
    SimpleImage* simpleImage{static_cast<SimpleImage*>(imageType.get())};
    simpleImage->set(imagePath);
}

void Image::setSimpleImage(const std::string& imagePath, SDL_Rect texExtent)
{
    imageType = std::make_unique<SimpleImage>();
    SimpleImage* simpleImage{static_cast<SimpleImage*>(imageType.get())};
    simpleImage->set(imagePath, texExtent);
}

void Image::setNineSliceImage(const std::string& imagePath,
                              NineSliceImage::SliceSizes sliceSizes)
{
    imageType = std::make_unique<NineSliceImage>();
    NineSliceImage* nineSliceImage{
        static_cast<NineSliceImage*>(imageType.get())};
    nineSliceImage->set(imagePath, sliceSizes, scaledExtent);
}

void Image::setMultiResImage(const std::vector<MultiResImageInfo>& imageInfo)
{
    imageType = std::make_unique<MultiResImage>();
    MultiResImage* multiResImage{static_cast<MultiResImage*>(imageType.get())};

    for (const MultiResImageInfo& info : imageInfo) {
        if ((info.texExtent.x == 0) && (info.texExtent.y == 0)
            && (info.texExtent.w == 0) && (info.texExtent.h == 0)) {
            multiResImage->addResolution(info.resolution, info.imagePath);
        }
        else {
            multiResImage->addResolution(info.resolution, info.imagePath,
                                         info.texExtent);
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

void Image::updateLayout(const SDL_Point& newParentOffset, const SDL_Rect& newClipExtent,
                          WidgetLocator* widgetLocator)
{
    const SDL_Rect oldScaledExtent{scaledExtent};

    // Do the normal layout updating.
    Widget::updateLayout(newParentOffset, newClipExtent, widgetLocator);

    // If this widget's size has changed.
    if (!SDL_RectEquals(&scaledExtent, &oldScaledExtent)) {
        // Refresh the image, in case it needs to regenerate to match the
        // new size.
        if (imageType != nullptr) {
            imageType->refresh(scaledExtent);
        }
    }
}

void Image::render(const SDL_Point& windowTopLeft)
{
    // If we don't have an ImageType to render, fail.
    if (imageType == nullptr) {
        AUI_LOG_FATAL("Tried to render Image with no ImageType. DebugName: %s",
                      debugName.c_str());
    }

    // If this widget is partially clipped, calculate a matching clipped 
    // extent for the texture.
    SDL_Rect clippedTexExtent{imageType->currentTexExtent};
    if (!SDL_RectEquals(&fullExtent, &clippedExtent)) {
        // Calc the size difference factor between the texture's extent and 
        // this widget's full extent.
        double widthDiffFactor{imageType->currentTexExtent.w
                               / static_cast<double>(fullExtent.w)};
        double heightDiffFactor{imageType->currentTexExtent.h
                                / static_cast<double>(fullExtent.h)};

        // Calc the size of the clipped region and scale it using the 
        // difference factor.
        clippedTexExtent.x += static_cast<int>((clippedExtent.x - fullExtent.x)
                                               * widthDiffFactor);
        clippedTexExtent.y += static_cast<int>((clippedExtent.y - fullExtent.y)
                                               * heightDiffFactor);
        clippedTexExtent.w *= static_cast<int>(widthDiffFactor);
        clippedTexExtent.h *= static_cast<int>(heightDiffFactor);
    }

    // Render the image.
    SDL_Rect finalExtent{clippedExtent};
    finalExtent.x += windowTopLeft.x;
    finalExtent.y += windowTopLeft.y;
    SDL_RenderCopy(Core::getRenderer(), imageType->currentTexture.get(),
                   &clippedTexExtent, &finalExtent);
}

} // namespace AUI
