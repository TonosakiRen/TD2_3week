#pragma once
#include "GameObject.h"
#include "Input.h"
#include "ParticleBox.h"
#include "DustParticle.h"
class Boss :
    public GameObject
{
public:

    void Initialize(ViewProjection* viewProjection, DirectionalLight* directionalLight);
    void Update();
    void Animation();
    void Draw();
    void ParticleDraw();
public:

private:
    Input* input_ = nullptr;
    enum parts {
        Head,
        Tin,

        partNum
    };
    Model modelParts_[partNum];
    WorldTransform partsTransform_[partNum];
    bool isJump_ = false;

    //animation
    float animationT_ = 0.0f;
    float animationSpeed_ = 0.01f;

    Vector3 accelaration_ = { 0.0f,-0.025f,0.0f };
    Vector3 velocity_ = { 0.0f,0.0f,0.0f };
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
};

