#pragma once

#include <windows.h>
#include "TimeSystem.h"
#include "InputSystem.h"
#include "RenderSystem.h"

#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600

class Application
{
public:
	Application();
	~Application();

public:
	void Initialize();
	void Loop();
	void Finalize();

private:
	BOOL InitInstance(HINSTANCE hInstance);
	ATOM MyRegisterClass(HINSTANCE hInstance);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	HWND _hWnd;
	MSG _msg;
	int _screenWidth;
	int _screenHeight;
	bool _isFullscreen;
	const WCHAR* _appName;

	wchar_t buffer[30];

private:
	void Run();

private:
	TimeSystem* _timeSystem;
	InputSystem* _inputSystem;
	RenderSystem* _renderSystem;
};

