#include "GameScene.h"
#include "externals/imgui/imgui.h"
#include <cassert>
#include "Easing.h"
#include <random>

using namespace DirectX;

void (GameScene::* GameScene::SceneTable[])() = {
	&GameScene::TitleUpdate,
	&GameScene::BossAppaerUpdate,
	&GameScene::InGameUpdate,
	&GameScene::ResultUpdate,
};

GameScene::GameScene() {};

GameScene::~GameScene() {};

void GameScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	
	viewProjection_.Initialize();

	viewProjection_.translation_ = titleCameraPos_;
	viewProjection_.target_ = titleCameraTar_;
	viewProjection_.UpdateMatrix();

	directionalLight_.Initialize();
	directionalLight_.direction_ = { 1.0f, -1.0f, 1.0f };
	directionalLight_.UpdateDirectionalLight();


	textureHandle_ = TextureManager::Load("uvChecker.png");

	sprite_.reset(Sprite::Create(textureHandle_, { 0.0f,0.0f }));

	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize("skydome",&viewProjection_,&directionalLight_);
	floor_ = std::make_unique<Floor>();
	floor_->Initialize("floor", &viewProjection_, &directionalLight_);
	player_ = std::make_unique<Player>();
	player_->Initialize("player", &viewProjection_, &directionalLight_);

	boss_ = std::make_unique<Boss>();
	boss_->Initialize(&viewProjection_, &directionalLight_);
	boss_->SetPlayer(player_.get());
	boss_->SetGameScene(this);

	sphere_.reset(GameObject::Create("sphere", &viewProjection_, &directionalLight_));

	particle_.reset(Particle::Create(10));

	


	tmpCollider_.Initialize("tmp", viewProjection_, directionalLight_);

	collapse_.Initialize();
	orbit_.Initialize();
}

void GameScene::Update(){
	//camera light
	{
		// camera
		viewProjection_.DebugMove();
		viewProjection_.UpdateMatrix();
		// light
		ImGui::DragFloat3("light", &directionalLight_.direction_.x, 0.01f);
		ImGui::DragFloat4("lightcolor", &directionalLight_.color_.x, 0.01f);
		directionalLight_.direction_ = Normalize(directionalLight_.direction_);
		directionalLight_.UpdateDirectionalLight();
	}

	enemyBullets_.remove_if([](const std::unique_ptr<EnemyBullet>& bullet) {
		if (bullet->IsDead()) {
			return true;
		}
		return false;
	});

	items_.remove_if([](const std::unique_ptr<Item>& item) {
		if (item->IsDead()) {
			return true;
		}
		return false;
	});

	if (sceneRequest_) {
		scene_ = sceneRequest_.value();

		switch (scene_) {
		case Scene::Title:
		default:
			TitleInitialize();
			break;
		case Scene::BossAppear:
			BossAppaerInitialize();
			break;
		case Scene::InGame:
			InGameInitialize();
			break;
		case Scene::Result:
			ResultInitialize();
			break;
		}
		sceneRequest_ = std::nullopt;
	}

	
	(this->*SceneTable[static_cast<size_t>(scene_)])();

	tmpCollider_.AdjustmentScale();
	bool isHitBulee = false;
	for (const auto& bullet : enemyBullets_) {
		isHitBulee  = tmpCollider_.Collision(bullet->collider_);
		if (isHitBulee == true) {
			break;
		}
	}

	collapse_.Update();
	orbit_.Update();
	 
	ImGui::Text("%d", isHitBulee);
}

void GameScene::ModelDraw()
{

	skydome_->Draw();
	floor_->Draw();
	player_->Draw();
	boss_->Draw();

	if (scene_ == Scene::InGame) {
		for (const auto& bullet : enemyBullets_) {
			bullet->Draw();
		}
		for (const auto& items : items_) {
			items->Draw();
		}
	}

	tmpCollider_.Draw({1.0f,0.0f,0.0f,1.0f});

}

void GameScene::ParticleDraw()
{
	

}

void GameScene::ParticleBoxDraw()
{
	player_->ParticleDraw();
	collapse_.Draw(&viewProjection_, &directionalLight_, { 0.5f,0.5f,0.5f,1.0f });
	orbit_.Draw(&viewProjection_, &directionalLight_, { 0.5f,0.5f,0.5f,1.0f });
}

