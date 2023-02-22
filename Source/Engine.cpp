#include "Engine.h"
#include <d3dcompiler.h>
#include <iostream>
#include <DirectXMath.h>
#include <WICTextureLoader.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "DirectXTK.lib")

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

Engine::Engine(HWND hWnd, Keyboard* keyboard, Mouse* mouse, int width, int height)
{
	background = DirectX::XMFLOAT3(0.95, 0.95, 0.95);

	weaponPosition = DirectX::XMFLOAT3(0.76, -0.78, 0.98);
	weaponRotation = DirectX::XMFLOAT3(0, -1.63, 0);

	l.ambientLightColor = DirectX::XMFLOAT3(1, 1, 1);
	l.ambientLightStrength = 0.15;
	l.LightAttenuation_a = 0.1;
	l.LightAttenuation_b = 0.1;
	l.LightAttenuation_c = 0.1;
	l.LightColor = DirectX::XMFLOAT3(1, 1, 1);
	l.LightPosition = DirectX::XMFLOAT3(5, 7, 0);
	l.LightStrength = 10;

	this->keyboard = keyboard;
	this->mouse = mouse;
	this->width = width;
	this->height = height;

	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwap,
		&pDevice,
		nullptr,
		&pContext
	);
	wrl::ComPtr<ID3D11Resource> backBuffer;
	pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer);
	pDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, &pTarget);

	wrl::ComPtr<ID3D11DepthStencilState> depthState;
	D3D11_DEPTH_STENCIL_DESC depth = {};
	depth.DepthEnable = TRUE;
	depth.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth.DepthFunc = D3D11_COMPARISON_LESS;
	pDevice->CreateDepthStencilState(&depth, &depthState);
	pContext->OMSetDepthStencilState(depthState.Get(), 1u);

	wrl::ComPtr<ID3D11Texture2D> depthStencil;
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1u;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	pDevice->CreateTexture2D(&descDepth, nullptr, &depthStencil);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;
	pDevice->CreateDepthStencilView(depthStencil.Get(), &descDSV, &pDepth);
	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), pDepth.Get());

	viewport.Width = (float)width;
	viewport.Height = (float)height;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	wrl::ComPtr<ID3DBlob> blob;
	D3DReadFileToBlob(L"pixelshader.cso", &blob);
	pDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pixelShader);

	D3DReadFileToBlob(L"vertexshader.cso", &blob);
	pDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &vertexShader);

	const D3D11_INPUT_ELEMENT_DESC ied[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
	};
	pDevice->CreateInputLayout(ied, (UINT)std::size(ied), blob->GetBufferPointer(), blob->GetBufferSize(), &inputLayout);

	D3D11_BUFFER_DESC descConst;
	descConst.Usage = D3D11_USAGE_DYNAMIC;
	descConst.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	descConst.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	descConst.MiscFlags = 0;
	descConst.ByteWidth = sizeof(ConstantBufferStruct);
	descConst.StructureByteStride = 0;

	pDevice->CreateBuffer(&descConst, 0, constantBuffer.GetAddressOf());

	D3D11_BUFFER_DESC descLight;
	descLight.Usage = D3D11_USAGE_DYNAMIC;
	descLight.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	descLight.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	descLight.MiscFlags = 0;
	descLight.ByteWidth = sizeof(ConstantBufferStruct);
	descLight.StructureByteStride = 0;

	pDevice->CreateBuffer(&descLight, 0, lightBuffer.GetAddressOf());

	camera.SetPosition(0.0f, 0.0f, -2.0f);
	camera.SetProjectionValues(90.0f, static_cast<float>(width) / static_cast<float>(height), 0.1f, 3000.0f);

	CD3D11_SAMPLER_DESC sampDesc(D3D11_DEFAULT);
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	pDevice->CreateSamplerState(&sampDesc, samplerState.GetAddressOf());

	D3D11_RASTERIZER_DESC raster_desc;
	raster_desc.FillMode = D3D11_FILL_SOLID;
	raster_desc.CullMode = D3D11_CULL_BACK;
	raster_desc.FrontCounterClockwise = false;
	raster_desc.DepthBias = 0;
	raster_desc.DepthBiasClamp = 0.0f;
	raster_desc.SlopeScaledDepthBias = 0.0f;
	raster_desc.DepthClipEnable = true;
	raster_desc.ScissorEnable = false;
	raster_desc.MultisampleEnable = false;
	raster_desc.AntialiasedLineEnable = false;

	pDevice->CreateRasterizerState(&raster_desc, rasterizerState.GetAddressOf());

	objects.push_back(DrawableGameObject(pContext.Get(), pDevice.Get(), indexBuffer.Get(), vertexBuffer.Get(), constantBuffer.Get(), L"car.png", "car.obj"));
	objects.push_back(DrawableGameObject(pContext.Get(), pDevice.Get(), indexBuffer.Get(), vertexBuffer.Get(), constantBuffer.Get(), L"bridge.png", "bridge.obj"));

	objects[0].AdjustPosition(2, -1.2, 2);
	objects[0].AdjustRotation(0, -1.5, 0);
	objects[1].AdjustPosition(0, -16, 0);

	pContext->RSSetViewports(1u, &viewport);

	light = Light(pContext.Get(), lightBuffer.Get());

	weapon = new DrawableGameObject(pContext.Get(), pDevice.Get(), indexBuffer.Get(), vertexBuffer.Get(), constantBuffer.Get(), L"pistol.jpg", "pistol.obj");
	bullets.push_back(DrawableGameObject(pContext.Get(), pDevice.Get(), indexBuffer.Get(), vertexBuffer.Get(), constantBuffer.Get(), L"bullet.png", "bullet.obj"));
	bullets[0].SetPosition(-100, -100, -100);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(pDevice.Get(), pContext.Get());
	ImGui::StyleColorsDark();
}

