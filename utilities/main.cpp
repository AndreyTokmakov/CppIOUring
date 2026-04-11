/**============================================================================
Name        : Utilities.cpp
Created on  : 29.02.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : C++ Utilities
============================================================================**/

#include <iostream>
#include <string_view>
#include <vector>
#include <array>
#include <list>
#include <map>
#include <iomanip>
#include <thread>
#include <fstream>
#include <charconv>
#include <optional>

#include <x86intrin.h>
#include <cstring>

#include "StringUtilities.hpp"
#include "FileUtilities.hpp"
#include "Base64.hpp"
#include "HexConverter.hpp"
#include "PerfUtilities.hpp"
#include "Testing.hpp"
#include "Random.hpp"


#include "FinalAction.hpp"

#include <experimental/scope>

namespace
{
    template<typename T>
    std::ostream& operator<<(std::ostream & stream,
                             const std::vector<T>& collection)
    {
        for (const T& v: collection)
            stream << v << ' ';
        return stream;
    }

    template<typename T>
    std::ostream& operator<<(std::ostream & stream,
                             const std::list<T>& collection)
    {
        for (const T& v: collection)
            stream << v << ' ';
        return stream;
    }

    template<typename T, size_t _Size>
    std::ostream& operator<<(std::ostream & stream,
                             const std::array<T, _Size>& collection)
    {
        for (const T& v: collection)
            stream << v << ' ';
        return stream;
    }
}


namespace StringUtilities
{
    void slice_string(std::string &str, size_t from, size_t until)
    {
        if (!(str.length() > until && until > from))
            return;

        size_t pos = 0;
        for (size_t idx = from; idx <= until; ++idx)
            str[pos++] = str[idx];
        str.resize(pos);
        str.shrink_to_fit();
    }
}

namespace StringUtilitiesTests
{
    using namespace StringUtilities;

    void split_test_1()
    {

        const std::string text { "11_22_33_44" };

        {
            const std::vector<std::string> parts = split(text, "_");
            std::cout << parts << std::endl;
        }

        {
            const std::vector<std::string> parts = split(text, 10,"_");
            std::cout << parts << std::endl;
        }

        {
            std::vector<std::string_view> parts;
            split_to(text, parts,"_");
            std::cout << parts << std::endl;
        }
    }

    void trim_string_test()
    {
        for (const std::string& base: std::vector<std::string>{
                "   Some   Sample    String  "
        })
        {   std::cout << "Input: " << std::quoted(base) << std::endl;

            if (std::string str(base); not str.empty())
            {
                StringUtilities::trim_1(str);
                std::cout << std::quoted(str) << std::endl;
            }
            if (std::string str(base); not str.empty())
            {
                StringUtilities::trim_2(str);
                std::cout << std::quoted(str) << std::endl;
            }
            if (std::string str(base); not str.empty())
            {
                StringUtilities::trim_3(str);
                std::cout << std::quoted(str) << std::endl;
            }
        }
    }
    void strip_string_test()
    {
        std::string str1 { "\t\t  A good   examplE    \n\t\n" };

        std::cout << std::quoted(str1) << std::endl;
        strip(str1);
        std::cout << std::quoted(str1) << std::endl;
    }

    void remove_chars_from_string_test()
    {
        std::string str1 { "\t\t  A good   examplE    \n\t\n" };

        std::cout << std::quoted(str1) << std::endl;
        remove_chars_from_string(str1);
        std::cout << std::quoted(str1) << std::endl;
    }

    void Update_string_test()
    {
        std::string str { "0123456789___________________" };
        std::cout << std::quoted(str) << "  " << str.capacity() << std::endl;

        slice_string(str, 3, 8);

        std::cout << std::quoted(str) << "  " << str.capacity() << std::endl;
    }

    void Random_String()
    {
        for (int i = 10; i < 20; ++i)
            std::cout << randomString(i) << std::endl;
    }
}

namespace FileUtilities_Tests
{
    const std::string testFilePath { R"(/home/andtokm/DiskS/Temp/Folder_For_Testing/test_file.txt)" };

    void ReadFile()
    {
        std::string text = FileUtilities::ReadFile(testFilePath);
        std::cout << text << std::endl;
    }

    void ReadFile2String()
    {
        std::string text;
        FileUtilities::ReadFile2String(testFilePath, text);
        std::cout << text << std::endl;
    }

    void FileSize()
    {
        std::cout << FileUtilities::getFileSize(testFilePath) << std::endl;
        std::cout << FileUtilities::getFileSizeFS(testFilePath) << std::endl;
    }

    void WriteToFile()
    {
        int32_t _ = FileUtilities::WriteToFile(testFilePath, "12345");
    }

    void AppendToFile()
    {
        int32_t bytesWriten = FileUtilities::AppendToFile(testFilePath, "12345");
        std::cout << bytesWriten << std::endl;
    }
}

namespace Base64Tests
{
    void Test()
    {
        //const std::string result = Base64::base64Encode("111111111122222222222223333333333333");
        const std::string result = Base64::base64Encode2("Man");
        std::cout << result << std::endl;

    }

}

