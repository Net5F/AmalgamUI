#include "AUI/Internal/ScalingHelpers.h"
#include "AUI/Core.h"
#include "AUI/Internal/Log.h"

namespace AUI
{

SDL_Rect ScalingHelpers::extentToActual(const SDL_Rect& logicalExtent)
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

SDL_Point ScalingHelpers::pointToLogical(const SDL_Point& actualPoint)
{
    // Calculate the scaling factor, going from actual position to logical.
    double scale = static_cast<double>(Core::GetLogicalScreenSize().width) / Core::GetActualScreenSize().width;

    return {static_cast<int>(actualPoint.x * scale), static_cast<int>(actualPoint.y * scale)};
}

int ScalingHelpers::fontSizeToActual(int logicalFontSize)
{
    // Calculate the scaling factor, going from logical size to actual.
    // TODO: When support for changing aspect ratios gets added, we'll have to
    //       tweak this.
    double scale = static_cast<double>(Core::GetActualScreenSize().width) / Core::GetLogicalScreenSize().width;

    return logicalFontSize * scale;
}

} // namespace AUI
