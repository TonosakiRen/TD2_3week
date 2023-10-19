#include "Floor.h"

void Floor::Initialize(const std::string name, ViewProjection* viewProjection, DirectionalLight* directionalLight)
{
	GameObject::Initialize(name, viewProjection, directionalLight);
	material_.enableLighting_ = false;
	worldTransform_.scale_ = worldTransform_.scale_ * 15.0f;
	worldTransform_.translation_.y = -worldTransform_.scale_.y;
}

void Floor::Update()
{
	material_.translation_.y += -0.001f;
	worldTransform_.UpdateMatrix();
}
