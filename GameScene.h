#pragma once
#include "DirectXCommon.h"
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Input.h"
#include "Audio.h"
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
#include "Pillar.h"
#include "ExplodeParticle.h"
#include "BossExplode.h"

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
	Audio* audio_ = nullptr;
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

	int gameTime = 60 * 90;
	int timer = gameTime;
  
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

	Stage stage_ = Stage::Stage1;

	//title or continue
	Selection select_ = Selection::ToTitle;
	//選択とカメラ移動
	Result result_ = Result::Select;

	bool isSavePlayerPos_ = false;
	int shakeFrame_ = 0;
	Vector3 shakeValue = { 0.2f,0.2f,0.2f };
	float downT = 0.0f;
	int waitFrame = 10;

	int collapseFrame = 0;

	//後ろの岩
	Collapse collapse_;

	OrbitParticle orbit_;

	static const int pillarNum = 10;
	Pillar pillar_[pillarNum];

	ExplodeParticle explodePlayerParticle_;
	ExplodeParticle explodeBossParticle_;

	ExplodeParticle speedUpPlayerParticle_;
	ExplodeParticle speedUpBossParticle_;

	int explodeShakeFrame_ = 0;

	BossExplode bossExplode_;

	uint32_t blockHandle_;
	int bossExplodeFrame = 0;

	std::unique_ptr< Sprite> titleSprite_;
	float titleT_ = 0.0f;

	


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

private: //スプライト

	uint32_t titleHandle_;
	std::unique_ptr<Sprite> title_;
	bool titleFlag = true;

	uint32_t progressBarHandle_;
	std::unique_ptr<Sprite> progressBar_;

	uint32_t progressPlayerHandle_;
	std::unique_ptr<Sprite> progressPlayer_;

	Vector2 progressPlayerStartPos_ = { 400.0f,650.0f };
	Vector2 progressPlayerEndPos_ = { 880.0f,650.0f };
	float progressT_ = 0.0f;

	uint32_t hpGaugeHandle_;
	std::unique_ptr<Sprite> hpGauge_;

	uint32_t hpBarHandle_;
	std::unique_ptr<Sprite> hpBar_;

};

