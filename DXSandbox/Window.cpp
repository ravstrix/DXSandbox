#include "Window.hpp"

#include "ErrorHandling.hpp"
#include "IWindowPresenter.hpp"

#include <bit>
#include <cassert>

namespace
{
    constexpr DWORD WindowedModeStyle = WS_OVERLAPPEDWINDOW;

    constexpr POINT DefaultClientSize = {1280, 720};

    constexpr const wchar_t* DefaultWindowTitle = L"DXSandbox";

    inline WNDPROC SetWindowProcedure(HWND handle, WNDPROC wndProc) noexcept
    {
        assert(wndProc);

        const auto newWndProc = std::bit_cast<LONG_PTR>(wndProc);
        const auto oldWndProc = SetWindowLongPtrW(handle, GWLP_WNDPROC, newWndProc);

        assert(newWndProc != oldWndProc);

        return std::bit_cast<WNDPROC>(oldWndProc);
    }

    inline DXSandbox::Window* SetWindowPtr(HWND handle, DXSandbox::Window* window) noexcept
    {
        const auto newWindow = std::bit_cast<LONG_PTR>(window);
        const auto oldWindow = SetWindowLongPtrW(handle, GWLP_USERDATA, newWindow);

        assert(newWindow != oldWindow);

        return std::bit_cast<DXSandbox::Window*>(oldWindow);
    }

    inline DXSandbox::Window& GetWindowRef(HWND handle) noexcept
    {
        const auto userData = GetWindowLongPtrW(handle, GWLP_USERDATA);
        const auto window = std::bit_cast<DXSandbox::Window*>(userData);

        assert(window && window->Handle() == handle);

        return *window;
    }

    inline POINT RectSize(const RECT& rect) noexcept
    {
        return
        {
            .x = rect.right - rect.left,
            .y = rect.bottom - rect.top
        };
    }

    inline POINT AdjustedSize(POINT size, DWORD style, BOOL hasMenu = FALSE)
    {
        RECT bounds{0, 0, size.x, size.y};

        if (!AdjustWindowRect(&bounds, style, hasMenu))
            DXSandbox::ThrowLastError();

        return RectSize(bounds);
    }
}

namespace DXSandbox
{
    Window::Window(HINSTANCE hInstance, IWindowPresenter& presenter)
        : m_class{hInstance, MessageHandlerPreCreate}
        , m_presenter{&presenter}
    {
        const POINT size = AdjustedSize(DefaultClientSize, WindowedModeStyle);

        m_hWnd = CreateWindowExW(0, MAKEINTATOM(m_class.Atom()),
                                 DefaultWindowTitle, WindowedModeStyle,
                                 CW_USEDEFAULT, CW_USEDEFAULT, size.x, size.y,
                                 nullptr, nullptr, m_class.ModuleHandle(), this);

        if (!m_hWnd)
            ThrowLastError();
    }

    Window::~Window()
    {
        assert(IsWindow(m_hWnd));

        [[maybe_unused]]
        const BOOL isDestroyed = DestroyWindow(m_hWnd);

        assert(isDestroyed && !m_hWnd);
    }

    HWND Window::Handle() const noexcept
    {
        assert(IsWindow(m_hWnd));

        return m_hWnd;
    }

    void Window::Show()
    {
        assert(IsWindow(m_hWnd));

        ShowWindow(m_hWnd, SW_NORMAL);
    }

    void Window::Hide()
    {
        assert(IsWindow(m_hWnd));

        ShowWindow(m_hWnd, SW_HIDE);
    }

    void Window::SetForeground()
    {
        assert(IsWindow(m_hWnd));

        SetForegroundWindow(m_hWnd);
    }

    void Window::Update()
    {
        assert(IsWindow(m_hWnd));

        UpdateWindow(m_hWnd);
    }

    LRESULT CALLBACK Window::MessageHandlerPreCreate(HWND hWnd, UINT message,
                                                     WPARAM wParam, LPARAM lParam)
    {
        if (message != WM_CREATE)
            return DefWindowProcW(hWnd, message, wParam, lParam);

        const auto createStruct = std::bit_cast<const CREATESTRUCTW*>(lParam);
        const auto window = std::bit_cast<Window*>(createStruct->lpCreateParams);

        assert(window && !window->m_hWnd);

        SetLastError(ERROR_SUCCESS);

        if (!SetWindowPtr(hWnd, window))
        {
            if (GetLastError() != ERROR_SUCCESS)
                ThrowLastError();
        }

        if (!SetWindowProcedure(hWnd, Window::MessageHandler))
        {
            if (GetLastError() != ERROR_SUCCESS)
                ThrowLastError();
        }

        return TRUE;
    }

    LRESULT CALLBACK Window::MessageHandler(HWND hWnd, UINT message,
                                            WPARAM wParam, LPARAM lParam)
    {
        auto& window = GetWindowRef(hWnd);

        switch (message)
        {
            case WM_GETMINMAXINFO:
                window.OnGetMinMaxInfo(*std::bit_cast<MINMAXINFO*>(lParam));
                return 0;

            case WM_CLOSE:
                window.OnClose();
                return 0;

            case WM_DESTROY:
                window.OnDestroy();
                return 0;
        }

        return DefWindowProcW(hWnd, message, wParam, lParam);
    }

    void Window::OnGetMinMaxInfo(MINMAXINFO& info)
    {
        const auto minSize = m_presenter->WindowMinSize();
        const auto style = GetStyle();

        info.ptMinTrackSize = AdjustedSize(minSize, style);
    }

    void Window::OnClose()
    {
        m_presenter->OnWindowClose(*this);
    }

    void Window::OnDestroy() noexcept
    {
        [[maybe_unused]]
        const auto oldWindow = SetWindowPtr(m_hWnd, nullptr);

        assert(oldWindow == this);

        [[maybe_unused]]
        const auto oldWndProc = SetWindowProcedure(m_hWnd, DefWindowProcW);

        assert(oldWndProc == Window::MessageHandler);

        m_hWnd = nullptr;
    }

    DWORD Window::GetStyle() const
    {
        SetLastError(ERROR_SUCCESS);

        const LONG_PTR style = GetWindowLongPtrW(m_hWnd, GWL_STYLE);

        if (!style && GetLastError() != ERROR_SUCCESS)
            ThrowLastError();

        return static_cast<DWORD>(style);
    }
}