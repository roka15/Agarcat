#include "RokaGraphicDevice_Dx11.h"
#include "Application.h"
#include "Renderer.h"
extern roka::Application application;
namespace roka::graphics
{
	GraphicDevice_Dx11::GraphicDevice_Dx11()
	{
		HWND hWnd = application.GetHwnd();
		UINT deviceFlag = D3D11_CREATE_DEVICE_DEBUG;
		D3D_FEATURE_LEVEL featurelevel = (D3D_FEATURE_LEVEL)0;

		D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
			deviceFlag, nullptr, 0,
			D3D11_SDK_VERSION, mDevice.GetAddressOf(), &featurelevel,
			mContext.GetAddressOf());

#pragma region Swap Chain
		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		swapChainDesc.BufferCount = 2;
		swapChainDesc.BufferDesc.Width = application.GetWidth();
		swapChainDesc.BufferDesc.Height = application.GetHeight();

		if (!CreateSwapChain(&swapChainDesc, hWnd))
			return;
#pragma endregion

		if (FAILED(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)mRenderTarget.GetAddressOf())))
			return;

		mDevice->CreateRenderTargetView((ID3D11Resource*)mRenderTarget.Get(), nullptr,
			mRenderTargetView.GetAddressOf());

		D3D11_TEXTURE2D_DESC depthStencilDesc = {};
		depthStencilDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;
		//깊이정보만 저장할거라 4byte
		depthStencilDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.Width = application.GetWidth();
		depthStencilDesc.Height = application.GetHeight();
		//배열의 텍스쳐 수. 여러장 쓰는 경우 ex ) 그림자 표현할 때.
		//방법에 따라 다름 이 방법을 쓰면 연산량 증가하고, 그냥 그림자 이미지 넣으면 메모리 손해보지만 연산량은 비교적 손해 안봄.
		depthStencilDesc.ArraySize = 1;
		//다중 표본화를 위한 구조체 SampleDesc
		//픽셀 당 추출할 표본의 개수 지정.
		depthStencilDesc.SampleDesc.Count = 1;
		//퀄리티
		depthStencilDesc.MiscFlags = 0;
		
		D3D11_SUBRESOURCE_DATA data;
		if (!CreateTexture(&depthStencilDesc, &data))
			return;

		RECT winRect = {};
		GetClientRect(hWnd, &winRect);
		mViewPort =
		{
			0.0f,0.0f,
			(float)(winRect.right - winRect.left),
			(float)(winRect.bottom - winRect.top),
			0.0f,1.0f
		};
		BindViewPort(&mViewPort);
		mContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), mDepthStencilView.Get());

	}

	GraphicDevice_Dx11::~GraphicDevice_Dx11()
	{
	}
	bool GraphicDevice_Dx11::CreateSwapChain(const DXGI_SWAP_CHAIN_DESC* desc, HWND hwnd)
	{
		DXGI_SWAP_CHAIN_DESC dxgiDesc = {};
		dxgiDesc.OutputWindow = hwnd;
		//창모드인지
		dxgiDesc.Windowed = true;
		dxgiDesc.BufferCount = desc->BufferCount;
		//swap시 지워지도록 설정
		dxgiDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD;

		//출력용
		dxgiDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		//크기
		dxgiDesc.BufferDesc.Width = desc->BufferDesc.Width;
		dxgiDesc.BufferDesc.Height = desc->BufferDesc.Height;
		//buffer 색상 설정
		dxgiDesc.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
		//fps max / min
		dxgiDesc.BufferDesc.RefreshRate.Numerator = 240;
		dxgiDesc.BufferDesc.RefreshRate.Denominator = 1;

		//해상도에 맞게 이미지가 늘어날지 아닌지 현재 설정은 안늘어남.
		dxgiDesc.BufferDesc.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED;
		//이미지 그리는 순서 - 순서지정 안함.
		dxgiDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

		//https://codingfarm.tistory.com/420
		dxgiDesc.SampleDesc.Count = 1;
		dxgiDesc.SampleDesc.Quality = 0;

		Microsoft::WRL::ComPtr<IDXGIDevice> pDXGIDevice = nullptr;
		Microsoft::WRL::ComPtr<IDXGIAdapter> pAdapter = nullptr;
		Microsoft::WRL::ComPtr<IDXGIFactory> pFactory = nullptr;

		if (FAILED(mDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)pDXGIDevice.GetAddressOf())))
			return false;
		if (FAILED(pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)pAdapter.GetAddressOf())))
			return false;
		if (FAILED(pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)pFactory.GetAddressOf())))
			return false;
		if (FAILED(pFactory->CreateSwapChain(mDevice.Get(), &dxgiDesc, mSwapChain.GetAddressOf())))
			return false;

		return true;
	}
	bool GraphicDevice_Dx11::CreateTexture(const D3D11_TEXTURE2D_DESC* desc, void* data)
	{
		D3D11_TEXTURE2D_DESC dxgiDesc = {};
		dxgiDesc.BindFlags = desc->BindFlags;
		dxgiDesc.Usage = desc->Usage;
		//CPU access 설정 -0 은 access x
		dxgiDesc.CPUAccessFlags = 0;

		dxgiDesc.Format = desc->Format;
		dxgiDesc.Width = desc->Width;
		dxgiDesc.Height = desc->Height;
		dxgiDesc.ArraySize = desc->ArraySize;
		dxgiDesc.SampleDesc.Count = desc->SampleDesc.Count;
		dxgiDesc.SampleDesc.Quality = 0;

		//연산량을 줄이기 위해 크기별로 텍스쳐를 찍어두는것을 설정 할 수 있다.
		dxgiDesc.MipLevels = desc->MipLevels;
		dxgiDesc.MiscFlags = desc->MiscFlags;

		if (FAILED(mDevice->CreateTexture2D(&dxgiDesc, nullptr, mDepthStencilBuffer.ReleaseAndGetAddressOf())))
			return false;
		if (FAILED(mDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), nullptr, mDepthStencilView.GetAddressOf())))
			return false;

		return true;
	}
	bool GraphicDevice_Dx11::CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT NumElements, ID3DBlob* byteCode, ID3D11InputLayout** ppInputLayout)
	{
		if (FAILED(mDevice->CreateInputLayout(pInputElementDescs, NumElements
			, byteCode->GetBufferPointer()
			, byteCode->GetBufferSize()
			, ppInputLayout)))
			return false;

		return true;
	}
	bool GraphicDevice_Dx11::CreateBuffer(ID3D11Buffer** buffer, D3D11_BUFFER_DESC* desc, D3D11_SUBRESOURCE_DATA* data)
	{
		if (FAILED(mDevice->CreateBuffer(desc, data, buffer)))
			return false;
		return true;
	}

	bool GraphicDevice_Dx11::CompileFromfile(const std::wstring& fileName, const std::string& funcName, const std::string& version, ID3DBlob** ppBlob)
	{
		ID3DBlob* errorBlob = nullptr;
		//hlsl compile 해서 trianglevsBlob에 코드를 넘김.
		D3DCompileFromFile(fileName.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
			funcName.c_str(), version.c_str(), 0, 0, ppBlob, &errorBlob);

		if (errorBlob != nullptr)
		{
			OutputDebugStringA((char*)(errorBlob->GetBufferPointer()));
			errorBlob->Release();
			errorBlob = nullptr;
			return false;
		}
		return true;
	}
	bool GraphicDevice_Dx11::CreateVertexShader(const void* pShaderBytecode, SIZE_T BytecodeLength, ID3D11VertexShader** ppVertexShader)
	{
		//compile된 hlsl 코드를 가지고 셰이더 개체를 만듦
		if (FAILED(mDevice->CreateVertexShader(pShaderBytecode, BytecodeLength, nullptr, ppVertexShader)))
		{
			return false;
		}
		return true;
	}
	bool GraphicDevice_Dx11::CreatePixelShader(const void* pShaderBytecode, SIZE_T BytecodeLength, ID3D11PixelShader** ppPixelShader)
	{
		if (FAILED(mDevice->CreatePixelShader(pShaderBytecode, BytecodeLength, nullptr, ppPixelShader)))
		{
			return false;
		}
		return true;
	}
	void GraphicDevice_Dx11::BindViewPort(D3D11_VIEWPORT* viewPort)
	{
		mContext->RSSetViewports(1, viewPort);
	}
	void GraphicDevice_Dx11::SetConstantBuffer(ID3D11Buffer* buffer, void* data, UINT size)
	{
		//cpu buffer를 gpu에 올리는 과정.
		D3D11_MAPPED_SUBRESOURCE SubRes = {};
		mContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubRes);
		memcpy(SubRes.pData, data, size);
		//올리고 나서 해제해줘야함.
		mContext->Unmap(buffer, 0);
	}
	void GraphicDevice_Dx11::BindConstantBuffer(EShaderStage stage, ECBType type, ID3D11Buffer* buffer)
	{
		switch (stage)
		{
		case EShaderStage::VS:
			mContext->VSSetConstantBuffers((UINT)type, 1, &buffer);
			break;
		case EShaderStage::HS:
			mContext->HSSetConstantBuffers((UINT)type, 1, &buffer);
			break;
		case EShaderStage::DS:
			mContext->DSSetConstantBuffers((UINT)type, 1, &buffer);
			break;
		case EShaderStage::GS:
			mContext->GSSetConstantBuffers((UINT)type, 1, &buffer);
			break;
		case EShaderStage::PS:
			mContext->PSSetConstantBuffers((UINT)type, 1, &buffer);
			break;
		case EShaderStage::CS:
			mContext->CSSetConstantBuffers((UINT)type, 1, &buffer);
			break;
		case EShaderStage::End:
			break;
		default:
			break;
		}
	}
	void GraphicDevice_Dx11::BindsConstantBuffer(EShaderStage stage, ECBType type, ID3D11Buffer* buffer)
	{
		mContext->VSSetConstantBuffers((UINT)type, 1, &buffer);
		mContext->HSSetConstantBuffers((UINT)type, 1, &buffer);
		mContext->DSSetConstantBuffers((UINT)type, 1, &buffer);
		mContext->GSSetConstantBuffers((UINT)type, 1, &buffer);
		mContext->PSSetConstantBuffers((UINT)type, 1, &buffer);
		mContext->CSSetConstantBuffers((UINT)type, 1, &buffer);
	}
	void GraphicDevice_Dx11::BindInputLayout(ID3D11InputLayout* pInputLayout)
	{
		mContext->IASetInputLayout(pInputLayout);
	}
	void GraphicDevice_Dx11::BindPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY Topology)
	{
		mContext->IASetPrimitiveTopology(Topology);
	}
	void GraphicDevice_Dx11::BindVertexBuffer(UINT StartSlot, ID3D11Buffer* const* ppVertexBuffers, const UINT* pStrides, const UINT* pOffsets)
	{
		mContext->IASetVertexBuffers(StartSlot, 1, ppVertexBuffers, pStrides, pOffsets);
	}
	void GraphicDevice_Dx11::BindIndexBuffer(ID3D11Buffer* pIndexBuffer, DXGI_FORMAT Format, UINT Offset)
	{
		mContext->IASetIndexBuffer(pIndexBuffer, Format, Offset);
	}
	void GraphicDevice_Dx11::BindVertexShader(ID3D11VertexShader* vs)
	{
		mContext->VSSetShader(vs, 0, 0);
	}
	void GraphicDevice_Dx11::BindPixelShader(ID3D11PixelShader* ps)
	{
		mContext->PSSetShader(ps, 0, 0);
	}
	
	
	void GraphicDevice_Dx11::UpdateVertexBuffer(ID3D11Buffer* buffer, const std::vector<renderer::Vertex>& vertexs)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT result = mContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (SUCCEEDED(result))
		{
			Vertex* vertices = static_cast<Vertex*>(mappedResource.pData);

			/*for (int i = 0; i < vertexs.size(); i++)
			{
				vertices[i] = vertexs[i];
			}
		*/
			memcpy(mappedResource.pData, vertexs.data(), sizeof(Vertex) * vertexs.size());
			mContext->Unmap(buffer, 0);
		}
	}

	void GraphicDevice_Dx11::DrawIndexed(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
	{
		mContext->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
	}

	void GraphicDevice_Dx11::Draw()
	{
		FLOAT bgColor[4] = { 0.3f,0.74f,0.88f,1.0f };
		mContext->ClearRenderTargetView(mRenderTargetView.Get(), bgColor);
		mContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0.0f);

		HWND hWnd = application.GetHwnd();
		RECT winRect = {};
		GetClientRect(hWnd, &winRect);
		mViewPort =
		{
			0.0f,0.0f,
			(float)(winRect.right - winRect.left),
			(float)(winRect.bottom - winRect.top),
			0.0f,1.0f
		};
		BindViewPort(&mViewPort);
		mContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), mDepthStencilView.Get());
	}
	void GraphicDevice_Dx11::Present()
	{
		mSwapChain->Present(0, 0);
	}
}

