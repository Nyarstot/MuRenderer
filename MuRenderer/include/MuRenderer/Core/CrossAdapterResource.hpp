#ifndef MURENDERER_CORE_CROSSADAPTERRESOURCE_HPP
#define MURENDERER_CORE_CROSSADAPTERRESOURCE_HPP

#include "MuRenderer/System/SystemTypes.hpp"
#include "MuRenderer/Core/RenderProperties.hpp"
#include "MuRenderer/Utils/MuD3D.hpp"

#include "MuRenderer/Device/Device.hpp"
#include "MuRenderer/Device/DevicePair.hpp"

#include <array>


namespace murenderer
{
    class CrossAdapterResource final
    {
    private:
        SharedPtr<DevicePair> m_devicePair;
        std::array<MS::ComPtr<ID3D12Resource>, CrossAdapterProperties_NumDevices> m_shaderResources;
        std::array<MS::ComPtr<ID3D12Resource>, CrossAdapterProperties_NumDevices> m_shaderResourceViews;

    protected:
        // Command list and allocator for copy operation on primary GPU
        MS::ComPtr<ID3D12CommandAllocator> m_copyCommandAllocator;
        MS::ComPtr<ID3D12CommandQueue> m_copyCommandQueue;
        MS::ComPtr<ID3D12GraphicsCommandList> m_copyGraphicsCommandList;

        // Cross-adapter synchronization
        std::array<MS::ComPtr<ID3D12Fence>, CrossAdapterProperties_NumDevices> m_dxFences;
        std::array<HANDLE, CrossAdapterProperties_NumDevices> m_fenceEvents;
        std::array<mu_uint64_t, CrossAdapterProperties_NumDevices> m_fenceValues;
        mu_uint64_t m_currentFenceValue;

    public:
        CrossAdapterResource(DevicePair* aDevicePair);
        ~CrossAdapterResource();

        void CreateResource();
        void CreateCommandList();
        void PopulateCommandList(int aCurrentFrameIndex);
        void SetupFences();

    };
}

#endif // !MURENDERER_CORE_CROSSADAPTERRESOURCE_HPP
