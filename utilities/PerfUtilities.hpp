/**============================================================================
Name        : PerfUtilities.h
Created on  : 21.11.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Utilities.h
============================================================================**/

#ifndef CPPPROJECTS_UTILITIES_HPP
#define CPPPROJECTS_UTILITIES_HPP

#include <string_view>
#include <chrono>
#include <x86intrin.h>


namespace PerfUtilities
{
    struct NonCopyableMovable
    {
        NonCopyableMovable() = default;

        NonCopyableMovable(const NonCopyableMovable&) = delete;
        NonCopyableMovable(NonCopyableMovable&&) = delete;
        NonCopyableMovable& operator=(const NonCopyableMovable&) = delete;
        NonCopyableMovable& operator=(NonCopyableMovable&&) = delete;
    };

    struct TimerBase: NonCopyableMovable
    {
        const std::string_view benchmarkName;

        explicit TimerBase(std::string_view info) :
                benchmarkName { info } {
        }
    };

    struct ScopedTimer: TimerBase
    {
        const std::chrono::high_resolution_clock::time_point start {
                std::chrono::high_resolution_clock::now()
        };

        explicit ScopedTimer(std::string_view info) : TimerBase(info) {
        }

        ~ScopedTimer();
    };

    struct TSCScopedTimer: TimerBase
    {
        const uint64_t start = __rdtsc();

        explicit TSCScopedTimer(std::string_view info) : TimerBase(info) {
        }

        ~TSCScopedTimer();
    };
}

#endif //CPPPROJECTS_UTILITIES_HPP
