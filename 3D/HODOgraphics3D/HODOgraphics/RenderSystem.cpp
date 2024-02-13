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
	if (!_model->Initialize(_device.Get()))
	{
		MessageBox(_hWnd, L"Could not initialize the model object.", L"Error", MB_OK);
	}

	_shader = new Shader();
	if (!_shader->Initialzie(_device.Get(), _hWnd))
	{
		MessageBox(_hWnd, L"Could not initialize the shader object.", L"Error", MB_OK);
	}
}

void RenderSystem::Update()
{
	BeginRender();
	Render();
	EndRender();
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

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	// 디바이스, 디바이스 컨텍스트, 스왑 체인 생성
	D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		&featureLevel,
		1,
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

	// 깊이 버퍼 구조체를 초기화 합니다.
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// 깊이 버퍼 구조체를 작성합니다.
	depthBufferDesc.Width = _screenWidth;
	depthBufferDesc.Height = _screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// 설정된 깊이버퍼 구조체를 사용하여 깊이 버퍼 텍스쳐를 생성한다.
	_device->CreateTexture2D(&depthBufferDesc, NULL, _depthStencilBuffer.GetAddressOf());

	// 스텐실 상태 구조체를 초기화 합니다.
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// 스텐실 상태 구조체를 작성합니다.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable= true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// 픽셀 정면의 스텐실 설정입니다.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 픽셀 뒷면의 스텐실 설정입니다.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 깊이 스텐실 상태를 설정합니다.
	_deviceContext->OMSetDepthStencilState(_depthStencilState.Get(), 1);

	// 깊이 스텐실 뷰의 구조체를 초기화합니다.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// 깊이 스텐실 뷰 구조체를 설정한다.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// 깊이 스텐실 뷰를 생성한다.
	_device->CreateDepthStencilView(_depthStencilBuffer.Get(), &depthStencilViewDesc, _depthStencilView.GetAddressOf());

	// 렌더 타겟을 백 버퍼로 설정하기
	_deviceContext->OMSetRenderTargets(1, _renderTarget.GetAddressOf(), NULL);

	// 그려지는 폴리곤과 방법을 결정할 레스터 구조체를 설정한다.
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	_device->CreateRasterizerState(&rasterDesc, _rasterState.GetAddressOf());

	// 이제 레스터 라이저 상태를 설정한다.
	_deviceContext->RSSetState(_rasterState.Get());

	// 뷰포트 설정
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)_screenWidth;
	viewport.Height = (float)_screenHeight;

	_deviceContext->RSSetViewports(1, &viewport);

	// 투영 행렬을 설정한다.
	float fieldOfView = 3.141592654 / 4.0f;
	float screenAspect = static_cast<float>(_screenWidth) / static_cast<float>(_screenHeight);

	// 3D 렌더링을 위핸 투영 행렬을 만든다.
	_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, 0.1f, 1000.f);

	// 세계 행렬을 항동 행렬로 초기화 한다.
	_worldMatrix = DirectX::XMMatrixIdentity();

	// 2D렌더링을위한 직교 투영 행렬을 만든다.
	_orthoMatrix = DirectX::XMMatrixOrthographicLH(static_cast<float>(_screenWidth), static_cast<float>(_screenHeight), 0.1f, 1000.f);
}

void RenderSystem::BeginRender()
{
	float color[4] = { 0.f, 0.2f, 0.4f, 1.0f };
	_deviceContext->ClearRenderTargetView(_renderTarget.Get(), color);
	_deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.f, 0);
}

void RenderSystem::Render()
{
	_camera->Render();

	Matrix worldMatrix, viewMatrix, projectionMatrix;

	worldMatrix = _worldMatrix;
	projectionMatrix = _projectionMatrix;
	_camera->GetViewMatrix(viewMatrix);

	_model->Render(_deviceContext.Get());
	_shader->Render(_deviceContext.Get(), _model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
}

void RenderSystem::EndRender()
{
	_swapChain->Present(0, 0);
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
