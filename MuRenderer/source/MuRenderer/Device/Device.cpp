#include "pch.hpp"
#include "MuRenderer/Device/Device.hpp"
#include "MuRenderer/Debug/DXException.hpp"


namespace murenderer
{
    Device::Device(IDXGIAdapter* aDXGIAdapter, DevicePriority aDevicePrio) : m_devicePrio(aDevicePrio)
    {
        // Create device

        HRESULT hres = D3D12CreateDevice(aDXGIAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_dxDevice));
        if (!SUCCEEDED(hres)) {
            // Try use the default device
            DXThrowIfFailed(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_dxDevice)));
        }

        // Create command queue

        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type  = D3D12_COMMAND_LIST_TYPE_DIRECT;
        DXThrowIfFailed(m_dxDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_dxCommandQueue)));

        // Create command allocator

        DXThrowIfFailed(m_dxDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_dxCommandAllocator)));

        // Create a fence for the command queue

        DXThrowIfFailed(m_dxDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_dxFence)));
        m_currentFenceValue = 1;
        m_fenceEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
    }

    Device::~Device()
    {
        CloseHandle(m_fenceEvent);
    }

    DevicePriority const Device::GetPriorityType() const
    {
        return m_devicePrio;
    }

    ID3D12Device* Device::GetDXDevice() const
    {
        return m_dxDevice.Get();
    }

    ID3D12CommandQueue* Device::GetCommandQueue() const
    {
        return m_dxCommandQueue.Get();
    }

    ID3D12DescriptorHeap* Device::GetRTVHeap() const
    {
        return m_dxRtvDescriptorHeap.Get();
    }

    ID3D12DescriptorHeap* Device::GetCbvSrvUavHeap() const
    {
        return m_dxCbvSrvUavHeap.Get();
    }

    mu_uint_t Device::GetCbvSrvUavDescriptorSize() const
    {
        return m_cbvSrvUavDescriptorSize;
    }

    mu_uint_t Device::GetRTVDescriptorSize() const
    {
        return m_rtvDescriptorSize;
    }

    void Device::SetRTVDescriptorSize(mu_uint_t aRTVDescSize)
    {
        m_rtvDescriptorSize = aRTVDescSize;
    }

    void Device::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_DESC aRtvHeapDesc, D3D12_DESCRIPTOR_HEAP_DESC aCbvSrvUavHeapDsc)
    {
        // Create the RTV descriptor heap
        aRtvHeapDesc.Type   = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        aRtvHeapDesc.Flags  = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        DXThrowIfFailed(m_dxDevice->CreateDescriptorHeap(&aRtvHeapDesc, IID_PPV_ARGS(&m_dxRtvDescriptorHeap)));

        // Set RTV descriptor size
        aCbvSrvUavHeapDsc.Type  = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        aCbvSrvUavHeapDsc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        DXThrowIfFailed(m_dxDevice->CreateDescriptorHeap(&aCbvSrvUavHeapDsc, IID_PPV_ARGS(&m_dxCbvSrvUavHeap)));

        // Set CBV / SRV/ UAV descriptor size
        m_cbvSrvUavDescriptorSize = m_dxDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }
}