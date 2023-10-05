#pragma once

#include "WindowsPlatform.hpp"

#include "WindowClass.hpp"

#include <string_view>

namespace DXSandbox
{
    class IWindowPresenter;

    class Window final
    {
    public:
        explicit Window(HINSTANCE hInstance, IWindowPresenter& presenter);
        ~Window();

        Window(const Window&) = delete;
        Window& operator = (const Window&) = delete;

        HWND Handle() const noexcept;

        void Show();
        void Hide();
        void SetForeground();
        void Update();

    private:
        static LRESULT CALLBACK MessageHandlerPreCreate(HWND hWnd, UINT message,
                                                        WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK MessageHandler(HWND hWnd, UINT message,
                                               WPARAM wParam, LPARAM lParam);

        void OnGetMinMaxInfo(MINMAXINFO& info);
        void OnClose();
        void OnDestroy() noexcept;

        DWORD GetStyle() const;

    private:
        WindowClass m_class;
        HWND m_hWnd = nullptr;
        IWindowPresenter* m_presenter = nullptr;
    };
}