void GameScene::PreSpriteDraw()
{

}

void GameScene::PostSpriteDraw()
{

}

void GameScene::Draw() {
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// 背景スプライト描画
	Sprite::PreDraw(commandList);
	PreSpriteDraw();
	Sprite::PostDraw();

	//深度バッファクリア
	dxCommon_->ClearDepthBuffer();

	//3Dオブジェクト描画
	Model::PreDraw(commandList);
	ModelDraw();
	Model::PostDraw();

	//Particle描画
	Particle::PreDraw(commandList);
	ParticleDraw();
	Particle::PostDraw();

	//ParticleBox描画
	ParticleBox::PreDraw(commandList);
	ParticleBoxDraw();
	ParticleBox::PostDraw();

	// 前景スプライト描画
	Sprite::PreDraw(commandList);
	PostSpriteDraw();
	Sprite::PostDraw();
}

void GameScene::CollisionCheck() {

	tmpCollider_.AdjustmentScale();

	//敵弾とプレイヤーとの衝突判定
	bool isHitBulee = false;
	for (const auto& bullet : enemyBullets_) {
		isHitBulee = player_->collider_.Collision(bullet->collider_);
		if (isHitBulee == true) {
			bullet->OnCollision();
			player_->Explosion();
			break;
		}
	}
	//敵弾と跳ね返りとの衝突判定
	isHitBulee = false;
	for (const auto& bullet : enemyBullets_) {
		isHitBulee = player_->reflectCollider_.Collision(bullet->collider_);
		if (isHitBulee && player_->IsAttack()) {
			bullet->SetVelocity({ -refBulletSpeed_, 0.0f, 0.0f });
			bullet->OnRefCollision();
		}
	}
	//ボスと跳ね返り弾の衝突判定
	isHitBulee = false;
	for (const auto& bullet : enemyBullets_) {
		isHitBulee = boss_->collider_.Collision(bullet->collider_);
		if (isHitBulee && bullet->IsReflected()) {
			bullet->OnCollision();
			boss_->OnRefCollision();
		}
	}
	////敵弾とアイテムとの衝突判定
	//isHitBulee = false;
	//for (const auto& bullet : enemyBullets_) {
	//	for (const auto& item : items_) {
	//		isHitBulee = bullet->collider_.Collision(item->collider_);
	//		if (isHitBulee) {
	//			bullet->OnCollision();
	//			item->EnBulletHit();
	//		}
	//	}
	//}
	
	//アイテムと跳ね返りとの衝突判定
	isHitBulee = false;
	for (const auto& item : items_) {
		isHitBulee = player_->reflectCollider_.Collision(item->collider_);
		if (isHitBulee) {
			item->EnBulletHit();
		}
	}

	//プレイヤーとアイテムとの衝突判定
	isHitBulee = false;
	for (const auto& item : items_) {
		isHitBulee = player_->collider_.Collision(item->collider_);
		if (isHitBulee) {
			item->CharaHit();
			if (item->GetType() == Type::Accel) {
				player_->Accel();
			}
			if (item->GetType() == Type::Bomb) {
				player_->Explosion();
			}
		}
	}
	//敵の口とアイテムとの衝突判定
	isHitBulee = false;
	for (const auto& item : items_) {
		isHitBulee = boss_->mouthCollider_.Collision(item->collider_);
		if (isHitBulee) {
			item->CharaHit();
			if (item->GetType() == Type::Accel) {
				boss_->SpeedUp();
			}
			if (item->GetType() == Type::Bomb) {
				boss_->Explosion();
			}
		}
	}

	//プレイヤーとボスとの衝突判定
	isHitBulee = false;
	isHitBulee = player_->collider_.Collision(boss_->collider_);
	if (isHitBulee) {
		//ゲームオーバー

	}


	ImGui::Text("%d", isHitBulee);
}

void GameScene::AddEnemyBullet(EnemyBullet* enemyBullet) {
	enemyBullets_.push_back(std::unique_ptr<EnemyBullet>(enemyBullet));
}