Engine::~Engine()
{
	if (nullptr != ImGui::GetCurrentContext())
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
}

void Engine::swapBuffer()
{
	pSwap->Present(1u, 0u);
}

void Engine::mainLoop()
{
	clearToColor(background.x, background.y, background.z);
	updateInput();
	drawTriangles();
	drawGui();
	swapBuffer();
}

void Engine::clearToColor(float r, float g, float b)
{
	const float color[] = { r, g, b, 1.0f };
	pContext->ClearRenderTargetView(pTarget.Get(), color);
	pContext->ClearDepthStencilView(pDepth.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void Engine::drawTriangles() {
	light.setBuffer();
	pContext->IASetInputLayout(inputLayout.Get());
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());
	pContext->PSSetShader(pixelShader.Get(), nullptr, 0);
	pContext->VSSetShader(vertexShader.Get(), nullptr, 0);
	pContext->PSSetConstantBuffers(0, 1, lightBuffer.GetAddressOf());
	pContext->RSSetState(rasterizerState.Get());

	weapon->Draw(camera.GetProjectionMatrix());
	weapon->SetPosition(weaponPosition);
	weapon->SetRotation(weaponRotation);

	for (int i = 0; i < objects.size(); i++) {
		objects[i].Draw(camera.GetViewMatrix() * camera.GetProjectionMatrix());
	}

	for (int i = 0; i < bullets.size(); i++) {
		bullets[i].Draw(camera.GetViewMatrix() * camera.GetProjectionMatrix());
		bullets[i].AdjustPosition(bullets[i].forward.x, bullets[i].forward.y, bullets[i].forward.z);
	}
}

void Engine::updateInput() {
	if (keyboard->keyIsPressed('W'))
		camera.AdjustPosition(camera.GetForwardVector() * camera.cameraSpeed);
	if (keyboard->keyIsPressed('S'))
		camera.AdjustPosition(camera.GetBackwardVector() * camera.cameraSpeed);
	if (keyboard->keyIsPressed('A'))
		camera.AdjustPosition(camera.GetLeftVector() * camera.cameraSpeed);
	if (keyboard->keyIsPressed('D'))
		camera.AdjustPosition(camera.GetRightVector() * camera.cameraSpeed);

	while (!mouse->EventBufferIsEmpty())
	{
		MouseEvent event = mouse->ReadEvent();
		if (mouse->rightClicked)
			camera.AdjustRotation((float)event.getMouseY() * 0.01f, (float)event.getMouseX() * 0.01f, 0);
		if (mouse->leftClicked == 1) {
			if (bullets[0].GetPositionFloat3().y < -20.0f) {
				bullets[0].SetPosition(camera.GetPositionVector() + camera.GetForwardVector());
				XMStoreFloat3(&bullets[0].forward, camera.GetForwardVector());
			}
			else {
				bullets.push_back(DrawableGameObject(pContext.Get(), pDevice.Get(), indexBuffer.Get(), vertexBuffer.Get(), constantBuffer.Get(), L"bullet.png", "bullet.obj"));
				bullets[bullets.size() - 1].SetPosition(camera.GetPositionVector() + camera.GetForwardVector());
				XMStoreFloat3(&bullets[bullets.size() - 1].forward, camera.GetForwardVector());
			}
			mouse->leftClicked = 2;
		}
	}
}

void Engine::drawGui()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Light");
	ImGui::DragFloat3("Ambient Light Color", &l.ambientLightColor.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Ambient Light Strength", &l.ambientLightStrength, 0.01f, 0.0f, 1.0f);

	ImGui::NewLine();
	ImGui::DragFloat3("Light Position", &l.LightPosition.x, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat3("Light Color", &l.LightColor.x, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("Light Strength", &l.LightStrength, 0.01f, 0.0f, 10.0f);

	ImGui::NewLine();
	ImGui::DragFloat("Light Attenuation A", &l.LightAttenuation_a, 0.01f, 0.1f, 10.0f);
	ImGui::DragFloat("Light Attenuation B", &l.LightAttenuation_b, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("Light Attenuation C", &l.LightAttenuation_c, 0.01f, 0.0f, 10.0f);
	ImGui::End();

	ImGui::Begin("Weapon");
	ImGui::DragFloat3("Weapon Position", &weaponPosition.x, 0.01f, -10.0f, 10.0f);
	ImGui::DragFloat3("Weapon Rotation", &weaponRotation.x, 0.01f, -10.0f, 10.0f);
	ImGui::End();

	ImGui::Begin("Background");
	ImGui::DragFloat3("Color", &background.x, 0.01f, 0.0f, 1.0f);
	ImGui::End();

	light.setLight(l);

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}