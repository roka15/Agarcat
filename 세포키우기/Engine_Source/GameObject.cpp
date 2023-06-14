#include "GameObject.h"
#include "RokaGraphicDevice_Dx11.h"
#include "Application.h"
#include "RokaTime.h"
#include "Input.h"

extern roka::Application application;

namespace roka
{

	//graphics::ConstantBuffer* GameObject::mConstBuffer = nullptr;
	GameObject::GameObject(EGameObjectType type)
		:mType(type)
		, mState(EState::Active)
		, mSpeed(2)
	{
	}

	GameObject::~GameObject()
	{
	}

	void GameObject::Initialize()
	{
		Vector2 winsize;
		winsize.x = application.GetWidth();
		winsize.y = application.GetHeight();
		mLength = 10;

		if (mType == EGameObjectType::Monster)
		{
			UINT dgree = 0;
			dgree = application.GetRandomValue(1, 360);
			mDir.x = std::cos(dgree);
			mDir.y = std::sin(dgree);

			bool is_positive = false;
			is_positive = application.GetRandomValue(0, 1);
			if (is_positive == 0)
				mDir *= -1;

			mDir.Normalize();
		}
		do
		{
			do
			{
				mCenter.x = application.GetRandomValue(1, winsize.x);
			} while (mCenter.x - mLength < 0 || mCenter.x + mLength > winsize.x);

			do
			{
				mCenter.y = application.GetRandomValue(1, winsize.y);
			} while (mCenter.y - mLength < 0 || mCenter.y + mLength > winsize.y);

		} while (InitCheckCollision());

		LoadBuffer();

		if (mState != EState::Active)
		{
			mState = EState::Active;
		}
		else
		{
			LoadShader();
			SetupState();
		}
	}

	void GameObject::Update()
	{
		if (mState != EState::Active)
			return;
		if (mType == EGameObjectType::Item)
			return;

		float speed = 1.0; //0.1;
		switch (mType)
		{
		case EGameObjectType::Monster:
			//방향값으로 이동
			mCurPosValue.x += mDir.x * speed * Time::DeltaTime();
			mCurPosValue.y += mDir.y * speed * Time::DeltaTime();
			break;
		case EGameObjectType::Player:
			//키 입력 이동
			bool xkeyDownFlag = false;
			bool ykeyDownFlag = false;
			if (Input::GetKey(EKeyCode::LEFT))
			{
				mDir.x = -1;
				xkeyDownFlag = true;
			}
			if (Input::GetKey(EKeyCode::RIGHT))
			{
				mDir.x = 1;
				xkeyDownFlag = true;
			}
			if (Input::GetKey(EKeyCode::UP))
			{
				mDir.y = 1;
				ykeyDownFlag = true;
			}
			if (Input::GetKey(EKeyCode::DOWN))
			{
				mDir.y = -1;
				ykeyDownFlag = true;
			}
			if (xkeyDownFlag == true)
				mCurPosValue.x += mDir.x * speed * Time::DeltaTime();
			if (ykeyDownFlag == true)
				mCurPosValue.y += mDir.y * speed * Time::DeltaTime();
			break;
		}

		ECollisionDirType dirType;
		GameObject* colObj = CheckCollision(dirType);
		float size = 0;;

		if (colObj != nullptr)
		{
			int original_length = GetLength();
			int other_length = colObj->GetLength();
			EGameObjectType type = colObj->GetType();
			if (colObj->GetState() == EState::Active)
			{
				switch (type)
				{
				case EGameObjectType::Item:
					//+10
					size = other_length / 10 + ((float)(other_length % 10) / 10.0f);
					AddLength(size);
					colObj->SetState(EState::Dead);
					application.GetActiveScene()->RegisterDeadItem(colObj);
					application.GetActiveScene()->DeadItemErase(colObj);
					break;
				case EGameObjectType::Monster:
				case EGameObjectType::Player:
					//상대와 크기가 동일하면 그냥 팅겨져 나가기
					if (original_length == other_length)
					{

						Vector2 dir = colObj->GetDir();
						if (dirType == ECollisionDirType::X_TRUE)
						{
							if (GetType() != EGameObjectType::Player)
								mDir.x *= -1;
							if (type != EGameObjectType::Player)
								dir.x *= -1;
						}
						else
						{
							if (GetType() != EGameObjectType::Player)
								mDir.y *= -1;
							if (type != EGameObjectType::Player)
								dir.y *= -1;
						}
						if (type != EGameObjectType::Player)
							colObj->SetDir(dir);
					}

					if (original_length > other_length)
					{
						size = other_length / 10 + ((float)(other_length % 10) / 10.0f);
						AddLength(size);
						colObj->SetState(EState::Dead);
						application.GetActiveScene()->RegisterDeleteObject(colObj);
					}
					break;

				}
			}
		}


		Vector2 winsize = {};
		winsize.x = application.GetWidth();
		winsize.y = application.GetHeight();

		UINT outRange = 10;
		switch (CheckMapOut())
		{
		case EMapOutType::OUTTRUE_LEFTOUT:
			mDir.x *= -1;
			if (mType == EGameObjectType::Player)
			{
				mCurPosValue.x += (outRange / winsize.x) * 2;
			}
			else
			mCurPosValue.x += outRange / (winsize.x / 2);
			break;
		case EMapOutType::OUTTRUE_RIGHTOUT:
			mDir.x *= -1;
			if (mType == EGameObjectType::Player)
			{
				mCurPosValue.x -= (outRange / winsize.x) *2;
			}
			else
			mCurPosValue.x -= outRange / (winsize.x / 2);
			break;
		case EMapOutType::OUTTRUE_UPOUT:
			mDir.y *= -1;
			if (mType == EGameObjectType::Player)
			{
				mCurPosValue.y -= (outRange / winsize.y) * 2;
			}
			else
			mCurPosValue.y -= outRange / (winsize.y / 2);
			break;
		case EMapOutType::OUTTRUE_DOWNOUT:
			mDir.y *= -1;
			if (mType == EGameObjectType::Player)
			{
				mCurPosValue.y += (outRange / winsize.y) * 2;
			}
			else
			mCurPosValue.y += outRange / (winsize.y / 2);
			break;
		}

		


	}

