#include "Model.h"
using namespace DirectX;

Model::Model()
	:_vertexBuffer(nullptr), _indexBuffer(nullptr),
	_indexCount(0), _vertexCount(0)
{

}

Model::Model(const Model&)
	:_vertexBuffer(nullptr), _indexBuffer(nullptr),
	_indexCount(0), _vertexCount(0)
{

}

Model::~Model()
{

}

bool Model::Initialize(ID3D11Device* device)
{
	bool result;

	result = InitializeBuffers(device);

	if (!result)
	{
		return false;
	}

	return true;
}

void Model::Render(ID3D11DeviceContext* dc)
{
	RenderBuffers(dc);

	return;
}

void Model::Finalize()
{
	FinalizeBuffers();
}

int Model::GetIndexCount()
{
	return _indexCount;
}

bool Model::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// set vertex, index buffer size
	_vertexCount = 4;
	_indexCount = 6;

	// create vertex array
	vertices = new VertexType[_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// create index array
	indices = new unsigned long[_indexCount];
	if (!indices)
	{
		return false;
	}

	// fill vertex buffer with value
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertices[0].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	vertices[1].position = XMFLOAT3(-1.0f, 1.0f, 0.0f);
	vertices[1].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	vertices[2].position = XMFLOAT3(1.0f, 1.0f, 0.0f);
	vertices[2].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	vertices[3].position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertices[3].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	// fill index buffer with value
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 2;
	indices[4] = 3;
	indices[5] = 0;


	// set vertex buffer type desc
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * _vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// create vertex buffer
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// set index buffer type desc
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * _indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// create index buffer
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void Model::RenderBuffers(ID3D11DeviceContext* dc)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	dc->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
	dc->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

void Model::FinalizeBuffers()
{
	// release vertex buffer
	if (_indexBuffer)
	{
		_indexBuffer->Release();
		_indexBuffer = 0;
	}

	if (_vertexBuffer)
	{
		_vertexBuffer->Release();
		_vertexBuffer = 0;
	}

	return;
}
