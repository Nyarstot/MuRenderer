#ifndef MURENDERER_VERTEX_HPP
#define MURENDERER_VERTEX_HPP

#include "MuRenderer/Utils/MuD3D.hpp"


namespace murenderer
{
    struct Vertex {};

    struct VertexUV : Vertex
    {
        DirectX::XMFLOAT4 m_position;
        DirectX::XMFLOAT2 m_uv;
    };

    struct VertexColor : Vertex
    {
        DirectX::XMFLOAT4 m_offset;
    };

}

#endif // !MURENDERER_VERTEX_HPP
