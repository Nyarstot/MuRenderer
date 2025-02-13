#ifndef MURENDERER_SYSTEMTYPES_HPP
#define MURENDERER_SYSTEMTYPES_HPP

#include <wrl.h>
#include <memory>

namespace MS = Microsoft::WRL;

namespace murenderer
{
    template<typename Ty>
    using SharedPtr = std::shared_ptr<Ty>;

    template<typename Ty>
    using UniquePtr = std::unique_ptr<Ty>;

    template<typename Ty>
    using WeakPtr = std::weak_ptr<Ty>;

    template<typename Ty>
    using ComPtr = Microsoft::WRL::ComPtr<Ty>;

    template<typename Ty, typename... Args>
    constexpr SharedPtr<Ty> CreateShared(Args&&... args)
    {
        return std::make_shared<Ty>(std::forward<Args>(args)...);
    }

    template<typename Ty, typename... Args>
    constexpr UniquePtr<Ty> CreateUnique(Args&&... args)
    {
        return std::make_unique<Ty>(std::forward<Args>(args)...);
    }

    using mu_wstring_t = std::wstring;

    using mu_dword_t    = DWORD;
    using mu_tchar_t    = TCHAR;
    using mu_lint_t     = LARGE_INTEGER;

    using mu_size_t     = std::size_t;
    using mu_uint_t     = unsigned int;
    using mu_uint8_t    = std::uint8_t;
    using mu_uint16_t   = std::uint16_t;
    using mu_uint32_t   = std::uint32_t;
    using mu_uint64_t   = std::uint64_t;
}

#endif // !MURENDERER_SYSTEMTYPES_HPP
