#include "StringUtils.hpp"

#include "WindowsPlatform.hpp"

#include "ErrorHandling.hpp"

#include <cassert>
#include <limits>
#include <stdexcept>

namespace DXSandbox::StringUtils
{
    std::wstring UTF8ToUTF16(std::string_view utf8)
    {
        if (utf8.size() > std::numeric_limits<int>::max())
            throw std::overflow_error{"UTF-8 string is too big"};

        const int requiredSize =
        {
            MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                utf8.data(), static_cast<int>(utf8.size()),
                                nullptr, 0)
        };

        if (requiredSize == 0)
            ThrowLastError();

        // TODO: Use C++23 std::wstring::resize_and_overwrite
        std::wstring utf16(requiredSize, L'\0');

        const int writtenSize =
        {
            MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                utf8.data(), static_cast<int>(utf8.size()),
                                utf16.data(), requiredSize)
        };

        if (writtenSize == 0)
            ThrowLastError();

        assert(requiredSize == writtenSize);

        return utf16;
    }

    std::string UTF16ToUTF8(std::wstring_view utf16)
    {
        if (utf16.size() > std::numeric_limits<int>::max())
            throw std::overflow_error{"UTF-16 string is too big"};

        const int requiredSize =
        {
            WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, utf16.data(),
                                static_cast<int>(utf16.size()), nullptr, 0,
                                nullptr, nullptr)
        };

        if (requiredSize == 0)
            ThrowLastError();

        // TODO: Use C++23 std::string::resize_and_overwrite
        std::string utf8(requiredSize, '\0');

        const int writtenSize =
        {
            WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS,
                                utf16.data(), static_cast<int>(utf16.size()),
                                utf8.data(), requiredSize,
                                nullptr, nullptr)
        };

        if (writtenSize == 0)
            ThrowLastError();

        assert(requiredSize == writtenSize);

        return utf8;
    }
}
