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
		return GameObject::CheckCollision(dirType);
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
	void RokaRectangle::SetBufferInfo(std::vector<Vertex>& vertexs, std::vector<UINT>& indexs)
	{
		Vector2 center = GetCenter();
		UINT length = GetLength();
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

		indexs.push_back(0);
		indexs.push_back(1);
		indexs.push_back(2);
		indexs.push_back(0);
		indexs.push_back(3);
		indexs.push_back(1);

	}
	void RokaRectangle::LoadBuffer()
	{
		GameObject::LoadBuffer();
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

		addsize.x = size / (winsize.x / 2);
		addsize.y = size / (winsize.y / 2);

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