void GameScene::PopItem() {

	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_real_distribution<float> distribution(-(Boss::size_.y / 2.0f) + 4.0f, (Boss::size_.y / 2.0f) - 4.0f);
	float random = distribution(gen);

	std::uniform_int_distribution<int> distribution2(1, 6);
	int lot = distribution2(gen);

	Item* newItem = new Item();
	if (lot % 2 == 0) {
		newItem->Initialize("bomb",&viewProjection_, &directionalLight_,{150, (Boss::size_.y * (3.0f / 2.0f)) + random, 0.0f}, Type::Bomb);
	}
	else if (lot % 2 == 1) {
		newItem->Initialize("accel", &viewProjection_, &directionalLight_, {150, (Boss::size_.y * (3.0f / 2.0f)) + random, 0.0f}, Type::Accel);
	}
	items_.push_back(std::unique_ptr<Item>(newItem));

}

void GameScene::TitleInitialize() {
	player_->isClear_ = false;
	player_->isDead_ = false;
	cameraT_ = 0.0f;
	isCameraMove_ = false;

}

void GameScene::TitleUpdate() {

	boss_->Animation();
	player_->Animation();
	player_->GravityUpdate();

	if (input_->TriggerKey(DIK_SPACE)) {
		isCameraMove_ = true;
	}
	if (cameraT_ >= 1.0f) {
		sceneRequest_ = Scene::BossAppear;
		cameraT_ = 0.0f;
		isCameraMove_ = false;
	}
	
	if (isCameraMove_) {
		viewProjection_.translation_ = Easing::easing(cameraT_, titleCameraPos_, gameCameraPos_, 0.01f, Easing::easeNormal, false);
		viewProjection_.target_ = Easing::easing(cameraT_, titleCameraTar_, gameCameraTar_, 0.01f, Easing::easeNormal, true);
	}
	

	skydome_->Update();
	floor_->Update();
}

void GameScene::BossAppaerInitialize() {
	bossT_ = 0.0f;
	player_->isClear_ = false;
	player_->isDead_ = false;
}

void GameScene::BossAppaerUpdate() {

	
	boss_->Appear(bossT_);
	if (bossT_ >= 1.0f) {
		bossT_ = 0.0f;
		sceneRequest_ = Scene::InGame;
	}
	
	player_->Animation();
	player_->GravityUpdate();
	skydome_->Update();
	floor_->Update();
	boss_->Animation();
}

void GameScene::InGameInitialize() {

	cameraT_ = 0.0f;
	isSavePlayerPos_ = false;
	shakeFrame_ = 6;
	downT = 0.0f;
	waitFrame = 10;
}

void GameScene::InGameUpdate() {	

	CollisionCheck();

	clearDirection();
	gameoverDirection();
	if (--ItemTimer <= 0) {
		PopItem();
		ItemTimer = kPopTime;
	}
	skydome_->Update();
	floor_->Update();
	player_->Update();
	boss_->Update();

	for (const auto& bullet : enemyBullets_) {
		bullet->Update();
	}
	for (const auto& items : items_) {
		items->Update();
	}

}

