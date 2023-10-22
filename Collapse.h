#pragma once
#include "ParticleBox.h"
class Collapse
{
public:
	static const uint32_t kParticleNum = 120;
	Collapse();
	void Initialize();
	void Update();
	void Draw(ViewProjection* viewProjection, DirectionalLight* directionalLight, Vector4 color = { 1.0f,1.0f,1.0f,1.0f });
	
	void SetIsEmit(bool isEmit) {
		isEmit_ = isEmit;
	}
	
	struct dustParticle {
		WorldTransform worldTransform_;
		float velocity_;
		bool isActive_;
	};
public:
	dustParticle particles[kParticleNum];
	int EmitNum_ = 10;
private:
	float accelaration_ = -0.047f;
	bool isEmit_ = false;
	std::unique_ptr<ParticleBox> particleBox_;

	float emitX = 180.0f;
	float emitHeight = 180.0f;
	float disapperHeigth = -15.0f;
	float offsetZ = 30.0f;
	Vector2 randScale = { 3.0f,5.0f };
};

