#include "MuRenderer/Core/CrossAdapterResource.hpp"
#include "MuRenderer/Debug/DXException.hpp"


namespace murenderer
{
    CrossAdapterResource::CrossAdapterResource(DevicePair* aDevicePair)
    {
        // Create copy command allocator on primary device
        ID3D12Device* primaryDevice = m_devicePair->GetPrimaryDevice()->GetDXDevice();
        DXThrowIfFailed(primaryDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_copyCommandAllocator)));

        // Create copy command queue on primary device
        D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
        commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        DXThrowIfFailed(primaryDevice->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&m_copyCommandQueue)));
    }

    CrossAdapterResource::~CrossAdapterResource()
    {
    }

    void CrossAdapterResource::CreateResource()
    {
        // Describe cross-adapter shared resources on primaryDevice adapter
        D3D12_RESOURCE_DESC crossAdapterResDesc = {};
        crossAdapterResDesc.Flags   = D3D12_RESOURCE_FLAG_ALLOW_CROSS_ADAPTER;
        crossAdapterResDesc.Layout  = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        for (mu_size_t i = 0; i < RenderProperties_NumRenderTargets; i++) {

            ID3D12Device* primaryDevice = m_devicePair->GetPrimaryDevice()->GetDXDevice();
            ID3D12Device* secondaryDevice = m_devicePair->GetSecondaryDevice()->GetDXDevice();

            DXThrowIfFailed(primaryDevice->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                D3D12_HEAP_FLAG_SHARED | D3D12_HEAP_FLAG_SHARED_CROSS_ADAPTER,
                &crossAdapterResDesc,
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
                nullptr,
                IID_PPV_ARGS(&m_shaderResources[i]))
            );

            HANDLE heapHandle = nullptr;
            DXThrowIfFailed(primaryDevice->CreateSharedHandle(
                m_shaderResources[i].Get(),
                nullptr,
                GENERIC_ALL,
                nullptr,
                &heapHandle
            ));

            // Open shared handle on a secondary device
            DXThrowIfFailed(secondaryDevice->OpenSharedHandle(&heapHandle, IID_PPV_ARGS(&m_shaderResourceViews[i])));
            CloseHandle(heapHandle);
        }
    }

    void CrossAdapterResource::CreateCommandList()
    {
    }

    void CrossAdapterResource::PopulateCommandList(int aCurrentFrameIndex)
    {
    }

    void CrossAdapterResource::SetupFences()
    {
    }
}