#include "HResultException.hpp"

#include <bit>
#include <cassert>
#include <format>
#include <type_traits>

namespace
{
    inline std::string HResultToString(HRESULT hResult)
    {
        using UHR = std::make_unsigned_t<HRESULT>;
        const auto value = std::bit_cast<UHR>(hResult);

        return std::format("HRESULT: {:#010x}", value);
    }
}

namespace DXSandbox
{
    HResultException::HResultException(HRESULT error)
        : std::runtime_error{HResultToString(error)}
        , m_hResult{error}
    {
        assert(FAILED(m_hResult));
    }
}
