#include "Floor.h"

void Floor::Initialize(const std::string name, ViewProjection* viewProjection, DirectionalLight* directionalLight)
{
	GameObject::Initialize(name, viewProjection, directionalLight);
	material_.enableLighting_ = false;
	worldTransform_.scale_ = worldTransform_.scale_ * 50.0f;
}

void Floor::Update()
{
	material_.translation_.y += -0.001f;
}
