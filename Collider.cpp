#include "Collider.h"
#include "ImGuiManager.h"

void Collider::Initialize(WorldTransform* objectWorldTransform, const std::string name, ViewProjection& viewProjection, DirectionalLight& directionalLight, Vector3 initialScale, Vector3 initialPos)
{
	objectWorldTransform->UpdateMatrix();
	worldTransform_.Initialize();
	worldTransform_.scale_ = initialScale;
	worldTransform_.translation_ = initialPos;
	worldTransform_.SetParent(objectWorldTransform);
	inverseSaveScaleMatrix_ = Inverse(MakeScaleMatrix(MakeScale(objectWorldTransform->matWorld_)));
	name_ = name;
	cube_.Initialize(&viewProjection, &directionalLight);
}

void Collider::AdjustmentScale()
{
#ifdef _DEBUG
	const std::string col = "collider";
	ImGui::Begin((name_ + col).c_str());
	ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.1f);
	ImGui::DragFloat3("translation", &worldTransform_.translation_.x, 0.1f);
	ImGui::End();
#endif // _DEBUG

	

	MatrixUpdate();
}

void Collider::Initialize(const std::string name, ViewProjection& viewProjection, DirectionalLight& directionalLight)
{
	worldTransform_.Initialize();
	inverseSaveScaleMatrix_ = MakeIdentity4x4();
	name_ = name;
	cube_.Initialize(&viewProjection, &directionalLight);
}

bool Collider::Collision(Collider& colliderB)
{

	MatrixUpdate();
	colliderB.MatrixUpdate();
	OBB obb1;
	OBB obb2;
	obb1.center = MakeTranslation(worldTransform_.matWorld_);
	obb2.center = MakeTranslation(colliderB.worldTransform_.matWorld_);
	obb1.size = MakeScale(worldTransform_.matWorld_);
	obb2.size = MakeScale(colliderB.worldTransform_.matWorld_);
	Matrix4x4 rotateMatrix1 = NormalizeMakeRotateMatrix(worldTransform_.matWorld_);
	Matrix4x4 rotateMatrix2 = NormalizeMakeRotateMatrix(colliderB.worldTransform_.matWorld_);
	obb1.orientations[0] = GetXAxis(rotateMatrix1);
	obb1.orientations[1] = GetYAxis(rotateMatrix1);
	obb1.orientations[2] = GetZAxis(rotateMatrix1);
	obb2.orientations[0] = GetXAxis(rotateMatrix2);
	obb2.orientations[1] = GetYAxis(rotateMatrix2);
	obb2.orientations[2] = GetZAxis(rotateMatrix2);

	return ObbCollision(obb1, obb2);
}

void Collider::MatrixUpdate()
{
	// スケール、回転、平行移動行列の計算
	Matrix4x4 scaleMatrix = MakeScaleMatrix(worldTransform_.scale_);

	Matrix4x4 rotateMatrix = MakeIdentity4x4();
	rotateMatrix = MakeRotateXYZMatrix(worldTransform_.rotation_);
	Matrix4x4 translationMatrix = MakeTranslateMatrix(worldTransform_.translation_);

	// ワールド行列の合成
	worldTransform_.matWorld_ = MakeIdentity4x4();
	worldTransform_.matWorld_ *= scaleMatrix;
	worldTransform_.matWorld_ *= rotateMatrix;
	worldTransform_.matWorld_ *= translationMatrix;

	//saveしたscaleInverseMatrixを掛ける
	worldTransform_.matWorld_ *= inverseSaveScaleMatrix_;

	// 親行列の指定がある場合は、掛け算する
	if (worldTransform_.GetParent()) {
		//scaleを反映させない
		Matrix4x4 inverseMatrix = MakeIdentity4x4();

		worldTransform_.matWorld_ *= worldTransform_.GetParent()->matWorld_;
	}

	// 定数バッファに書き込み
	worldTransform_.ConstUpdate();
}

void Collider::Draw(Vector4 color)
{
	cube_.Draw(worldTransform_, color);
}

