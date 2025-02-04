#ifndef MURENDERER_RENDERER_HPP
#define MURENDERER_RENDERER_HPP

#include "MuRenderer/SystemTypes.hpp"
#include "MuRenderer/Utils/MuD3D.hpp"
#include "MuRenderer/RenderProperties.hpp"
#include "MuRenderer/Device/DevicePool.hpp"

#include "MuRenderer/Bindable/ConstantBuffer.hpp"
#include "MuRenderer/Bindable/VertexBuffer.hpp"

using namespace Microsoft;

namespace murenderer
{
    class Renderer
    {
    private:
        DevicePool m_devicePool;

        D3D12_VIEWPORT m_dxViewport;
        D3D12_RECT m_dxScissorRect;

        MS::ComPtr<IDXGIFactory4> m_dxDXGIFactory;
        MS::ComPtr<IDXGISwapChain> m_dxSwapChain;
        int m_currentFrameIndex;

        UINT64 m_currentFenceValue;
        UINT64 m_fenceValues[RenderProperties_NumRenderTargets];
        HANDLE m_fenceEvent;

    public:
        Renderer();
        ~Renderer();

        void UpdateConstantBuffer(MS::ComPtr<ID3D12Resource> aConstantBuffer, ConstantBuffer* aConstantData, const UINT& aDataSize);
        void UpdateVertexBuffer(MS::ComPtr<ID3D12Resource> aVertexBuffer, VertexBuffer* aVertexBufferData, const UINT& aDataSize);

    };
}

#endif // !MURENDERER_RENDERER_HPP
