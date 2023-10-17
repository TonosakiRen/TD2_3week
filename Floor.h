#pragma once
#include "GameObject.h"
class Floor :
    public GameObject
{
public:

    void Initialize(const std::string name, ViewProjection* viewProjection, DirectionalLight* directionalLight);
    void Update();
};

