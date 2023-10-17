#pragma once
#include <Windows.h>
#include <cstdlib>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <vector>
#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")
#include "externals/DirectXTex/d3dx12.h"
#include "WinApp.h"

class DirectXCommon
{
public:

	//うんち設計
	uint32_t kSrvHeapDescritorNum = 1024;

	static DirectXCommon* GetInstance();
	void Initialize(int32_t backBufferWidth = 1280, int32_t backBufferHeight = 720);
	
	void PreDraw();
	void PostDraw();
	void ClearRenderTarget();
	void ClearDepthBuffer();

	Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(const std::wstring& filePath,const wchar_t* profile);


	ID3D12Device* GetDevice() { return device_.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() { return commandList_.Get(); }
	size_t GetBackBufferCount() const { return backBuffers_.size(); }
	ID3D12DescriptorHeap* GetSRVHeap() { return srvHeap_.Get(); }
	uint32_t GetSrvHeapCount() { return srvHeapCount_; }
	void IncrementSrvHeapCount() { srvHeapCount_++; }
private:
	WinApp* winApp_;

	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
	UINT64 fenceVal_ = 0;
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_;
	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_;
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers_;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer_;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap_;
	
	int32_t backBufferWidth_ = 0;
	int32_t backBufferHeight_ = 0;

	int32_t srvHeapCount_ = 0;
private:
	void InitializeDXGIDevice();
	void InitializeCommand();
	void CreateSwapChain();
	void CreateDirectXCompilier();
	void CreateFinalRenderTargets();
	void CreateDepthBuffer();
	void CreateSrvHeap();
	void CreateFence();
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource();
};