	void GameObject::LateUpdate()
	{
	}

	void GameObject::Render()
	{
		if (mState != EState::Active)
			return;
		mMesh->BindBuffer();
		mShader->Binds();
		mConstBuffer->SetData(&mCurPosValue);
		mConstBuffer->Bind(EShaderStage::VS);
		graphics::GetDevice()->DrawIndexed(mMesh->GetIndexCount(), 0, 0);
	}
	Vector2 GameObject::ConvertGPUPos(Vector2 value)
	{
		Vector2 winsize = {};
		winsize.x = application.GetWidth();
		winsize.y = application.GetHeight();

		value.x -= winsize.x / 2;
		value.y -= winsize.y / 2;

		value.x /= winsize.x / 2;
		value.y /= winsize.y / 2;

		return Vector2(value);
	}
	bool GameObject::InitCheckCollision()
	{
		Scene* scene = application.GetActiveScene();
		const std::vector<GameObject*>& objs = scene->GetObjects();

		bool flag = false;
		if (objs.size() == 0)
			return false;
		for (auto& obj : objs)
		{
			Vector2 center = obj->GetCenter();
			UINT length = obj->GetLength();

			if (mCenter.x == center.x && mCenter.y == center.y)
				return true;

			if (mCenter.x + mLength < center.x - length || mCenter.x - mLength > center.x + length)
				continue;
			else
			{
				if (mCenter.y + mLength < center.y - length || mCenter.y - mLength > center.y + length)
					continue;
				else
					return true;
			}
		}
		return false;
	}
	GameObject* GameObject::CheckCollision(ECollisionDirType& dirType)
	{
		Scene* scene = application.GetActiveScene();
		Vector2 winsize = {};
		winsize.x = application.GetWidth();
		winsize.y = application.GetHeight();
		const std::vector<GameObject*>& objs = scene->GetObjects();
		const std::vector<Vertex>& this_vertex = GetVertexs();
		const Vector4& this_diff = GetCurPosDiff();
		UINT length = GetLength();
		Vector2 range = {};
		range.x = length / (winsize.x / 2);
		range.y = length / (winsize.y / 2);
		Vector2 center = {};
		center.x = this_vertex[0].pos.x + this_diff.x + range.x;
		center.y = this_vertex[0].pos.y + this_diff.y - range.y;
		bool flag = false;
		if (objs.size() == 0)
			return nullptr;
		for (auto& obj : objs)
		{
			if (obj == this)
				continue;

			const std::vector<Vertex>& vertex = obj->GetVertexs();
			const Vector4& diff = obj->GetCurPosDiff();
			const UINT& objlength = obj->GetLength();

			Vector2 objrange = {};
			objrange.x = objlength / winsize.x / 2;
			objrange.y = objlength / winsize.y / 2;
			Vector2 objcenter = {};
			objcenter.x = vertex[0].pos.x + diff.x + objrange.x;
			objcenter.y = vertex[0].pos.y + diff.y - objrange.y;

			float xdiff = std::fabsf(center.x - objcenter.x);
			float ydiff = std::fabsf(center.y - objcenter.y);

			bool xflag = false;
			bool yflag = false;

			if (xdiff <= range.x + objrange.x)
			{
				xflag = true;
			}
			if (ydiff <= range.y + objrange.y)
			{
				yflag = true;
			}

			if (xflag == true && yflag == true)
			{
				if (xdiff < ydiff)
				{
					dirType = ECollisionDirType::Y_TRUE;
				}
				else
				{
					dirType = ECollisionDirType::X_TRUE;
				}
				return obj;
			}
		}
		return nullptr;
	}
	GameObject::EMapOutType GameObject::CheckMapOut()
	{
		/*if (mCenter.x + mLength + (mDir.x * mMoveCnt) >= 1)
			return true;
		if (mCenter.x - mLength - (mDir.x * mMoveCnt) <= -1)
			return true;

		if (mCenter.y + mLength + (mDir.y * mMoveCnt) >= 1)
			return true;
		if (mCenter.y - mLength - (mDir.y * mMoveCnt) <= -1)
			return true;*/


		return EMapOutType::OUTFALSE;
	}
	void GameObject::SetBufferInfo(std::vector<Vertex>& vertexs, std::vector<UINT>& indexs)
	{
	}
	void GameObject::LoadBuffer()
	{
		if (mMesh == nullptr)
			mMesh = new Mesh();
		if (mConstBuffer == nullptr)
		{
			mConstBuffer = new roka::graphics::ConstantBuffer(ECBType::Transform);
			mConstBuffer->Create(sizeof(Vector4));
		}

		SetBufferInfo(mVertexs, mIndexs);

		if (GetState() == EState::Active)
		{
			mMesh->CreateVertexBuffer(mVertexs.data(), mVertexs.size());
			mMesh->CreateIndexBuffer(mIndexs.data(), mIndexs.size());
		}
		else
		{
			mMesh->UpdateVertexBuffer(mVertexs);
		}
	}
	void GameObject::LoadShader()
	{
		mShader = new Shader();
		mShader->Create(EShaderStage::VS, L"TriangleVS.hlsl", "main");
		mShader->Create(EShaderStage::PS, L"TrianglePS.hlsl", "main");
	}
	void GameObject::SetupState()
	{
		// Input layout 정점 구조 정보를 넘겨줘야한다.
		D3D11_INPUT_ELEMENT_DESC arrLayout[2] = {};

		//첫번째 요소 정보 정의(Vertex)
		arrLayout[0].AlignedByteOffset = 0; // 시작 offet
		arrLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT; // float3
		arrLayout[0].InputSlot = 0;//등록된 vertex buffer / index buffer가 여럿 존재할 때 몇번째 buffer를 쓸 것인지.
		arrLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		arrLayout[0].SemanticName = "POSITION";//hlsl 식별
		arrLayout[0].SemanticIndex = 0;//동일한 semantic이름을 가진 입력값의 구분을 위한 index
		//두번째 요소 정보 정의(Color)
		arrLayout[1].AlignedByteOffset = 12;// 시작 offset
		arrLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;//float4
		arrLayout[1].InputSlot = 0;
		arrLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		arrLayout[1].SemanticName = "COLOR";
		arrLayout[1].SemanticIndex = 0;

		roka::graphics::GetDevice()->CreateInputLayout(arrLayout, 2
			, mShader->GetVSCode(), mShader->GetInputLayoutAddressOf());
	}
	Color GameObject::GetRandomColor()
	{
		std::vector<UINT> color = {};
		for (int i = 0; i < 3; i++)
		{
			color.push_back(application.GetRandomValue(0, 255));
		}
		return Color(color[0], color[1], color[2]);
	}
	void GameObject::AddLength(float size)
	{

	}
}

