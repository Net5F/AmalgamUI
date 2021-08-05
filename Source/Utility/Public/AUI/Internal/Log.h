#pragma once

#include <SDL2/SDL_stdinc.h>
#include <atomic>
#include <cstdlib>
#include <string>

/**
 * Use these macros instead of calling the functions directly.
 */
#define AUI_LOG_INFO(...)                                                      \
    {                                                                          \
        AUI::Log::info(__VA_ARGS__);                                           \
    }

#define AUI_LOG_ERROR(...)                                                     \
    {                                                                          \
        AUI::Log::error(__FILE__, __LINE__, __VA_ARGS__);                      \
        std::abort();                                                          \
    }

namespace AUI
{
/**
 * Facilitates logging info and errors to stdout or a log file.
 */
class Log
{
public:
    /**
     * Prints the given info to stdout (and a file, if enableFileLogging() was
     * called.), then flushes the buffer.
     */
    static void info(const char* expression, ...);

    /**
     * Prints the given info to stdout (and a file, if enableFileLogging() was
     * called.), then flushes the buffer and calls abort().
     */
    static void error(const char* fileName, int line, const char* expression,
                      ...);

    /**
     * Opens a file with the given file name and enables file logging.
     */
    static void enableFileLogging(const std::string& fileName);
};

} /* End namespace AUI */
