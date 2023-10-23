#include "Collapse.h"
#include "ImGuiManager.h"

Collapse::Collapse()
{
	particleBox_ = std::make_unique<ParticleBox>(kParticleNum);
}

void Collapse::Initialize()
{

	particleBox_->Initialize();
	
}

void Collapse::Update() {
#ifdef _DEBUG
	ImGui::Begin("collape");
	ImGui::DragFloat("emitX", &emitX);
	ImGui::DragFloat("emitHeight", &emitHeight);
	ImGui::DragFloat("disaaperHaeight", &disapperHeigth);
	ImGui::DragFloat2("randScale", &randScale.x);
	ImGui::DragFloat("accelaration_", &accelaration_, 0.0001f);
	ImGui::DragFloat("offsetZ", &offsetZ);

	ImGui::End();
#endif // _DEBUG

	

	if (isEmit_) {
		for (size_t i = 0; i < EmitNum_; i++) {
			for (size_t i = 0; i < kParticleNum; i++) {
				if (particles[i].isActive_ == false) {
					particles[i].isActive_ = true;
			
					particles[i].worldTransform_.translation_ = {Rand(-emitX,emitX),Rand(180.0f,300.0f),Rand(30.0f,40.0f)};
					particles[i].worldTransform_.rotation_ = { 0.0f,0.0f,0.0f };
					particles[i].velocity_ = 0.0f;

					float scale = Rand(randScale.x, randScale.y);
					particles[i].worldTransform_.scale_ = { scale ,scale ,scale };
					break;
				}
			}
		}
	}

	for (size_t i = 0; i < kParticleNum; i++) {
		float rotationSpeed = Radian(1.0f) * (float(i % 2) * 2.0f - 1.0f);
		if (particles[i].isActive_ == true) {
			particles[i].worldTransform_.rotation_ = particles[i].worldTransform_.rotation_ + rotationSpeed;
			particles[i].velocity_ += accelaration_;
			particles[i].worldTransform_.translation_.y += particles[i].velocity_;
			if (particles[i].worldTransform_.translation_.y <= disapperHeigth) {
				particles[i].isActive_ = false;
			}
		}

	}

}

void Collapse::Draw(ViewProjection* viewProjection, DirectionalLight* directionalLight, Vector4 color)
{

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
