#ifndef MURENDERER_RENDERPROPERTIES_HPP
#define MURENDERER_RENDERPROPERTIES_HPP

#include "MuRenderer/System/SystemTypes.hpp"


namespace murenderer
{
    enum RenderProperties : mu_size_t
    {
        RenderProperties_NumRenderTargets = 3
    };

    enum CrossAdapterProperties : mu_size_t
    {
        CrossAdapterProperties_NumDevices = 2
    };
}

#endif // !MURENDERER_RENDERPROPERTIES_HPP
