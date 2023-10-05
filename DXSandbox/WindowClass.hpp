#pragma once

#include "WindowsPlatform.hpp"

namespace DXSandbox
{
    class WindowClass final
    {
    public:
        explicit WindowClass(HINSTANCE hInstance, WNDPROC wndProc);
        ~WindowClass();

        WindowClass(const WindowClass&) = delete;
        WindowClass& operator = (const WindowClass&) = delete;

        ATOM Atom() const noexcept;
        HINSTANCE ModuleHandle() const noexcept;

    private:
        ATOM m_atom = INVALID_ATOM;
        HINSTANCE m_hInstance = nullptr;
    };
}
