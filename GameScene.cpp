#include "GameScene.h"
#include "externals/imgui/imgui.h"
#include <cassert>
#include "Easing.h"
#include <random>

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

	boss_ = std::make_unique<Boss>();
	boss_->Initialize(&viewProjection_, &directionalLight_);
	boss_->SetPlayer(player_.get());
	boss_->SetGameScene(this);

	sphere_.reset(GameObject::Create("sphere", &viewProjection_, &directionalLight_));

	particle_.reset(Particle::Create(10));
	
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

	
	(this->*SceneTable[static_cast<size_t>(scene_)])();

	

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

void GameScene::AddEnemyBullet(EnemyBullet* enemyBullet) {
	enemyBullets_.push_back(std::unique_ptr<EnemyBullet>(enemyBullet));
}

void GameScene::PopItem() {

	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_real_distribution<float> distribution(-(Boss::size_.y / 2.0f) + 2.0f, (Boss::size_.y / 2.0f) - 2.0f);
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



}

void GameScene::TitleUpdate() {

	boss_->Animation();
	player_->Animation();

	if (input_->TriggerKey(DIK_SPACE)) {
		isCameraMove_ = true;
	}
	if (cameraT_ >= 1.0f) {
		isBossAppear_ = true;
		isCameraMove_ = false;
		cameraT_ = 0.0f;
	}
	if (bossT_ >= 1.0f) {
		scene_ = Scene::InGame;
		bossT_ = 0.0f;
		isBossAppear_ = false;
	}
	if (isCameraMove_) {
		viewProjection_.translation_ = Easing::easing(cameraT_, titleCameraPos_, gameCameraPos_, 0.01f, Easing::easeNormal, false);
		viewProjection_.target_ = Easing::easing(cameraT_, titleCameraTar_, gameCameraTar_, 0.01f, Easing::easeNormal, true);
	}
	if (isBossAppear_) {
		boss_->Appear(bossT_);
	}

	skydome_->Update();
	floor_->Update();
}

void GameScene::InGameInitialize() {



}

void GameScene::InGameUpdate() {

	if (input_->TriggerKey(DIK_E)) {
		isCameraMove_ = true;
		resultCameraPos_ = {
			player_->GetCharaWorldPos().x + 10.0f,
			player_->GetCharaWorldPos().y,
			player_->GetCharaWorldPos().z - 40.0f
		};
	}
	if (cameraT_ >= 1.0f) {
		scene_ = Scene::Result;
		isCameraMove_ = false;
		cameraT_ = 0.0f;
	}
	if (isCameraMove_) {
		viewProjection_.translation_ = Easing::easing(cameraT_, gameCameraPos_, resultCameraPos_, 0.01f, Easing::easeNormal, false);
		viewProjection_.target_ = Easing::easing(cameraT_, gameCameraTar_, resultCameraTar_, 0.01f, Easing::easeNormal, true);
	}
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

void GameScene::ResultInitialize() {



}

void GameScene::ResultUpdate() {

	switch (result_) {
	    case Result::Select:
			switch (select_){
			    case GameScene::Selection::ToTitle:



			    	break;
			    case GameScene::Selection::Continue:



			    	break;
			    default:
			    	break;
			}

	    	break;
		case Result::Translation:

			break;
	}

}
