/**============================================================================
Name        : StringUtilities.h
Created on  : 10.02.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : StringUtilities.h
============================================================================**/

#ifndef CPPPROJECTS_STRINGUTILITIES_HPP
#define CPPPROJECTS_STRINGUTILITIES_HPP

#include <vector>
#include <string>
#include <string_view>


namespace StringUtilities
{
    [[nodiscard]]
    std::vector<std::string> split(std::string_view input,
                                   std::string_view delimiter = " ");


    [[nodiscard]]
    std::vector<std::string> split(const std::string &str,
                                   size_t partsExpected = 10,
                                   const std::string& delimiter = std::string {";"});

    void split_to(const std::string &str,
                  std::vector<std::string_view>& parts,
                  const std::string& delimiter = std::string {";"});


    template<typename StrType = std::string_view >
        requires std::is_constructible_v<StrType, std::string::iterator, std::string::iterator>
    [[nodiscard]]
    std::vector<StrType> stringToChunks(const std::string& str, const int64_t chunkSize)
    {
        std::vector<StrType> chunks;
        int64_t prev { 0 };
        for (int64_t size = std::ssize(str), idx = 0; idx < size; ++idx) {
            if (chunkSize == idx - prev) {
                chunks.emplace_back(str.begin() + prev, str.begin() + idx);
                prev = idx;
            }
        }

        chunks.emplace_back(str.begin() + prev, str.end());
        return chunks;
    }

    void trim_1(std::string& str);
    void trim_2(std::string& str);
    void trim_3(std::string& str);

    std::string_view trim(const std::string& str);

    void strip(std::string &str);
    void strip_fast(std::string &str);

    void remove_chars_from_string(std::string& str);

    [[nodiscard("Could be expensive to call. Do not discard the result")]]
    std::string randomString(size_t size = 16);
};

#endif //CPPPROJECTS_STRINGUTILITIES_HPP
