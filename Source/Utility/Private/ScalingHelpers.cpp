#include "AUI/ScalingHelpers.h"
#include "AUI/Core.h"
#include "AUI/SDLHelpers.h"
#include "AUI/Internal/Log.h"

namespace AUI
{
int ScalingHelpers::logicalToActual(int logicalInt)
{
    // Calculate the scaling factor, going from logical size to actual.
    // TODO: When support for changing aspect ratios gets added, we'll have to
    //       tweak this.
    double scale{static_cast<double>(Core::getActualScreenSize().width)
                 / Core::getLogicalScreenSize().width};

    return static_cast<int>(std::round(logicalInt * scale));
}

unsigned int ScalingHelpers::logicalToActual(unsigned int logicalInt)
{
    // Calculate the scaling factor, going from logical size to actual.
    // TODO: When support for changing aspect ratios gets added, we'll have to
    //       tweak this.
    double scale{static_cast<double>(Core::getActualScreenSize().width)
                 / Core::getLogicalScreenSize().width};

    return static_cast<unsigned int>(std::round(logicalInt * scale));
}

float ScalingHelpers::logicalToActual(float logicalFloat)
{
    // Calculate the scaling factor, going from logical size to actual.
    // TODO: When support for changing aspect ratios gets added, we'll have to
    //       tweak this.
    double scale{static_cast<double>(Core::getActualScreenSize().width)
                 / Core::getLogicalScreenSize().width};

    return static_cast<float>(std::round(logicalFloat * scale));
}

SDL_FRect ScalingHelpers::logicalToActual(const SDL_FRect& logicalExtent)
{
    // Calculate the scaling factor, going from logical size to actual.
    double xScale{static_cast<double>(Core::getActualScreenSize().width)
                  / Core::getLogicalScreenSize().width};
    double yScale{static_cast<double>(Core::getActualScreenSize().height)
                  / Core::getLogicalScreenSize().height};

    // Note: We'll eventually support other aspect ratios by centering the UI,
    //       but for now we just fail if you try to change the aspect ratio.
    if (xScale != yScale) {
        AUI_LOG_FATAL("We currently only support scaling within the same aspect"
                      " ratio.");
    }

    // Scale from logical to actual.
    SDL_FRect actualExtent{};
    actualExtent.x = static_cast<float>(std::round(logicalExtent.x * xScale));
    actualExtent.y = static_cast<float>(std::round(logicalExtent.y * yScale));
    actualExtent.w = static_cast<float>(std::round(logicalExtent.w * xScale));
    actualExtent.h = static_cast<float>(std::round(logicalExtent.h * yScale));

    return actualExtent;
}

SDL_FPoint ScalingHelpers::logicalToActual(const SDL_FPoint& logicalPoint)
{
    // Calculate the scaling factor, going from logical size to actual.
    double xScale{static_cast<double>(Core::getActualScreenSize().width)
                  / Core::getLogicalScreenSize().width};
    double yScale{static_cast<double>(Core::getActualScreenSize().height)
                  / Core::getLogicalScreenSize().height};

    // Note: We'll eventually support other aspect ratios by centering the UI,
    //       but for now we just fail if you try to change the aspect ratio.
    if (xScale != yScale) {
        AUI_LOG_FATAL("We currently only support scaling within the same aspect"
                      " ratio.");
    }

    // Scale from logical to actual.
    SDL_FPoint actualPoint{};
    actualPoint.x = static_cast<float>(std::round(logicalPoint.x * xScale));
    actualPoint.y = static_cast<float>(std::round(logicalPoint.y * yScale));

    return actualPoint;
}

int ScalingHelpers::actualToLogical(int actualInt)
{
    // Calculate the scaling factor, going from actual size to logical.
    // TODO: When support for changing aspect ratios gets added, we'll have to
    //       tweak this.
    double scale{static_cast<double>(Core::getLogicalScreenSize().width)
                 / Core::getActualScreenSize().width};

    return static_cast<int>(std::round(actualInt * scale));
}

unsigned int ScalingHelpers::actualToLogical(unsigned int actualInt)
{
    // Calculate the scaling factor, going from actual size to logical.
    // TODO: When support for changing aspect ratios gets added, we'll have to
    //       tweak this.
    double scale{static_cast<double>(Core::getLogicalScreenSize().width)
                 / Core::getActualScreenSize().width};

    return static_cast<unsigned int>(std::round(actualInt * scale));
}

float ScalingHelpers::actualToLogical(float actualFloat)
{
    // Calculate the scaling factor, going from actual size to logical.
    // TODO: When support for changing aspect ratios gets added, we'll have to
    //       tweak this.
    double scale{static_cast<double>(Core::getLogicalScreenSize().width)
                 / Core::getActualScreenSize().width};

    return static_cast<float>((actualFloat * scale));
}

SDL_FRect ScalingHelpers::actualToLogical(const SDL_FRect& actualExtent)
{
    // Calculate the scaling factor, going from actual size to logical.
    double xScale{static_cast<double>(Core::getLogicalScreenSize().width)
                  / Core::getActualScreenSize().width};
    double yScale{static_cast<double>(Core::getLogicalScreenSize().height)
                  / Core::getActualScreenSize().height};

    // Note: We'll eventually support other aspect ratios by centering the UI,
    //       but for now we just fail if you try to change the aspect ratio.
    if (xScale != yScale) {
        AUI_LOG_FATAL("We currently only support scaling within the same aspect"
                      " ratio.");
    }

    // Scale from actual to logical.
    SDL_FRect logicalExtent{};
    logicalExtent.x = static_cast<float>(std::round(actualExtent.x * xScale));
    logicalExtent.y = static_cast<float>(std::round(actualExtent.y * yScale));
    logicalExtent.w = static_cast<float>(std::round(actualExtent.w * xScale));
    logicalExtent.h = static_cast<float>(std::round(actualExtent.h * yScale));

    return logicalExtent;
}

SDL_FPoint ScalingHelpers::actualToLogical(const SDL_FPoint& actualPoint)
{
    // Calculate the scaling factor, going from actual size to logical.
    double xScale{static_cast<double>(Core::getLogicalScreenSize().width)
                  / Core::getActualScreenSize().width};
    double yScale{static_cast<double>(Core::getLogicalScreenSize().height)
                  / Core::getActualScreenSize().height};

    // Note: We'll eventually support other aspect ratios by centering the UI,
    //       but for now we just fail if you try to change the aspect ratio.
    if (xScale != yScale) {
        AUI_LOG_FATAL("We currently only support scaling within the same aspect"
                      " ratio.");
    }

    // Scale from actual to logical.
    SDL_FPoint logicalPoint{};
    logicalPoint.x = static_cast<float>(std::round(actualPoint.x * xScale));
    logicalPoint.y = static_cast<float>(std::round(actualPoint.y * yScale));

    return logicalPoint;
}

SDL_FRect ScalingHelpers::logicalToClipped(const SDL_FRect& logicalExtent,
                                           const SDL_FPoint& startPosition,
                                           const SDL_FRect& availableExtent)
{
    // Scale our logicalExtent to get our scaled extent, then offset it to get
    // our fullExtent.
    SDL_FRect fullExtent{ScalingHelpers::logicalToActual(logicalExtent)};
    fullExtent.x += startPosition.x;
    fullExtent.y += startPosition.y;

    // Clip fullExtent to the available space to get our clipped extent.
    SDL_FRect intersectionResult{};
    SDL_GetRectIntersectionFloat(&fullExtent, &availableExtent,
                                 &intersectionResult);
    if (SDLHelpers::hasPositiveArea(intersectionResult)) {
        return intersectionResult;
    }
    else {
        // fullExtent does not intersect availableExtent (e.g. the extent
        // is fully clipped). Return all 0s, instead of potentially returning 
        // negatives.
        return {0, 0, 0, 0};
    }
}

} // namespace AUI
