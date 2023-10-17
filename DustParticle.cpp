#include "DustParticle.h"
#include "ImGuiManager.h"
DustParticle::DustParticle()
{
	particleBox_ = std::make_unique<ParticleBox>(kParticleNum);
}

void DustParticle::Initialize(Vector3 minDirection, Vector3 maxDirection)
{
	particleBox_->Initialize();
	emitterWorldTransform_.SetIsScaleParent(false);
	SetDirection(minDirection, maxDirection);
	for (size_t i = 0; i < kParticleNum; i++) {
		particles[i].worldTransform_.SetParent(&emitterWorldTransform_);
	}
}

void DustParticle::Update() {
	
	ImGui::Begin("effect");
	ImGui::DragFloat3("emitPos",&emitterWorldTransform_.translation_.x,0.01f);
	ImGui::End();
	for (size_t i = 0; i < EmitNum_;i++) {
		for (size_t i = 0; i < kParticleNum; i++) {
			if (particles[i].isActive_ == false) {
				particles[i].isActive_ = true;
				particles[i].direction_ = Normalize({ Rand(0.0f,0.0f) ,Rand(0.0f,1.0f) ,Rand(-1.0f,0.0f) });
				particles[i].worldTransform_.translation_ = { 0.0f,0.0f,0.0f };
				particles[i].worldTransform_.rotation_ = { 0.0f,0.0f,0.0f };
				particles[i].worldTransform_.scale_ = initialScale_;
				break;
			}
		}
	}

	for (size_t i = 0; i < kParticleNum; i++) {
		float rotationSpeed = Radian(1.0f) * (float(i % 2) * 2.0f - 1.0f);
		if (particles[i].isActive_ == true) {
			particles[i].worldTransform_.rotation_ = particles[i].worldTransform_.rotation_ + rotationSpeed;
			particles[i].worldTransform_.translation_ += particles[i].direction_ * 0.1f;
			particles[i].worldTransform_.scale_ = particles[i].worldTransform_.scale_ - 0.01f;
			if (particles[i].worldTransform_.scale_.x <= 0.0f) {
				particles[i].isActive_ = false;
			}
		}

	}

}

void DustParticle::Draw(ViewProjection* viewProjection, DirectionalLight* directionalLight, Vector4 color)
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
