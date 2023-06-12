#include "RokaRectangle.h"
#include "Application.h"

extern roka::Application application;
namespace roka
{

	RokaRectangle::RokaRectangle(EGameObjectType type) :GameObject(type)
	{
	}

	RokaRectangle::~RokaRectangle()
	{
	}

	void RokaRectangle::Initialize()
	{
		GameObject::Initialize();
	}

	void RokaRectangle::Update()
	{
		GameObject::Update();
	}

	void RokaRectangle::LateUpdate()
	{
		GameObject::LateUpdate();
	}
	void RokaRectangle::Render()
	{
		GameObject::Render();
	}
	bool RokaRectangle::InitCheckCollision()
	{
		return GameObject::InitCheckCollision();
	}
	GameObject* RokaRectangle::CheckCollision(ECollisionDirType& dirType)
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
			RokaRectangle* rect = dynamic_cast<RokaRectangle*>(obj);

			const std::vector<Vertex>& vertex = rect->GetVertexs();
			const Vector4& diff = rect->GetCurPosDiff();
			UINT objlength = rect->GetLength();
			Vector2 objrange = {};
			objrange.x = objlength  / winsize.x / 2;
			objrange.y = objlength  / winsize.y / 2;
			Vector2 objcenter = {};
			objcenter.x = vertex[0].pos.x + diff.x + objrange.x;
			objcenter.y = vertex[0].pos.y + diff.y - objrange.y;

			float xdiff = std::fabsf(center.x - objcenter.x);
			float ydiff = std::fabsf(center.y - objcenter.y);

			bool xflag = false;
			bool yflag = false;
			if (rect != nullptr)
			{
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
					return rect;
				}
			}
			else
				return nullptr;
		}
		return nullptr;
	}
	GameObject::EMapOutType roka::RokaRectangle::CheckMapOut()
	{
		std::vector<Vertex> vertex = GetVertexs();
		UINT length = GetLength();
		const Vector2& dir = GetDir();
		const Vector4 diff = GetCurPosDiff();

		if (vertex[3].pos.x + diff.x >= 1)
		{
			return EMapOutType::OUTTRUE_RIGHTOUT;
		}
		if (vertex[0].pos.x + diff.x <= -1)
		{
			return EMapOutType::OUTTRUE_LEFTOUT;
		}
		if (vertex[0].pos.y + diff.y >= 1)
		{
			return EMapOutType::OUTTRUE_UPOUT;
		}
		if (vertex[2].pos.y + diff.y <= -1)
		{
			return EMapOutType::OUTTRUE_DOWNOUT;
		}
		return EMapOutType::OUTFALSE;
	}
	void RokaRectangle::LoadBuffer()
	{
		GameObject::LoadBuffer();
		Mesh* mesh = GetMesh();
		Vector2 center = GetCenter();
		UINT length = GetLength();
		std::vector<Vertex>& vertexs = GetVertexs();
		std::vector<UINT>& indexs = GetIndexs();




		Vector2 winsize = {};
		winsize.x = application.GetWidth();
		winsize.y = application.GetHeight();

		center.x -= winsize.x / 2;
		center.y -= winsize.y / 2;


		Color color = {};
		if (GetType() == EGameObjectType::Item)
		{
			color = ITEM_COLOR;
		}
		else
		{
			do
			{
				color = GetRandomColor();
				color /= Color(255.0f, 255.0f, 255.0f);
			} while (color.R() == 0 && color.G() == 0 && color.B() == 1);
		}

		color.A(1.0f);
		vertexs.clear();
		Vertex vertex = {};
		vertex.pos = Vector3{ (float)(center.x - length),(float)(center.y + length),0.0f };
		vertex.color = color;
		vertexs.push_back(vertex);

		vertex.pos = Vector3{ (float)(center.x + length),(float)(center.y - length),0.0f };
		vertex.color = color;
		vertexs.push_back(vertex);

		vertex.pos = Vector3{ (float)(center.x - length),(float)(center.y - length),0.0f };
		vertex.color = color;
		vertexs.push_back(vertex);

		vertex.pos = Vector3{ (float)(center.x + length),(float)(center.y + length),0.0f };
		vertex.color = color;
		vertexs.push_back(vertex);

		for (int i = 0; i < vertexs.size(); i++)
		{
			vertexs[i].pos.x /= winsize.x / 2;
			vertexs[i].pos.y /= winsize.y / 2;
		}
		/*vertexs.resize(4);
		vertexs[0].pos = { -0.25f, -0.25f, 0.0f };
		vertexs[0].color = { 1.0f,0.0f,0.0f,1.0f };
		vertexs[1].pos = { -0.25f, +0.25f, 0.0f };
		vertexs[1].color = { 0.0f,1.0f,0.0f,1.0f };
		vertexs[2].pos = { +0.25f, -0.25f, 0.0f };
		vertexs[2].color = { 0.0f,0.0f,1.0f,1.0f };
		vertexs[3].pos = { +0.25f, +0.25f, 0.0f };
		vertexs[3].color = { 0.0f,1.0f,0.0f,1.0f };*/

		indexs.push_back(0);
		indexs.push_back(1);
		indexs.push_back(2);
		indexs.push_back(0);
		indexs.push_back(3);
		indexs.push_back(1);

		if (GetState() == EState::Active)
		{
			mesh->CreateVertexBuffer(vertexs.data(), vertexs.size());
			mesh->CreateIndexBuffer(indexs.data(), indexs.size());
		}
		else
		{
			mesh->UpdateVertexBuffer(vertexs);
		}

	}
	void RokaRectangle::LoadShader()
	{
		GameObject::LoadShader();
	}
	void RokaRectangle::SetupState()
	{
		GameObject::SetupState();
	}
	void RokaRectangle::AddLength(float size)
	{
		float length = GetLength();
		length += size;
		SetLength(length);
		Vector2 winsize = {};
		winsize.x = application.GetWidth();
		winsize.y = application.GetHeight();

		Vector2 addsize = {};

		addsize.x = size  / (winsize.x / 2);
		addsize.y = size  / (winsize.y / 2);

		std::vector<Vertex>& vertexs = GetVertexs();

		vertexs[0].pos.x -= addsize.x;
		vertexs[0].pos.y += addsize.y;

		vertexs[1].pos.x += addsize.x;
		vertexs[1].pos.y -= addsize.y;

		vertexs[2].pos.x -= addsize.x;
		vertexs[2].pos.y -= addsize.y;

		vertexs[3].pos.x += addsize.x;
		vertexs[3].pos.y += addsize.y;

		GetMesh()->UpdateVertexBuffer(vertexs);
	}
}
