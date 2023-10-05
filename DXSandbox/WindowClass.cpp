#include "WindowClass.hpp"

#include "ErrorHandling.hpp"

#include <cassert>

namespace DXSandbox
{
    WindowClass::WindowClass(HINSTANCE hInstance, WNDPROC wndProc)
        : m_hInstance{hInstance}
    {
        assert(m_hInstance && wndProc);

        const auto hIcon = LoadIconW(nullptr, IDI_APPLICATION);
        if (!hIcon)
            ThrowLastError();

        const auto hCursor = LoadCursorW(nullptr, IDC_ARROW);
        if (!hCursor)
            ThrowLastError();

        const auto hBrush = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
        if (!hBrush)
            ThrowHResultError(E_INVALIDARG);

        const WNDCLASSEXW windowClass =
        {
            .cbSize = sizeof(windowClass),
            .style = CS_VREDRAW | CS_HREDRAW,
            .lpfnWndProc = wndProc,
            .cbClsExtra = 0,
            .cbWndExtra = 0,
            .hInstance = m_hInstance,
            .hIcon = hIcon,
            .hCursor = hCursor,
            .hbrBackground = hBrush,
            .lpszMenuName = nullptr,
            .lpszClassName = L"DXSandbox::WindowClass",
            .hIconSm = nullptr
        };

        m_atom = RegisterClassExW(&windowClass);

        if (!m_atom)
            ThrowLastError();
    }

    WindowClass::~WindowClass()
    {
        [[maybe_unused]]
        const BOOL isUnregistered = UnregisterClassW(MAKEINTATOM(m_atom), m_hInstance);

        assert(isUnregistered);
    }

    ATOM WindowClass::Atom() const noexcept
    {
        return m_atom;
    }

    HINSTANCE WindowClass::ModuleHandle() const noexcept
    {
        return m_hInstance;
    }
}
