#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace DXSandbox
{
    class CommandLineArgs final
    {
    public:
        explicit CommandLineArgs(wchar_t* cmdLine);

        bool Contains(std::string_view arg) const;

        const auto begin() const noexcept
        {
            return m_args.begin();
        }

        const auto end() const noexcept
        {
            return m_args.end();
        }

    private:
        std::vector<std::string> m_args;
    };
}
