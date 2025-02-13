#include "pch.hpp"
#include "MuRenderer/Bindable/VertexBuffer.hpp"


namespace murenderer
{
    VertexBuffer::VertexBuffer(Device* aDevice, D3D12_VERTEX_BUFFER_VIEW* aVertexBufferView, Vertex* aVertexData)
        : m_vertexDataSize(sizeof(aVertexData)), m_vertexSize(sizeof(VertexUV))
    {
    }

    VertexBuffer::~VertexBuffer()
    {
    }

    void VertexBuffer::Bind()
    {
    }
}