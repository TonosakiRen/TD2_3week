#pragma once
#include "ParticleBox.h"
class DustParticle
{
public:
	static const uint32_t kParticleNum = 10240;
	DustParticle();
	void Initialize(Vector3 minDirection, Vector3 maxDirection);
	void Update();
	void Draw(ViewProjection* viewProjection, DirectionalLight* directionalLight, Vector4 color = { 1.0f,1.0f,1.0f,1.0f });
	void SetDirection(Vector3 minDirection, Vector3 maxDirection) {
		minDirection_ = minDirection;
		maxDirection_ = maxDirection;
	}
	void SetInitialScale(Vector3 scale) {
		initialScale_ = scale;
	}
	void SetIsEmit(bool isEmit) {
		isEmit_ = isEmit;
	}
	struct dustParticle {
		WorldTransform worldTransform_;
		Vector3 direction_;
		Vector3 velocity_;
		bool isActive_;
	};
public:
	WorldTransform emitterWorldTransform_;
	dustParticle particles[kParticleNum];
	int EmitNum_ = 1;
private:
	bool isEmit_ = false;
	std::unique_ptr<ParticleBox> particleBox_;
	Vector3 minDirection_;
	Vector3 maxDirection_;
	Vector3 initialScale_ = { 0.4f,0.4f,0.4f };
};

