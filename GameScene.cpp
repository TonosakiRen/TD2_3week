#include "GameScene.h"
#include "externals/imgui/imgui.h"
#include <cassert>
#include "Easing.h"

using namespace DirectX;

GameScene::GameScene() {};

GameScene::~GameScene() {};

void GameScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	
	viewProjection_.Initialize();

	viewProjection_.translation_ = { 0.0f,8.6f,-27.0f };
	viewProjection_.target_ = { 0.0f,0.0f,0.0f };

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
	if (input_->TriggerKey(DIK_SPACE)) {
		isCameraMove_ = true;
	}
	if (isCameraMove_) {
		viewProjection_.translation_ = Easing::easing(cameraT_, { 11.1f,4.2f,0.11f }, { 0.0f,8.6f,-27.0f }, 0.01f, Easing::easeNormal, false);
		viewProjection_.target_ = Easing::easing(cameraT_, { 0.0f,-1.6f,0.0f }, { 0.0f,0.0f,0.0f }, 0.01f, Easing::easeNormal, true);
	}
	
	skydome_->Update();
	floor_->Update();
	player_->Update();
	boss_->Update();
}

void GameScene::ModelDraw()
{
	skydome_->Draw();
	floor_->Draw();
	player_->Draw();
	boss_->Draw();
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