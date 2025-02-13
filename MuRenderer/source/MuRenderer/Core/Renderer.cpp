#include "pch.hpp"
#include "MuRenderer/Core/Renderer.hpp"
#include "MuRenderer/Debug/DXException.hpp"
#include "MuRenderer/Device/Device.hpp"
#include "MuRenderer/Core/Feature.hpp"

#include "MuRenderer/Core/MultiGPURenderer.hpp"
#include "MuRenderer/Core/SingleGPURenderer.hpp"



namespace murenderer
{
    Renderer::Renderer()
    {
        m_devicePair = std::make_shared<DevicePair>();
        m_devicePair->CreateDevices();
    }

    void Renderer::OnRender()
    {
        m_currentFrameIndex = m_dxSwapChain->GetCurrentBackBufferIndex();

        PopulateCommandList(); // Record all the commands we want to perform
        ExecuteCommandList(); // Execure all the commands from list

        DXThrowIfFailed(m_dxSwapChain->Present(0, NULL));
        MoveToNextFrame();
    }

    void Renderer::OnDestroy()
    {
        // Wait for GPU to be done
        MoveToNextFrame();
        CloseHandle(m_fenceEvent);
    }

    void Renderer::UpdateRenderWindowInfo(const RenderWindowInfo& aRenderWindowInfo)
    {
        m_renderWindowInfo = aRenderWindowInfo;
    }

    void Renderer::CreateSwapChain(Device* aDevice, DXGI_SWAP_CHAIN_DESC1 aSwapChainDesc)
    {
        // Invariant swapchain props
        aSwapChainDesc.Width        = static_cast<UINT>(m_renderWindowInfo.m_width);
        aSwapChainDesc.Height       = static_cast<UINT>(m_renderWindowInfo.m_height);
        aSwapChainDesc.BufferCount  = static_cast<UINT>(RenderProperties_NumRenderTargets);

        // Create SwapChain
        IDXGISwapChain1* tempSwapChain = nullptr; // Call to CreateSwapChainForHwnd() requires a SwapChain1 object
        DXThrowIfFailed(m_dxDXGIFactory->CreateSwapChainForHwnd(aDevice->GetCommandQueue(), m_renderWindowInfo.m_hwnd, &aSwapChainDesc, nullptr, nullptr, &tempSwapChain));
        DXThrowIfFailed(tempSwapChain->QueryInterface(IID_PPV_ARGS(&(m_dxSwapChain)))); // Copy the temp SwapChain1 into our desired SwapChain3
        tempSwapChain->Release();

        // Get a handle to the memory location (CPU) of the descriptor heap
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(aDevice->GetRTVHeap()->GetCPUDescriptorHandleForHeapStart());
        mu_uint_t rtvDescSize = static_cast<mu_uint_t>(aDevice->GetDXDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
        aDevice->SetRTVDescriptorSize(rtvDescSize);

        // Create render target and view for each swap chain buffer
        for (UINT iSchain = 0; iSchain <= aSwapChainDesc.BufferCount; iSchain++) {
            // Get buffer render target
            DXThrowIfFailed(m_dxSwapChain->GetBuffer(iSchain, IID_PPV_ARGS(&aDevice->m_renderTargets[iSchain])));
            // Create rtv
            aDevice->GetDXDevice()->CreateRenderTargetView(aDevice->m_renderTargets[iSchain].Get(), nullptr, rtvHandle);
            // Increment the rtv heap handle
            rtvHandle.Offset(1, aDevice->GetRTVDescriptorSize());
        }
    }

    void Renderer::CreateTexture(Device* aDevice, D3D12_RESOURCE_DESC textureDesc, MS::ComPtr<ID3D12Resource>* texture, MS::ComPtr<ID3D12Resource>* textureUploadHeap, const UINT heapIndex)
    {
        // Describe and create a Texture2D.
        textureDesc.MipLevels           = 1;
        textureDesc.DepthOrArraySize    = 1;
        textureDesc.SampleDesc.Count    = 1;
        textureDesc.SampleDesc.Quality  = 0;
        textureDesc.Dimension           = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

        CD3DX12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

        DXThrowIfFailed(aDevice->GetDXDevice()->CreateCommittedResource(
            &heapProps, D3D12_HEAP_FLAG_NONE, &textureDesc,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            nullptr, IID_PPV_ARGS(&(*texture))
        ));

        // Only create upload heap if CPU uploads to texture
        if (textureUploadHeap != nullptr) {
            const mu_uint64_t uploadBufferSize = GetRequiredIntermediateSize((*texture).Get(), 0, 1);

            CD3DX12_HEAP_PROPERTIES heapProps2 = {};
            heapProps2.Type = D3D12_HEAP_TYPE_UPLOAD;

            CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

            // Create the GPU upload buffer.
            DXThrowIfFailed(aDevice->GetDXDevice()->CreateCommittedResource(
                &heapProps2, D3D12_HEAP_FLAG_NONE, &resDesc,
                D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&(*textureUploadHeap))
            ));
        }

        // Describe and create a SRV for the texture.
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = textureDesc.Format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;

        // Add SRV to descriptor heap
        CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(aDevice->GetCbvSrvUavHeap()->GetCPUDescriptorHandleForHeapStart());
        srvHandle.Offset(aDevice->GetCbvSrvUavDescriptorSize() * heapIndex);
        aDevice->GetDXDevice()->CreateShaderResourceView((*texture).Get(), &srvDesc, srvHandle);
    }

