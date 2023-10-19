#pragma once
#include "DirectXCommon.h"
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Input.h"
#include "Sprite.h"
#include "DirectionalLight.h"
#include "Particle.h"
#include "GameObject.h"
#include "Skydome.h"
#include "Floor.h"
#include "Player.h"
#include "Boss.h"
#include "EnemyBullet.h"
#include "Item.h"
#include <list>
#include <optional>
#include "Collider.h"


class GameScene
{

public:
	GameScene();
	~GameScene();

	void Initialize();
	void Update();
	void ModelDraw();
	void ParticleDraw();
	void ParticleBoxDraw();
	void PreSpriteDraw();
	void PostSpriteDraw();
	void Draw();

	void AddEnemyBullet(EnemyBullet* enemyBullet);

	void PopItem();

private: 
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	DirectionalLight directionalLight_;

	//カメラ
	ViewProjection viewProjection_;
	bool isCameraMove_ = false;
	float cameraT_ = 0.0f;
	//タイトル用
	Vector3 titleCameraPos_ = { 70.0f,1.0f,0.0f };
	Vector3 titleCameraTar_ = { 0.0f,-1.55f,-0.3f };
	//インゲーム用
	Vector3 gameCameraPos_ = { 0.0f,50.0f,-200.0f };
	Vector3 gameCameraTar_ = {};
	//リザルト用
	Vector3 resultCameraPos_ = {};
	Vector3 resultCameraTar_ = {};

	uint32_t textureHandle_;

	std::unique_ptr<Sprite>sprite_;
	WorldTransform spriteTransform_;

	std::unique_ptr<Skydome> skydome_;
	std::unique_ptr<Floor> floor_;
	std::unique_ptr<GameObject> sphere_;
	std::unique_ptr<Player> player_;
	std::unique_ptr<Boss> boss_;

	std::list<std::unique_ptr<EnemyBullet>> enemyBullets_;

	const int kPopTime = 60 * 5;
	int ItemTimer = kPopTime;
	std::list<std::unique_ptr<Item>> items_;

	std::unique_ptr<Particle> particle_;


	bool isBossAppear_ = false;
	float bossT_ = 0.0f;
  
  Collider tmpCollider_;

private: //シーン用

	enum class Scene {
		Title,
		InGame,
		Result,
	};
	
	enum class Selection {
		ToTitle,
		Continue,
	};

	enum class Result {
		Select,
		Translation,
	};

	Scene scene_ = Scene::Title;
	static void (GameScene:: *SceneTable[])();
	std::optional<Scene> sceneRequest_ = std::nullopt;

	Selection select_ = Selection::ToTitle;
	Result result_ = Result::Select;

public:

	void TitleInitialize();
	void TitleUpdate();
	void InGameInitialize();
	void InGameUpdate();
	void ResultInitialize();
	void ResultUpdate();



};

