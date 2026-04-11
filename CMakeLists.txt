cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 26)

project(CppIOUring)
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
# add_compile_options(-c -Wall -Wextra -O3 -std=c++26)
# add_compile_options(-c -Wall -O3 -std=c++26)
add_compile_options(-c -Wall -O3 -freflection -std=c++26)

# 5.15 - My home Ubuntu-20.04
if (CMAKE_SYSTEM_VERSION MATCHES "5.15")
    set(THIRD_PARTY_DIR /home/andtokm/DiskS/ProjectsUbuntu/third_party)
    set(EXTRA_LIBS tbb)
else()
    set(THIRD_PARTY_DIR /home/andtokm/Projects/third_party)
endif()

add_subdirectory(experiments)
add_subdirectory(utilities)
