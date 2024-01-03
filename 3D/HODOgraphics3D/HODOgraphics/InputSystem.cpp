#include "InputSystem.h"
#include <cassert>

InputSystem* InputSystem::_inputSystem = nullptr;



void InputSystem::Initialize(HWND hWnd, HINSTANCE instance, int screenWidth, int screenHeight)
{
	_hWnd = hWnd;
	_instance = instance;

	_screenWidth = screenWidth;
	_screenHeight = screenHeight;

	StartDXInput();
}

void InputSystem::Update()
{
	// ��ġ ����� üũ
	if (_mouseDevice == NULL || _keyboardDevice == NULL)
	{
		assert(false, "no input device");
	}

	// ������ �� Ű������ input ���� �޾ƿ���
	HRESULT hr;

	if (FAILED(hr = _keyboardDevice->GetDeviceState(256, _keyState)))
	{
		while (_keyboardDevice->Acquire() == DIERR_INPUTLOST);
	}
	if (FAILED(hr = _mouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), &_DImouseState)))
	{
		while (_mouseDevice->Acquire() == DIERR_INPUTLOST);
	}

	// ���콺 ���� ���� (������ ����...)
	for (int i = 0; i < 3; i++)
	{
		_mouseState[i] = _DImouseState.rgbButtons[i];
	}

	// ��ũ�� ��ǥ�� ���߱�
	GetCursorPos(&_mousePos);
	ScreenToClient(_hWnd, &_mousePos);

	_mousePos.x += _DImouseState.lX;
	_mousePos.y += _DImouseState.lY;
	_mouseWheel += _DImouseState.lZ;

	// ������ ����� ��� ��ǥ�� ����
	if (_mousePos.x < 0 || _mousePos.x > _screenWidth || _mousePos.y < 0 || _mousePos.y > _screenHeight)
	{
		_mousePos.x = 0;
		_mousePos.y = 0;
	}
}

void InputSystem::Finalize()
{
	FinishDXInput();
}

bool InputSystem::StartDXInput()
{
	HRESULT hr = S_OK;

	// ��ġ �ʱ�ȭ
	if (FAILED(hr = DirectInput8Create(_instance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&_DI, NULL)))
	{
		return false;
	}

	// Ű���� ����
	if (FAILED(hr = _DI->CreateDevice(GUID_SysKeyboard,
		&_keyboardDevice, NULL)))
	{
		return false;
	}

	// ���콺 ����
	if (FAILED(hr = _DI->CreateDevice(GUID_SysMouse,
		&_mouseDevice, NULL)))
	{
		return false;
	}

	// ��ġ�� ��ȯ ������ ����
	_keyboardDevice->SetDataFormat(&c_dfDIKeyboard);
	_mouseDevice->SetDataFormat(&c_dfDIMouse);


	// ��ġ�� ���� �� ���� ����(��������)
	if (FAILED(hr = _keyboardDevice->SetCooperativeLevel(
		_hWnd,
		DISCL_NONEXCLUSIVE |
		DISCL_FOREGROUND |
		DISCL_NOWINKEY)))
	{
		return false;
	}
	while (_keyboardDevice->Acquire() == DIERR_INPUTLOST);


	if (FAILED(hr = _mouseDevice->SetCooperativeLevel(
		_hWnd,
		DISCL_NONEXCLUSIVE |
		DISCL_FOREGROUND)))
	{
		return false;
	}
	while (_mouseDevice->Acquire() == DIERR_INPUTLOST);

	return true;
}

bool InputSystem::GetKey(BYTE key)
{
	return (_keyState[key] & 0x80) && (_prevKeyState[key] & 0x80);
}

bool InputSystem::GetKeyDown(BYTE key)
{
	return _keyState[key] && _prevKeyState[key] == false;
}

bool InputSystem::GetKeyUp(BYTE key)
{
	return _keyState[key] == false && _prevKeyState[key];
}

bool InputSystem::GetMouse(BYTE key)
{
	return _mouseState[key] && _prevMouseState[key];
}

bool InputSystem::GetMouseDown(BYTE key)
{
	return _mouseState[key] && _prevMouseState[key] == false;
}

bool InputSystem::GetMouseUp(BYTE key)
{
	return _mouseState[key] == false && _prevMouseState[key];
}

float InputSystem::GetMouseXpos()
{
	return _mousePos.x;
}

float InputSystem::GetMouseYpos()
{
	return _mousePos.y;
}

float InputSystem::GetMouseWheel()
{
	return _mouseWheel;
}

void InputSystem::Flush()
{
	for (int i = 0; i < 256; ++i)
	{
		_prevKeyState[i] = _keyState[i];
		_keyState[i] = false;
	}

	for (int i = 0; i < 3; ++i)
	{
		_prevMouseState[i] = _mouseState[i];
		_mouseState[i] = false;
	}
}

bool InputSystem::FinishDXInput()
{
	if (_mouseDevice)
	{
		_mouseDevice->Unacquire();
		_mouseDevice->Release();
		_mouseDevice = NULL;
	}
	if (_keyboardDevice)
	{
		_keyboardDevice->Unacquire();
		_keyboardDevice->Release();
		_keyboardDevice = NULL;
	}
	if (_DI)
	{
		_DI->Release();
		_DI = NULL;
	}

	return true;
}

InputSystem* InputSystem::GetInstance()
{
	if (_inputSystem == nullptr)
	{
		_inputSystem = new InputSystem();
	}

	return _inputSystem;
}

