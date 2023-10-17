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
    Vector2 velocity_;
    Vector2 accelaration_;
    bool isJump_ = false;

    //animation
    float animationT_ = 0.0f;
    float animationBodyT_ = 0.0f;
    float animationSpeed_ = 0.05f;
    float animationBodySpeed_ = animationSpeed_ * 2.0f;
    float runUpAnimation_ = 0.08f;

    //particle
    std::unique_ptr<DustParticle> dustParticle_;
};

