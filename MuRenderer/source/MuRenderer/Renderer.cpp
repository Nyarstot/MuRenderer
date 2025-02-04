#include "MuRenderer/Renderer.hpp"
#include "MuRenderer/Debug/DXException.hpp"
#include "MuRenderer/Device/Device.hpp"



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

    Renderer::~Renderer()
    {
    }

    void Renderer::UpdateConstantBuffer(MS::ComPtr<ID3D12Resource> aConstantBuffer, ConstantBuffer* aConstantData, const UINT& aDataSize)
    {
    }
    void Renderer::UpdateVertexBuffer(MS::ComPtr<ID3D12Resource> aVertexBuffer, VertexBuffer* aVertexBufferData, const UINT& aDataSize)
    {
    }
}