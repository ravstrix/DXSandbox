#include "Application.hpp"

#include "CommandLineArgs.hpp"
#include "GraphicsSystem.hpp"
#include "Window.hpp"

#include <cassert>

namespace DXSandbox
{
    Application::Application(HINSTANCE hInstance, PWSTR commandLine)
        : m_hInstance{hInstance}
        , m_commandLineArgs{commandLine}
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
        MakeWindow();
        MakeGraphicsSystem();

        m_window->Show();
        m_window->SetForeground();
        m_window->Update();

        ProcessWindowMessages();
    }

    void Application::MakeWindow()
    {
        assert(!m_window);

        IWindowPresenter& presenter = *this;

        m_window = std::make_unique<Window>(m_hInstance, presenter);
    }

    void Application::MakeGraphicsSystem()
    {
        assert(m_window && !m_graphicsSystem);

        const POINT size = m_window->ClientSize();

        const GraphicsSystem::InitParams params =
        {
            .hWnd = m_window->Handle(),
            .width = static_cast<UINT>(size.x),
            .height = static_cast<UINT>(size.y),
            .enableDebugLayer = m_commandLineArgs.Contains("--d3dEnableDebugLayer")
        };

        m_graphicsSystem = std::make_unique<GraphicsSystem>(params);
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
        m_window->Hide();

        DestroyGraphicsSystem();
        DestroyWindow();
    }

    void Application::DestroyGraphicsSystem()
    {
        assert(m_window && m_graphicsSystem);

        m_graphicsSystem = nullptr;
    }

    void Application::DestroyWindow()
    {
        assert(m_window);

        m_window = nullptr;
    }
}
