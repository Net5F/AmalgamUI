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

    return std::round(logicalInt * scale);
}

unsigned int ScalingHelpers::logicalToActual(unsigned int logicalInt)
{
    // Calculate the scaling factor, going from logical size to actual.
    // TODO: When support for changing aspect ratios gets added, we'll have to
    //       tweak this.
    double scale = static_cast<double>(Core::GetActualScreenSize().width) / Core::GetLogicalScreenSize().width;

    return std::round(logicalInt * scale);
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
    actualExtent.x = std::round(logicalExtent.x * xScale);
    actualExtent.y = std::round(logicalExtent.y * yScale);
    actualExtent.w = std::round(logicalExtent.w * xScale);
    actualExtent.h = std::round(logicalExtent.h * yScale);

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
    actualPoint.x = std::round(logicalPoint.x * xScale);
    actualPoint.y = std::round(logicalPoint.y * yScale);

    return actualPoint;
}

int ScalingHelpers::actualToLogical(int actualInt)
{
    // Calculate the scaling factor, going from actual size to logical.
    // TODO: When support for changing aspect ratios gets added, we'll have to
    //       tweak this.
    double scale = static_cast<double>(Core::GetLogicalScreenSize().width) / Core::GetActualScreenSize().width;

    return std::round(actualInt * scale);
}

unsigned int ScalingHelpers::actualToLogical(unsigned int actualInt)
{
    // Calculate the scaling factor, going from actual size to logical.
    // TODO: When support for changing aspect ratios gets added, we'll have to
    //       tweak this.
    double scale = static_cast<double>(Core::GetLogicalScreenSize().width) / Core::GetActualScreenSize().width;

    return std::round(actualInt * scale);
}

SDL_Rect ScalingHelpers::actualToLogical(const SDL_Rect& actualExtent)
{
    // Calculate the scaling factor, going from actual size to logical.
    double xScale = static_cast<double>(Core::GetLogicalScreenSize().width) / Core::GetActualScreenSize().width;
    double yScale = static_cast<double>(Core::GetLogicalScreenSize().height) / Core::GetActualScreenSize().height;

    // Note: We'll eventually support other aspect ratios by centering the UI,
    //       but for now we just fail if you try to change the aspect ratio.
    if (xScale != yScale) {
        AUI_LOG_ERROR("We currently only support scaling within the same aspect"
                      " ratio.");
    }

    // Scale from actual to logical.
    SDL_Rect logicalExtent;
    logicalExtent.x = std::round(actualExtent.x * xScale);
    logicalExtent.y = std::round(actualExtent.y * yScale);
    logicalExtent.w = std::round(actualExtent.w * xScale);
    logicalExtent.h = std::round(actualExtent.h * yScale);

    return logicalExtent;
}

SDL_Point ScalingHelpers::actualToLogical(const SDL_Point& actualPoint)
{
    // Calculate the scaling factor, going from actual size to logical.
    double xScale = static_cast<double>(Core::GetLogicalScreenSize().width) / Core::GetActualScreenSize().width;
    double yScale = static_cast<double>(Core::GetLogicalScreenSize().height) / Core::GetActualScreenSize().height;

    // Note: We'll eventually support other aspect ratios by centering the UI,
    //       but for now we just fail if you try to change the aspect ratio.
    if (xScale != yScale) {
        AUI_LOG_ERROR("We currently only support scaling within the same aspect"
                      " ratio.");
    }

    // Scale from actual to logical.
    SDL_Point logicalPoint;
    logicalPoint.x = std::round(actualPoint.x * xScale);
    logicalPoint.y = std::round(actualPoint.y * yScale);

    return logicalPoint;
}

} // namespace AUI
