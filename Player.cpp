#include "Player.h"
#include "ImGuiManager.h"
#include "Easing.h"
void Player::Initialize(const std::string name, ViewProjection* viewProjection, DirectionalLight* directionalLight)
{
	GameObject::Initialize(name, viewProjection, directionalLight);
	input_ = Input::GetInstance();

	dustParticle_ = std::make_unique<DustParticle>();
	dustParticle_->Initialize({ 0.0f,0.0f,-0.8f }, { 0.0f,1.0f,0.0f });
	dustParticle_->emitterWorldTransform_.SetParent(&worldTransform_);
	//煙の出る場所
	dustParticle_->emitterWorldTransform_.translation_ = { 0.0f,-10.0f,-5.0f };
	dustParticle_->emitterWorldTransform_.scale_ = { 1.0f,1.0f,1.0f };
	dustParticle_->SetSpeed(0.55f);
	dustParticle_->SetScaleSpeed(0.03f);


	material_.enableLighting_ = false;
	worldTransform_.rotation_.y = Radian(90.0f);
	modelParts_.Initialize("player_part");

	for (int i = 0; i < partNum; i++) {
		partsTransform_[i].Initialize();
		partsTransform_[i].SetParent(&worldTransform_);
	}
	//model位置初期化
	{
		partsTransform_[LeftArm].translation_.x = 1.510f;
		partsTransform_[RightArm].translation_.x = -1.510f;
		partsTransform_[LeftArm].scale_ = { 0.4f,0.4f, 0.4f };
		partsTransform_[RightArm].scale_ = { 0.4f,0.4f, 0.4f };

		partsTransform_[RightLeg].translation_.y = 1.470f - 2.79f;
		partsTransform_[LeftLeg].translation_.y = 1.470f - 2.79f;
		partsTransform_[RightLeg].translation_.x = 1.120f;
		partsTransform_[LeftLeg].translation_.x = -1.120f;

		partsTransform_[LeftLeg].scale_ = { 0.4f,0.4f, 0.4f };
		partsTransform_[RightLeg].scale_ = { 0.4f,0.4f, 0.4f };
	}
	float playerScale = 5.0f;

	size_ = { 1.0f * playerScale,1.0f * playerScale + 4.0f,1.0f * playerScale + 2.0f };

	worldTransform_.translation_.y = size_.y;
	worldTransform_.scale_ = { playerScale,playerScale,playerScale };

	reflectWT_.Initialize();
	reflectWT_.translation_ = {};
	reflectWT_.scale_ = size_ * 1.5f;
	reflectWT_.rotation_.y = Radian(90.0f);
	reflectWT_.SetParent(&worldTransform_);
	reflectWT_.SetIsRotateParent(false);

	isDead_ = false;

	//行列更新
	reflectWT_.UpdateMatrix();
	worldTransform_.UpdateMatrix();
	for (int i = 0; i < partNum; i++) {
		partsTransform_[i].UpdateMatrix();
	}


	collider_.Initialize(&worldTransform_, name, *viewProjection, *directionalLight, {5.5f,5.5f,5.5f});
	reflectCollider_.Initialize(&reflectWT_, name, *viewProjection, *directionalLight, {7.0f,7.0f,7.0f},{0.0f,0.0f,-size_.z*2.0f});

}

void Player::Update()
{


#ifdef _DEBUG
	ImGui::Begin("Player");

	ImGui::DragFloat("firstSpeed", &firstSpeed_, 0.01f);
	ImGui::DragFloat("gravity", &gravity_, 0.01f);

	ImGui::End();
#endif // _DEBUG

	

	//止める
	if (isClear_ == false && isDead_ == false) {
		Animation();
	}
	else {
		ParticleStop();
	}
	RotationAnimation();

	if (behaviorRequest_) {
		behavior_ = behaviorRequest_.value();

		switch (behavior_) {
		    case Behavior::kRoot:
		    default:
		    	RootInitialize();
		    	break;
		    case Behavior::kAccel:
		    	AccelInitialize();
		    	break;
		    case Behavior::kBombHit:
		    	BombHitInitialize();
		    	break;
		}
		behaviorRequest_ = std::nullopt;
	}

	switch (behavior_) {
	    case Behavior::kRoot:
	    default:
	    	RootUpdate();
	    	break;
	    case Behavior::kAccel:
	    	AccelUpdate();
	    	break;
	    case Behavior::kBombHit:
	    	BombHitUpdate();
	    	break;
	}
#ifdef _DEBUG
	ImGui::Begin("Player");
	ImGui::DragFloat3("pos", &worldTransform_.translation_.x, 0.01f);
	ImGui::DragFloat3("ro", &worldTransform_.rotation_.x, 0.01f);
	ImGui::End();
#endif // _DEBUG

	worldTransform_.translation_.y = min(worldTransform_.translation_.y, jampLimitHeight_);

	//行列更新
	reflectWT_.UpdateMatrix();
	
	worldTransform_.UpdateMatrix();
	for (int i = 0; i < partNum; i++) {
		partsTransform_[i].UpdateMatrix();
	}

	collider_.AdjustmentScale();
	reflectCollider_.AdjustmentScale();
	collider_.MatrixUpdate();
	reflectCollider_.MatrixUpdate();
}
void Player::Animation() {
	dustParticle_->SetIsEmit(true);
	dustParticle_->Update();
	if (animationT_ >= 1.0f || animationT_ <= 0.0f)
	{
		animationSpeed_ *= -1.0f;
	}

	if (animationBodyT_ >= 1.0f)
	{
		animationBodyT_ = 0.0f;
		runUpAnimation_ *= -1.0f;
	}

	partsTransform_[RightArm].rotation_.x = Easing::easing(animationT_, -0.6f, 0.6f, animationSpeed_, Easing::EasingMode::easeNormal, false);
	partsTransform_[LeftArm].rotation_.x = -partsTransform_[RightArm].rotation_.x;

	partsTransform_[RightLeg].rotation_.x = Easing::easing(animationT_, -0.4f, 0.4f, animationSpeed_, Easing::EasingMode::easeNormal, false);
	partsTransform_[LeftLeg].rotation_.x = -partsTransform_[RightLeg].rotation_.x;

	worldTransform_.rotation_.y = Radian(90.0f) + Easing::easing(animationT_, -0.4f, 0.4f, animationSpeed_, Easing::EasingMode::easeNormal, false);

	worldTransform_.translation_.y += Easing::easing(animationBodyT_, 0.0f, runUpAnimation_, animationBodySpeed_, Easing::EasingMode::easeNormal, false);

	animationT_ += animationSpeed_;
	animationBodyT_ += animationBodySpeed_;

	worldTransform_.UpdateMatrix();
	for (int i = 0; i < partNum; i++) {
		partsTransform_[i].UpdateMatrix();
	}
}
void Player::ParticleStop()
{
	dustParticle_->SetIsEmit(false);
	dustParticle_->Update();
	for (int i = 0; i < partNum; i++) {
		partsTransform_[i].UpdateMatrix();
	}
}
void Player::Draw() {

	model_.Draw(worldTransform_, *viewProjection_, *directionalLight_, material_);
	for (int i = 0; i < partNum; i++) {
		modelParts_.Draw(partsTransform_[i], *viewProjection_, *directionalLight_, material_);
	}
	//collider_.Draw();
	//reflectCollider_.Draw();
}

