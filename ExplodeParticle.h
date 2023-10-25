#pragma once
#include "ParticleBox.h"
class ExplodeParticle
{
public:
	static const uint32_t kParticleNum = 100;
	const int32_t kEmitFrame_ = 10;
	ExplodeParticle();
	void Initialize();
	void Initialize(Vector3 minDirection,Vector3 maxDirection);
	void Update();
	void Draw(ViewProjection* viewProjection, DirectionalLight* directionalLight, Vector4 color = { 1.0f,1.0f,1.0f,1.0f });
	void SetDirection(Vector3 minDirection, Vector3 maxDirection) {
		minDirection_ = minDirection;
		maxDirection_ = maxDirection;
	}
	void SetIsEmit(bool isEmit) {
		isEmit_ = isEmit;
	}
	void SetSpeed(float speed) {
		speed_ = speed;
	}
	void SetScaleSpeed(float scaleSpeed) {
		scaleSpeed_ = scaleSpeed;
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
	int EmitNum_ = 5;
	float scaleSpeed_ = 0.05f;
private:
	float speed_ = 0.8f;
	bool isEmit_ = false;
	int32_t emitFrame_ = 0;
	std::unique_ptr<ParticleBox> particleBox_;
	Vector3 minDirection_;
	Vector3 maxDirection_;
};

