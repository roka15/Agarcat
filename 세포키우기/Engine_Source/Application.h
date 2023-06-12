#pragma once
#include "RokaEngine.h"
#include "RokaGraphicDevice_Dx11.h"
#include "Scene.h"
namespace roka
{
	class Application
	{
	public:
		Application();
		~Application();

		void Run();

		void Initialize();
		void Update();
		void LateUpdate();
		void Render();

		void SetWindow(HWND hwnd, UINT width, UINT height);

		UINT GetWidth() { return mWidth; }
		UINT GetHeight() { return mHeight; }
		HWND GetHwnd() { return mHwnd; }

		UINT GetRandomValue(UINT min, UINT max);

		Scene* GetActiveScene() { return mScene; }
	private:
		bool mbInitialize = false;
		// 오로지 한개의 객체만 만들수 있는 스마트 포인터
		std::unique_ptr<roka::graphics::GraphicDevice_Dx11> graphicDevice;

		// HDC mHdc; -> GPU API
		HWND mHwnd;
		UINT mWidth;
		UINT mHeight;

		std::random_device m_rd;
		std::mt19937 mgen;
		Scene* mScene;
	};
}