void Player::ParticleDraw() {
	dustParticle_->Draw(viewProjection_, directionalLight_,{0.5f,0.5f,0.5f,1.0f});
}

void Player::GravityUpdate()
{
	acceleration_ = { 0.0f, -gravity_, 0.0f };
	velocity_ += acceleration_;
	worldTransform_.translation_ += velocity_;

	if (worldTransform_.translation_.y <= size_.y) {
		worldTransform_.translation_.y = size_.y;
	}
}

void Player::RootInitialize() {



}

void Player::RotationAnimation() {
	if (isRotation == true) {
		isRotation = false;
		rotationT = 0.0f;
		worldTransform_.rotation_.x = Easing::easing(rotationT, 0.0f, Radian(360.0f), 0.05f,Easing::easeNormal);
	}
	if (rotationT > 0.0f) {
		worldTransform_.rotation_.x = Easing::easing(rotationT, 0.0f, Radian(360.0f), 0.05f, Easing::easeNormal);
	}
	if (rotationT >= 1.0f) {
		rotationT = 0.0f;
	}

}

void Player::RootUpdate() {
	if (isClear_ == false && isDead_ == false) {
		if (input_->TriggerKey(DIK_SPACE)) {
			isRotation = true;
			velocity_ = { 0.0f, firstSpeed_, 0.0f };
			isAttack_ = true;
		}

		if (isAttack_) {
			if (--attackTimer <= 0) {
				isAttack_ = false;
				attackTimer = kAttackTime;
			}
		}

		acceleration_ = { 0.0f, -gravity_, 0.0f };
		velocity_ += acceleration_;
		worldTransform_.translation_ += velocity_;

		if (worldTransform_.translation_.y <= size_.y) {
			worldTransform_.translation_.y = size_.y;
		}
	}

}

void Player::AccelInitialize() {

	Vector3 move = { size_.x * 2.0f, 0.0f, 0.0f };
	num = 0.0f;
	easeStart = worldTransform_.translation_;
	easeEnd = worldTransform_.translation_ + move;
	isAttack_ = false;
	attackTimer = kAttackTime;

}

void Player::AccelUpdate() {

	if (worldTransform_.translation_.x >= 90.0f) {
		behaviorRequest_ = Behavior::kRoot;
		return; 
	}
	
	worldTransform_.translation_ = Easing::easing(num, easeStart, easeEnd, 0.1f, Easing::EasingMode::easeOutQuart);

	if (num >= 1.0f) {
		num = 1.0f;
		behaviorRequest_ = Behavior::kRoot;
	}


}

void Player::BombHitInitialize() {

	Vector3 move = { size_.x * 2.0f, 0.0f, 0.0f };
	num = 0.0f;
	easeStart = worldTransform_.translation_;
	easeEnd = worldTransform_.translation_ - move;
	isAttack_ = false;
	attackTimer = kAttackTime;

}

void Player::BombHitUpdate() {

	worldTransform_.translation_ = Easing::easing(num, easeStart, easeEnd, 0.1f, Easing::EasingMode::easeOutQuart);

	if (num >= 1.0f) {
		num = 1.0f;
		behaviorRequest_ = Behavior::kRoot;
	}

}





void Player::Accel() {

	behaviorRequest_ = Behavior::kAccel;

}

void Player::Explosion() {

	behaviorRequest_ = Behavior::kBombHit;

}

Vector3 Player::GetCharaWorldPos() const {
	Vector3 worldPos{};

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

Vector3 Player::GetRefWorldPos() const {
	Vector3 worldPos{};

	worldPos.x = reflectWT_.matWorld_.m[3][0];
	worldPos.y = reflectWT_.matWorld_.m[3][1];
	worldPos.z = reflectWT_.matWorld_.m[3][2];

	return worldPos;
}
