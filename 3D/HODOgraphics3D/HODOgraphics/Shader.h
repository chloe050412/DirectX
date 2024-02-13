#pragma once
#include "HDmath.h"
#include "HDd3d.h"

/// <summary>
///  call hlsl shader
/// </summary>

class Shader
{
private:
	struct MatrixBufferType
	{
		Matrix world;
		Matrix view;
		Matrix projection;
	};

public:
	Shader();
	Shader(const Shader&);
	~Shader();
	bool Initialzie(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext*, int, Matrix, Matrix, Matrix);
	void Finalize();

	bool InitializeShader(ID3D11Device*, HWND, const WCHAR*, const WCHAR*);
	void FinalizeShader();

	bool SetShaderParameters(ID3D11DeviceContext*, Matrix, Matrix, Matrix);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	HWND _hWnd;

	ID3D11VertexShader* _vertexShader;
	ID3D11PixelShader* _pixelShader;
	ID3D11InputLayout* _layout;
	ID3D11Buffer* _matrixBuffer;

};

