#pragma once
#pragma once
#include "TextureManager.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <Windows.h>
#include <d3d12.h>
#include "externals/DirectXTex/d3dx12.h"
#include <vector>
#include <wrl.h>
#include "Mymath.h"
#include "DirectionalLight.h"
#include "Material.h"

class DirectXCommon;

class ParticleBox
{
public:

	const uint32_t kParticleBoxNum;

	enum class RootParameter {
		kWorldTransform,
		kViewProjection,
		kTexture,
		kMaterial,
		kDirectionalLight,
		parameterNum
	};

	struct VertexData {
		Vector3 pos;
		Vector3 normal;
		Vector2 uv;
	};

	struct InstancingBufferData {
		Matrix4x4 matWorld;
	};

	static void StaticInitialize();
	static void PreDraw(ID3D12GraphicsCommandList* commandList);
	static void PostDraw();
	static ParticleBox* Create(uint32_t particleNum);

	ParticleBox(uint32_t particleNum);

	void Initialize();
	void Draw(const std::vector<InstancingBufferData>& bufferData, const ViewProjection& viewProjection, const DirectionalLight& directionalLight, const Vector4& color = { 1.0f,1.0f,1.0f,1.0f }, const uint32_t textureHadle = 0);
	void CreateMesh();

public:
	std::vector<InstancingBufferData> particleDatas_;
	uint32_t instanceNum_ = 0;
	Material material_;
private:
	static void InitializeGraphicsPipeline();
private:
	static DirectXCommon* sDirectXCommon;
	static UINT sDescriptorHandleIncrementSize;
	static ID3D12GraphicsCommandList* sCommandList;
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> sRootSignature;
	static Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState;

	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	D3D12_INDEX_BUFFER_VIEW ibView_{};
	D3D12_CPU_DESCRIPTOR_HANDLE particleDataSRVHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE particleDataSRVHandleGPU_;
	std::vector<VertexData> vertices_;
	std::vector<uint16_t> indices_;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuff_;
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingBuff_;
	InstancingBufferData* instanceMap = nullptr;
};