void GameScene::clearDirection() {
	if (input_->TriggerKey(DIK_E) && isSavePlayerPos_ == false) {
		player_->isClear_ = true;
		isCameraMove_ = true;
		isSavePlayerPos_ = true;
		//リザルトシーンのカメラの位置の設定
		//プレイヤーの位置から少しずらしたところ
		resultCameraPos_ = {
			player_->GetCharaWorldPos().x + 10.0f,
			player_->GetCharaWorldPos().y,
			player_->GetCharaWorldPos().z - 40.0f
		};
		//
	}
	if (cameraT_ >= 1.0f && player_->GetIsClear()) {
		sceneRequest_ = Scene::Result;
		isCameraMove_ = false;
		cameraT_ = 0.0f;
	}
	if (isCameraMove_ && player_->GetIsClear()) {
		viewProjection_.translation_ = Easing::easing(cameraT_, gameCameraPos_, resultCameraPos_, 0.01f, Easing::easeNormal, false);
		viewProjection_.target_ = Easing::easing(cameraT_, gameCameraTar_, resultCameraTar_, 0.01f, Easing::easeNormal, true);
	}

}
void GameScene::gameoverDirection() {
	if (input_->TriggerKey(DIK_R) && isSavePlayerPos_ == false) {
		player_->isDead_ = true;
		isCameraMove_ = true;
		isSavePlayerPos_ = true;
		//リザルトシーンのカメラの位置の設定
		resultCameraPos_ = {
			player_->GetCharaWorldPos().x ,
			player_->GetCharaWorldPos().y,
			player_->GetCharaWorldPos().z
		};
	}

	if (isCameraMove_ && player_->GetIsDead()) {
		boss_->Disappear(cameraT_);
		player_->SetTranslation(Easing::easing(cameraT_, resultCameraPos_, gameoverPlayerPos_, 0.05f, Easing::easeNormal, true));
		player_->SetRotation(Easing::easing(cameraT_, { 0.0f,0.0f,0.0f }, gameoverPlayerRotation, 0.05f, Easing::easeNormal, false));
	}

	if (cameraT_ >= 1.0f && player_->GetIsDead()) {
		isCameraMove_ = false;
		bool isFinish = true;
		isFinish = viewProjection_.Shake(shakeValue, shakeFrame_);
		collapse_.SetIsEmit(true);
		if (!isFinish) {
			waitFrame--;
			if (waitFrame < 0) {
				collapse_.SetIsEmit(false);
				Vector3 downPos = { gameoverPlayerPos_.x,gameoverPlayerPos_.y - 20.0f,gameoverPlayerPos_.z };
				player_->SetTranslation(Easing::easing(downT, gameoverPlayerPos_, downPos, 0.01f, Easing::easeNormal));
			}
		}
	}
	if (downT >= 1.0f) {
		downT = 1.0f;
		sceneRequest_ = Scene::Result;
	}
}


void GameScene::ResultInitialize() {

	cameraT_ = 0.0f;
	select_ = Selection::ToTitle;
	result_ = Result::Select;
}

void GameScene::ResultUpdate() {

	bossT_ = 0.0f;
	boss_->Appear(bossT_);
	player_->ParticleStop();
	switch (result_) {
	    case Result::Select: //タイトルに戻るか続けるかの選択

			if (input_->TriggerKey(DIK_SPACE)) {
				result_ = Result::Translation;
				if (player_->isDead_) {
					player_->SetTranslation({0.0f,20.0f,0.0f});
					player_->SetRotation({ 0.0f,0.0f,0.0f });
				}
			}

			switch (select_){
			    case GameScene::Selection::ToTitle:
					nextScene = Scene::Title;
					if (input_->TriggerKey(DIK_DOWN)) {
						select_ = Selection::Continue;
					}

			    	break;
			    case GameScene::Selection::Continue:
					nextScene = Scene::BossAppear;
					if (input_->TriggerKey(DIK_UP)) {
						select_ = Selection::ToTitle;
					}

			    	break;
			    default:
			    	break;
			}

	    	break;
		case Result::Translation: //カメラの移動
			switch (select_) {
			    case GameScene::Selection::ToTitle:
					if (player_->isClear_) {
						viewProjection_.translation_ = Easing::easing(cameraT_, resultCameraPos_, titleCameraPos_, 0.01f, Easing::easeNormal, false);
						viewProjection_.target_ = Easing::easing(cameraT_, resultCameraTar_, titleCameraTar_, 0.01f, Easing::easeNormal, true);
					}
					if (player_->isDead_) {
						viewProjection_.translation_ = Easing::easing(cameraT_, gameCameraPos_, titleCameraPos_, 0.01f, Easing::easeNormal, false);
						viewProjection_.target_ = Easing::easing(cameraT_, resultCameraTar_, titleCameraTar_, 0.01f, Easing::easeNormal, true);
					}
					
			    	break;
			    case GameScene::Selection::Continue:
					if (player_->isClear_) {
						viewProjection_.translation_ = Easing::easing(cameraT_, resultCameraPos_, gameCameraPos_, 0.01f, Easing::easeNormal, false);
						viewProjection_.target_ = Easing::easing(cameraT_, resultCameraTar_, gameCameraTar_, 0.01f, Easing::easeNormal, true);
					}
					if (player_->isDead_) {
						cameraT_ = 1.0f;
					}
					
			    	break;
			    default:
			    	break;
			}
			if (cameraT_ >= 1.0f) {
				sceneRequest_ = nextScene;
			}
			

			break;
	}

}
