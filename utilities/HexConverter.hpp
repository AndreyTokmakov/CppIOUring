/**============================================================================
Name        : HexConverter.h
Created on  : 19.06.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : HexConverter
============================================================================**/

#ifndef HEXCONVERTER_H
#define HEXCONVERTER_H

#include <string>
#include <cstdint>
#include <vector>

namespace HexConverter
{
    std::string bytesToHexStr(const char *src,
                              size_t len);

    std::string bytesToHex(const std::string& bytesStr);

    [[nodiscard]]
    std::string intToHex(int value);

    std::vector<uint8_t> hex2Bytes(std::string_view hexString) noexcept;

    void TestAll();
};

#endif //HEXCONVERTER_H
