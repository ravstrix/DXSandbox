#pragma once

#include "WindowsPlatform.hpp"

#include <stdexcept>

namespace DXSandbox
{
    class [[nodiscard]] HResultException final : public std::runtime_error
    {
    public:
        static HResultException FromLastError()
        {
            return {HRESULT_FROM_WIN32(GetLastError())};
        }

        static HResultException FromHResult(HRESULT hResult)
        {
            return {hResult};
        }

        HRESULT Result() const noexcept { return m_hResult; }

    private:
        HResultException(HRESULT error);

    private:
        const HRESULT m_hResult = E_FAIL;
    };
}
