#pragma once
#include "DirectXClass.h"
#include <dxcapi.h>
#include"Vector4.h"
#include "Triangle.h"
#pragma comment(lib,"dxcompiler.lib")

class Sprite
{
public:
	void Initialize();

	void Initialization(WindowsClass* win, const wchar_t* title, int32_t width, int32_t height);

	void BeginFrame();

	void EndFrame();

	void Finalize();

	void Update();

	void DrawTriangle(const Vector4& a, const Vector4& b, const Vector4& c);

private:
	static WindowsClass* win_;
	static	DirectXClass* dxClass_;

	Triangle* triangle_[10];

	int triangleCount_;

	IDxcUtils* dxcUtils_;
	IDxcCompiler3* dxcCompiler_;

	IDxcIncludeHandler* includeHandler_;

	ID3DBlob* signatureBlob_;
	ID3DBlob* errorBlob_;
	ID3D12RootSignature* rootSignature_;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_{};

	D3D12_BLEND_DESC blendDesc_{};

	IDxcBlob* vertexShaderBlob_;

	IDxcBlob* pixelShaderBlob_;

	D3D12_RASTERIZER_DESC rasterizerDesc_{};

	ID3D12PipelineState* graphicsPipelineState_;

	ID3D12Resource* vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;

	D3D12_VIEWPORT viewport_{};
	D3D12_RECT scissorRect_{};

	D3D12_INPUT_ELEMENT_DESC inputElementDescs_[1];

	//頂点リソースにデータを書き込む
	Vector4* vertexData_;
	Vector4 leftBottom[10];
	Vector4 top[10];
	Vector4 rightBottom[10];

	IDxcBlob* CompileShader(
		//CompileShaderするShaderファイルへのパス
		const std::wstring& filePath,
		//Compielerに使用するProfile
		const wchar_t* profile,
		//初期化で生成したものを3つ
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler
	);

	void InitializeDxcCompiler();
	void CreateRootSignature();
	void CreateInputlayOut();
	void BlendState();
	void RasterizerState();
	void InitializePSO();
	void VertexResource();
	void ViewPort();
	void ScissorRect();

};