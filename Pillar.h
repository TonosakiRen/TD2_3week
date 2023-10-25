#pragma once
#include "GameObject.h"
class Pillar :
    public GameObject
{
public:

    void Initialize(const std::string name, ViewProjection* viewProjection, DirectionalLight* directionalLight, Vector3 initialScale, Vector3 intialPos);
    void Update(bool isMove);
};

