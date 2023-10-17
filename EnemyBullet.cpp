#include "EnemyBullet.h"

void EnemyBullet::Initialize(const std::string name, ViewProjection* viewProjection, DirectionalLight* directionalLight, const Vector3& position, const Vector3& velocity) {

	GameObject::Initialize(name, viewProjection, directionalLight);
	worldTransform_.translation_ = position;
	velocity_ = velocity;

}

void EnemyBullet::Update() {

	worldTransform_.translation_ += velocity_;

	//行列更新
	worldTransform_.UpdateMatrix();
}

void EnemyBullet::Draw() {
	model_.Draw(worldTransform_, *viewProjection_, *directionalLight_, material_);
}

void EnemyBullet::OnCollision() { isDead_ = true; }

void EnemyBullet::OnRefCollision() { isReflected = true; }

Vector3 EnemyBullet::GetWorldPos() const {
	Vector3 worldPos{};

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
