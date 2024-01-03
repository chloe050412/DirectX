#pragma once
#include <windows.h>
#include <dinput.h>

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

/// 개선된 인풋 시스템.
/// Windows의 GetAsync 딜레이를 없애기 위해
/// dxinput을 사용했다.

#define MOUSE_LEFT 0
#define MOUSE_RIGHT 1
#define MOUSE_MIDDLE 2

#define KEY_W DIK_W

class InputSystem
{
public:
	static InputSystem* GetInstance();

private:
	static InputSystem* _inputSystem;
	InputSystem() = default;
	~InputSystem() = default;

public:
	void Initialize(HWND hWnd, HINSTANCE instance, int screenWidth, int screenHeight);
	void Update();
	void Finalize();

	bool GetKey(BYTE key);
	bool GetKeyDown(BYTE key);
	bool GetKeyUp(BYTE key);

	bool GetMouse(BYTE key);
	bool GetMouseDown(BYTE key);
	bool GetMouseUp(BYTE key);

	float GetMouseXpos();
	float GetMouseYpos();
	float GetMouseWheel();

	void Flush();

private:
	bool StartDXInput();
	bool FinishDXInput();

private:
	HWND					_hWnd;
	HINSTANCE				_instance;

	int						_screenWidth;
	int						_screenHeight;

	LPDIRECTINPUT8			_DI;
	LPDIRECTINPUTDEVICE8	_keyboardDevice;
	LPDIRECTINPUTDEVICE8	_mouseDevice;

	DIMOUSESTATE			_DImouseState;
	bool					_mouseState[3];
	bool					_prevMouseState[3];
	bool					_keyState[256];
	bool					_prevKeyState[256];

	POINT					_mousePos;
	int						_mouseWheel;
};

