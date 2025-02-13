#ifndef MURENDERER_DEVICE_DEVICEPAIR_HPP
#define MURENDERER_DEVICE_DEVICEPAIR_HPP

#include "MuRenderer/Device/Device.hpp"
#include "MuRenderer/Device/IDeviceContainer.hpp"

namespace murenderer
{
    class DevicePair final : public IDeviceContainer
    {
        using AdapterPair       = std::pair<Device*, Device*>;
        using CommandListPair   = std::pair<MS::ComPtr<ID3D12GraphicsCommandList>, MS::ComPtr<ID3D12GraphicsCommandList>>;
        using PipelineStatePair = std::pair<MS::ComPtr<ID3D12PipelineState>, MS::ComPtr<ID3D12PipelineState>>;
        using RootSignaturePair = std::pair<MS::ComPtr<ID3D12RootSignature>, MS::ComPtr<ID3D12RootSignature>>;
        using BufferViewPair    = std::pair<D3D12_VERTEX_BUFFER_VIEW*, D3D12_VERTEX_BUFFER_VIEW>;

    private:
        AdapterPair m_devices;
        CommandListPair m_commandLists;
        PipelineStatePair m_pipelineStates;
        RootSignaturePair m_rootSignatures;
        BufferViewPair m_bufferViews;

    public:
        DevicePair();
        DevicePair(Device* aPrimaryDevice, Device* aSecondaryDevice);
        ~DevicePair();

        void CreateDevices(bool aForceSingleGPU=false);

        Device* GetPrimaryDevice() const;
        Device* GetSecondaryDevice() const;

    };
}

#endif // !MURENDERER_DEVICE_DEVICEPAIR_HPP
