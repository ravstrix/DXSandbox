#pragma once

#include "WindowsPlatform.hpp"

#include "ComPtr.hpp"

#include <array>

interface IDXGIFactory6;
interface ID3D12Device;
interface ID3D12CommandQueue;
interface IDXGISwapChain3;
interface ID3D12Fence;
interface ID3D12DescriptorHeap;
interface ID3D12CommandAllocator;
interface ID3D12GraphicsCommandList;
interface ID3D12PipelineState;
interface ID3D12Resource;

namespace DXSandbox
{
    class Window;

    class GraphicsSystem final
    {
    public:
        struct InitParams final
        {
            HWND hWnd = nullptr;
            UINT width = 0;
            UINT height = 0;

            bool enableDebugLayer = false;
        };

        explicit GraphicsSystem(const InitParams& params);
        ~GraphicsSystem();

        void Render();

    private:
        void CreateFactory(bool enableDebug);
        void CreateDevice();
        void CreateCommandQueue();
        void CreateSwapChain(const InitParams& params);
        void CreateCommandList();
        void CreateFence();

        void PopulateCommandList();
        void WaitForPreviousFrame();

    private:
        ComPtr<IDXGIFactory6> m_factory;
        ComPtr<ID3D12Device> m_device;
        ComPtr<ID3D12CommandQueue> m_commandQueue;
        ComPtr<IDXGISwapChain3> m_swapChain;
        ComPtr<ID3D12Fence> m_fence;
        ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
        ComPtr<ID3D12CommandAllocator> m_commandAllocator;
        ComPtr<ID3D12GraphicsCommandList> m_commandList;
        ComPtr<ID3D12PipelineState> m_pipelineState;

        HANDLE m_fenceEvent = nullptr;
        UINT64 m_fenceValue = 0;

        UINT m_rtvDescriptorSize = 0;

        static constexpr UINT BackBufferCount = 2;

        std::array<ComPtr<ID3D12Resource>, BackBufferCount> m_backBuffers;

        UINT m_currentBackBufferIndex = 0;
    };
}
