#include "GameScene.h"
#include "externals/imgui/imgui.h"
#include <cassert>
#include "Easing.h"
#include <random>
#include <fstream>

using namespace DirectX;

void (GameScene::* GameScene::SceneTable[])() = {
	&GameScene::TitleUpdate,
	&GameScene::InGameUpdate,
	&GameScene::ResultUpdate
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

	LoadBossPopData();
	BossPopComand();
	

	sphere_.reset(GameObject::Create("sphere", &viewProjection_, &directionalLight_));

	particle_.reset(Particle::Create(10));

	


	tmpCollider_.Initialize("tmp", viewProjection_, directionalLight_);

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

	boss_.erase(std::remove_if(boss_.begin(), boss_.end(), [](const std::unique_ptr<Boss>& boss) {
		if (boss->IsDead()) {
			return true;
		}
		return false;
	}),
	boss_.end());
	
}

void GameScene::ModelDraw()
{
	skydome_->Draw();
	floor_->Draw();
	player_->Draw();
	for (const auto& boss : boss_) {
		boss->Draw();
	}
	

	if (scene_ == Scene::InGame) {
		for (const auto& bullet : enemyBullets_) {
			bullet->Draw();
		}
		for (const auto& items : items_) {
			items->Draw();
		}
	}

	

	//tmpCollider_.Draw({1.0f,0.0f,0.0f,1.0f});

}

void GameScene::ParticleDraw()
{
	

}

void GameScene::ParticleBoxDraw()
{
	player_->ParticleDraw();
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
		isHitBulee = boss_[0]->collider_.Collision(bullet->collider_);
		if (isHitBulee && bullet->IsReflected()) {
			bullet->OnCollision();
			boss_[0]->OnRefCollision();
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
		isHitBulee = boss_[0]->mouthCollider_.Collision(item->collider_);
		if (isHitBulee) {
			item->CharaHit();
			if (item->GetType() == Type::Accel) {
				boss_[0]->SpeedUp();
			}
			if (item->GetType() == Type::Bomb) {
				boss_[0]->Explosion();
			}
		}
	}

	//プレイヤーとボスとの衝突判定
	isHitBulee = false;
	isHitBulee = player_->collider_.Collision(boss_[0]->collider_);
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
	Item* newItem = new Item();

	switch (stage_) {
	    case Stage::Stage1: //アイテムなし
	    
	    	break;
		case Stage::Stage2: //爆弾のみ

			
			newItem->Initialize("bomb", &viewProjection_, &directionalLight_, { 150, (Boss::size_.y * (3.0f / 2.0f)) + random, 0.0f }, Type::Bomb);
			items_.push_back(std::unique_ptr<Item>(newItem));

			break;
		case Stage::Stage3: //スピードのみ

			
			newItem->Initialize("accel", &viewProjection_, &directionalLight_, { 150, (Boss::size_.y * (3.0f / 2.0f)) + random, 0.0f }, Type::Accel);
			items_.push_back(std::unique_ptr<Item>(newItem));

			break;
		case Stage::Actual:
		default:

			std::uniform_int_distribution<int> distribution2(1, 6);
			int lot = distribution2(gen);
			
			if (lot % 2 == 0) {
				newItem->Initialize("bomb", &viewProjection_, &directionalLight_, { 150, (Boss::size_.y * (3.0f / 2.0f)) + random, 0.0f }, Type::Bomb);
			}
			else if (lot % 2 == 1) {
				newItem->Initialize("accel", &viewProjection_, &directionalLight_, { 150, (Boss::size_.y * (3.0f / 2.0f)) + random, 0.0f }, Type::Accel);
			}
			items_.push_back(std::unique_ptr<Item>(newItem));

			break;


	}

	

}

void GameScene::BossPop(int hp, float speed, int second) {

	Boss* boss = new Boss();
	boss->Initialize(&viewProjection_, &directionalLight_);
	boss->SetState(hp, speed, second);
	boss->SetPlayer(player_.get());
	boss->SetGameScene(this);
	boss_.push_back(std::unique_ptr<Boss>(boss));

}

void GameScene::LoadBossPopData() {

	std::ifstream file;
	std::string filePath = "./Resources/BossPop.csv";
	file.open(filePath);
	assert(file.is_open());

	BossPopCommands_ << file.rdbuf();

	file.close();
}

