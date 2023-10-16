#include "CommandLineArgs.hpp"

#include "WindowsPlatform.hpp"

#include "ErrorHandling.hpp"
#include "StringUtils.hpp"

#include <shellapi.h>

#include <algorithm>

namespace
{
    std::vector<std::string> MakeCommandLineArgs(wchar_t* cmdLine)
    {
        std::vector<std::string> args;

        if (cmdLine && *cmdLine != L'\0')
        {
            int argc = 0;
            PWSTR* argv = CommandLineToArgvW(cmdLine, &argc);

            if (!argv)
                DXSandbox::ThrowLastError();

            args.resize(argc);

            for (int i = 0; i < argc; ++i)
                args[i] = DXSandbox::StringUtils::UTF16ToUTF8(argv[i]);

            LocalFree(argv);
        }

        return args;
    }
}

namespace DXSandbox
{
    CommandLineArgs::CommandLineArgs(wchar_t* cmdLine)
        : m_args(MakeCommandLineArgs(cmdLine))
    {
    }

    bool CommandLineArgs::Contains(std::string_view arg) const
    {
        return std::find(m_args.begin(), m_args.end(), arg) != m_args.end();
    }
}
