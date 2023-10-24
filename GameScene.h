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
#include <vector>
#include <optional>
#include "Collider.h"
#include <sstream>
#include "Collapse.h"
#include "OrbitParticle.h"

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

	void CollisionCheck();

	void AddEnemyBullet(EnemyBullet* enemyBullet);

	void PopItem();

	void BossPop(int hp, float speed, int second);

	void LoadBossPopData();

	void BossPopComand();

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
	std::vector<std::unique_ptr<Boss>> boss_;

	std::stringstream BossPopCommands_;
	int order_ = 1;
	std::list<std::unique_ptr<EnemyBullet>> enemyBullets_;
	float refBulletSpeed_ = 1.0f;

	const int kPopTime = 60 * 5;
	int ItemTimer = kPopTime;
	std::list<std::unique_ptr<Item>> items_;
	//出現確率
	float probabilityAccel = 0.5;
	float probabilityBomb = 1.0f - probabilityAccel;

	float itemBorderHighLine_ = 50.0f;
	float itemBorderLowLine_ = -50.0f;

	std::unique_ptr<Particle> particle_;


	
  
  Collider tmpCollider_;

  //gameover用
  Vector3 gameoverPlayerPos_ = { 0.0f,50.0f,-180.0f };
  Vector3 gameoverPlayerRotation = { 0.0f,Radian(180.0f),Radian(360.0f * 8.0f + 45.0f)};

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

	enum class Stage {
		Stage1,
		Stage2,
		Stage3,
		Actual,
	};

	Scene scene_ = Scene::Title;
	Scene nextScene = Scene::Title;
	static void (GameScene:: *SceneTable[])();
	std::optional<Scene> sceneRequest_ = std::nullopt;

	Stage stage_ = Stage::Stage3;

	//title or continue
	Selection select_ = Selection::ToTitle;
	//選択とカメラ移動
	Result result_ = Result::Select;

	bool isSavePlayerPos_ = false;
	int shakeFrame_ = 6;
	Vector3 shakeValue = { 0.2f,0.2f,0.2f };
	float downT = 0.0f;
	int waitFrame = 10;

	//後ろの岩
	Collapse collapse_;

	OrbitParticle orbit_;
public:
	//タイトル
	void TitleInitialize();
	void TitleUpdate();
	//インゲーム
	void InGameInitialize();
	void InGameUpdate();
	void clearDirection();
	void gameoverDirection();
	//リザルト
	void ResultInitialize();
	void ResultUpdate();

};

