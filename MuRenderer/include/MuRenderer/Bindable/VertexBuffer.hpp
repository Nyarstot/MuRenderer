#ifndef MURENDERER_BINDABLE_VERTEXBUFFER_HPP
#define MURENDERER_BINDABLE_VERTEXBUFFER_HPP

#include "MuRenderer/Bindable/IBindable.hpp"
#include "MuRenderer/System/SystemTypes.hpp"
#include "MuRenderer/Core/Vertex.hpp"
#include "MuRenderer/Device/Device.hpp"

namespace murenderer
{
    class VertexBuffer : public IBindable
    {
    private:
        MS::ComPtr<ID3D12Resource> m_vertexBufferResource;
        const mu_uint_t m_vertexDataSize;
        const mu_uint_t m_vertexSize;

    public:
        VertexBuffer(Device* aDevice, D3D12_VERTEX_BUFFER_VIEW* aVertexBufferView, Vertex* aVertexData);
        ~VertexBuffer();

        void Bind() override;

    };
}

#endif // !MURENDERER_BINDABLE_VERTEXBUFFER_HPP
