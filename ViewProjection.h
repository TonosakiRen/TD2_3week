#pragma once
#include <DirectXMath.h>
#include <d3d12.h>
#include <wrl.h>
#include "Mymath.h"
#include <numbers>

struct ConstBufferDataViewProjection {
	Matrix4x4 view;			
	Matrix4x4 projection;	
	Vector3 viewPosition;
};

class ViewProjection {
public:
	void Initialize();
	void DebugMove();
	void UpdateMatrix();

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const {
		return constBuff_->GetGPUVirtualAddress();
	}
public:
	Vector3 translation_ = { 0, 0, -10.0f };
	Vector3 target_ = { 0, 0, 0 };
private:
	void CreateConstBuffer();
	void Map();
private:
	float fovAngleY = 45.0f * std::numbers::pi_v <float> / 180.0f;
	float aspectRatio = (float)16 / (float)9;
	float nearZ = 0.1f;
	float farZ = 1000.0f;

	Matrix4x4 matView;
	Matrix4x4 matProjection;

	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;
	ConstBufferDataViewProjection* constMap = nullptr;
};