namespace TimeMeasurement
{
    using namespace PerfUtilities;

    void doSomething()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds (150UL));
    }

    void testScopedTimer()
    {
        ScopedTimer timer { "Test1" };
        doSomething();
    }

    void testScopedTimer_TSC()
    {
        TSCScopedTimer timer { "Test2" };
        doSomething();
    }
}


namespace StringUtilitiesTests
{

    void stringToChunks_Test()
    {
        std::string str { "123456789"};
        std::vector parts = stringToChunks(str, 2);
        std::cout << parts.size() << std::endl;
        for (const auto& s: parts) {
            std::cout << s << std::endl;
        }
    }
}

namespace StringUtilitiesTests
{
    constexpr std::array<char, 256> toExclude = []() -> std::array<char, 256> {
        std::array<char, 256> tmp{};
        for (const char c: {'\t', '\n', '\r', '\n', ' '})
            tmp[c] = 1;
        return tmp;
    }();

    static_assert(toExclude.size() == 256);
    static_assert(toExclude['\t'] == 1);
    static_assert(toExclude['\n'] == 1);

    void trim(std::string &str)
    {
        uint32_t idx = 0, left = 0, right = str.size() - 1;
        for (; left <= right && 1 == toExclude[str[left]]; ++left) {}
        for (; right >= left && 1 == toExclude[str[right]]; --right) {}
        for (; left <= right; ++left, ++idx) {
            str[idx] = str[left];
        }

        str.resize(idx);
        str.shrink_to_fit();
    }

    void Trim_Test()
    {
        std::string str = "\t\n\r\n123456789  \r\n";

        std::cout << std::quoted(str) << std::endl;
        trim(str);
        std::cout << std::quoted(str) << std::endl;
    }
}


namespace HexConverter_Tests
{
    using namespace HexConverter;

    constexpr std::array<char, 16> table { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

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

    void Test()
    {
        const short val { 512 };
        char bytes[sizeof(val)];
        memcpy(bytes, &val, sizeof(val));

        const std::string hexStr = bytesToHexStr(bytes, sizeof(val));

        std::cout << hexStr << std::endl;
        std::cout << intToHex(512) << std::endl;
    }
}

namespace final_action_test
{
    void test()
    {
        {
            auto cleanup = []{ std::cout << "Cleanup-1" << std::endl; };
            final_action::ScopeExit onExit(cleanup);
            std::cout << "Done-1" << std::endl;
        }

        {
            auto cleanup = []{ std::cout << "Cleanup-2" << std::endl; };
            final_action::ScopeExit onExit(cleanup);

            onExit.release();
            std::cout << "Done-2" << std::endl;
        }
    }
}

namespace testing_utils
{
    using namespace testing;

    void test_assert_equal()
    {
        // AssertEqual(1, 2);
        AssertEqual(1, 2, "OPS");
    }

    void test_assert_true()
    {
        AssertTrue(true);
        AssertTrue(1 == 1);
        // AssertTrue(false);
        AssertTrue(false, "Should Fail");
    }

    void test_assert_false()
    {
        AssertFalse(false);
        AssertFalse(1 == 2);
        // AssertFalse(true);
        AssertFalse(true, "Should Fail");
    }

    void test_assert_null()
    {
        // int *ptr = new int(1);
        // AssertNotNull(ptr);
        // AssertNotNull(ptr, "Shall not be null");

        // AssertIsNull(ptr);
        // AssertIsNull(ptr, "Shall be null");
    }
}

// TODO: BitUtils
//      - check bit is set
//      - set bit
//      - unset bit
//      - check is Odd
//      - check is Even

int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    // StringUtilitiesTests::split_test_1();
    // StringUtilitiesTests::strip_string_test();
    // StringUtilitiesTests::trim_string_test();
    // StringUtilitiesTests::remove_chars_from_string_test();
    // StringUtilitiesTests::Update_string_test();
    // StringUtilitiesTests::Random_String();
    // StringUtilitiesTests::stringToChunks_Test();
    // StringUtilitiesTests::Trim_Test();

    // FileUtilities_Tests::ReadFile();
    // FileUtilities_Tests::ReadFile2String();
    // FileUtilities_Tests::FileSize();

    // FileUtilities_Tests::WriteToFile();
    // FileUtilities_Tests::AppendToFile();

    // Base64Tests::Test();

    // CSV_Reader_Tests::TestAll();

    // HexConverter::TestAll();

    // TimeMeasurement::testScopedTimer();
    // TimeMeasurement::testScopedTimer_TSC();

    // HexConverter_Tests::Test();

    // final_action_test::test();

    // testing_utils::test_assert_equal();
    // testing_utils::test_assert_true();
    // testing_utils::test_assert_false();
    // testing_utils::test_assert_null();
    {
        const auto v = utilities::random::getRandomInRange<int>(1.0, 20.0);
        std::cout << v << std::endl;
    }

    {
        const auto v = utilities::random::getRandomInRange<double>(1.0, 20.0);
        std::cout << v << std::endl;
    }

    return EXIT_SUCCESS;
}
