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
}

#endif // !MURENDERER_SYSTEMTYPES_HPP
