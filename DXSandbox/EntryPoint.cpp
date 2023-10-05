#include "WindowsPlatform.hpp"

#include "Application.hpp"

_Use_decl_annotations_
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int)
{
    DXSandbox::Application app{hInstance};

    return app.Run();
}
