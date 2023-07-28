#pragma once
#include "DirectXCommon.h"
#include <dxcapi.h>
#include"Vector4.h"
#include "Triangle.h"
#include "ConvertString.h"
#include "WinApp.h"
#include <assert.h>

#pragma comment(lib,"dxcompiler.lib")

class MyEngine
{
public:

	MyEngine();

	~MyEngine();

	void Initialize();

	void Update();

	void UpdateEnd();

	void Finalize();

private:

	IDxcBlob* CompileShader
	(
		const std::wstring& filePath,
		const wchar_t* profile,
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler
	);

	void DXCInitialize();

	void CreateRootSignature();

	void SettingInputLayout();

	void SettingBlendState();

	void SettingRasterizerState();

	void ShaderCompile();

	void CreatePSO();

	void VertexResource();

	void Render();

	void StateChange();

	void Relese();

	WinApp winApp_;
	DirectXCommon* dxCommon_ = new DirectXCommon;

	Triangle* triangle_[11];
	int triangleCount_;

	HRESULT hr_;

	IDxcUtils* dxcUtils_ = nullptr;
	IDxcCompiler3* dxcCompiler_ = nullptr;

	IDxcIncludeHandler* includeHandler_ = nullptr;

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_{};

	ID3DBlob* signatureBlob_;
	ID3DBlob* errorBlob_;
	ID3D12RootSignature* rootSignature_ = nullptr;

	D3D12_INPUT_ELEMENT_DESC inputElementDescs_[1] = {};
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_{};

	D3D12_BLEND_DESC blendDesc_{};

	IDxcBlob* vertexShaderBlob_;

	IDxcBlob* pixelShaderBlob_;

	D3D12_RASTERIZER_DESC rasterizerDesc_{};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc_{};

	ID3D12PipelineState* graphicsPipelineState_ = nullptr;

	D3D12_RESOURCE_DESC vertexResourceDesc_{};

	ID3D12Resource* vertexResource_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	UINT backBufferIndex;

	D3D12_RESOURCE_BARRIER barrier{};

	D3D12_VIEWPORT viewport_{};
	D3D12_RECT scissorRect_{};

	D3D12_CPU_DESCRIPTOR_HANDLE RTVHandle_[2];
	uint64_t fenceValue_;

	Vector4* vertexData_;

};