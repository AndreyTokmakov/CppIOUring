/**============================================================================
Name        : ThreadUtilities.h
Created on  : 05.07.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : ThreadUtilities.h
============================================================================**/

#ifndef CPPPROJECTS_THREADUTILITIES_HPP
#define CPPPROJECTS_THREADUTILITIES_HPP

#include <cstdint>

namespace ThreadUtilities
{
    bool setThreadCore(const uint32_t coreId) noexcept;

    int32_t getCpu() noexcept;
}

#endif //CPPPROJECTS_THREADUTILITIES_HPP
