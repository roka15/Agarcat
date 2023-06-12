#include "GameObject.h"
#include "RokaGraphicDevice_Dx11.h"
#include "Application.h"
#include "RokaTime.h"

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
					size = other_length /10 + ((float)(other_length %10)/10.0f);
					AddLength(size);
					colObj->SetState(EState::Dead);
					application.GetActiveScene()->RegisterDeadItem(colObj);
					application.GetActiveScene()->DeadItemErase(colObj);
					break;
				case EGameObjectType::Monster:
				case EGameObjectType::Player:
				    //상대와 크기가 동일하면 그냥 팅겨져 나가기
				{
					
					if (original_length == other_length)
					{
						Vector2 dir = colObj->GetDir();
						if (dirType == ECollisionDirType::X_TRUE)
						{
							mDir.x *= -1;
							dir.x *= -1;
						}
						else
						{
							mDir.y *= -1;
							dir.y *= -1;
						}
						colObj->SetDir(dir);
					}
					//다르다면 큰 쪽이 흡수
					else if(original_length>other_length)
					{
						size = other_length / 10 + ((float)(other_length % 10) / 10.0f);
						AddLength(size);
						colObj->SetState(EState::Dead);
						application.GetActiveScene()->RegisterDeleteObject(colObj);
					}
			
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
			mCurPosValue.x += outRange / (winsize.x / 2);
			break;
		case EMapOutType::OUTTRUE_RIGHTOUT:
			mDir.x *= -1;
			mCurPosValue.x -= outRange / (winsize.x / 2);
			break;
		case EMapOutType::OUTTRUE_UPOUT:
			mDir.y *= -1;
			mCurPosValue.y -= outRange / (winsize.y / 2);
			break;
		case EMapOutType::OUTTRUE_DOWNOUT:
			mDir.y *= -1;
			mCurPosValue.y += outRange / (winsize.y / 2);
			break;
		}

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
	void GameObject::LoadBuffer()
	{
		if (mMesh == nullptr)
			mMesh = new Mesh();
		if (mConstBuffer == nullptr)
		{
			mConstBuffer = new roka::graphics::ConstantBuffer(ECBType::Transform);
			mConstBuffer->Create(sizeof(Vector4));
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

