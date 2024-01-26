#pragma once
#include <d3d11.h>
#include <wrl.h>

#pragma comment(lib, "d3d11.lib")

using namespace Microsoft::WRL;

class RenderSystem
{
public:
	static RenderSystem* GetInstance();

private:
	static RenderSystem* _system;
	RenderSystem();
	~RenderSystem() = default;

public:
	void Initialize(HWND hWnd, int screenWidth, int screenHeight);
	void Update(bool isFullScreen);
	void Finalize();

private:
	void StartDx();
	void BeginRender();
	void EndRender();
	void FinishDx();
	void ChangeFullScreenMode();

private:
	HWND							_hWnd;

	int								_screenWidth;
	int								_screenHeight;
	bool							_isFullscreen;

	ComPtr<ID3D11Device>			_device;
	ComPtr<ID3D11DeviceContext>     _deviceContext;
	ComPtr<IDXGISwapChain>			_swapChain;

	ComPtr<ID3D11RenderTargetView>  _renderTarget;

};

