/**============================================================================
Name        : Random.hpp
Created on  : 02.12.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Random.hpp
============================================================================**/

#ifndef CPPPROJECTS_RANDOM_HPP
#define CPPPROJECTS_RANDOM_HPP

#include <random>
#include <stdexcept>

namespace utilities::random
{
    static inline std::random_device rd{};
    static inline std::mt19937 generator = std::mt19937 {rd()};

    template<typename Ty>
    [[nodiscard]]
    Ty getRandomInRange(const Ty start, const Ty end) noexcept
    {
        if constexpr (std::is_integral_v<Ty>) {
            return std::uniform_int_distribution<Ty> {start, end}(generator);
        }
        else if constexpr (std::is_floating_point_v<Ty>) {
            return std::uniform_real_distribution<Ty> {start, end}(generator);
        }
        else {
            throw std::invalid_argument("getRandomInRange: Invalid type");
        }
    }
}

#endif //CPPPROJECTS_RANDOM_HPP