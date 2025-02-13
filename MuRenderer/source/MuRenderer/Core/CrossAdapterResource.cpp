#include "pch.hpp"
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

        Device* primaryDevice = m_devicePair->GetPrimaryDevice();
        Device* secondaryDevice = m_devicePair->GetSecondaryDevice();

        for (mu_size_t i = 0; i < RenderProperties_NumRenderTargets; i++) {
            D3D12_HEAP_PROPERTIES heapProps = {};
            heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
            heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
            heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
            heapProps.CreationNodeMask = 1;
            heapProps.VisibleNodeMask = 1;

            DXThrowIfFailed(primaryDevice->GetDXDevice()->CreateCommittedResource(
                &heapProps,
                D3D12_HEAP_FLAG_SHARED | D3D12_HEAP_FLAG_SHARED_CROSS_ADAPTER,
                &crossAdapterResDesc,
                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
                nullptr,
                IID_PPV_ARGS(&m_shaderResources[i]))
            );

            HANDLE heapHandle = nullptr;
            DXThrowIfFailed(primaryDevice->GetDXDevice()->CreateSharedHandle(
                m_shaderResources[i].Get(),
                nullptr,
                GENERIC_ALL,
                nullptr,
                &heapHandle
            ));

            // Open shared handle on a secondary device
            DXThrowIfFailed(secondaryDevice->GetDXDevice()->OpenSharedHandle(&heapHandle, IID_PPV_ARGS(&m_shaderResourceViews[i])));
            CloseHandle(heapHandle);
        }

        // Create shader resource view (SRV) for each of the cross adapter resources
        CD3DX12_CPU_DESCRIPTOR_HANDLE secondarySRVHandle(secondaryDevice->GetCbvSrvUavHeap()->GetCPUDescriptorHandleForHeapStart());
        for (mu_size_t i = 0; i < RenderProperties_NumRenderTargets; i++) {
            secondaryDevice->GetDXDevice()->CreateShaderResourceView(m_shaderResourceViews[i].Get(), nullptr, secondarySRVHandle);
            secondarySRVHandle.Offset(secondaryDevice->GetCbvSrvUavDescriptorSize());
        }

        // Create fence for cross-adapter resource
        DXThrowIfFailed(primaryDevice->GetDXDevice()->CreateFence(
            m_currentFenceValue,
            D3D12_FENCE_FLAG_SHARED | D3D12_FENCE_FLAG_SHARED_CROSS_ADAPTER,
            IID_PPV_ARGS(&m_dxFences[DevicePriority_Primary])
        ));

        // Create shared handle for cross-adapter fence
        HANDLE fenceHandle = nullptr;
        DXThrowIfFailed(primaryDevice->GetDXDevice()->CreateSharedHandle(
            m_dxFences[DevicePriority_Primary].Get(),
            nullptr, GENERIC_ALL, nullptr, &fenceHandle
        ));

        // Open shared handle to fence on secondary device GPU
        DXThrowIfFailed(secondaryDevice->GetDXDevice()->OpenSharedHandle(fenceHandle, IID_PPV_ARGS(&m_dxFences[DevicePriority_Secondary])));
    }

    void CrossAdapterResource::CreateCommandList()
    {
        // Create copy command list on primary device
        Device* primaryDevice = m_devicePair->GetPrimaryDevice();
        DXThrowIfFailed(primaryDevice->GetDXDevice()->CreateCommandList(
            0, D3D12_COMMAND_LIST_TYPE_DIRECT,
            m_copyCommandAllocator.Get(), nullptr,
            IID_PPV_ARGS(&m_copyGraphicsCommandList)
        ));

        // Release the copy allocator
        DXThrowIfFailed(m_copyGraphicsCommandList->Close());
    }

    void CrossAdapterResource::PopulateCommandList(int aCurrentFrameIndex)
    {
        // Create the copy command list for the primaryDevice adapter
        DXThrowIfFailed(m_copyCommandAllocator->Reset());
        DXThrowIfFailed(m_copyGraphicsCommandList->Reset(m_copyCommandAllocator.Get(), nullptr));

        // Transition cross adapter resources to be a copy destination

        CD3DX12_RESOURCE_BARRIER copyDestResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
            m_shaderResources[aCurrentFrameIndex].Get(),
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            D3D12_RESOURCE_STATE_COPY_DEST
        );

        m_copyGraphicsCommandList->ResourceBarrier(1, &copyDestResourceBarrier);

        // Copy from primaryDevice render targets into cross adapter resources
        Device* primaryDevice = m_devicePair->GetPrimaryDevice();
        m_copyGraphicsCommandList->CopyResource(m_shaderResources[aCurrentFrameIndex].Get(), primaryDevice->m_renderTargets[aCurrentFrameIndex].Get());

        // Transition cross adapter resources back to being a pixel shader resource

        CD3DX12_RESOURCE_BARRIER pixelShaderResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
            m_shaderResources[aCurrentFrameIndex].Get(),
            D3D12_RESOURCE_STATE_COPY_DEST,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
        );

        m_copyGraphicsCommandList->ResourceBarrier(1, &pixelShaderResourceBarrier);

        DXThrowIfFailed(m_copyGraphicsCommandList->Close());
    }

    void CrossAdapterResource::SetupFences()
    {
        // Setup fence for Primary GPU and wait for setup to finish
        {
            m_fenceEvents[DevicePriority_Primary] = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
            if (m_fenceEvents[DevicePriority_Primary] == nullptr) {
                DXThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
            }

            // Schedule a signal in the command queue
            Device* primaryDevice = m_devicePair->GetPrimaryDevice();
            DXThrowIfFailed(primaryDevice->GetCommandQueue()->Signal(m_dxFences[DevicePriority_Primary].Get(), m_currentFenceValue));
            m_fenceValues[0] = m_currentFenceValue;
            m_currentFenceValue++;

            // Wait until fence has been processed
            DXThrowIfFailed(m_dxFences[DevicePriority_Primary]->SetEventOnCompletion(m_fenceValues[0], m_fenceEvents[DevicePriority_Primary]));
            WaitForSingleObjectEx(m_fenceEvents[DevicePriority_Primary], INFINITE, FALSE);
        }

        // Setup fence for Secondary GPU and wait for setup to finish
        {
            m_fenceEvents[DevicePriority_Secondary] = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
            if (m_fenceEvents[DevicePriority_Secondary] == nullptr)
            {
                DXThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
            }
            // Schedule a signal in the command queue
            Device* secondaryDevice = m_devicePair->GetSecondaryDevice();
            DXThrowIfFailed(secondaryDevice->GetCommandQueue()->Signal(m_dxFences[DevicePriority_Secondary].Get(), m_currentFenceValue));
            m_fenceValues[0] = m_currentFenceValue;
            m_currentFenceValue++;

            // Wait until fence has been processed
            DXThrowIfFailed(m_dxFences[DevicePriority_Secondary]->SetEventOnCompletion(m_fenceValues[0], m_fenceEvents[DevicePriority_Secondary]));
            WaitForSingleObjectEx(m_fenceEvents[DevicePriority_Secondary], INFINITE, FALSE);
        }
    }
}