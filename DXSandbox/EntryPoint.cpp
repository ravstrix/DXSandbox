#include "WindowsPlatform.hpp"

#include "Application.hpp"

#ifndef NDEBUG
#   include <crtdbg.h>
#endif

_Use_decl_annotations_
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int)
{
#ifndef NDEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    DXSandbox::Application app{hInstance};

    return app.Run();
}
