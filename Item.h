#pragma once
#include "GameObject.h"

enum class Type {
	Accel,
	Bomb,
};

class Item : public GameObject{
private:

	Type type_{};
	Vector3 size_{};

	Vector3 acceleration_{};
	Vector3 velocity_{};
	float gravity_ = 0.05f;
	float speed_ = 0.1f;

	bool isDead_ = false;

public:

	void Initialize(const std::string name, ViewProjection* viewProjection, DirectionalLight* directionalLight, const Vector3& position, Type type);

	void Update();

	void Draw();

	void EnBulletHit();

	void CharaHit();

	bool IsDead() { return isDead_; }

	Vector3 GetWorldPos() const;
	Type GetType() const { return type_; }

};

