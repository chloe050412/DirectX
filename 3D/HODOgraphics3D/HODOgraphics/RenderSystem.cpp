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

void RenderSystem::Update(bool isFullScreen)
{
	_isFullscreen = isFullScreen;

	BeginRender();

	if (GetAsyncKeyState('F'))
	{
		ChangeFullScreenMode();
	}
}

void RenderSystem::Finalize()
{
	FinishDx();
}

void RenderSystem::StartDx()
{
	// ���� ü���� ���� ������ ����ü ����
	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferCount = 1;                           // �� ���� ����
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;   // 32bit �÷� ���
	swapChainDesc.BufferDesc.Width = _screenWidth;
	swapChainDesc.BufferDesc.Height = _screenHeight;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;   // ���� ü���� ���۹�(���� Ÿ��) ������� ����� ����
	swapChainDesc.OutputWindow = _hWnd;                        // ��� ������� ������ ������
	swapChainDesc.SampleDesc.Count = 4;                        // ��Ƽ ���� �ܰ迡���� �ȼ� ��
	swapChainDesc.Windowed = true;                           // window ��������, Ǯ ��ũ������
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // Ǯ ��ũ�� ����Ī�� ����Ѵ�

	// ����̽�, ����̽� ���ؽ�Ʈ, ���� ü�� ����
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

	// �׸��� �׸� �� ������ �ּ� ��������
	ID3D11Texture2D* pbackBuffer;
	// ���� ü���� �� ���۸� ������ �ؽ�ó ��ü�� �Ҵ�
	_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pbackBuffer);

	// �� ���� �ּҸ� �̿��� ���� Ÿ�� �����ϱ�
	_device->CreateRenderTargetView(pbackBuffer, NULL, _renderTarget.GetAddressOf());
	pbackBuffer->Release();

	// ���� Ÿ���� �� ���۷� �����ϱ�
	_deviceContext->OMSetRenderTargets(1, _renderTarget.GetAddressOf(), NULL);

	// ����Ʈ ����
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
	// ������ ���� ���� ��ȯ�� �� ����
	_swapChain->SetFullscreenState(false, NULL); 

	_swapChain->Release();
	_device->Release();
	_deviceContext->Release();
}

void RenderSystem::ChangeFullScreenMode()
{
	_swapChain->SetFullscreenState(true, NULL);

	// ������ ����� �ٽ� �����´�
	RECT rect;

	GetClientRect(_hWnd, &rect);

	_screenWidth = rect.right - rect.left;
	_screenHeight = rect.bottom - rect.top;

	//// ���� ���� Ÿ���� �����
	//_deviceContext->OMSetRenderTargets(0, NULL, NULL);
	//_renderTarget->Release();

	//// �� ������ ����� �����Ѵ�
	//_swapChain->ResizeBuffers(1,
	//	_screenWidth,
	//	_screenHeight,
	//	DXGI_FORMAT_R8G8B8A8_UNORM,
	//	0);

	//// ���� Ÿ���� �ٽ� �����Ѵ�
	//// �׸��� �׸� �� ������ �ּ� ��������
	//ID3D11Texture2D* pbackBuffer;
	//// ���� ü���� �� ���۸� ������ �ؽ�ó ��ü�� �Ҵ�
	//_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pbackBuffer);

	//// �� ���� �ּҸ� �̿��� ���� Ÿ�� �����ϱ�
	//_device->CreateRenderTargetView(pbackBuffer, NULL, _renderTarget.GetAddressOf());
	//pbackBuffer->Release();

	//// ���� Ÿ���� �� ���۷� �����ϱ�
	//_deviceContext->OMSetRenderTargets(1, _renderTarget.GetAddressOf(), NULL);

	//// ����Ʈ ����
	//D3D11_VIEWPORT viewport;
	//ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	//viewport.TopLeftX = 0;
	//viewport.TopLeftY = 0;
	//viewport.Width = _screenWidth;
	//viewport.Height = _screenHeight;

	//_deviceContext->RSSetViewports(1, &viewport);

	// ���� ü���� ���� ������ ����ü ����
	DXGI_MODE_DESC desc;

	desc.Width = _screenWidth;
	desc.Height = _screenHeight;
	desc.RefreshRate.Numerator = 60;
	desc.RefreshRate.Denominator = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	_swapChain->ResizeTarget(&desc);

}
