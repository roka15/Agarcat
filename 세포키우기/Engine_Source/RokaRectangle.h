#pragma once
#include "GameObject.h"

namespace roka
{
    class RokaRectangle : public GameObject
    {
    public:
        RokaRectangle(EGameObjectType type);
        ~RokaRectangle();

        virtual void Initialize()override;
        virtual void Update()override;
        virtual void LateUpdate()override;
        virtual void Render()override;
        virtual bool InitCheckCollision()override;
        virtual GameObject* CheckCollision(ECollisionDirType& dirType)override;
        virtual EMapOutType CheckMapOut()override;
    protected:
        virtual void LoadBuffer()override;
        virtual void LoadShader()override;
        virtual void SetupState()override;
        virtual void AddLength(float size)override;
    private:
        
    };
}

