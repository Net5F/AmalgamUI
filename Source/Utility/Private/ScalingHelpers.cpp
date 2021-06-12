#include "AUI/ScalingHelpers.h"
#include "AUI/Core.h"
#include "AUI/Internal/Log.h"

namespace AUI
{

int ScalingHelpers::logicalToActual(int logicalInt)
{
    // Calculate the scaling factor, going from logical size to actual.
    // TODO: When support for changing aspect ratios gets added, we'll have to
    //       tweak this.
    double scale = static_cast<double>(Core::GetActualScreenSize().width) / Core::GetLogicalScreenSize().width;

    return logicalInt * scale;
}

unsigned int ScalingHelpers::logicalToActual(unsigned int logicalInt)
{
    // Calculate the scaling factor, going from logical size to actual.
    // TODO: When support for changing aspect ratios gets added, we'll have to
    //       tweak this.
    double scale = static_cast<double>(Core::GetActualScreenSize().width) / Core::GetLogicalScreenSize().width;

    return logicalInt * scale;
}

SDL_Rect ScalingHelpers::logicalToActual(const SDL_Rect& logicalExtent)
{
    // Calculate the scaling factor, going from logical size to actual.
    double xScale = static_cast<double>(Core::GetActualScreenSize().width) / Core::GetLogicalScreenSize().width;
    double yScale = static_cast<double>(Core::GetActualScreenSize().height) / Core::GetLogicalScreenSize().height;

    // Note: We'll eventually support other aspect ratios by centering the UI,
    //       but for now we just fail if you try to change the aspect ratio.
    if (xScale != yScale) {
        AUI_LOG_ERROR("We currently only support scaling within the same aspect"
                      " ratio.");
    }

    // Scale from logical to actual.
    SDL_Rect actualExtent;
    actualExtent.x = logicalExtent.x * xScale;
    actualExtent.y = logicalExtent.y * yScale;
    actualExtent.w = logicalExtent.w * xScale;
    actualExtent.h = logicalExtent.h * yScale;

    return actualExtent;
}

SDL_Point ScalingHelpers::logicalToActual(const SDL_Point& logicalPoint)
{
    // Calculate the scaling factor, going from logical size to actual.
    double xScale = static_cast<double>(Core::GetActualScreenSize().width) / Core::GetLogicalScreenSize().width;
    double yScale = static_cast<double>(Core::GetActualScreenSize().height) / Core::GetLogicalScreenSize().height;

    // Note: We'll eventually support other aspect ratios by centering the UI,
    //       but for now we just fail if you try to change the aspect ratio.
    if (xScale != yScale) {
        AUI_LOG_ERROR("We currently only support scaling within the same aspect"
                      " ratio.");
    }

    // Scale from logical to actual.
    SDL_Point actualPoint;
    actualPoint.x = logicalPoint.x * xScale;
    actualPoint.y = logicalPoint.y * yScale;

    return actualPoint;
}

Margins ScalingHelpers::logicalToActual(Margins logicalMargins)
{
    // Calculate the scaling factor, going from logical size to actual.
    double xScale = static_cast<double>(Core::GetActualScreenSize().width) / Core::GetLogicalScreenSize().width;
    double yScale = static_cast<double>(Core::GetActualScreenSize().height) / Core::GetLogicalScreenSize().height;

    // Note: We'll eventually support other aspect ratios by centering the UI,
    //       but for now we just fail if you try to change the aspect ratio.
    if (xScale != yScale) {
        AUI_LOG_ERROR("We currently only support scaling within the same aspect"
                      " ratio.");
    }

    // Scale from logical to actual.
    Margins actualMargins{};
    actualMargins.left = logicalMargins.left * xScale;
    actualMargins.top = logicalMargins.top * yScale;
    actualMargins.right = logicalMargins.right * xScale;
    actualMargins.bottom = logicalMargins.bottom * yScale;

    return actualMargins;
}

SDL_Point ScalingHelpers::actualToLogical(const SDL_Point& actualPoint)
{
    // Calculate the scaling factor, going from actual position to logical.
    double scale = static_cast<double>(Core::GetLogicalScreenSize().width) / Core::GetActualScreenSize().width;

    return {static_cast<int>(actualPoint.x * scale), static_cast<int>(actualPoint.y * scale)};
}

} // namespace AUI
