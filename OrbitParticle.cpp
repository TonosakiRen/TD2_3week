#include "OrbitParticle.h"
#include "ImGuiManager.h"

OrbitParticle::OrbitParticle()
{
	particleBox_ = std::make_unique<ParticleBox>(kParticleNum);
}

void OrbitParticle::Initialize()
{
	particleBox_->Initialize();
	emitterWorldTransform_.SetIsScaleParent(false);
	emitterWorldTransform_.SetIsRotateParent(false);
}

void OrbitParticle::Update() {

	emitterWorldTransform_.UpdateMatrix();

	if (isEmit_) {
		for (size_t i = 0; i < EmitNum_; i++) {
			for (size_t i = 0; i < kParticleNum; i++) {
				if (particles[i].isActive_ == false) {
					particles[i].isActive_ = true;
					Vector3 emitPosDiff = { 0.0f,0.0f,0.5f };
					Matrix4x4 rotationMatrix = MakeRotateXYZMatrix({emitAllNum_ * Radian(36.0f)});
					particles[i].direction_ = Normalize(emitPosDiff * rotationMatrix);
					particles[i].worldTransform_.translation_ = MakeTranslation(emitterWorldTransform_.matWorld_) ;
					particles[i].worldTransform_.rotation_ = { 0.0f,0.0f,0.0f };
					particles[i].worldTransform_.scale_ = emitterWorldTransform_.scale_;
					emitAllNum_++;
					break;
				}
			}
		}
	}

	for (size_t i = 0; i < kParticleNum; i++) {
		float rotationSpeed = Radian(1.0f) * (float(i % 2) * 2.0f - 1.0f);
		if (particles[i].isActive_ == true) {
			particles[i].worldTransform_.rotation_ = particles[i].worldTransform_.rotation_ + rotationSpeed;
			particles[i].worldTransform_.scale_ = particles[i].worldTransform_.scale_ - scaleSpeed_;
			particles[i].worldTransform_.translation_ += particles[i].direction_ * speed_;
			if (particles[i].worldTransform_.scale_.x <= 0.0f) {
				particles[i].isActive_ = false;
			}
		}

	}

}

void OrbitParticle::Draw(ViewProjection* viewProjection, DirectionalLight* directionalLight, Vector4 color)
{

	emitterWorldTransform_.UpdateMatrix();

	std::vector<ParticleBox::InstancingBufferData> instancingBufferDatas;
	instancingBufferDatas.reserve(kParticleNum);

	for (size_t i = 0; i < kParticleNum; i++)
	{
		if (particles[i].isActive_) {
			particles[i].worldTransform_.UpdateMatrix();
			instancingBufferDatas.emplace_back(particles[i].worldTransform_.matWorld_);
		}
	}

	if (!instancingBufferDatas.empty()) {
		particleBox_->Draw(instancingBufferDatas, *viewProjection, *directionalLight, color);
	}
}
