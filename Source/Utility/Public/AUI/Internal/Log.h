#pragma once

#include <SDL3/SDL_stdinc.h>
#include <atomic>
#include <cstdarg>
#include <cstdlib>
#include <string>

/**
 * Logging macros.
 * Use these macros instead of calling the Log functions directly.
 */
#define AUI_LOG_INFO(...)                                                      \
    do {                                                                       \
        AUI::Log::info(__VA_ARGS__);                                           \
    } while (false)

#ifdef NDEBUG
#define AUI_LOG_DEBUG(...)                                                     \
    do {                                                                       \
    } while (false)

#define AUI_LOG_ERROR(...)                                                     \
    do {                                                                       \
        AUI::Log::error(__FILE__, __LINE__, __VA_ARGS__);                      \
    } while (false)
#else
#define AUI_LOG_DEBUG(...)                                                     \
    do {                                                                       \
        AM::Log::info(__VA_ARGS__);                                            \
    } while (false)

#define AUI_LOG_ERROR(...)                                                     \
    do {                                                                       \
        AUI::Log::error(__FILE__, __LINE__, __VA_ARGS__);                      \
        std::abort();                                                          \
    } while (false)
#endif

#define AUI_LOG_WARN(...)                                                      \
    do {                                                                       \
        AUI::Log::warn(__VA_ARGS__);                                           \
    } while (false)

#define AUI_LOG_FATAL(...)                                                     \
    do {                                                                       \
        AUI::Log::error(__FILE__, __LINE__, __VA_ARGS__);                      \
        std::abort();                                                          \
    } while (false)

namespace AUI
{
/**
 * Facilitates logging info and errors to stdout or a log file.
 *
 * Our logging system has 5 levels:
 *   AUI_LOG_INFO: Print the given string in release and debug.
 *   AUI_LOG_DEBUG: Print the given string in debug.
 *   AUI_LOG_WARN: Print "Warning: " followed by the given string in release
 *                 and debug. Never aborts.
 *   AUI_LOG_ERROR: Print file name, line number, and the given string. In
 *                  debug this will also std::abort().
 *   AUI_LOG_FATAL: Print file name, line number, and the given string. In
 *                  debug and release this will also std::abort().
 *
 * Use AUI_LOG_INFO for general info.
 * Use AUI_LOG_DEBUG for info that you don't want to show to players.
 * Use AUI_LOG_WARN for data errors (e.g. a save that fails to load).
 * Use AUI_LOG_ERROR for code errors that are recoverable in Release (make sure
 * you write appropriate recovery logic after the LOG_ERROR).
 * Use AUI_LOG_FATAL for code errors that are unrecoverable.
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
     * Prints "Warning: " followed by the given info to stdout (and a file, if
     * enableFileLogging() was called.), then flushes the buffer.
     */
    static void warn(const char* expression, ...);

    /**
     * Prints the given info to stdout (and a file, if enableFileLogging() was
     * called.), then flushes the buffer.
     */
    static void error(const char* fileName, int line, const char* expression,
                      ...);

    /**
     * Opens a file with the given file name and enables file logging.
     */
    static void enableFileLogging(const std::string& fileName);

private:
    /**
     * Prints the given prefix, followed by the given expression, to stdout
     * (and a file, if enabled).
     */
    static void print(const char* prefix, const char* expression,
                      std::va_list arg);
};

} /* End namespace AUI */
