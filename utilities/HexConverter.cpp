/**============================================================================
Name        : HexConverter.cpp
Created on  : 19.06.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : HexConverter
============================================================================**/

#include "HexConverter.hpp"

#include <string_view>
#include <vector>
#include <array>
#include <cstdint>
#include <iostream>
#include <cstring>
#include <iomanip>

namespace
{
    constexpr std::array<char, 16> table { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
}

namespace HexConverter
{
    void bytesToHexStr(const char *src,
                       size_t len,
                       char *dest)
    {
        for (uint8_t ch = 0; len > 0; --len) {
            ch = *src++;
            *dest++ = table[ch >> 4];
            *dest++ = table[ch & 0x0f];
        }
    }

    std::string bytesToHexStr(const char *src,
                              size_t len)
    {
        uint8_t ch = *src;
        std::string result(len * 2, '0');
        for (size_t pos = 0; len > 0; --len) {
            ch = *src++;
            result[pos++] = table[ch >> 4];
            result[pos++] = table[ch & 0x0f];
        }
        return result;
    }

    std::string bytesToHex(const std::string& bytesStr)
    {
        std::string result(bytesStr.size() * 2, '0');
        for (size_t pos = 0; const uint8_t ch: bytesStr) {
            result[pos++] = table[ch >> 4];
            result[pos++] = table[ch & 0x0f];
        }
        return result;
    }


    static constexpr uint8_t hexCode(unsigned char symbol) noexcept
    {
        if (symbol >= '0' && symbol <= '9')
            return symbol - '0';
        if (symbol >= 'A' && symbol <= 'F')
            return symbol - 'A' + 10;
        if (symbol >= 'a' && symbol <= 'f')
            return symbol - 'a' + 10;
        return 0;
    }

    static constexpr uint8_t hex2UChar(std::string_view hexValue) noexcept
    {
        return 16 * hexCode(hexValue[0]) + hexCode(hexValue[1]);
    }

    std::vector<uint8_t> hex2Bytes(std::string_view hexString) noexcept
    {
        std::vector<uint8_t> bytes;
        bytes.reserve(hexString.length()/2);
        for (size_t length = hexString.length(), i = 0; i < length; i += 2)
            bytes.push_back(hex2UChar(hexString.substr(i, 2)));
        return bytes;
    }

    std::string hex2BytesString(const std::string& hexString) noexcept
    {
        std::string bytesStr;
        bytesStr.reserve(hexString.length()/2);
        for (size_t length = hexString.length(), i = 0; i < length; i += 2)
            bytesStr.push_back(static_cast<char>(hex2UChar(hexString.substr(i, 2))));
        return bytesStr;
    }

    std::string intToHex(int value)
    {
        std::string result;
        unsigned int num = static_cast<unsigned int>(value);

        if (num == 0) {
            result.push_back(table[0]);
            return result;
        }
        while (num > 0) {
            int remainder = num & 0xF;
            result.push_back(table[remainder]);
            num >>= 4;
        }

        std::reverse(result.begin(), result.end());
        return result;
    }
};

template<std::integral T>
void dump(T v, const char term = '\n')
{
    std::cout << std::hex << std::uppercase << std::setfill('0')
              << std::setw(sizeof(T) * 2) << v << " : ";
    for (std::size_t i{}; i != sizeof(T); ++i, v >>= 8)
        std::cout << std::setw(2) << static_cast<unsigned>(T(0xFF) & v) << ' ';
    std::cout << std::dec << term;
}


void HexConverter::TestAll()
{
#if 0
    const int val { 123456 };
    char bytes[sizeof(val)];

    /** To bytes **/
    memcpy(bytes, &val, sizeof(val));

    /** From bytes --> HEX string **/
    const std::string hexStr = bytesToHexStr(bytes, sizeof(val));

    /** HEX string --> To bytes **/
    const std::vector<uint8_t> bytes2 = hex2Bytes(hexStr);

    /** From bytes --> to original data **/
    int result = 0;
    memcpy(&result, bytes2.data(), sizeof(val));

    std::cout << val << " --> " << hexStr << " --> " << result << std::endl;
#endif


#if 0
    {
        const std::string data {"some_test_data"};
        const std::string hexStr = bytesToHexStr(data.data(), data.size());
        std::cout << hexStr << std::endl;
    }


    {
        const std::string data {"some_test_data"};
        const std::string hexStr = bytesToHex(data);
        std::cout << hexStr << std::endl;
    }

    {
        const std::string data {"736F6D655F746573745F64617461"};
        const auto hexStr = hex2BytesString(data);
        std::cout << hexStr << std::endl;
    }
#endif


    std::cout << "byteswap for U16:\n";
    constexpr auto x = std::uint16_t(0xCAFE);
    dump(x);
    dump(std::byteswap(x));
}
