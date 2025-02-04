#ifndef MURENDERER_BINDABLE_CONSTANTBUFFER_HPP
#define MURENDERER_BINDABLE_CONSTANTBUFFER_HPP

#include "MuRenderer/Bindable/IBindable.hpp"
#include "MuRenderer/Utils/MuD3D.hpp"

namespace murenderer
{
    struct ConstantBuffer : public IBindable {};

    struct ConstantBufferTransform : ConstantBuffer
    {
        DirectX::XMFLOAT4 m_offset;
    };

    struct ConstantBufferTime : ConstantBuffer
    {
        float m_time;
    };

    struct ConstantBufferDepthImage : ConstantBuffer
    {
        DirectX::XMMATRIX m_modelViewProjection;
    };
}

#endif // !MURENDERER_BINDABLE_CONSTANTBUFFER_HPP
