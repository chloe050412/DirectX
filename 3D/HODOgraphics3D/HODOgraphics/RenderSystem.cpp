#include "RenderSystem.h"


RenderSystem* RenderSystem::_system = nullptr;

RenderSystem::RenderSystem()
	: _hWnd(NULL),
	_screenWidth(0), _screenHeight(0),
	_device(nullptr), _deviceContext(nullptr),
	_swapChain(nullptr)
{

}

RenderSystem* RenderSystem::GetInstance()
{
	if (_system == nullptr)
	{
		_system = new RenderSystem();
	}

	return _system;
}

void RenderSystem::Initialize(HWND hWnd, int screenWidth, int screenHeight)
{
	_hWnd = hWnd;
	_screenWidth = screenWidth;
	_screenHeight = screenHeight;

	StartDx();

	_camera = new Camera();
	_camera->SetPosition(0.f, 0.f, -10.f);

	_model = new Model();
	_model->Initialize(_device.Get());

	_shader = new Shader();
	_shader->Initialzie(_device.Get(), hWnd);
}

void RenderSystem::Update()
{
	BeginRender();
}

void RenderSystem::Finalize()
{
	_shader->Finalize();
	_model->Finalize();
	delete _camera;

	FinishDx();
}

void RenderSystem::StartDx()
{
	// 스왑 체인을 위한 데이터 구조체 세팅
	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferCount = 1;                           // 백 버퍼 개수
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;   // 32bit 컬러 사용
	swapChainDesc.BufferDesc.Width = _screenWidth;
	swapChainDesc.BufferDesc.Height = _screenHeight;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;   // 스왑 체인을 백퍼버(렌더 타겟) 출력으로 사용할 것임
	swapChainDesc.OutputWindow = _hWnd;                        // 어느 윈도우로 내보낼 것인지
	swapChainDesc.SampleDesc.Count = 4;                        // 멀티 샘플 단계에서의 픽셀 수
	swapChainDesc.Windowed = true;                           // window 형태인지, 풀 스크린인지
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // 풀 스크린 스위칭을 허용한다

	// 디바이스, 디바이스 컨텍스트, 스왑 체인 생성
	D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		_swapChain.GetAddressOf(),
		_device.GetAddressOf(),
		NULL,
		_deviceContext.GetAddressOf());

	// 그림을 그릴 백 버퍼의 주소 가져오기
	ID3D11Texture2D* pbackBuffer;
	// 스왑 체인의 백 버퍼를 가져와 텍스처 객체에 할당
	_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pbackBuffer);

	// 백 버퍼 주소를 이용해 렌더 타겟 생성하기
	_device->CreateRenderTargetView(pbackBuffer, NULL, _renderTarget.GetAddressOf());
	pbackBuffer->Release();

	// 렌더 타겟을 백 버퍼로 설정하기
	_deviceContext->OMSetRenderTargets(1, _renderTarget.GetAddressOf(), NULL);

	// 뷰포트 설정
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)_screenWidth;
	viewport.Height = (float)_screenHeight;

	_deviceContext->RSSetViewports(1, &viewport);
}

void RenderSystem::BeginRender()
{
	Matrix worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	float color[4] = { 0.f, 0.2f, 0.4f, 1.0f };


	_camera->Render();

	_camera->GetViewMatrix(viewMatrix);


	_model->Render(_deviceContext.Get());
	_shader->Render(_deviceContext.Get(), _model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);



	_deviceContext->ClearRenderTargetView(_renderTarget.Get(), color);

	_swapChain->Present(0, 0);
}

void RenderSystem::EndRender()
{

}

void RenderSystem::FinishDx()
{
	// 윈도우 모드로 먼저 전환한 뒤 종료
	_swapChain->SetFullscreenState(false, NULL); 

	_swapChain->Release();
	_device->Release();
	_deviceContext->Release();
}

void RenderSystem::ChangeFullScreenMode(int screenWidth, int screenHeight)
{
	_screenWidth = screenWidth;
	_screenHeight = screenHeight;

	// 기존 렌더 타겟을 지운다
	_deviceContext->OMSetRenderTargets(0, NULL, NULL);
	_renderTarget->Release();

	// 백 버퍼의 사이즈를 조절한다
	_swapChain->ResizeBuffers(1,
		_screenWidth,
		_screenHeight,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		0);

	// 렌더 타겟을 다시 생성한다
	// 그림을 그릴 백 버퍼의 주소 가져오기
	ID3D11Texture2D* pbackBuffer;
	// 스왑 체인의 백 버퍼를 가져와 텍스처 객체에 할당
	_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pbackBuffer);

	// 백 버퍼 주소를 이용해 렌더 타겟 생성하기
	_device->CreateRenderTargetView(pbackBuffer, NULL, _renderTarget.GetAddressOf());
	pbackBuffer->Release();

	// 렌더 타겟을 백 버퍼로 설정하기
	_deviceContext->OMSetRenderTargets(1, _renderTarget.GetAddressOf(), NULL);

	// 뷰포트 설정
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)_screenWidth;
	viewport.Height = (float)_screenHeight;

	_deviceContext->RSSetViewports(1, &viewport);

}
