#pragma once
#include "GameObject.h"
namespace roka
{
    class RokaEllipse :
        public GameObject
    {
    public:
        RokaEllipse(EGameObjectType type);
        virtual ~RokaEllipse();
        virtual void Initialize();
        virtual void Update();
        virtual void LateUpdate();
        virtual void Render();

        virtual GameObject* CheckCollision(ECollisionDirType& dirType);
        virtual EMapOutType CheckMapOut();
    protected:
        virtual void SetBufferInfo(std::vector<Vertex>& vertexs, std::vector<UINT>& indexs)override;
        virtual void LoadBuffer();
        virtual void LoadShader();
        virtual void SetupState();
        virtual void AddLength(float size);
    private:

    };
}


