#ifndef MURENDERER_DEBUG_DXEXCEPTION_HPP
#define MURENDERER_DEBUG_DXEXCEPTION_HPP

#include <stdexcept>
#include <exception>
#include <Windows.h>


namespace murenderer
{
    class DXException final : public std::exception
    {
    private:
        HRESULT m_hresult;

    public:
        explicit DXException(HRESULT aHRESULT);
        ~DXException() = default;
        const char* what() const noexcept override;

    };

    inline void DXThrowIfFailed(HRESULT aHRESULT) {
        if (FAILED(aHRESULT)) {
            auto exception = DXException(aHRESULT);
            throw exception;
        }
    }
}

#endif // !MURENDERER_DEBUG_DXEXCEPTION_HPP
