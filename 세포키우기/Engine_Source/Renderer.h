#pragma once
#include "RokaEngine.h"
#include "RokaGraphicDevice_Dx11.h"
#include "Mesh.h"
#include "Shader.h"
#include "ConstantBuffer.h"
using namespace roka::math;
namespace roka::renderer
{
	struct Vertex
	{
		Vector3 pos;
		Vector4 color;
	};

	extern std::vector<Vertex> vertexs;
	extern std::vector<UINT> indexs;
	extern roka::graphics::ConstantBuffer* constantBuffer;
	extern Shader* shader;
	extern Mesh* mesh;

	void Initialize();
	void Release();
}
