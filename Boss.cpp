#include "Boss.h"
#include "ImGuiManager.h"
#include "Easing.h"
#include "Player.h"
#include "GameScene.h"

Vector3 Boss::size_ = {20.0f,58.0f,15.0f};
Vector3 Boss::mouthSize_ = { 20.0f,size_.y/2.0f,15.0f };

void Boss::Initialize(ViewProjection* viewProjection, DirectionalLight* directionalLight)
{
	GameObject::Initialize(viewProjection, directionalLight);

	dustParticle_ = std::make_unique<DustParticle>();
	dustParticle_->Initialize({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f });
	dustParticle_->emitterWorldTransform_.SetParent(&worldTransform_);
	//煙の出る場所
	dustParticle_->emitterWorldTransform_.translation_ = { 0.0f,-2.1f,-1.2f };

	/*worldTransform_.translation_.y = 0.0f;
	worldTransform_.translation_.x = -150.0f;*/
	worldTransform_.translation_ = startPos_;

	worldTransform_.scale_ = worldTransform_.scale_ * 10.0f;

	worldTransform_.rotation_.y = Radian(90.0f);
	modelParts_[Head].Initialize("boss_head");
	modelParts_[Tin].Initialize("boss_tin");
	for (int i = 0; i < partNum; i++) {
		partsTransform_[i].Initialize();
		partsTransform_[i].SetParent(&worldTransform_);
	}
	//model位置初期化
	{
		partsTransform_[Head].translation_.y = 8.0f;
		partsTransform_[Tin].translation_.y = 3.0f;
	}

	mouthWT_.Initialize();
	mouthWT_.SetParent(&worldTransform_);

	velocity_ = { 0.02f,0.0f,0.0f };
	bulletVelocity_ = { 0.4f, 0.0f, 0.0f };


	//行列更新
	worldTransform_.UpdateMatrix();
	for (int i = 0; i < partNum; i++) {
		partsTransform_[i].UpdateMatrix();
	}
	mouthWT_.UpdateMatrix();
	dustParticle_->emitterWorldTransform_.UpdateMatrix();

	collider_.Initialize(&worldTransform_, "boss", *viewProjection, *directionalLight,{20.9f,59.6f,21.7f},{0.0f,62.1f,0.0f});
	

}

void Boss::Update()
{

	Animation();
	ImGui::Begin("Boss");
	ImGui::DragFloat3("core", &worldTransform_.translation_.x, 0.01f);
	ImGui::DragFloat3("head", &partsTransform_[Head].translation_.x, 0.01f);
	ImGui::DragFloat3("tin", &partsTransform_[Tin].translation_.x, 0.01f);

	ImGui::DragFloat("jumpPower", &jumpPower_, 0.001f);
	ImGui::DragFloat3("accelerariotn", &animationAccelaration_.x, 0.001f);

	ImGui::DragFloat("animationT_", &animationT_, 0.001f);
	ImGui::DragFloat("animationSpeed_", &animationSpeed_, 0.001f);
	
	ImGui::End();
	
	if (behaviorRequest_) {
		behavior_ = behaviorRequest_.value();

		switch (behavior_) {
		    case Behavior::kRoot:
		    default:
		    	RootInitialize();
		    	break;
		    case Behavior::kHit:
		    	HitInitialize();
		    	break;
		    case Behavior::kBombHit:
		    	BombHitInitialize();
		    	break;
		    case Behavior::kBreak:
		    
		    	break;
		}
		behaviorRequest_ = std::nullopt;
	}

	switch (behavior_) {
	    case Behavior::kRoot:
	    default:
	    	RootUpdate();
	    	break;
	    case Behavior::kHit:
	    	HitUpdate();
	    	break;
	    case Behavior::kBombHit:
	    	BombHitUpdate();
	    	break;
	    case Behavior::kBreak:
	    
	    	break;
	}


	//worldTransform_.translation_.y = clamp(worldTransform_.translation_.y, 10.0f, 13.9f);
	

	//行列更新
	for (int i = 0; i < partNum; i++) {
		partsTransform_[i].UpdateMatrix();
	}
	worldTransform_.UpdateMatrix();
	mouthWT_.UpdateMatrix();
	dustParticle_->Update();

	collider_.AdjustmentScale();
} 
void Boss::Animation() {

	//jump
	animationVelocity_ += animationAccelaration_;
	if (worldTransform_.translation_.y <= 0.0f) {
		animationVelocity_.y = jumpPower_;
	}
	worldTransform_.translation_ += animationVelocity_;
	
	
	//上の頭アニメーション
	partsTransform_[Head].translation_.y = Easing::easing(worldTransform_.translation_.y / 5.0f, 8.0f, 9.14f);


	//行列更新
	for (int i = 0; i < partNum; i++) {
		partsTransform_[i].UpdateMatrix();
	}
	worldTransform_.UpdateMatrix();

}
void Boss::Draw() {
	for (int i = 0; i < partNum; i++) {
		modelParts_[i].Draw(partsTransform_[i], *viewProjection_, *directionalLight_, material_);
	}
	collider_.Draw();
}

void Boss::ParticleDraw() {
	dustParticle_->Draw(viewProjection_, directionalLight_, { 0.5f,0.5f,0.5f,1.0f });
}

void Boss::Appear(float& t) {

	worldTransform_.translation_ = Easing::easing(t, startPos_, endPos_, 0.01f, Easing::EasingMode::easeOutQuart);

}

void Boss::RootInitialize() {



}

void Boss::RootUpdate() {

	//worldTransform_.translation_ += velocity_;

	if (--attackTimer <= 0) {
		attackTimer = kAttackTime;

		Vector3 pos{};
		pos.x = worldTransform_.translation_.x + size_.x + 1.0f;
		pos.y = player_->GetCharaWorldPos().y;

		pos.y = min(pos.y, size_.y * 2);

		EnemyBullet* newBullet = new EnemyBullet();
		newBullet->Initialize("enemyBullet",viewProjection_,directionalLight_, pos, bulletVelocity_);
		gameScene_->AddEnemyBullet(newBullet);
	}

}

void Boss::HitInitialize() {



}

void Boss::HitUpdate() {



}

void Boss::BombHitInitialize() {



}

void Boss::BombHitUpdate() {



}

Vector3 Boss::GetWorldPos() const {
	Vector3 worldPos{};

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1] + size_.y;
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

Vector3 Boss::GetMouthWorldPos() const {
	Vector3 worldPos{};

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1] + mouthSize_.y;
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}
