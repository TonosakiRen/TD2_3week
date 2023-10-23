#include "Item.h"
#include "ImGuiManager.h"

float Item::speed_ = 0.3f;
float Item::gravity_ = 0.1f;

void Item::Initialize(const std::string name, ViewProjection* viewProjection, DirectionalLight* directionalLight, const Vector3& position, Type type) {

	GameObject::Initialize(name, viewProjection, directionalLight);
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = Radian(-90.0f);
	worldTransform_.scale_ = { 4.0f,4.0f,4.0f };
	size_ = worldTransform_.scale_;
	velocity_ = { -speed_,0.0f,0.0f };
	type_ = type;

	collider_.Initialize(&worldTransform_, name, *viewProjection, *directionalLight, { 4.0f,4.0f,4.0f });

}

void Item::Update() {

	ImGui::Begin("item");

	ImGui::DragFloat("speed", &speed_, 0.01f);
	ImGui::DragFloat("gravity", &gravity_, 0.01f);

	ImGui::End();

	velocity_ += acceleration_;
	worldTransform_.translation_ += velocity_;

	if (worldTransform_.translation_.y <= size_.y) {
		worldTransform_.translation_.y = size_.y;
		acceleration_ = {};
		velocity_.y = 0.0f;
	}

	if (worldTransform_.translation_.x <= -200.0f) {
		isDead_ = true;
	}

	worldTransform_.UpdateMatrix();
	collider_.AdjustmentScale();
	collider_.MatrixUpdate();
}

void Item::Draw() {
	model_.Draw(worldTransform_, *viewProjection_, *directionalLight_, material_);
	//collider_.Draw();
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
