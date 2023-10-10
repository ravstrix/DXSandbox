#pragma once

#include <string>
#include <string_view>

namespace DXSandbox::StringUtils
{
    std::wstring UTF8ToUTF16(std::string_view utf8);

    std::string UTF16ToUTF8(std::wstring_view utf16);
}
