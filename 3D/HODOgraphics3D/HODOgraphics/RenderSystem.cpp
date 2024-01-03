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
}

void RenderSystem::Update()
{
	BeginRender();
}

void RenderSystem::Finalize()
{
	FinishDx();
}

void RenderSystem::StartDx()
{
	// 스왑 체인을 위한 데이터 구조체 세팅
	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferCount = 1;                           // 백 버퍼 개수
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;   // 32bit 컬러 사용
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;   // 스왑 체인을 백퍼버(렌더 타겟) 출력으로 사용할 것임
	swapChainDesc.OutputWindow = _hWnd;                        // 어느 윈도우로 내보낼 것인지
	swapChainDesc.SampleDesc.Count = 4;                        // 멀티 샘플 단계에서의 픽셀 수
	swapChainDesc.Windowed = true;                           // window 형태인지, 풀 스크린인지

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
	viewport.Width = _screenWidth;
	viewport.Height = _screenHeight;

	_deviceContext->RSSetViewports(1, &viewport);
}

void RenderSystem::BeginRender()
{
	float color[4] = { 0.f, 0.2f, 0.4f, 1.0f };

	_deviceContext->ClearRenderTargetView(_renderTarget.Get(), color);

	_swapChain->Present(0, 0);
}

void RenderSystem::EndRender()
{

}

void RenderSystem::FinishDx()
{
	_swapChain->Release();
	_device->Release();
	_deviceContext->Release();
}