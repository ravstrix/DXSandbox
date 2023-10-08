#include "Debug.hpp"

#ifndef NDEBUG

#include "WindowsPlatform.hpp"

#include <cassert>

namespace DXSandbox::Debug
{
    void Write(const char* text) noexcept
    {
        assert(text);

        OutputDebugStringA(text);
    }
}

#endif // NDEBUG
