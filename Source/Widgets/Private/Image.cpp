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
, lastUsedRenderExtent{}
{
}

void Image::setSimpleImage(const std::string& imagePath)
{
    imageType = std::make_unique<SimpleImage>();
    SimpleImage* simpleImage{
        static_cast<SimpleImage*>(imageType.get())};
    simpleImage->set(imagePath);
}

void Image::setSimpleImage(const std::string& imagePath, SDL_Rect texExtent)
{
    imageType = std::make_unique<SimpleImage>();
    SimpleImage* simpleImage{
        static_cast<SimpleImage*>(imageType.get())};
    simpleImage->set(imagePath, texExtent);
}

void Image::setNineSliceImage(const std::string& imagePath,
                              NineSliceImage::SliceSizes sliceSizes)
{
    imageType = std::make_unique<NineSliceImage>();
    NineSliceImage* nineSliceImage{
        static_cast<NineSliceImage*>(imageType.get())};
    nineSliceImage->set(imagePath, sliceSizes, renderExtent);
}

void Image::setMultiResImage(const std::vector<MultiResImageInfo>& imageInfo)
{
    imageType = std::make_unique<MultiResImage>();
    MultiResImage* multiResImage{
        static_cast<MultiResImage*>(imageType.get())};

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
    TiledImage* tiledImage{
        static_cast<TiledImage*>(imageType.get())};
    tiledImage->set(imagePath, renderExtent);
}

void Image::setCustomImage(std::unique_ptr<ImageType> inImageType) {
    imageType = std::move(inImageType);
}

void Image::updateLayout(const SDL_Rect& parentExtent,
                         WidgetLocator* widgetLocator)
{
    // Do the normal layout updating.
    Widget::updateLayout(parentExtent, widgetLocator);

    // If this widget's renderExtent has changed.
    if (!SDL_RectEquals(&renderExtent, &lastUsedRenderExtent)) {
        // Refresh the image, in case it needs to regenerate to match the 
        // new renderExtent.
        if (imageType != nullptr) {
            imageType->refresh(renderExtent);
        }

        lastUsedRenderExtent = renderExtent;
    }
}

void Image::render()
{
    // If we don't have an ImageType to render, fail.
    if (imageType == nullptr) {
        AUI_LOG_FATAL("Tried to render Image with no ImageType. DebugName: %s",
                      debugName.c_str());
    }

    // Render the image.
    SDL_RenderCopy(Core::getRenderer(), imageType->currentTexture.get(),
                   &(imageType->currentTexExtent),
                   &renderExtent);
}

} // namespace AUI
