#pragma once
#include "ParticleBox.h"
class OrbitParticle
{
public:
	static const uint32_t kParticleNum = 100;
	OrbitParticle();
	void Initialize();
	void Update();
	void Draw(ViewProjection* viewProjection, DirectionalLight* directionalLight, Vector4 color = { 1.0f,1.0f,1.0f,1.0f });
	
	void SetIsEmit(bool isEmit) {
		isEmit_ = isEmit;
	}
	void SetSpeed(float speed) {
		speed_ = speed;
	}
	void SetScaleSpeed(float scaleSpeed) {
		scaleSpeed_ = scaleSpeed;
	}
	struct orbitParticle {
		WorldTransform worldTransform_;
		Vector3 direction_;
		Vector3 velocity_;
		bool isActive_;
	};
public:
	WorldTransform emitterWorldTransform_;
	orbitParticle particles[kParticleNum];
	int EmitNum_ = 1;
private:
	float speed_ = 0.1f;
	float scaleSpeed_ = 0.01f;
	bool isEmit_ = true;
	uint32_t emitAllNum_ = 0;
	std::unique_ptr<ParticleBox> particleBox_;

	
};