bool Collider::ObbCollision(const OBB& obb1, const OBB& obb2)
{
	Vector3 separationAxisCandidate[15];
	// 各軸
	separationAxisCandidate[0] = obb1.orientations[0];
	separationAxisCandidate[1] = obb1.orientations[1];
	separationAxisCandidate[2] = obb1.orientations[2];
	separationAxisCandidate[3] = obb2.orientations[0];
	separationAxisCandidate[4] = obb2.orientations[1];
	separationAxisCandidate[5] = obb2.orientations[2];
	// 各辺のクロス積
	separationAxisCandidate[6] = Cross(obb1.orientations[0], obb2.orientations[0]);
	separationAxisCandidate[7] = Cross(obb1.orientations[0], obb2.orientations[1]);
	separationAxisCandidate[8] = Cross(obb1.orientations[0], obb2.orientations[2]);
	separationAxisCandidate[9] = Cross(obb1.orientations[1], obb2.orientations[0]);
	separationAxisCandidate[10] = Cross(obb1.orientations[1], obb2.orientations[1]);
	separationAxisCandidate[11] = Cross(obb1.orientations[1], obb2.orientations[2]);
	separationAxisCandidate[12] = Cross(obb1.orientations[2], obb2.orientations[0]);
	separationAxisCandidate[13] = Cross(obb1.orientations[2], obb2.orientations[1]);
	separationAxisCandidate[14] = Cross(obb1.orientations[2], obb2.orientations[2]);
	// 頂点
	Matrix4x4 obb1WorldMatrix = {
		obb1.orientations[0].x, obb1.orientations[0].y, obb1.orientations[0].z, 0.0f,
		obb1.orientations[1].x, obb1.orientations[1].y, obb1.orientations[1].z, 0.0f,
		obb1.orientations[2].x, obb1.orientations[2].y, obb1.orientations[2].z, 0.0f,
		obb1.center.x,          obb1.center.y,          obb1.center.z,          1.0f };
	Matrix4x4 obb2WorldMatrix = {
		obb2.orientations[0].x, obb2.orientations[0].y, obb2.orientations[0].z, 0.0f,
		obb2.orientations[1].x, obb2.orientations[1].y, obb2.orientations[1].z, 0.0f,
		obb2.orientations[2].x, obb2.orientations[2].y, obb2.orientations[2].z, 0.0f,
		obb2.center.x,          obb2.center.y,          obb2.center.z,          1.0f };

	Vector3 vertices1[] = {
		-obb1.size,
		{obb1.size.x,  -obb1.size.y, -obb1.size.z},
		{obb1.size.x,  -obb1.size.y, obb1.size.z },
		{-obb1.size.x, -obb1.size.y, obb1.size.z },
		{-obb1.size.x, obb1.size.y,  -obb1.size.z},
		{obb1.size.x,  obb1.size.y,  -obb1.size.z},
		obb1.size,
		{-obb1.size.x, obb1.size.y,  obb1.size.z }
	};

	Vector3 vertices2[] = {
		-obb2.size,
		{obb2.size.x,  -obb2.size.y, -obb2.size.z},
		{obb2.size.x,  -obb2.size.y, obb2.size.z },
		{-obb2.size.x, -obb2.size.y, obb2.size.z },
		{-obb2.size.x, obb2.size.y,  -obb2.size.z},
		{obb2.size.x,  obb2.size.y,  -obb2.size.z},
		obb2.size,
		{-obb2.size.x, obb2.size.y,  obb2.size.z }
	};

	for (int i = 0; i < 8; i++) {
		vertices1[i] = vertices1[i] * obb1WorldMatrix;
		vertices2[i] = vertices2[i] * obb2WorldMatrix;
	}

	// 各軸
	for (int i = 0; i < 15; i++) {
		// 影の長さの合計
		float sumSpan;
		// 2つの影の両端の差分
		float longSpan;
		// 射影した最大値最小値
		float max1, min1, max2, min2;
		// 差分が形状を分離軸に射影した長さ
		float L1, L2;
		// すべての頂点を射影した値
		float Dot1[8];
		float Dot2[8];
		// 各頂点
		for (int j = 0; j < 8; j++) {
			Dot1[j] = Dot(separationAxisCandidate[i], vertices1[j]);
		}
		for (int k = 0; k < 8; k++) {
			Dot2[k] = Dot(separationAxisCandidate[i], vertices2[k]);
		}
		max1 = (std::max)({ Dot1[0], Dot1[1], Dot1[3], Dot1[4], Dot1[5], Dot1[6], Dot1[7] });
		min1 = (std::min)({ Dot1[0], Dot1[1], Dot1[3], Dot1[4], Dot1[5], Dot1[6], Dot1[7] });
		L1 = max1 - min1;
		max2 = (std::max)({ Dot2[0], Dot2[1], Dot2[3], Dot2[4], Dot2[5], Dot2[6], Dot2[7] });
		min2 = (std::min)({ Dot2[0], Dot2[1], Dot2[3], Dot2[4], Dot2[5], Dot2[6], Dot2[7] });
		L2 = max2 - min2;

		sumSpan = L1 + L2;
		longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);
		if (sumSpan < longSpan) {
			return false;
		}
	}
	return true;
}
