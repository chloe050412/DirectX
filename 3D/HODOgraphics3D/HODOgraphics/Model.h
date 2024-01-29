#pragma once

#include "HDd3d.h"
#include "HDmath.h"

class Model
{
private:
	struct VertexType
	{
		Vector3 position;
		Vector4 color;
	};

public:
	Model();
	Model(const Model&);
	~Model();

	bool Initialize(ID3D11Device*);
	void Render(ID3D11DeviceContext*);
	void Finalize();

	int GetIndexCount();

private:
	bool InitializeBuffers(ID3D11Device*);
	void RenderBuffers(ID3D11DeviceContext*);
	void FinalizeBuffers();

private:
	ID3D11Buffer* _vertexBuffer;
	ID3D11Buffer* _indexBuffer;
	int _vertexCount;
	int _indexCount;

};

