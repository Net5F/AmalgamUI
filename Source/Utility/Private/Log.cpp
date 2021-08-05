#include "AUI/Internal/Log.h"
#include <cstdio>
#include <cstdarg>

namespace AUI
{
FILE* logFilePtr = nullptr;

void Log::info(const char* expression, ...)
{
    // Get the va_list into arg.
    std::va_list arg;
    va_start(arg, expression);

    // If enabled, write to file.
    if (logFilePtr != nullptr) {
        // Copy the va_list since it's undefined to use it twice.
        std::va_list argCopy;
        va_copy(argCopy, arg);

        std::vfprintf(logFilePtr, expression, argCopy);
        std::fprintf(logFilePtr, "\n");
        std::fflush(logFilePtr);

        va_end(argCopy);
    }

    // Write to stdout.
    std::vprintf(expression, arg);
    std::printf("\n");
    std::fflush(stdout);

    va_end(arg);
}

void Log::error(const char* fileName, int line, const char* expression, ...)
{
    // Get the va_list into arg.
    std::va_list arg;
    va_start(arg, expression);

    // If enabled, write to file.
    if (logFilePtr != nullptr) {
        // Copy the va_list since it's undefined to use it twice.
        std::va_list argCopy;
        va_copy(argCopy, arg);

        std::fprintf(logFilePtr, "Error at file: %s, line: %d\n", fileName,
                     line);
        std::vfprintf(logFilePtr, expression, argCopy);
        std::fprintf(logFilePtr, "\n");
        std::fflush(logFilePtr);

        va_end(argCopy);
    }

    // Write to stdout.
    std::printf("Error at file: %s, line: %d\n", fileName, line);
    std::vprintf(expression, arg);
    std::printf("\n");
    std::fflush(stdout);

    va_end(arg);
}

void Log::enableFileLogging(const std::string& fileName)
{
    // Open the log file.
    logFilePtr = fopen(fileName.c_str(), "w");
    if (logFilePtr == nullptr) {
        std::printf("Failed to open log file for writing.\n");
    }
}

} // namespace AUI
