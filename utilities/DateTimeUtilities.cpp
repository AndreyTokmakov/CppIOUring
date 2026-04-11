/**============================================================================
Name        : DateTimeUtilities.cpp
Created on  : 05.07.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : DateTimeUtilities.cpp
============================================================================**/

#include "DateTimeUtilities.hpp"

namespace DateTimeUtilities
{
    using namespace std::chrono;

    constexpr std::string_view formatSeconds { "{:%Y-%m-%d %H:%M:%OS}" };
    constexpr std::string_view formatMSeconds { "%d-%02d-%02d %02d:%02d:%02d.%06ld" };

    [[nodiscard]]
    std::string timeString(const std::chrono::time_point<std::chrono::system_clock>& timestamp)
    {
        std::string buffer;
        buffer.reserve(32);
        std::format_to(std::back_inserter(buffer), formatSeconds, timestamp);
        buffer.shrink_to_fit();
        return buffer;
    }


    [[nodiscard]]
    std::string getCurrentTime(const time_point<system_clock>& timestamp)
    {
        const time_t time { std::chrono::system_clock::to_time_t(timestamp) };
        std::tm tm {};
        ::localtime_r(&time, &tm);

        std::string buffer(64, '\0');
        const int32_t size = std::sprintf(buffer.data(), formatMSeconds.data(),
                                          tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
                                          duration_cast<microseconds>(timestamp - time_point_cast<seconds>(timestamp)).count()
        );
        buffer.resize(size);
        buffer.shrink_to_fit();
        return buffer;
    }
}