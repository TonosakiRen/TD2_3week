#include "Item.h"

void Item::Initialize(const std::string name, ViewProjection* viewProjection, DirectionalLight* directionalLight, const Vector3& position, Type type) {

	GameObject::Initialize(name, viewProjection, directionalLight);
	worldTransform_.translation_ = position;
	velocity_ = { -speed_,0.0f,0.0f };
	size_ = { 2.0f,2.0f,2.0f };
	type_ = type;

}

void Item::Update() {

	velocity_ += acceleration_;
	worldTransform_.translation_ += velocity_;

	if (worldTransform_.translation_.y <= size_.y) {
		worldTransform_.translation_.y = size_.y;
		acceleration_ = {};
		velocity_.y = 0.0f;
	}

	worldTransform_.UpdateMatrix();

}

void Item::Draw() {
	model_.Draw(worldTransform_, *viewProjection_, *directionalLight_, material_);
}

void Item::EnBulletHit() {

	acceleration_ = { 0.0f, -gravity_, 0.0f };

}

void Item::CharaHit() {

	isDead_ = true;

}

Vector3 Item::GetWorldPos() const {
	Vector3 worldPos{};

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
