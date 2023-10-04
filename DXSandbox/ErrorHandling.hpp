#include "WindowsPlatform.hpp"

#include "HResultException.hpp"

namespace DXSandbox
{
    [[noreturn]] inline void ThrowLastError()
    {
        throw HResultException::FromLastError();
    }

    [[noreturn]] inline void ThrowHResultError(HRESULT hResult)
    {
        throw HResultException::FromHResult(hResult);
    }

    inline void ThrowIfFailed(HRESULT hResult)
    {
        if (FAILED(hResult))
            ThrowHResultError(hResult);
    }
}
