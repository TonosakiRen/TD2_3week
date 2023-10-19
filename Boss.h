#pragma once
#include "GameObject.h"
#include "ParticleBox.h"
#include "DustParticle.h"
#include <optional>
#include "Collider.h"

class GameScene;
class Player;

class Boss :
    public GameObject
{
public:

    void Initialize(ViewProjection* viewProjection, DirectionalLight* directionalLight);
    void Update();
    void Animation();
    void Draw();
    void ParticleDraw();
    void Appear(float& t);
    void OnRefCollision();
    void SpeedUp();
    void Explosion();

    void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }
    void SetPlayer(Player* player) { player_ = player; }

    Vector3 GetWorldPos() const;
    Vector3 GetMouthWorldPos() const;

private: //行動系

    //通常行動初期化
    void RootInitialize();
    //通常行動更新
    void RootUpdate();
    //弾的中初期化
    void HitInitialize();
    //弾的中更新
    void HitUpdate();
    //爆弾的中初期化
    void BombHitInitialize();
    //爆弾的中更新
    void BombHitUpdate();
public:
    //collider
    Collider collider_;
    Collider mouthCollider_;
private:
    enum parts {
        Head,
        Tin,

        partNum
    };
    Model modelParts_[partNum];
    WorldTransform partsTransform_[partNum];

    //animation
    float animationT_ = 0.0f;
    float animationSpeed_ = 0.01f;

    Vector3 animationAccelaration_ = { 0.0f,-0.025f,0.0f };
    Vector3 animationVelocity_ = { 0.0f,0.0f,0.0f };
    float jumpPower_ = 0.25f;

    enum AnimationStats {
        HeadJump,
        AllJump,
        HeadDown,
        AllDown
    };

    AnimationStats animationStats = HeadJump;

    //particle
    std::unique_ptr<DustParticle> dustParticle_;

    enum class Behavior {
        kRoot,
        kHit,
        kBombHit,
        kBreak,
    };
    Behavior behavior_ = Behavior::kRoot;
    std::optional<Behavior> behaviorRequest_ = std::nullopt;

    WorldTransform mouthWT_;
    Vector3 velocity_ = {};

    float bulletSpeed_ = 0.5f;
    Vector3 bulletVelocity_{};

    const int kAttackTime = 60 * 2;
    int attackTimer = kAttackTime;

    GameScene* gameScene_ = nullptr;
    Player* player_ = nullptr;

    bool isDead_ = false;

    float num = 0.0f;
    Vector3 easeStart{};
    Vector3 easeEnd{};


    Vector3 startPos_ = {-250.0f,0.0f,0.0f};
    Vector3 endPos_ = { -150.0f,0.0f,0.0f };

public:

    static Vector3 size_;
    static Vector3 mouthSize_;

};

