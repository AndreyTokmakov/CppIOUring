/**============================================================================
Name        : ThreadUtilities.cpp
Created on  : 05.07.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : ThreadUtilities.cpp
============================================================================**/

#include "ThreadUtilities.hpp"

#include <thread>

namespace ThreadUtilities
{
    bool setThreadCore(const uint32_t coreId) noexcept
    {
        cpu_set_t cpuSet {};
        CPU_ZERO(&cpuSet);
        CPU_SET(coreId, &cpuSet);
        return 0 == pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuSet);
    }

    int32_t getCpu() noexcept
    {
        return sched_getcpu();
    }
}
