#pragma once

#define __WRL_NO_DEFAULT_LIB__
#define __WRL_CLASSIC_COM_STRICT__

#include <wrl/client.h>

namespace DXSandbox
{
    template <typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;
}
