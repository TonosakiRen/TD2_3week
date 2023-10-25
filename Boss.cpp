#include "Boss.h"
#include "ImGuiManager.h"
#include "Easing.h"
#include "Player.h"
#include "GameScene.h"

Vector3 Boss::size_ = {20.0f,62.0f,21.0f};
Vector3 Boss::mouthSize_ = { 20.0f,size_.y/2.0f,21.0f };
float Boss::bulletSpeed_ = 0.5f;
Vector3 Boss::knockbackdis = { 10.0f, 0.0f, 0.0f };
int Boss::damage_ = 20;
float Boss::bombBaseDamage_ = 10.0f;
int Boss::shotCount = 1;

void Boss::Initialize(ViewProjection* viewProjection, DirectionalLight* directionalLight)
{
	audio_ = Audio::GetInstance();
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


	//行列更新
	worldTransform_.UpdateMatrix();
	for (int i = 0; i < partNum; i++) {
		partsTransform_[i].UpdateMatrix();
	}
	mouthWT_.UpdateMatrix();
	dustParticle_->emitterWorldTransform_.UpdateMatrix();


	collider_.Initialize(&worldTransform_, "boss", *viewProjection, *directionalLight,{20.9f,59.6f,25.0f},{0.0f,62.1f,10.0f});
	mouthCollider_.Initialize(&mouthWT_, "boss", *viewProjection, *directionalLight, {20.9f,28.0f,25.0f}, { 0.0f,31.0f,16.0f });
  itemDisapeerCollider_.Initialize(&worldTransform_, "disaaperer", *viewProjection, *directionalLight, { 20.9f,59.6f,21.7f }, { 0.0f,62.1f,0.0f });
}

void Boss::Update()
{

	Animation();
#ifdef _DEBUG
	ImGui::Begin("Boss");
	ImGui::DragFloat3("core", &worldTransform_.translation_.x, 0.01f);
	ImGui::DragFloat3("head", &partsTransform_[Head].translation_.x, 0.01f);
	ImGui::DragFloat3("tin", &partsTransform_[Tin].translation_.x, 0.01f);

	ImGui::DragFloat("jumpPower", &jumpPower_, 0.001f);
	ImGui::DragFloat3("accelerariotn", &animationAccelaration_.x, 0.001f);

	ImGui::DragFloat("animationT_", &animationT_, 0.001f);
	ImGui::DragFloat("animationSpeed_", &animationSpeed_, 0.001f);

	ImGui::DragFloat("bullet speed", &bulletSpeed_, 0.001f);
	ImGui::DragFloat3("knockback", &knockbackdis.x, 0.1f);

	ImGui::InputInt("Damage", &damage_);
	ImGui::InputFloat("BombBaseDamage", &bombBaseDamage_);

	ImGui::InputInt("HP", &hp_);

	ImGui::End();
#endif // _DEBUG

	
	
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
			case Behavior::kAppear:
				AppearInitialize();
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
		case Behavior::kAppear:
			AppearUpdate();
			break;
	}


	if (hp_ <= 0) {
		isDead_ = true;
 		//isBreak_ = true;
	}

	//worldTransform_.translation_.y = clamp(worldTransform_.translation_.y, 10.0f, 13.9f);

	

	//行列更新
	for (int i = 0; i < partNum; i++) {
		partsTransform_[i].UpdateMatrix();
	}
	worldTransform_.UpdateMatrix();
	mouthWT_.UpdateMatrix();
	dustParticle_->Update();

	itemDisapeerCollider_.AdjustmentScale();
	collider_.AdjustmentScale();
	mouthCollider_.AdjustmentScale();
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
	//行列更新
	for (int i = 0; i < partNum; i++) {
		partsTransform_[i].UpdateMatrix();
	}
	worldTransform_.UpdateMatrix();
	for (int i = 0; i < partNum; i++) {
		modelParts_[i].Draw(partsTransform_[i], *viewProjection_, *directionalLight_, material_);
	}

	/*mouthCollider_.Draw();
		collider_.Draw();
		itemDisapeerCollider_.Draw({1.0f,0.0f,0.0f,1.0f});*/
	//collider_.Draw();
	//mouthCollider_.Draw();
}

