/**============================================================================
Name        : main.cpp
Created on  : 01.05.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include <iostream>
#include <string_view>
#include <vector>

#include "Networking.hpp"

int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    // Networking::HttpServer::TestAll();
    Networking::DebugHttpServer::TestAll();

    return EXIT_SUCCESS;
}
