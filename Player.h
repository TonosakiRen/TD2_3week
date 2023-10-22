#pragma once
#include "GameObject.h"
#include "Input.h"
#include "ParticleBox.h"
#include "DustParticle.h"
#include <optional>
#include "Collider.h"

class Player :
    public GameObject
{
public:
   
    void Initialize(const std::string name, ViewProjection* viewProjection, DirectionalLight* directionalLight);
    void Update();
    void Animation();
    void ParticleStop();
    void Draw();
    void ParticleDraw();

    void GravityUpdate();

    void Accel();

    void Explosion();

    Vector3 GetCharaWorldPos() const;
    Vector3 GetRefWorldPos() const;


    bool isDead_ = false;
    bool isClear_ = false;
 
    bool GetIsDead() {
        return isDead_;
    }
    bool GetIsClear() {
        return isClear_;
    }

    void SetIsEmit() {
        dustParticle_->SetIsEmit(false);
    }

    void SetTranslation(Vector3 translation) {
        worldTransform_.translation_ = translation;
    }

    void SetRotation(Vector3 rotation) {
        worldTransform_.rotation_ = rotation;
    }

    bool IsAttack() const { return isAttack_; }

private: //行動系

    void RootInitialize();

    void RootUpdate();

    void AccelInitialize();

    void AccelUpdate();

    void BombHitInitialize();

    void BombHitUpdate();

public:
    //collider
    Collider collider_;
    Collider reflectCollider_;

private:
    Input* input_ = nullptr;
    enum parts {
        LeftArm,
        RightArm,
        LeftLeg,
        RightLeg,

        partNum
    };
    Model modelParts_;
    WorldTransform partsTransform_[partNum];
    //Vector2 velocity_;
    //Vector2 accelaration_;
    bool isJump_ = false;

    //animation
    float animationT_ = 0.0f;
    float animationBodyT_ = 0.0f;
    float animationSpeed_ = 0.05f;
    float animationBodySpeed_ = animationSpeed_ * 2.0f;
    float runUpAnimation_ = 0.2f;

    //particle
    std::unique_ptr<DustParticle> dustParticle_;

    enum class Behavior {
        kRoot,
        kAccel,
        kBombHit,
    };
    Behavior behavior_ = Behavior::kRoot;
    std::optional<Behavior> behaviorRequest_ = std::nullopt;

    WorldTransform reflectWT_;
    Vector3 size_{};

    float firstSpeed_ = 2.0f;
    Vector3 velocity_{};
    Vector3 acceleration_{};
    float gravity_ = 0.1f;

    bool isAttack_ = false;
    const int kAttackTime = 15;
    int attackTimer = kAttackTime;

    float num = 0.0f;
    Vector3 easeStart{};
    Vector3 easeEnd{};
};

