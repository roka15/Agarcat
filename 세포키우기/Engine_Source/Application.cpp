#include "Application.h"
#include "Input.h"
#include "RokaTime.h"
#include "Renderer.h"


namespace roka
{

	Application::Application()
		:graphicDevice(nullptr),
		mHwnd(NULL),
		mWidth(-1),
		mHeight(-1),
		mgen(m_rd())
	{
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		Update();
		LateUpdate();
		Render();
	}

	void Application::Initialize()
	{
		Time::Initiailize();
		Input::Initialize();

		roka::renderer::Initialize();
		mScene = new Scene();
		mScene->Initialize();
	}

	void Application::Update()
	{
		Input::Update();
		Time::Update();
		
		mScene->Update();
	}

	void Application::LateUpdate()
	{
		mScene->DeleteObjects();
	}

	void Application::Render()
	{
		Time::Render();

		graphicDevice->Draw();
		mScene->Render();
		graphicDevice->Present();
	}

	void Application::SetWindow(HWND hwnd, UINT width, UINT height)
	{
		if (graphicDevice == nullptr)
		{
			mHwnd = hwnd;
			mWidth = width;
			mHeight = height;

			graphicDevice = std::make_unique<roka::graphics::GraphicDevice_Dx11>();
			roka::graphics::GetDevice() = graphicDevice.get();
		}

		RECT rt = { 0, 0, (LONG)width , (LONG)height };
		AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, false);
		SetWindowPos(mHwnd, nullptr, 0, 0, rt.right - rt.left, rt.bottom - rt.top, 0);
		ShowWindow(mHwnd, true);
		UpdateWindow(mHwnd);
	}

	UINT Application::GetRandomValue(UINT min, UINT max)
	{
		std::uniform_int_distribution<int> dis(min, max);
		return dis(mgen);
	}

}