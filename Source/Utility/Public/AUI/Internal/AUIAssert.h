#pragma once

#include "Log.h"

/**
 * Assert macro. Use this in place of assert().
 */
#ifdef NDEBUG
#define AUI_ASSERT(condition, ...)                                             \
    do {                                                                       \
    } while (false)
#else
#define AUI_ASSERT(condition, ...)                                             \
    do {                                                                       \
        if (!(condition)) {                                                    \
            AUI_LOG_ERROR(__VA_ARGS__);                                        \
        }                                                                      \
    } while (false)
#endif
