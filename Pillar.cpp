#include "Pillar.h"
#include "ImGuiManager.h"
void Pillar::Initialize(const std::string name, ViewProjection* viewProjection, DirectionalLight* directionalLight,Vector3 initialScale,Vector3 intialPos)
{
	GameObject::Initialize(name, viewProjection, directionalLight);
	worldTransform_.translation_ = intialPos;
	worldTransform_.scale_ = initialScale;
}

void Pillar::Update()
{
	worldTransform_.translation_.x -= 0.5f;
	if (worldTransform_.translation_.x <= -620) {
		worldTransform_.translation_.x = 380.0f;
	}
	worldTransform_.UpdateMatrix();
}
