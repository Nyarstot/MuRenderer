#ifndef MURENDERER_CORE_RENDERER_HPP
#define MURENDERER_CORE_RENDERER_HPP

#include "MuRenderer/Core/RenderProperties.hpp"
#include "MuRenderer/Device/DevicePair.hpp"
#include "MuRenderer/Utils/MuD3D.hpp"

#include "MuRenderer/System/RenderWindowInfo.hpp"
#include "MuRenderer/System/SystemTypes.hpp"

#include "MuRenderer/Bindable/ConstantBuffer.hpp"
#include "MuRenderer/Bindable/VertexBuffer.hpp"

using namespace Microsoft;

namespace murenderer
{
    class Renderer
    {
    private:
        SharedPtr<DevicePair> m_devicePair;

        D3D12_VIEWPORT m_dxViewport;
        D3D12_RECT m_dxScissorRect;

        MS::ComPtr<IDXGIFactory4> m_dxDXGIFactory;
        MS::ComPtr<IDXGISwapChain3> m_dxSwapChain;
        int m_currentFrameIndex;

        UINT64 m_currentFenceValue;
        UINT64 m_fenceValues[RenderProperties_NumRenderTargets];
        HANDLE m_fenceEvent;
        RenderWindowInfo m_renderWindowInfo;

    private:
        void UpdateConstantBuffer(MS::ComPtr<ID3D12Resource> aConstantBuffer, ConstantBuffer* aConstantData, const UINT& aDataSize);
        void UpdateVertexBuffer(MS::ComPtr<ID3D12Resource> aVertexBuffer, VertexBuffer* aVertexBufferData, const UINT& aDataSize);
        void UpdateTextureData(MS::ComPtr<ID3D12GraphicsCommandList> aCommandList, MS::ComPtr<ID3D12Resource> aTextureResource,
            MS::ComPtr<ID3D12Resource> aTextureUploadHeap, void* aTextureData, UINT aPixelSize);

    public:
        Renderer();
        virtual ~Renderer() = default;

        void OnRender();
        void OnDestroy();

        void UpdateRenderWindowInfo(const RenderWindowInfo& aRenderWindowInfo);
        void CreateSwapChain(Device* aDevice, DXGI_SWAP_CHAIN_DESC1 aSwapChainDesc);

        virtual void OnInit() = 0;
        virtual void OnUpdate() = 0;

        virtual void CreateRootSignature()  = 0;
        virtual void LoadPipeline()         = 0;
        virtual void LoadAsset()            = 0;
        virtual void CreateCommandList()    = 0;
        virtual void PopulateCommandList()  = 0;
        virtual void ExecuteCommandList()   = 0;
        virtual void MoveToNextFrame()      = 0;

    };

}

#endif // !MURENDERER_CORE_RENDERER_HPP
