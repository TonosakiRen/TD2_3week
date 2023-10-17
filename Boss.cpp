#include "Boss.h"
#include "Input.h"
#include "ImGuiManager.h"
#include "Easing.h"
void Boss::Initialize(ViewProjection* viewProjection, DirectionalLight* directionalLight)
{
	GameObject::Initialize(viewProjection, directionalLight);
	input_ = Input::GetInstance();

	dustParticle_ = std::make_unique<DustParticle>();
	dustParticle_->Initialize({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f });
	dustParticle_->emitterWorldTransform_.SetParent(&worldTransform_);
	//煙の出る場所
	dustParticle_->emitterWorldTransform_.translation_ = { 0.0f,-2.1f,-1.2f };

	worldTransform_.translation_.y = 10.0f;
	worldTransform_.translation_.x = -15.0f;

	worldTransform_.rotation_.y = Radian(90.0f);
	accelaration_ = { 0.0f,0.002f };
	modelParts_[Head].Initialize("boss_head");
	modelParts_[Tin].Initialize("boss_tin");
	for (int i = 0; i < partNum; i++) {
		partsTransform_[i].Initialize();
		partsTransform_[i].SetParent(&worldTransform_);
	}
	//model位置初期化
	{
		partsTransform_[Head].translation_.y = -2.0f;
		partsTransform_[Tin].translation_.y = -6.0f;
		
	}
}

void Boss::Update()
{

	Animation();
	ImGui::Begin("Boss");
	ImGui::DragFloat3("core", &worldTransform_.translation_.x, 0.01f);
	ImGui::DragFloat3("head", &partsTransform_[Head].translation_.x, 0.01f);
	ImGui::DragFloat3("tin", &partsTransform_[Tin].translation_.x, 0.01f);
	ImGui::End();
	
	worldTransform_.UpdateMatrix();
	for (int i = 0; i < partNum; i++) {
		partsTransform_[i].UpdateMatrix();
	}
	worldTransform_.translation_.y = clamp(worldTransform_.translation_.y, 10.0f, 13.9f);
	dustParticle_->Update();
}
void Boss::Animation() {
	partsTransform_[Head].translation_.y = clamp(partsTransform_[Head].translation_.y, -3.0f, 0.8f);
}
void Boss::Draw() {
	for (int i = 0; i < partNum; i++) {
		modelParts_[i].Draw(partsTransform_[i], *viewProjection_, *directionalLight_, material_);
	}
}

void Boss::ParticleDraw() {
	dustParticle_->Draw(viewProjection_, directionalLight_, { 0.5f,0.5f,0.5f,1.0f });
}
