#pragma once
#include "RokaGraphics.h"
namespace roka::graphics
{
	class ConstantBuffer:public GpuBuffer
	{
	public:
		ConstantBuffer(const ECBType type);
		~ConstantBuffer();

		bool Create(size_t size);
		void SetData(void* data);
		void Bind(EShaderStage stage);
	private:
		const ECBType mType;
	};
}

