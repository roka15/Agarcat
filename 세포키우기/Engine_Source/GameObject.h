#pragma once
#include "Entity.h"
#include "Mesh.h"
#include "Shader.h"
#include "Renderer.h"
#define ITEM_COLOR 0x000000ff // ARGB

namespace roka
{
	using namespace math;
	using namespace renderer;
	using namespace enums;
	class GameObject : public Entity
	{
	public:
		enum class EState
		{
			Active,
			Paused,
			Dead,
		};
		enum class EMapOutType
		{
			OUTFALSE,
			OUTTRUE_LEFTOUT,
			OUTTRUE_RIGHTOUT,
			OUTTRUE_UPOUT,
			OUTTRUE_DOWNOUT
		};
		enum class ECollisionDirType
		{
			COL_FALSE,
			X_TRUE,
			Y_TRUE,
		};
		
		GameObject(EGameObjectType type);
		virtual ~GameObject();

		virtual void Initialize();
		virtual void Update();
		virtual void LateUpdate();
		virtual void Render();

		void SetCenter(const Vector2& center) { mCenter = center; }
		const Vector2& GetCenter() { return mCenter; }
		void SetLength(const float& length) { mLength = length; }
		const float& GetLength() { return mLength; }
		const EState& GetState() { return mState; }
		void SetState(EState state) { mState = state; }
		std::vector<Vertex>& GetVertexs() { return mVertexs; }
		std::vector<UINT>& GetIndexs() { return mIndexs; }
		const Vector4& GetCurPosDiff() { return mCurPosValue; }

		Vector2 ConvertGPUPos(Vector2 value);
		
		virtual bool InitCheckCollision();
		virtual GameObject* CheckCollision(ECollisionDirType& dirType);
		virtual EMapOutType CheckMapOut();
		const EGameObjectType& GetType() { return mType; }
	
	protected:
		virtual void SetBufferInfo(std::vector<Vertex>& vertexs, std::vector<UINT>& indexs);
		virtual void LoadBuffer();
		virtual void LoadShader();
		virtual void SetupState();
		virtual void AddLength(float size);
		Color GetRandomColor();
	
		const Vector2& GetDir() { return mDir; }
		void SetDir(const Vector2& dir) { mDir = dir; }
		
		Mesh* GetMesh() { return mMesh; }
		Shader* GetShader() { return mShader; }
		
	private:
		EState mState;
		Mesh* mMesh;
		Shader* mShader;
		Vector2 mCenter;
		float mLength;
		const UINT mSpeed;
		Vector2 mDir;
		Vector4 mCurPosValue;
		EGameObjectType mType;
		std::vector<Vertex> mVertexs;
		std::vector<UINT> mIndexs;
		graphics::ConstantBuffer* mConstBuffer;
	
	};
}


