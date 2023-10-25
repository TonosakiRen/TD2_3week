#pragma once
#include "GameObject.h"
#include "Collider.h"

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
	

	bool isDead_ = false;
	
private:

	static float speed_;
	static float gravity_;

public: //Collider
	Collider collider_;
	//画面外に
	bool isDrop_ = false;
public:

	void Initialize(const std::string name, ViewProjection* viewProjection, DirectionalLight* directionalLight, const Vector3& position, Type type);

	void Update();

	void Draw();

	void EnBulletHit();

	void CharaHit();

	void BossHitAnimation();

	bool IsDead() { return isDead_; }

	Vector3 GetWorldPos() const;
	Type GetType() const { return type_; }

};

