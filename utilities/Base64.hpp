/**============================================================================
Name        : Base64.h
Created on  : 24.03.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Base64.h
============================================================================**/

#ifndef CPPPROJECTS_BASE64_HPP
#define CPPPROJECTS_BASE64_HPP

#include <string>

namespace Base64
{
    std::string base64Encode(const std::string &str);
    std::string base64Encode2(const std::string &str);
};

#endif //CPPPROJECTS_BASE64_HPP
