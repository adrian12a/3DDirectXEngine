#pragma once
#include <d3d11.h>
#include <wrl.h>
#include "Camera.h"
#include "ConstantBufferStruct.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Model.h"
#include "DrawableGameObject.h"
#include "Light.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"

class Engine
{
public:
	Engine(HWND hWnd, Keyboard* keyboard, Mouse* mouse, int width, int height);
	~Engine();
	void swapBuffer();
	void mainLoop();
	void clearToColor(float r, float g, float b);
	void drawTriangles();
	void updateInput();
	void drawGui();
	int width;
	int height;
	Camera camera;
	Keyboard* keyboard;
	Mouse* mouse;
	Model model;
	Light light;
	LightBufferStruct l;
	std::vector<DrawableGameObject> objects;
	std::vector<DrawableGameObject> bullets;
	DrawableGameObject* weapon;
	DirectX::XMFLOAT3 weaponPosition;
	DirectX::XMFLOAT3 weaponRotation;
	DirectX::XMFLOAT3 background;
private:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepth;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> lightBuffer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
	D3D11_VIEWPORT viewport;
};