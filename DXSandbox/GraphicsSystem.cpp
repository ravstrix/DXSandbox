#include "GraphicsSystem.hpp"

#include "Debug.hpp"
#include "ErrorHandling.hpp"
#include "Window.hpp"

#include <d3d12.h>
#include <dxgi1_6.h>

#include <cassert>

namespace
{
    bool EnableDebugLayer() noexcept
    {
        DXSandbox::ComPtr<ID3D12Debug1> d3dDebug;

        if (FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&d3dDebug))))
            return false;

        d3dDebug->EnableDebugLayer();

        return true;
    }
}

namespace DXSandbox
{
    GraphicsSystem::GraphicsSystem(const Window& window)
    {
        CreateFactory();
        CreateDevice();
        CreateCommandQueue();
        CreateSwapChain(window);
        CreateCommandList();
        CreateFence();
    }

    GraphicsSystem::~GraphicsSystem()
    {
        WaitForPreviousFrame();

        CloseHandle(m_fenceEvent);
    }

    void GraphicsSystem::Render()
    {
        PopulateCommandList();

        ID3D12CommandList* commandLists = m_commandList.Get();
        m_commandQueue->ExecuteCommandLists(1, &commandLists);

        ThrowIfFailed(m_swapChain->Present(1, 0));

        WaitForPreviousFrame();
    }

    void GraphicsSystem::CreateFactory()
    {
        const UINT factoryFlags = EnableDebugLayer() ? DXGI_CREATE_FACTORY_DEBUG : 0;

        ThrowIfFailed(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&m_factory)));
    }

    void GraphicsSystem::CreateDevice()
    {
        assert(m_factory);

        auto nextAdapter = [this, index = 0U]() mutable
        {
            ComPtr<IDXGIAdapter1> adapter;

            HRESULT hr = S_OK;

            do
            {
                hr = m_factory->EnumAdapterByGpuPreference(index++, DXGI_GPU_PREFERENCE_UNSPECIFIED,
                                                           IID_PPV_ARGS(&adapter));
            }
            while (FAILED(hr) && hr != DXGI_ERROR_NOT_FOUND);

            return adapter;
        };

        while (ComPtr<IDXGIAdapter1> adapter = nextAdapter())
        {
            DXGI_ADAPTER_DESC1 desc;

            if (FAILED(adapter->GetDesc1(&desc)))
                continue;
            if (desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)
                continue;
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_1,
                                            IID_PPV_ARGS(&m_device))))
                break;
        }

        if (!m_device)
            ThrowHResultError(DXGI_ERROR_NOT_FOUND);
    }

    void GraphicsSystem::CreateCommandQueue()
    {
        assert(m_device);

        static constexpr D3D12_COMMAND_QUEUE_DESC queueDesc =
        {
            .Type = D3D12_COMMAND_LIST_TYPE_DIRECT
        };

        ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));
    }

    void GraphicsSystem::CreateSwapChain(const Window& window)
    {
        assert(m_factory && m_commandQueue);

        const POINT size = window.ClientSize();

        const DXGI_SWAP_CHAIN_DESC1 swapChainDesc =
        {
            .Width = static_cast<UINT>(size.x),
            .Height = static_cast<UINT>(size.y),
            .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
            .SampleDesc = {.Count = 1},
            .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
            .BufferCount = BackBufferCount,
            .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD
        };

        ComPtr<IDXGISwapChain1> baseSwapChain;

        ThrowIfFailed(m_factory->CreateSwapChainForHwnd(m_commandQueue.Get(), window.Handle(),
                                                        &swapChainDesc, nullptr, nullptr,
                                                        &baseSwapChain));
        ThrowIfFailed(baseSwapChain.As(&m_swapChain));
        ThrowIfFailed(m_factory->MakeWindowAssociation(window.Handle(), DXGI_MWA_NO_ALT_ENTER));

        m_currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

        static constexpr D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc =
        {
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
            .NumDescriptors = BackBufferCount
        };

        ThrowIfFailed(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));

        m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();

        for (UINT i = 0; i < BackBufferCount; ++i)
        {
            ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_backBuffers[i])));
            m_device->CreateRenderTargetView(m_backBuffers[i].Get(), nullptr, rtvHandle);
            rtvHandle.ptr += m_rtvDescriptorSize;
        }
    }

    void GraphicsSystem::CreateCommandList()
    {
        assert(m_device);

        ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                       IID_PPV_ARGS(&m_commandAllocator)));
        ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                  m_commandAllocator.Get(), nullptr,
                                                  IID_PPV_ARGS(&m_commandList)));
        ThrowIfFailed(m_commandList->Close());
    }

    void GraphicsSystem::CreateFence()
    {
        assert(m_device);

        ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));

        m_fenceEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);

        if (!m_fenceEvent)
            ThrowLastError();

        m_fenceValue = 1;
    }

    void GraphicsSystem::PopulateCommandList()
    {
        ThrowIfFailed(m_commandAllocator->Reset());
        ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), m_pipelineState.Get()));

        const D3D12_RESOURCE_BARRIER renderTargetBarrier =
        {
            .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
            .Transition =
            {
                .pResource = m_backBuffers[m_currentBackBufferIndex].Get(),
                .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                .StateBefore = D3D12_RESOURCE_STATE_PRESENT,
                .StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET
            }
        };

        m_commandList->ResourceBarrier(1, &renderTargetBarrier);

        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();

        rtvHandle.ptr += static_cast<SIZE_T>(m_currentBackBufferIndex) * m_rtvDescriptorSize;

        static constexpr float clearColor[] = {0.0f, 0.2f, 0.4f, 1.0f};

        m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

        const D3D12_RESOURCE_BARRIER presentBarrier =
        {
            .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
            .Transition =
            {
                .pResource = m_backBuffers[m_currentBackBufferIndex].Get(),
                .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                .StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET,
                .StateAfter = D3D12_RESOURCE_STATE_PRESENT
            }
        };

        m_commandList->ResourceBarrier(1, &presentBarrier);

        ThrowIfFailed(m_commandList->Close());
    }

    void GraphicsSystem::WaitForPreviousFrame()
    {
        const UINT64 currentFenceValue = m_fenceValue;

        ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), currentFenceValue));

        ++m_fenceValue;

        if (m_fence->GetCompletedValue() < currentFenceValue)
        {
            ThrowIfFailed(m_fence->SetEventOnCompletion(currentFenceValue, m_fenceEvent));
            WaitForSingleObject(m_fenceEvent, INFINITE);
        }

        m_currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
    }
}
