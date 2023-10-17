#pragma once

#include <format>
#include <string_view>

namespace DXSandbox::Debug
{
#ifndef NDEBUG
    void Write(const char* text) noexcept;

    template <typename... Args>
    void WriteLine(const std::format_string<Args...> format, Args&&... args)
    {
        static constexpr std::size_t EOLSize = 2;
        static constexpr std::size_t MaxTextLength = 512;

        char buffer[MaxTextLength + EOLSize];

        auto result = std::format_to_n(buffer, MaxTextLength,
                                       format, std::forward<Args>(args)...);

        *result.out++ = '\n';
        *result.out++ = '\0';

        Write(buffer);
    }

    inline void WriteLine(std::string_view text)
    {
        WriteLine("{}", text);
    }
#else // NDEBUG - Release build
    inline void Write(const char*) noexcept {}

    template <typename... Args>
    inline void WriteLine(const std::format_string<Args...>, Args&&...) noexcept {}

    inline void WriteLine(std::string_view) noexcept {}
#endif
}
