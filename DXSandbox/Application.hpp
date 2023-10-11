#pragma once

#include "WindowsPlatform.hpp"

#include "IWindowPresenter.hpp"

#include <memory>

namespace DXSandbox
{
    class GraphicsSystem;
    class Window;

    class Application final : private IWindowPresenter
    {
    public:
        explicit Application(HINSTANCE hInstance);
        ~Application();

        int Run();

        void ExitRequest(int exitCode = 0);
        bool IsExitRequested() const;
        int ExitCode() const;

    private:
        void OnWindowClose(Window& sender) override;

        POINT WindowMinSize() const override;

    private:
        void Startup();
        void MainLoop();
        bool ProcessWindowMessages();
        void PostMainLoopQuitMessage();
        void Shutdown();

    private:
        HINSTANCE m_hInstance = nullptr;

        std::unique_ptr<Window> m_window;
        std::unique_ptr<GraphicsSystem> m_graphicsSystem;

        bool m_isExitRequested = false;
        int m_exitCode = 0;
    };
}
