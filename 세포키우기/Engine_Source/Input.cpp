#include "Input.h"
#include "Application.h"

extern roka::Application application;

namespace roka
{
	int ASCII[(UINT)EKeyCode::END] =
	{
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
		'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
		'Z', 'X', 'C', 'V', 'B', 'N', 'M',

		VK_UP, VK_DOWN, VK_LEFT ,VK_RIGHT, VK_SPACE,
		VK_LBUTTON, VK_RBUTTON,
	};

	std::vector<Input::Key> Input::mKeys;
	Vector2 Input::mMousePos = Vector2::Zero;

	void Input::Initialize()
	{
		for (UINT i = 0; i < (UINT)EKeyCode::END; i++)
		{
			Key keyInfo;
			keyInfo.key = (EKeyCode)i;
			keyInfo.state = EKeyState::None;
			keyInfo.bPressed = false;

			mKeys.push_back(keyInfo);
		}
	}
	
	void Input::Update()
	{
		if (GetFocus())
		{

			for (UINT i = 0; i < (UINT)EKeyCode::END; i++)
			{
				if (GetAsyncKeyState(ASCII[i]) & 0x8000)
				{
					// 이전 프레임에도 눌려 있었다
					if (mKeys[i].bPressed)
						mKeys[i].state = EKeyState::Pressed;
					else
						mKeys[i].state = EKeyState::Down;

					mKeys[i].bPressed = true;
				}
				else // 현재 프레임에 키가 눌려있지 않다.
				{
					// 이전 프레임에 내키가 눌려있엇다.
					if (mKeys[i].bPressed)
						mKeys[i].state = EKeyState::Up;
					else
						mKeys[i].state = EKeyState::None;

					mKeys[i].bPressed = false;
				}
			}

			POINT mousePos = {};
			GetCursorPos(&mousePos);

			ScreenToClient(application.GetHwnd(), &mousePos);
			mMousePos.x = mousePos.x;
			mMousePos.y = mousePos.y;
		}
		else
		{
			for (UINT i = 0; i < (UINT)EKeyCode::END; i++)
			{
				if (EKeyState::Down == mKeys[i].state
					|| EKeyState::Pressed == mKeys[i].state)
				{
					mKeys[i].state = EKeyState::Up;
				}
				else if (EKeyState::Up == mKeys[i].state)
				{
					mKeys[i].state = EKeyState::None;
				}

				mKeys[i].bPressed = false;
			}
		}
	}

	void Input::Render(HDC hdc)
	{

	}
}
