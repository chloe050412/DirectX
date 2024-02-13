#pragma once
#include "HDd3d.h"
#include "HDd2d.h"
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
	void Render();
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
	ComPtr<ID3D11Texture2D> _depthStencilBuffer;
	ComPtr<ID3D11DepthStencilState> _depthStencilState;
	ComPtr<ID3D11DepthStencilView> _depthStencilView;
	ComPtr<ID3D11RasterizerState> _rasterState;

	//test

	Model* _model;
	Shader* _shader;
	Camera* _camera;

private:

	Matrix _projectionMatrix;
	Matrix _worldMatrix;
	Matrix _orthoMatrix;

};

