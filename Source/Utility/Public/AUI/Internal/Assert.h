#pragma once

#include "Log.h"

/**
 * Assert macro. Use this in place of assert().
 */
#ifndef NDEBUG
#define AUI_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            AUI_LOG_ERROR(message); \
        } \
    } while (false)
#else
#define AUI_ASSERT(condition, message) do { } while (false)
#endif
