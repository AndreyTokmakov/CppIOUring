/**============================================================================
Name        : DateTimeUtilities.h
Created on  : 05.07.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : DateTimeUtilities.h
============================================================================**/

#ifndef CPPPROJECTS_DATETIMEUTILITIES_HPP
#define CPPPROJECTS_DATETIMEUTILITIES_HPP

#include <chrono>

namespace DateTimeUtilities
{
    [[nodiscard]]
    std::string timeString(const std::chrono::time_point<std::chrono::system_clock>& timestamp
            = std::chrono::system_clock::now());

    [[nodiscard]]
    std::string getCurrentTime(const std::chrono::time_point<std::chrono::system_clock>& timestamp
            = std::chrono::system_clock::now());
}

#endif //CPPPROJECTS_DATETIMEUTILITIES_HPP
