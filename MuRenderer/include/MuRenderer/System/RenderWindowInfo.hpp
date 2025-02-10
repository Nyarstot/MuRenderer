#ifndef MURENDERER_SYSTEM_RENDERWINDOWINFO_HPP
#define MURENDERER_SYSTEM_RENDERWINDOWINFO_HPP

#include "MuRenderer/System/SystemTypes.hpp"


namespace murenderer
{
    struct RenderWindowInfo final
    {
        mu_uint_t m_height;
        mu_uint_t m_width;
        HWND m_hwnd;
    };
}

#endif // !MURENDERER_SYSTEM_RENDERWINDOWINFO_HPP
