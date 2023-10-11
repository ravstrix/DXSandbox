#include "Application.hpp"

#include "GraphicsSystem.hpp"
#include "Window.hpp"

#include <cassert>

namespace DXSandbox
{
    Application::Application(HINSTANCE hInstance) : m_hInstance{hInstance}
    {
    }

    Application::~Application() = default;

    int Application::Run()
    {
        Startup();
        MainLoop();
        Shutdown();

        return ExitCode();
    }

    void Application::ExitRequest(int exitCode)
    {
        m_exitCode = exitCode;
        m_isExitRequested = true;
    }

    bool Application::IsExitRequested() const
    {
        return m_isExitRequested;
    }

    int Application::ExitCode() const
    {
        return m_exitCode;
    }

    void Application::OnWindowClose(Window& /*sender*/)
    {
        ExitRequest();
    }

    POINT Application::WindowMinSize() const
    {
        return {1280, 720};
    }

    void DXSandbox::Application::Startup()
    {
        assert(!m_window);

        IWindowPresenter& presenter = *this;

        m_window = std::make_unique<Window>(m_hInstance, presenter);
        m_graphicsSystem = std::make_unique<GraphicsSystem>(*m_window);

        m_window->Show();
        m_window->SetForeground();
        m_window->Update();

        ProcessWindowMessages();
    }

    void Application::MainLoop()
    {
        if (IsExitRequested())
            return;

        while (ProcessWindowMessages())
        {
            if (IsExitRequested())
                PostMainLoopQuitMessage();

            m_graphicsSystem->Render();
        }

        assert(IsExitRequested());
    }

    bool Application::ProcessWindowMessages()
    {
        MSG msg;

        while (PeekMessageW(&msg, nullptr, WM_NULL, WM_NULL, PM_REMOVE))
        {
            if (msg.message != WM_QUIT)
            {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            else
            {
                assert(IsExitRequested());
                assert(ExitCode() == static_cast<int>(msg.wParam));

                return false;
            }
        }

        return true;
    }

    void Application::PostMainLoopQuitMessage()
    {
        assert(IsExitRequested());

        PostQuitMessage(ExitCode());
    }

    void Application::Shutdown()
    {
        assert(m_window && m_graphicsSystem);

        m_window->Hide();

        m_graphicsSystem = nullptr;
        m_window = nullptr;
    }
}
