#pragma once

#include "WindowsPlatform.hpp"

namespace DXSandbox
{
    class Window;

    class IWindowPresenter
    {
    public:
        virtual void OnWindowClose(Window& sender) = 0;

        virtual POINT WindowMinSize() const = 0;

    protected:
        ~IWindowPresenter() = default;
    };
}
