#ifndef MURENDERER_DEVICE_DEVICEPOOL_HPP
#define MURENDERER_DEVICE_DEVICEPOOL_HPP

#include "MuRenderer/SystemTypes.hpp"
#include "MuRenderer/Utils/MuD3D.hpp"
#include "MuRenderer/Device/Device.hpp"

#include <vector>


namespace murenderer
{
    class DevicePool final
    {
    private:
        std::vector<Device*> m_devices;
        std::vector<MS::ComPtr<ID3D12GraphicsCommandList>> m_commandLists;
        std::vector<MS::ComPtr<ID3D12PipelineState>> m_pipelineStates;
        std::vector<MS::ComPtr<ID3D12RootSignature>> m_rootSignatures;
        //std::vector<MS::ComPtr<D3D12_VERTEX_BUFFER_VIEW>> m_vtxBufferViews;
        size_t m_primaryDeviceIndex;

    public:
        DevicePool();
        ~DevicePool();

        void AppendDevice(Device* aDevice);
        void RemoveDevice(Device* aDevice);

        std::size_t DeviceCount() const;
        Device* GetPrimaryDevice() const;

    };
}

#endif // !MURENDERER_DEVICE_DEVICEPOOL_HPP
