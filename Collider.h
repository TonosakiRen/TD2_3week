#pragma once
#include "GameObject.h"
class Collider
{
public:
	void Initialize(WorldTransform* objectWorldTransform, const std::string name, ViewProjection& viewProjection, DirectionalLight& directionalLight, Vector3 initialScale = { 1.0f,1.0f,1.0f }, Vector3 initialPos = { 0.0f,0.0f,0.0f });
	//ただの四角形用
	void Initialize(const std::string name, ViewProjection& viewProjection, DirectionalLight& directionalLight);
	bool Collision(Collider& colliderB);
	void AdjustmentScale();
	void MatrixUpdate();
	void Draw(Vector4 color = { 1.0f,1.0f,1.0f,1.0f });

	bool ObbCollision(const OBB& obb1, const OBB& obb2);

	WorldTransform worldTransform_;
private:
	GameObject cube_;
	
	std::string name_;
	//saveScale元のモデルをscaleで、でかくしたりしているときのためにここにその初期スケールmatrixを保存する
	Matrix4x4 inverseSaveScaleMatrix_;

};

