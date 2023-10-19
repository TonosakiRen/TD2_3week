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
	
	ViewProjection viewProjection_;
	DirectionalLight directionalLight_;
	bool isCameraMove_ = false;
	float cameraT_ = 0.0f;

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

	Collider tmpCollider_;
};