    void Renderer::CreatePipelineState(Device* aDevice, MS::ComPtr<ID3D12PipelineState>* pipelineStateObject, D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc, std::wstring shaderPath, D3D12_INPUT_ELEMENT_DESC inputLayoutDesc[], UINT inputLayoutCount, MS::ComPtr<ID3D12RootSignature> rootSignature)
    {
        #if DEBUG
                // Enable better shader debugging with the graphics debugging tools.
                UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
        #else
                UINT compileFlags = 0;
        #endif

        ID3D12PipelineState* pso = (*pipelineStateObject).Get();
        ComPtr<ID3DBlob> vertexShader   = nullptr;
        ComPtr<ID3DBlob> pixelShader    = nullptr;
        ComPtr<ID3DBlob> errorBlob      = nullptr;

        // TODO: Move to shader class
        if (shaderPath.compare(L"")) {
            // Compile shaders
            HRESULT hres = D3DCompileFromFile(shaderPath.c_str(), nullptr, nullptr, "VShader", "vs_5_0", compileFlags, 0, vertexShader.GetAddressOf(), errorBlob.GetAddressOf());
            if (FAILED(hres)) {
                const char* errMessage = (const char*)errorBlob.Get()->GetBufferPointer();
                std::cout << errMessage << std::endl;
            }

            hres = D3DCompileFromFile(shaderPath.c_str(), nullptr, nullptr, "PShader", "ps_5_0", compileFlags, 0, pixelShader.GetAddressOf(), errorBlob.GetAddressOf());
            if (FAILED(hres)) {
                const char* errMessage = (const char*)errorBlob.Get()->GetBufferPointer();
                std::cout << errMessage << std::endl;
            }

            psoDesc.VS = { reinterpret_cast<mu_uint8_t*>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize() };
            psoDesc.PS = { reinterpret_cast<mu_uint8_t*>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize() };
        }

        // Describe PSO
        psoDesc.InputLayout                 = { inputLayoutDesc, inputLayoutCount };
        psoDesc.pRootSignature              = rootSignature.Get();
        psoDesc.RasterizerState             = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.RasterizerState.CullMode    = D3D12_CULL_MODE_NONE;
        psoDesc.SampleMask                  = UINT_MAX;
        psoDesc.PrimitiveTopologyType       = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets            = 1;
        psoDesc.RTVFormats[0]               = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.DSVFormat                   = DXGI_FORMAT_D32_FLOAT;
        psoDesc.SampleDesc.Count            = 1;

        // Create PSO
        DXThrowIfFailed(aDevice->GetDXDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&(*pipelineStateObject))));
    }

    void Renderer::CreateVertexBuffer(Device* aDevice, D3D12_VERTEX_BUFFER_VIEW* vertexBufferView, MS::ComPtr<ID3D12Resource>* vertexBuffer, Vertex* vertexData, const UINT vertexDataSize, const UINT vertexSize)
    {
        CD3DX12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

        CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexDataSize);

        // Create resource on device for vertex buffer
        DXThrowIfFailed(aDevice->GetDXDevice()->CreateCommittedResource(
            &heapProps, D3D12_HEAP_FLAG_NONE, &resDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&(*vertexBuffer))
        ));

        // Copy vertices into vertex buffer
        // TODO: Move to VertexBuffer Bind method
        mu_uint8_t* dataBegin = nullptr;
        DXThrowIfFailed((*vertexBuffer)->Map(0, nullptr, reinterpret_cast<void**>(&dataBegin)));
        memcpy(dataBegin, vertexData, vertexDataSize);
        (*vertexBuffer)->Unmap(0, nullptr);

        // Initialize the vertex buffer view.
        vertexBufferView->BufferLocation = (*vertexBuffer).Get()->GetGPUVirtualAddress();
        vertexBufferView->StrideInBytes = vertexSize;
        vertexBufferView->SizeInBytes = vertexDataSize;
    }

    void Renderer::CreateConstantBuffer(Device* aDevice, MS::ComPtr<ID3D12Resource>* constantBuffer, ConstantBuffer* constantBufferData, const UINT constantBufferSize, const UINT cbvHeapPosition)
    {
        CD3DX12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

        CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(constantBufferSize);

        // Create resource for constant buffer
        DXThrowIfFailed(aDevice->GetDXDevice()->CreateCommittedResource(
            &heapProps, D3D12_HEAP_FLAG_NONE, &resDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&(*constantBuffer))
        ));

        // Describe and create a constant buffer view
        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
        cbvDesc.BufferLocation = (*constantBuffer)->GetGPUVirtualAddress();
        cbvDesc.SizeInBytes = (constantBufferSize + 255) & ~255; // Require size to be 256 byte aligned
        CD3DX12_CPU_DESCRIPTOR_HANDLE cbvHandle(aDevice->GetCbvSrvUavHeap()->GetCPUDescriptorHandleForHeapStart());
        cbvHandle.Offset(aDevice->GetCbvSrvUavDescriptorSize() * cbvHeapPosition);
        aDevice->GetDXDevice()->CreateConstantBufferView(&cbvDesc, cbvHandle);

        // Copy constant buffer data into constant buffer
        // TODO: Move to ConstantBuffer bind
        mu_uint8_t* cbvDataBegin = nullptr;
        DXThrowIfFailed((*constantBuffer)->Map(0, nullptr, reinterpret_cast<void**>(&cbvDataBegin)));
        memcpy(cbvDataBegin, &constantBuffer, constantBufferSize);
        (*constantBuffer)->Unmap(0, nullptr);
    }

    void Renderer::UpdateConstantBuffer(MS::ComPtr<ID3D12Resource> aConstantBuffer, ConstantBuffer* aConstantData, const UINT& aDataSize)
    {
        if (aConstantBuffer == NULL) return;

        // TODO: Move to ConstantBuffer Bind method
        mu_uint8_t* cbvDataBegin = nullptr;
        DXThrowIfFailed(aConstantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&cbvDataBegin)));
        memcpy(cbvDataBegin, aConstantData, aDataSize);
        aConstantBuffer->Unmap(0, nullptr);
    }

    void Renderer::UpdateVertexBuffer(MS::ComPtr<ID3D12Resource> aVertexBuffer, VertexBuffer* aVertexBufferData, const UINT& aDataSize)
    {
        if (aVertexBuffer == NULL) return;

        // TODO: Move to VertexBuffer Bind method
        mu_uint8_t* vtxDataBegin = nullptr;
        DXThrowIfFailed(aVertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&vtxDataBegin)));
        memcpy(vtxDataBegin, aVertexBufferData, aDataSize);
        aVertexBuffer->Unmap(0, nullptr);
    }

    void Renderer::UpdateTextureData(MS::ComPtr<ID3D12GraphicsCommandList> aCommandList, MS::ComPtr<ID3D12Resource> aTextureResource, MS::ComPtr<ID3D12Resource> aTextureUploadHeap, void* aTextureData, UINT aPixelSize)
    {
        CD3DX12_RESOURCE_BARRIER texResBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
            aTextureResource.Get(),
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            D3D12_RESOURCE_STATE_COPY_DEST
        );

        aCommandList.Get()->ResourceBarrier(1, &texResBarrier);
        D3D12_RESOURCE_DESC textureDesc = aTextureResource->GetDesc();
        mu_uint64_t width   = textureDesc.Width;
        mu_uint_t height    = textureDesc.Height;

        // Copy data to the intermediate upload heap and then schedule a copy
        // from the upload heap to the Texture2D.
        D3D12_SUBRESOURCE_DATA textureSubResData = {};
        textureSubResData.pData = aTextureData;
        textureSubResData.RowPitch = width * aPixelSize;
        textureSubResData.SlicePitch = textureSubResData.RowPitch * height;
        UpdateSubresources(aCommandList.Get(), aTextureResource.Get(), aTextureUploadHeap.Get(), 0, 0, 1, &textureSubResData);

        CD3DX12_RESOURCE_BARRIER texResBarrier2 = CD3DX12_RESOURCE_BARRIER::Transition(
            aTextureResource.Get(),
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
        );
        aCommandList.Get()->ResourceBarrier(1, &texResBarrier2);
    }


}