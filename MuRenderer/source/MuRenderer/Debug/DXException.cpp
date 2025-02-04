#include "MuRenderer/Debug/DXException.hpp"


namespace murenderer
{
    DXException::DXException(HRESULT aHRESULT) : m_hresult(aHRESULT)
    {
    }

    const char* DXException::what() const noexcept
    {
        static char s_str[64] = {};
        sprintf_s(s_str, "Operation failed with HRESULT of %08X", static_cast<unsigned int>(m_hresult));
        return s_str;
    }
}