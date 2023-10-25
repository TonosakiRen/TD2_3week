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
	directionalLight_.direction_ = { 0.966f, -0.258f, -0.031f };
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

	collapse_.Initialize();
	orbit_.Initialize();

	for (int i = 0; i < pillarNum; i++) {
		pillar_[i].Initialize("pillar", &viewProjection_, &directionalLight_, { 10.0f,10.0f,10.0f },{380.0f - (i * 100.0f),-40.0f,140.0f});
	}

	explodePlayerParticle_.Initialize();
	explodeBossParticle_.Initialize();

	bossExplode_.Initialize();

	blockHandle_ = TextureManager::Load("block.png");
}

void GameScene::Update(){
	//camera light
	{
		// camera
		viewProjection_.DebugMove();
		viewProjection_.UpdateMatrix();
		// light
#ifdef _DEBUG
		ImGui::DragFloat3("light", &directionalLight_.direction_.x, 0.01f);
		ImGui::DragFloat4("lightcolor", &directionalLight_.color_.x, 0.01f);
#endif // _DEBUG

		
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

	bool isHitBulee = false;
	for (const auto& bullet : enemyBullets_) {
		isHitBulee  = tmpCollider_.Collision(bullet->collider_);
		if (isHitBulee == true) {
			break;
		}
	}

	collapseFrame--;
	if (collapseFrame > 0) {
		collapse_.SetIsEmit(true);
	}
	else {
		collapse_.SetIsEmit(false);
	}

	collapse_.Update();

	explodePlayerParticle_.Update();
	explodeBossParticle_.Update();
	
	if (explodeFrame_ > 0) {
		viewProjection_.Shake({ 3.0f,3.0f,3.0f }, explodeFrame_);
	}

	bossExplodeFrame--;
	if (bossExplodeFrame > 0) {
		bossExplode_.SetIsEmit(true);
	}
	else {
		bossExplode_.SetIsEmit(false);
	}
	bossExplode_.Update();
	for (int i = 0; i < pillarNum; i++) {
		pillar_[i].Update();
	}
	ImGui::Begin("piii");
	ImGui::DragFloat3("tranpillar1", &pillar_[0].GetWorldTransform()->translation_.x, 0.01f);
	ImGui::DragFloat3("tranpillar2", &pillar_[1].GetWorldTransform()->translation_.x, 0.01f);
	ImGui::DragFloat3("tranpillar3", &pillar_[2].GetWorldTransform()->translation_.x, 0.01f);
	ImGui::DragFloat3("tranpillar4", &pillar_[3].GetWorldTransform()->translation_.x, 0.01f);
	ImGui::DragFloat3("tranpillar5", &pillar_[4].GetWorldTransform()->translation_.x, 0.01f);
	ImGui::DragFloat3("tranpillar6", &pillar_[5].GetWorldTransform()->translation_.x, 0.01f);
	ImGui::DragFloat3("tranpillar7", &pillar_[6].GetWorldTransform()->translation_.x, 0.01f);
	ImGui::DragFloat3("tranpillar8", &pillar_[7].GetWorldTransform()->translation_.x, 0.01f);
	ImGui::DragFloat3("tranpillar9", &pillar_[8].GetWorldTransform()->translation_.x, 0.01f);
	ImGui::DragFloat3("tranpillar10", &pillar_[9].GetWorldTransform()->translation_.x, 0.01f);
	ImGui::End();
}

void GameScene::ModelDraw()
{

	skydome_->Draw();
	floor_->Draw();
	player_->Draw();
	for (int i = 0; i < pillarNum; i++) {
		pillar_[i].Draw();
	}
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
	explodePlayerParticle_.Draw(&viewProjection_, &directionalLight_,{1.0f,0.0f,0.0f,1.0f});
	explodeBossParticle_.Draw(&viewProjection_, &directionalLight_, { 1.0f,0.0f,0.0f,1.0f });
	collapse_.Draw(&viewProjection_, &directionalLight_, { 121.0f / (255.0f * 4.0f), 101.0f / (255.0f * 4.0f), 53.0f / (255.0f * 4.0f) });
	orbit_.Draw(&viewProjection_, &directionalLight_, { 0.5f,0.5f,0.5f,1.0f });
	bossExplode_.Draw(&viewProjection_, &directionalLight_, { 1.0f,1.0f,1.0f,1.0f },blockHandle_);
	for (const auto& bullet : enemyBullets_) {
		bullet->ParicleDraw();
	}
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
				explodeFrame_ = 6;
				explodePlayerParticle_.SetIsEmit(true);
				explodePlayerParticle_.emitterWorldTransform_.translation_ = player_->GetWorldTransform()->translation_;
				collapseFrame = 10;
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
				explodeFrame_ = 6;
				explodeBossParticle_.SetIsEmit(true);
				explodeBossParticle_.emitterWorldTransform_.translation_ = boss_[0]->GetWorldTransform()->translation_;
				collapseFrame = 10;
			}
		}
	}

	//プレイヤーとボスとの衝突判定
	isHitBulee = false;
	isHitBulee = player_->collider_.Collision(boss_[0]->collider_);
	if (isHitBulee) {
		//ゲームオーバー
		player_->isDead_ = true;
		isCameraMove_ = true;
		isSavePlayerPos_ = true;
		//リザルトシーンのカメラの位置の設定
		resultCameraPos_ = {
			player_->GetCharaWorldPos().x,
			player_->GetCharaWorldPos().y,
			player_->GetCharaWorldPos().z
		};
		enemyBullets_.clear();
		items_.clear();
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
		case Stage::Stage2: //スピードのみ
			
			newItem->Initialize("accel", &viewProjection_, &directionalLight_, { 150, (Boss::size_.y * (3.0f / 2.0f)) + random, 0.0f }, Type::Accel);
			items_.push_back(std::unique_ptr<Item>(newItem));

			break;
		case Stage::Stage3: //爆弾のみ
			
			newItem->Initialize("bomb", &viewProjection_, &directionalLight_, { 150, (Boss::size_.y * (3.0f / 2.0f)) + random, 0.0f }, Type::Bomb);
			items_.push_back(std::unique_ptr<Item>(newItem));

			break;
		case Stage::Actual:
		default:

			std::uniform_real_distribution<float> distribution2(0.0f, 1.0f);
			float lot = distribution2(gen);

			if (boss_[0]->GetWorldPos().x < itemBorderLowLine_) {
				probabilityAccel = 0.8f;
			}else if (boss_[0]->GetWorldPos().x >= itemBorderLowLine_ && boss_[0]->GetWorldPos().x < itemBorderHighLine_) {
				probabilityAccel = 0.5f;
			}else if (boss_[0]->GetWorldPos().x >= itemBorderHighLine_) {
				probabilityAccel = 0.1f;
			}

			probabilityBomb = 1.0f - probabilityAccel;
			
			if (lot < probabilityBomb) {
				newItem->Initialize("bomb", &viewProjection_, &directionalLight_, { 150, (Boss::size_.y * (3.0f / 2.0f)) + random, 0.0f }, Type::Bomb);
			}
			else {
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
	player_->isClear_ = false;
	player_->isDead_ = false;
	cameraT_ = 0.0f;
	isCameraMove_ = false;
	stage_ = Stage::Stage1;

}

void GameScene::TitleUpdate() {

	boss_[0]->Animation();
	player_->Animation();
	player_->GravityUpdate();

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

	isSavePlayerPos_ = false;
	shakeFrame_ = 6;
	downT = 0.0f;
	waitFrame = 10;
	player_->isClear_ = false;
	player_->isDead_ = false;
	Boss::shotCount = 1;
	cameraT_ = 0.0f;
	timer = gameTime;
}

void GameScene::InGameUpdate() {

	if (--timer <= 0) {
		player_->isClear_ = true;
		isCameraMove_ = true;
		isSavePlayerPos_ = true;
		//リザルトシーンのカメラの位置の設定
		//プレイヤーの位置から少しずらしたところ
		resultCameraPos_ = {
			10.0f,
			9.0f,
			-40.0f
		};
		//
		enemyBullets_.clear();
		items_.clear();
		player_->ClearEasingInitialize();
	}

	CollisionCheck();

	clearDirection();
	gameoverDirection();

	
	skydome_->Update();
	floor_->Update();
	player_->Update();
	boss_[0]->Update();

	if (boss_[0]->IsDead()) {
		bossExplode_.emitterWorldTransform_.translation_= boss_[0]->GetWorldPos();
		BossPopComand();
		
		if (stage_ == Stage::Stage1) { stage_ = Stage::Stage2; }
		else if (stage_ == Stage::Stage2) { stage_ = Stage::Stage3; }
		else if (stage_ == Stage::Stage3) { stage_ = Stage::Actual; }
		//Boss::isBreak_ = false;
		bossExplodeFrame = 10;
	}

	if (player_->isClear_ == false && player_->isDead_ == false) {
		if (--ItemTimer <= 0) {
			PopItem();
			ItemTimer = kPopTime;
		}
		for (const auto& bullet : enemyBullets_) {
			bullet->Update();
		}
		for (const auto& items : items_) {
			items->Update();
		}
	}
	
}

void GameScene::clearDirection() {
#ifdef _DEBUG
	if (input_->TriggerKey(DIK_E) && isSavePlayerPos_ == false) {
		player_->isClear_ = true;
		isCameraMove_ = true;
		isSavePlayerPos_ = true;
		//リザルトシーンのカメラの位置の設定
		//プレイヤーの位置から少しずらしたところ
		resultCameraPos_ = {
			10.0f,
			9.0f,
			-40.0f
		};
		//
		enemyBullets_.clear();
		items_.clear();
		player_->ClearEasingInitialize();
	}
#endif // _DEBUG

	
	if (cameraT_ >= 1.0f && player_->GetIsClear()) {
		sceneRequest_ = Scene::Result;
		isCameraMove_ = false;
		cameraT_ = 0.0f;
	}
	if (isCameraMove_ && player_->GetIsClear()) {
		boss_[0]->Disappear(cameraT_);
		player_->ClearEasingUpdate(cameraT_);
		viewProjection_.translation_ = Easing::easing(cameraT_, gameCameraPos_, resultCameraPos_, 0.01f, Easing::easeNormal, false);
		viewProjection_.target_ = Easing::easing(cameraT_, gameCameraTar_, resultCameraTar_, 0.01f, Easing::easeNormal, true);
	}

	/*if (stage_ != Stage::Stage1) {
		if (--ItemTimer <= 0) {
			PopItem();
			ItemTimer = kPopTime;
		}
	}*/
	
	//skydome_->Update();
	//floor_->Update();
	//player_->Update();
	//boss_[0]->Update();

	

}
void GameScene::gameoverDirection() {
#ifdef _DEBUG
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
		enemyBullets_.clear();
		items_.clear();
	}
#endif // _DEBUG

	if (isCameraMove_ && player_->GetIsDead()) {
		boss_[0]->Disappear(cameraT_);
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

	if (enemyBullets_.size() != 0) {
		enemyBullets_.clear();
	}
	if (items_.size() != 0) {
		items_.clear();
	}
	
	boss_.clear();
}

void GameScene::ResultUpdate() {
	player_->ParticleStop();
	switch (result_) {
	    case Result::Select: //タイトルに戻るか続けるかの選択

			if (input_->TriggerKey(DIK_SPACE)) {
				result_ = Result::Translation;
				if (player_->isDead_) {
					player_->SetTranslation({0.0f,150.0f,0.0f});
					player_->SetRotation({ 0.0f,0.0f,0.0f });
				}
				BossPopComand();
			}

			switch (select_){
			    case GameScene::Selection::ToTitle:
					nextScene = Scene::Title;
					if (input_->TriggerKey(DIK_DOWN)) {
						select_ = Selection::Continue;
					}
					order_ = 1;
					stage_ = Stage::Stage1;
			    	break;
			    case GameScene::Selection::Continue:
					nextScene = Scene::InGame;
					if (input_->TriggerKey(DIK_UP)) {
						select_ = Selection::ToTitle;
					}
					order_ = 1;
					stage_ = Stage::Actual;
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

	if (player_->isClear_) {
		skydome_->Update();
		floor_->Update();
		player_->Animation();
	}

}