void Boss::ParticleDraw() {
	dustParticle_->Draw(viewProjection_, directionalLight_, { 0.5f,0.5f,0.5f,1.0f });
}


void Boss::Appear(float& t) {

	worldTransform_.translation_.x = Easing::easing(t, startPos_.x, endPos_.x, 0.01f, Easing::EasingMode::easeOutQuart);

}

void Boss::Disappear(float& t) {

	worldTransform_.translation_.x = Easing::easing(t, endPos_.x, startPos_.x, 0.01f, Easing::EasingMode::easeOutQuart,false);

}


void Boss::OnRefCollision() {
	if (behavior_ != Behavior::kAppear) {
		behaviorRequest_ = Behavior::kHit;
		hp_ -= damage_;
	}
}

void Boss::SpeedUp() {
	velocity_ = velocity_ * 1.5f;
	size_t speedHandle = audio_->SoundLoadWave("speedup.wav");
	size_t speedPlayHandle = audio_->SoundPlayWave(speedHandle);
}

void Boss::SetState(int hp, float speed, int second) {

	hp_ = hp;
	velocity_ = { speed,0.0f,0.0f };
	attackTime = 60 * second;
	attackTimer = attackTime;

}

void Boss::Explosion() {
	behaviorRequest_ = Behavior::kBombHit;
	size_t explodeHandle = audio_->SoundLoadWave("explosion.wav");
	size_t explodePlayHandle = audio_->SoundPlayWave(explodeHandle);
}

void Boss::RootInitialize() {



}

void Boss::RootUpdate() {

	worldTransform_.translation_ += velocity_;

	if (--attackTimer <= 0) {
		attackTimer = attackTime;
		bulletVelocity_ = { bulletSpeed_, 0.0f, 0.0f };
		Vector3 pos{};
		pos.x = worldTransform_.translation_.x + size_.x + 1.0f;
		pos.y = 20.0f * (float)shotCount;
		shotCount++;
		if(shotCount>=7){
			shotCount = 1;
		}

		pos.y = min(pos.y, size_.y * 2);

		EnemyBullet* newBullet = new EnemyBullet();
		newBullet->Initialize("enemyBullet",viewProjection_,directionalLight_, pos, bulletVelocity_);
		gameScene_->AddEnemyBullet(newBullet);
	}

}

void Boss::HitInitialize() {

	
	num = 0.0f;
	easeStart = worldTransform_.translation_;
	easeEnd = worldTransform_.translation_ - knockbackdis;
	size_t hitHandle = audio_->SoundLoadWave("hit.wav");
	size_t hitPlayHandle = audio_->SoundPlayWave(hitHandle);
}

void Boss::HitUpdate() {
	
	worldTransform_.translation_ = Easing::easing(num, easeStart, easeEnd, 0.02f, Easing::EasingMode::easeOutQuart);

	if (num >= 1.0f) {
		num = 1.0f;
		behaviorRequest_ = Behavior::kRoot;
	}

}

void Boss::BombHitInitialize() {

	num = 0.0f;
	easeStart = worldTransform_.translation_;
	easeEnd = endPos_;

}

void Boss::BombHitUpdate() {

	float distance = Length(startPos_) - Length(endPos_);

	worldTransform_.translation_ = Easing::easing(num, easeStart, easeEnd, 0.02f, Easing::EasingMode::easeOutQuart);

	if (num >= 1.0f) {
		num = 1.0f;
		velocity_ = velocity_ * 2.0f;
		behaviorRequest_ = Behavior::kRoot;
		bombBaseDamage_ = bombBaseDamage_ * (1.00f + (distance / 100.0f));
		hp_ -= static_cast<int>(bombBaseDamage_);
	}

}

void Boss::AppearInitialize() {

	num = 0.0f;

}

void Boss::AppearUpdate() {

	worldTransform_.translation_ = Easing::easing(num, startPos_, endPos_, 0.01f, Easing::EasingMode::easeOutQuart);

	if (num >= 1.0f) {
		behaviorRequest_ = Behavior::kRoot;
	}

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


