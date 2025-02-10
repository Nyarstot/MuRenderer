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
        #ifdef MU_DEBUG
            ID3D12Debug* debugController;
            if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
                debugController->EnableDebugLayer();
                debugController->Release();
            }
        #endif // MU_DEBUG

        // Create global DXGI Factory for enum adapters
        MS::ComPtr<IDXGIFactory4> dxgiFactory;
        DXThrowIfFailed(CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgiFactory)));

        // Enumerate the primary adapter
        IDXGIAdapter* primaryAdapter;
        DXGI_ADAPTER_DESC primaryAdapterDesc;

        DXThrowIfFailed(dxgiFactory->EnumAdapters(DevicePriority_Primary, &primaryAdapter));
        primaryAdapter->GetDesc(&primaryAdapterDesc);

        // Enumerate the secondary adapter - if only one GPU defaults to WARP
        IDXGIAdapter* secondaryAdapter;
        DXGI_ADAPTER_DESC secondaryAdapterDesc;
        HRESULT hres = dxgiFactory->EnumAdapters(DevicePriority_Secondary, &secondaryAdapter);

        DXGI_ADAPTER_DESC usedAdapterDescPrimary;
        DXGI_ADAPTER_DESC usedAdapterDescSecondary;

        // Create two devices
        Device* primaryDevice = nullptr;
        Device* secondaryDevice = nullptr;

        if (FAILED(hres)) {
            primaryDevice = new Device(primaryAdapter, DevicePriority_Primary);
            m_devicePool.AppendDevice(primaryDevice);
        }
        else
        {
            secondaryAdapter->GetDesc(&secondaryAdapterDesc);

            if (secondaryAdapterDesc.VendorId == 4318) {
                primaryDevice = new Device(secondaryAdapter, DevicePriority_Primary);
                secondaryDevice = new Device(primaryAdapter, DevicePriority_Secondary);
                usedAdapterDescPrimary = secondaryAdapterDesc;
                usedAdapterDescSecondary = primaryAdapterDesc;
            }
            else
            {
                primaryDevice = new Device(primaryAdapter, DevicePriority_Primary);
                secondaryDevice = new Device(secondaryAdapter, DevicePriority_Secondary);
                usedAdapterDescPrimary = primaryAdapterDesc;
                usedAdapterDescSecondary = secondaryAdapterDesc;
            }

            m_devicePool.AppendDevice(primaryDevice);
            m_devicePool.AppendDevice(secondaryDevice);
        }
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

    void Renderer::UpdateConstantBuffer(MS::ComPtr<ID3D12Resource> aConstantBuffer, ConstantBuffer* aConstantData, const UINT& aDataSize)
    {
    }

    void Renderer::UpdateVertexBuffer(MS::ComPtr<ID3D12Resource> aVertexBuffer, VertexBuffer* aVertexBufferData, const UINT& aDataSize)
    {
    }


}