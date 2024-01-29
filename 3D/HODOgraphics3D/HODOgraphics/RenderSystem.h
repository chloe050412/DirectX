#pragma once
#include "HDd3d.h"
#include <wrl.h>
#include "Model.h"
#include "Shader.h"
#include "Camera.h"

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
	void Update();
	void Finalize();
	void ChangeFullScreenMode(int screenWidth, int screenHeight);

private:
	void StartDx();
	void BeginRender();
	void EndRender();
	void FinishDx();

private:
	HWND							_hWnd;

	int								_screenWidth;
	int								_screenHeight;

	ComPtr<ID3D11Device>			_device;
	ComPtr<ID3D11DeviceContext>     _deviceContext;
	ComPtr<IDXGISwapChain>			_swapChain;

	ComPtr<ID3D11RenderTargetView>  _renderTarget;

	//test

	Model* _model;
	Shader* _shader;
	Camera* _camera;

};

