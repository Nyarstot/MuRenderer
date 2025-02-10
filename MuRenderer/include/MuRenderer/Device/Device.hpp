#ifndef MURENDERER_DEVICE_HPP
#define MURENDERER_DEVICE_HPP

#include <array>

#include "MuRenderer/Core/RenderProperties.hpp"
#include "MuRenderer/System/SystemTypes.hpp"
#include "MuRenderer/Utils/MuD3D.hpp"

namespace murenderer
{
    enum DevicePriority : std::uint8_t
    {
        DevicePriority_Primary,
        DevicePriority_Secondary,
        DevicePriority_Count
    };

    class Device final
    {
    private:
        MS::ComPtr<ID3D12Device> m_dxDevice;
        MS::ComPtr<ID3D12CommandQueue> m_dxCommandQueue;
        MS::ComPtr<ID3D12CommandAllocator> m_dxCommandAllocator;

        MS::ComPtr<ID3D12Fence> m_dxFence;
        std::array<UINT64, RenderProperties_NumRenderTargets> m_fenceValues;
        UINT64 m_currentFenceValue;
        HANDLE m_fenceEvent;

        MS::ComPtr<ID3D12DescriptorHeap> m_dxRtvDescriptorHeap;
        MS::ComPtr<ID3D12DescriptorHeap> m_dxCbvSrvUavHeap;
        UINT m_rtvDescriptorSize;
        UINT m_cbvSrvUavHeapSize;

        DevicePriority m_devicePrio;

    public:
        std::array<MS::ComPtr<ID3D12Resource>, RenderProperties_NumRenderTargets> m_renderTargets;

    public:
        Device(IDXGIAdapter* aDXGIAdapter, DevicePriority aDevicePrio);
        ~Device();

        ID3D12Device* GetDXDevice() const;
        ID3D12CommandQueue* GetCommandQueue() const;
        ID3D12DescriptorHeap* GetRTVHeap() const;

        mu_uint_t GetRTVDescriptorSize() const;
        void SetRTVDescriptorSize(mu_uint_t aRTVDescSize);

        DevicePriority const GetPriorityType() const;
        void CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_DESC aRtvHeapDesc, D3D12_DESCRIPTOR_HEAP_DESC aCbvSrvUavHeapDsc);

    };
}

#endif // !MURENDERER_DEVICE_HPP
