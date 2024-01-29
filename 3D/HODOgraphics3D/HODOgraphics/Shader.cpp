#include "Shader.h"

Shader::Shader()
	: _vertexShader(nullptr), _pixelShader(nullptr),
	_layout(nullptr), _matrixBuffer(nullptr)
{

}

Shader::Shader(const Shader& shader)
	: _vertexShader(nullptr), _pixelShader(nullptr),
	_layout(nullptr), _matrixBuffer(nullptr)
{

}

Shader::~Shader()
{

}

bool Shader::Initialzie(ID3D11Device* device, HWND hWnd)
{
	bool result;

	// init vertex shader and pixel shader
	result = InitializeShader(device, hWnd, L"../x64/Debug/VertexShader.cso", L"../x64/Debug/PixelShader.cso");
	if (!result)
	{
		return false;
	}

	return true;
}

bool Shader::Render(ID3D11DeviceContext* dc, int indexCount, Matrix world, Matrix view, Matrix proj)
{
	bool result;

	result = SetShaderParameters(dc, world, view, proj);
	if (!result)
	{
		return false;
	}

	RenderShader(dc, indexCount);

	return true;
}

void Shader::Finalize()
{
	FinalizeShader();

	return;
}

bool Shader::InitializeShader(ID3D11Device* device, HWND hWnd, const WCHAR* vertexShader, const WCHAR* pixelShader)
{
	HRESULT result = true;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	// init all pointer
	errorMessage = nullptr;
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer = nullptr;

	result = D3DCompileFromFile(vertexShader,
		NULL, 
		NULL, 
		"main",  // entry point
		"vs_5_0", 
		D3D10_SHADER_ENABLE_STRICTNESS,
		0, 
		NULL, 
		&errorMessage);

	if (FAILED(result)) return false;

	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(),
		NULL,
		&_vertexShader);

	if (FAILED(result)) return false;

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(),
		NULL,
		&_pixelShader);

	if (FAILED(result)) return false;

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = device->CreateInputLayout(polygonLayout,
		numElements,
		vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(),
		&_layout);

	if (FAILED(result)) return false;

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// description for constant buffer
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&matrixBufferDesc, NULL, &_matrixBuffer);

	if (FAILED(result)) return false;

	return true;
}


void Shader::FinalizeShader()
{
	if (_matrixBuffer)
	{
		_matrixBuffer->Release();
		_matrixBuffer = 0;
	}

	if (_layout)
	{
		_layout->Release();
		_layout = 0;
	}

	if (_pixelShader)
	{
		_pixelShader->Release();
		_pixelShader = 0;
	}

	if (_vertexShader)
	{
		_vertexShader->Release();
		_vertexShader = 0;
	}

	return;
}

bool Shader::SetShaderParameters(ID3D11DeviceContext* dc, Matrix world, Matrix view, Matrix proj)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	world.Transpose(world);
	view.Transpose(view);
	proj.Transpose(proj);

	result = dc->Map(_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	
	if (FAILED(result)) return false;

	dataPtr = (MatrixBufferType*)mappedResource.pData;

	dataPtr->world = world;
	dataPtr->view = view;
	dataPtr->projection = proj;

	dc->Unmap(_matrixBuffer, 0);

	bufferNumber = 0;

	dc->VSSetConstantBuffers(bufferNumber, 1, &_matrixBuffer);

	return true;
}

void Shader::RenderShader(ID3D11DeviceContext* dc, int indexCount)
{
	dc->IASetInputLayout(_layout);
	dc->VSSetShader(_vertexShader, NULL, 0);
	dc->PSSetShader(_pixelShader, NULL, 0);
	dc->DrawIndexed(indexCount, 0, 0);

	return;
}
