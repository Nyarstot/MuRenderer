#ifndef MURENDERER_CORE_RENDERER_HPP
#define MURENDERER_CORE_RENDERER_HPP

#include "MuRenderer/Device/DevicePair.hpp"
#include "MuRenderer/System/RenderWindowInfo.hpp"
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

    protected:
        void UpdateConstantBuffer(MS::ComPtr<ID3D12Resource> aConstantBuffer, ConstantBuffer* aConstantData, const UINT& aDataSize);
        void UpdateVertexBuffer(MS::ComPtr<ID3D12Resource> aVertexBuffer, VertexBuffer* aVertexBufferData, const UINT& aDataSize);
        void UpdateTextureData(MS::ComPtr<ID3D12GraphicsCommandList> aCommandList, MS::ComPtr<ID3D12Resource> aTextureResource,
            MS::ComPtr<ID3D12Resource> aTextureUploadHeap, void* aTextureData, UINT aPixelSize);

        void CreateSwapChain(Device* aDevice, DXGI_SWAP_CHAIN_DESC1 aSwapChainDesc);
        void CreateTexture(Device* aDevice, D3D12_RESOURCE_DESC textureDesc, MS::ComPtr<ID3D12Resource>* texture, MS::ComPtr<ID3D12Resource>* textureUploadHeap, const UINT heapIndex);
        void CreatePipelineState(Device* aDevice, MS::ComPtr<ID3D12PipelineState>* pipelineStateObject, D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc, std::wstring shaderPath, D3D12_INPUT_ELEMENT_DESC inputLayoutDesc[], UINT inputLayoutCount, MS::ComPtr<ID3D12RootSignature> rootSignature);
        void CreateVertexBuffer(Device* aDevice, D3D12_VERTEX_BUFFER_VIEW* vertexBufferView, MS::ComPtr<ID3D12Resource>* vertexBuffer, Vertex* vertexData, const UINT vertexDataSize, const UINT vertexSize);
        void CreateConstantBuffer(Device* aDevice, MS::ComPtr<ID3D12Resource>* constantBuffer, ConstantBuffer* constantBufferData, const UINT constantBufferSize, const UINT cbvHeapPosition);

    public:
        Renderer();
        virtual ~Renderer() = default;

        void OnRender();
        void OnDestroy();

        void UpdateRenderWindowInfo(const RenderWindowInfo& aRenderWindowInfo);

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
