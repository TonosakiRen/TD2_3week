#pragma once
#include "GameObject.h"
#include "Collider.h"
class EnemyBullet : public GameObject {
private:

	Vector3 velocity_{};
	bool isReflected = false;
	bool isDead_ = false;
public:
	Collider collider_;
public:

	void Initialize(const std::string name, ViewProjection* viewProjection, DirectionalLight* directionalLight,const Vector3& position,const Vector3& velocity);

	void Update();

	void Draw();

	void OnCollision();

	void OnRefCollision();

	Vector3 GetWorldPos() const;

	bool IsDead() const { return isDead_; }

};

