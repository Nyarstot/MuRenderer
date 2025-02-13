#include "pch.hpp"
#include "MuRenderer/Device/DevicePair.hpp"
#include "MuRenderer/Debug/DXException.hpp"
#include <iostream>


namespace murenderer
{
    DevicePair::DevicePair() : m_devices(nullptr, nullptr)
    {
    }

    DevicePair::DevicePair(Device* aPrimaryDevice, Device* aSecondaryDevice)
    {
        assert(aPrimaryDevice->GetPriorityType() == DevicePriority_Primary);
        assert(aSecondaryDevice->GetPriorityType() == DevicePriority_Secondary);
        m_devices.first = aPrimaryDevice;
        m_devices.second = aSecondaryDevice;
    }

    DevicePair::~DevicePair()
    {
    }

    void DevicePair::CreateDevices(bool aForceSingleGPU)
    {
        assert(m_devices.first == nullptr);
        assert(m_devices.second == nullptr);

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
            m_devices.first = primaryDevice;
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

            m_devices.first = primaryDevice;
            m_devices.second = secondaryDevice;
        }

        //TODO: Make logger
        std::wcout << "Created Primary Device: " << usedAdapterDescPrimary.Description << std::endl;
        std::wcout << "Created Secondary Device: " << usedAdapterDescSecondary.Description << std::endl;
    }

    Device* DevicePair::GetPrimaryDevice() const
    {
        return m_devices.first;
    }

    Device* DevicePair::GetSecondaryDevice() const
    {
        return m_devices.second;
    }
}