#include "Application.h"
#include <cassert>
#include <string>

Application::Application()
	: buffer(L"test"),
	_appName(buffer),
	_screenWidth(800),
	_screenHeight(600),
	_isFullscreen(false),
	_timeSystem(TimeSystem::GetInstance()),
	_inputSystem(InputSystem::GetInstance()),
	_renderSystem(RenderSystem::GetInstance())
{

}

Application::~Application()
{

}

void Application::Initialize()
{
	HINSTANCE ins = GetModuleHandle(NULL);
	MyRegisterClass(ins);
	InitInstance(ins);

	_renderSystem->Initialize(_hWnd, _screenWidth, _screenHeight);
	_timeSystem->Initialize();
	_inputSystem->Initialize(_hWnd, ins, _screenWidth, _screenHeight);
}

void Application::Loop()
{
	while (1)
	{
		if (PeekMessage(&_msg, NULL, 0, 0, PM_REMOVE))
		{
			if (_msg.message == WM_QUIT) break;

			DispatchMessage(&_msg);
		}
		else
		{
			Run();
		}
	}
}

void Application::Finalize()
{
	_renderSystem->Finalize();
	_inputSystem->Finalize();
}

BOOL Application::InitInstance(HINSTANCE hInstance)
{
	_hWnd = CreateWindowW(
		_appName,
		_appName,
		WS_OVERLAPPEDWINDOW | WS_POPUP,
		300, 300,
		_screenWidth, _screenHeight,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	if (!_hWnd)
	{
		return FALSE;
	}

	RECT rect;

	GetClientRect(_hWnd, &rect);

	_screenWidth = rect.right - rect.left;
	_screenHeight = rect.bottom - rect.top;

	ShowWindow(_hWnd, SW_SHOWNORMAL);
	UpdateWindow(_hWnd);

	return TRUE;
}

ATOM Application::MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = _appName;
	wcex.hIconSm = NULL;

	return RegisterClassExW(&wcex);
}

LRESULT CALLBACK Application::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void Application::Run()
{
	_renderSystem->Update();
	_timeSystem->Update();
	_inputSystem->Update();

	if (_inputSystem->GetKeyDown(DIK_F))
	{
		if (_isFullscreen == false)
		{
			// 스크린 전체 사이즈를 가져온다
			_screenWidth = GetSystemMetrics(SM_CXSCREEN);
			_screenHeight = GetSystemMetrics(SM_CYSCREEN);

			SetWindowPos(_hWnd, NULL, 0, 0, _screenWidth, _screenHeight, 0);
			_renderSystem->ChangeFullScreenMode(_screenWidth, _screenHeight);

			_isFullscreen = true;
		}
		else
		{
			_screenWidth = DEFAULT_WIDTH;
			_screenHeight = DEFAULT_HEIGHT;

			SetWindowPos(_hWnd, NULL, 200, 200, _screenWidth, _screenHeight, 0);

			_renderSystem->ChangeFullScreenMode(_screenWidth, _screenHeight);

			_isFullscreen = false;
		}
	}

	float x = _inputSystem->GetMouseXpos();
	float y = _inputSystem->GetMouseYpos();
	float wheel = _inputSystem->GetMouseWheel();

	swprintf(buffer, sizeof(buffer) / sizeof(buffer[0]), L"%f", wheel);

	SetWindowText(GetActiveWindow(), buffer);

	_inputSystem->Flush();
}
