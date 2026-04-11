/**============================================================================
Name        : Base64.cpp
Created on  : 24.03.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Base64.h
============================================================================**/

#include "Base64.hpp"
#include <iostream>
#include <array>
#include <cstdint>

namespace
{
    constexpr std::array<char, 64> ENCODING_TABLE
    {
            'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
            'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
            'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
            'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
            'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
            'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
            'w', 'x', 'y', 'z', '0', '1', '2', '3',
            '4', '5', '6', '7', '8', '9', '+', '/'
    };

    // constexpr char PADDING { '=' };
    constexpr size_t MAX_BITS_TO_ENCODE {6};
    constexpr size_t BITS_IN_BYTE {8};
}

namespace Base64
{
    std::string base64Encode(const std::string &str)
    {
        std::string result;
        result.reserve(str.size());

        int encodedByte {0};
        for (size_t bit_to_set {MAX_BITS_TO_ENCODE}; const char byte: str)
        {
            for (int i = BITS_IN_BYTE - 1; i >= 0; --i, --bit_to_set)
            {
                if (bit_to_set <= 0)
                {
                    bit_to_set = MAX_BITS_TO_ENCODE;
                    // if ((result.size() + 1 ) >= result.capacity()) std::cout  << "RESIZE!" << std::endl;
                    result.append(1, ENCODING_TABLE[encodedByte]);
                    encodedByte = 0;
                }
                if (byte & (1u << i)) {
                    encodedByte |= (1 << (bit_to_set - 1));
                }
            }
        }
        result.append(1, ENCODING_TABLE[encodedByte]);
        result.append(2 == str.size() % 3 ? 1 : 2, '=');
        return result;
    }

    std::string base64Encode2(const std::string &str)
    {
        std::cout << str << std::endl;

        for (uint32_t idx = 0, size = str.size(); idx + 3 < size; ++idx)
        {

        }


        return {};
    }
};