void GameScene::BossPopComand() {

	std::string line;

	BossPopCommands_.clear();                 // エラー状態をクリア
	BossPopCommands_.seekg(0, std::ios::beg); // ファイルの先頭に移動

	while (getline(BossPopCommands_, line)) {

		std::istringstream line_stream(line);

		std::string word;
		getline(line_stream, word, ',');

		if (word.find("//") == 0) {
			continue;
		}

		if (order_ == (int)std::atof(word.c_str())) {

			getline(line_stream, word, ',');
			int hp = (int)std::atof(word.c_str());

			getline(line_stream, word, ',');
			float speed = (float)std::atof(word.c_str());

			getline(line_stream, word, ',');
			int second = (int)std::atof(word.c_str());

			BossPop(hp, speed, second);
			order_++;
			break;
		}


	}

}

void GameScene::TitleInitialize() {

	cameraT_ = 0.0f;
	isCameraMove_ = false;

}

void GameScene::TitleUpdate() {

	boss_[0]->Animation();
	player_->Animation();

	if (input_->TriggerKey(DIK_SPACE)) {
		isCameraMove_ = true;
	}
	if (cameraT_ >= 1.0f) {
		sceneRequest_ = Scene::InGame;
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

void GameScene::InGameInitialize() {

	cameraT_ = 0.0f;
	order_ = 1;
	if (boss_.size() == 0) {
		BossPopComand();
	}

}

void GameScene::InGameUpdate() {

	CollisionCheck();

	

	if (input_->TriggerKey(DIK_E)) {
		isCameraMove_ = true;
		//リザルトシーンのカメラの位置の設定
		//プレイヤーの位置から少しずらしたところ
		resultCameraPos_ = {
			player_->GetCharaWorldPos().x + 10.0f,
			player_->GetCharaWorldPos().y,
			player_->GetCharaWorldPos().z - 40.0f
		};
		//
	}
	if (cameraT_ >= 1.0f) {
		sceneRequest_ = Scene::Result;
		isCameraMove_ = false;
		cameraT_ = 0.0f;
	}
	if (isCameraMove_) {
		viewProjection_.translation_ = Easing::easing(cameraT_, gameCameraPos_, resultCameraPos_, 0.01f, Easing::easeNormal, false);
		viewProjection_.target_ = Easing::easing(cameraT_, gameCameraTar_, resultCameraTar_, 0.01f, Easing::easeNormal, true);
	}
	if (stage_ != Stage::Stage1) {
		if (--ItemTimer <= 0) {
			PopItem();
			ItemTimer = kPopTime;
		}
	}
	
	skydome_->Update();
	floor_->Update();
	player_->Update();
	boss_[0]->Update();

	if (boss_[0]->IsDead()) {
		BossPopComand();
		//Boss::isBreak_ = false;
	}

	for (const auto& bullet : enemyBullets_) {
		bullet->Update();
	}
	for (const auto& items : items_) {
		items->Update();
	}

}

void GameScene::ResultInitialize() {

	cameraT_ = 0.0f;
	select_ = Selection::ToTitle;
	result_ = Result::Select;

	enemyBullets_.clear();
	items_.clear();
	boss_.clear();

}

void GameScene::ResultUpdate() {

	switch (result_) {
	    case Result::Select: //タイトルに戻るか続けるかの選択

			if (input_->TriggerKey(DIK_SPACE)) {
				result_ = Result::Translation;
			}

			switch (select_){
			    case GameScene::Selection::ToTitle:
					nextScene = Scene::Title;
					if (input_->TriggerKey(DIK_DOWN)) {
						select_ = Selection::Continue;
					}

			    	break;
			    case GameScene::Selection::Continue:
					nextScene = Scene::InGame;
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
					viewProjection_.translation_ = Easing::easing(cameraT_, resultCameraPos_, titleCameraPos_, 0.01f, Easing::easeNormal, false);
					viewProjection_.target_ = Easing::easing(cameraT_, resultCameraTar_, titleCameraTar_, 0.01f, Easing::easeNormal, true);
					
			    	break;
			    case GameScene::Selection::Continue:
					viewProjection_.translation_ = Easing::easing(cameraT_, resultCameraPos_, gameCameraPos_, 0.01f, Easing::easeNormal, false);
					viewProjection_.target_ = Easing::easing(cameraT_, resultCameraTar_, gameCameraTar_, 0.01f, Easing::easeNormal, true);
					
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
