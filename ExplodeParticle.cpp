#include "ExplodeParticle.h"
#include "ImGuiManager.h"

ExplodeParticle::ExplodeParticle()
{
	particleBox_ = std::make_unique<ParticleBox>(kParticleNum);
}

void ExplodeParticle::Initialize()
{

	particleBox_->Initialize();
	emitterWorldTransform_.SetIsScaleParent(false);
	minDirection_ = { -1.0f,-1.0f ,-1.0f};
	maxDirection_ = { 1.0f,1.0f,1.0f };

}

void ExplodeParticle::Update() {

	emitterWorldTransform_.UpdateMatrix();

	emitFrame_--;

	if (isEmit_ == true) {
		emitFrame_ = kEmitFrame_;
		isEmit_ = false;
	}

	if (emitFrame_ > 0) {
		for (size_t i = 0; i < EmitNum_; i++) {
			for (size_t i = 0; i < kParticleNum; i++) {
				if (particles[i].isActive_ == false) {
					particles[i].isActive_ = true;
					if (emitterWorldTransform_.GetParent()) {
						particles[i].direction_ = Normalize(Vector3{ Rand(minDirection_.x, maxDirection_.x) ,Rand(minDirection_.y,maxDirection_.y) ,Rand(minDirection_.z,maxDirection_.z) } *NormalizeMakeRotateMatrix(emitterWorldTransform_.GetParent()->matWorld_));
					}
					else {
						particles[i].direction_ = Normalize({ Rand(minDirection_.x, maxDirection_.x) ,Rand(minDirection_.y,maxDirection_.y) ,Rand(minDirection_.z,maxDirection_.z) });
					}
					particles[i].worldTransform_.translation_ = MakeTranslation(emitterWorldTransform_.matWorld_);
					particles[i].worldTransform_.rotation_ = { 0.0f,0.0f,0.0f };
					particles[i].worldTransform_.scale_ = emitterWorldTransform_.scale_;
					break;
				}
			}
		}
	}

	for (size_t i = 0; i < kParticleNum; i++) {
		float rotationSpeed = Radian(1.0f) * (float(i % 2) * 2.0f - 1.0f);
		if (particles[i].isActive_ == true) {
			particles[i].worldTransform_.rotation_ = particles[i].worldTransform_.rotation_ + rotationSpeed;
			particles[i].worldTransform_.translation_ += particles[i].direction_ * speed_;
			particles[i].worldTransform_.scale_ = particles[i].worldTransform_.scale_ - scaleSpeed_;
			if (particles[i].worldTransform_.scale_.x <= 0.0f) {
				particles[i].isActive_ = false;
			}
		}

	}

}

void ExplodeParticle::Draw(ViewProjection* viewProjection, DirectionalLight* directionalLight, Vector4 color)
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
