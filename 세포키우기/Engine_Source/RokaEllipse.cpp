#include "RokaEllipse.h"

#include "Application.h"
extern roka::Application application;
namespace roka
{
	RokaEllipse::RokaEllipse(EGameObjectType type) :GameObject(type)
	{
	}

	RokaEllipse::~RokaEllipse()
	{
	}

	void RokaEllipse::Initialize()
	{
		SetName(L"Ellipse");
		GameObject::Initialize();
	}

	void RokaEllipse::Update()
	{
		GameObject::Update();
	}

	void RokaEllipse::LateUpdate()
	{
		GameObject::LateUpdate();
	}

	void RokaEllipse::Render()
	{
		GameObject::Render();
	}

	GameObject* RokaEllipse::CheckCollision(ECollisionDirType& dirType)
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
		range.x = (length / winsize.x) * 2;
		range.y = (length / winsize.y) * 2;
		Vector2 center = {};
		center.x = this_vertex[0].pos.x + this_diff.x;
		center.y = this_vertex[0].pos.y + this_diff.y;
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
			Vector2 objcenter = obj->GetCenter();
			objcenter.x -= winsize.x / 2;
			objcenter.y -= winsize.y / 2;
			objcenter.x /= winsize.x / 2;
			objcenter.y /= winsize.y / 2;
			Vector2 objrange = {};
			objrange.x = objlength / winsize.x / 2;
			objrange.y = objlength / winsize.y / 2;
			
			objcenter.x = objcenter.x + diff.x;
 			objcenter.y = objcenter.y + diff.y;

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

	GameObject::EMapOutType RokaEllipse::CheckMapOut()
	{
		return EMapOutType();
	}

	void RokaEllipse::SetBufferInfo(std::vector<Vertex>& vertexs, std::vector<UINT>& indexs)
	{
		Vector2 winsize = {};
		winsize.x = application.GetWidth();
		winsize.y = application.GetHeight();
		UINT radius = GetLength();

		Vector2 range = { radius / winsize.x *2,radius / winsize.y*2 };
		float sin = 0.0f;
		float cos = 0.0f;

		Vector2 center = GetCenter();
		center.x -= winsize.x / 2;
		center.y -= winsize.y / 2;
		center.x /= winsize.x / 2;
		center.y /= winsize.y / 2;
		Color color = GetRandomColor();
		color /= Color(255.0f, 255.0f, 255.0f);
		{
			Vertex vcenter = {};
			vcenter.pos.x = center.x;
			vcenter.pos.y = center.y;
			vcenter.color = color;
			vertexs.push_back(vcenter);
		}
		Vertex v = {};

		//360 = 2pi
		for (int i = 0; i < 360; i++)
		{
			if (i > 0)
			{
				if (i == 359)
				{
					indexs.push_back(0);
					indexs.push_back(1);
					indexs.push_back(359);
				}
				else
				{
					indexs.push_back(0);
					indexs.push_back(i + 1);
					indexs.push_back(i);
				}
			}

			v.pos.x = center.x + (range.x) * std::cosf(Deg2Rad((float)i));
			v.pos.y = center.y + (range.y) * std::sinf(Deg2Rad((float)i));
			v.color = color;
			vertexs.push_back(v);
		}
	}

	void RokaEllipse::LoadBuffer()
	{
		GameObject::LoadBuffer();
	}

	void RokaEllipse::LoadShader()
	{
		GameObject::LoadShader();
	}

	void RokaEllipse::SetupState()
	{
		GameObject::SetupState();
	}

	void RokaEllipse::AddLength(float size)
	{
		float length = GetLength();
		length += size;
		SetLength(length);
		Vector2 winsize = {};
		winsize.x = application.GetWidth();
		winsize.y = application.GetHeight();

		std::vector<Vertex>& vertexs = GetVertexs();

		Vector2 range = {};
		range.x = (length / winsize.x) * 2;
		range.y = (length / winsize.y) * 2;

		Vector2 center = GetCenter();
		center.x -= winsize.x / 2;
		center.y -= winsize.y / 2;
		center.x /= winsize.x / 2;
		center.y /= winsize.y / 2;
		for (int i = 1; i < 361; i++)
		{
			vertexs[i].pos.x = center.x + (range.x) * std::cosf(Deg2Rad((float)i));
			vertexs[i].pos.y = center.y + (range.y) * std::sinf(Deg2Rad((float)i));
		}

		GetMesh()->UpdateVertexBuffer(vertexs);
	}


}

