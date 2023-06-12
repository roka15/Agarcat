#include "ConstantBuffer.h"
#include "RokaGraphicDevice_Dx11.h"
namespace roka::graphics
{
	ConstantBuffer::ConstantBuffer(const ECBType type)
		:GpuBuffer(), mType(type)
	{
	}
	ConstantBuffer::~ConstantBuffer()
	{
	}
	bool ConstantBuffer::Create(size_t size)
	{
		desc.ByteWidth = size;
		desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
		desc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;

		if (FAILED(roka::graphics::GetDevice()->CreateBuffer(buffer.GetAddressOf(), &desc, nullptr)))
		{
			return false;
		}
		return true;
	}
	void ConstantBuffer::SetData(void* data)
	{
		roka::graphics::GetDevice()->SetConstantBuffer(buffer.Get(), data, desc.ByteWidth);
	}
	void ConstantBuffer::Bind(EShaderStage stage)
	{
		roka::graphics::GetDevice()->BindConstantBuffer(stage, mType, buffer.Get());
	}
}