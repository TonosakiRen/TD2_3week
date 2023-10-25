#pragma once
#include "GameObject.h"
#include "Collider.h"
#include "OrbitParticle.h"
class EnemyBullet : public GameObject {
private:

	Vector3 velocity_{};
	
	bool isDead_ = false;
public:
	bool isReflected_ = false;
	Collider collider_;
	OrbitParticle orbitParticle_;
public:

	void Initialize(const std::string name, ViewProjection* viewProjection, DirectionalLight* directionalLight,const Vector3& position,const Vector3& velocity);

	void Update();

	void Draw();

	void ParicleDraw();

	void OnCollision();

	void OnRefCollision();

	void SetVelocity(const Vector3& velocity) { velocity_ = velocity; }

	Vector3 GetWorldPos() const;

	bool IsDead() const { return isDead_; }
	bool IsReflected() const { return isReflected_; }

};

