#include <Windows.h>
#include "Engine.h"
#include "Keyboard.h"
#include "Mouse.h"
#include <memory>

Keyboard* keyboard = new Keyboard();
Mouse* mouse = new Mouse();
Engine* engine;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	if (msg == WM_CLOSE)
		PostQuitMessage(0);

	if (msg == WM_KEYDOWN) {
		if (wParam == VK_ESCAPE)
			PostQuitMessage(0);
		keyboard->keyPressed(static_cast<unsigned char>(wParam));
		engine->updateInput();
	}

	if (msg == WM_KEYUP) {
		keyboard->keyReleased(static_cast<unsigned char>(wParam));
	}

	if (msg == WM_LBUTTONDOWN)
	{
		mouse->leftPressed();
	}

	if (msg == WM_RBUTTONDOWN)
	{
		mouse->rightPressed();
	}

	if (msg == WM_LBUTTONUP)
	{
		mouse->leftReleased();
	}

	if (msg == WM_RBUTTONUP)
	{
		mouse->rightReleased();
	}

	if (msg == WM_INPUT) {
		UINT dataSize;
		GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER)); //Need to populate data size first

		if (dataSize > 0)
		{
			std::unique_ptr<BYTE[]> rawdata = std::make_unique<BYTE[]>(dataSize);
			if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawdata.get(), &dataSize, sizeof(RAWINPUTHEADER)) == dataSize)
			{
				RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawdata.get());
				if (raw->header.dwType == RIM_TYPEMOUSE)
				{
					mouse->mouseMoved(raw->data.mouse.lLastX, raw->data.mouse.lLastY);
					engine->updateInput();
				}
			}
		}
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	// tworzenie klasy okna
	const wchar_t CLASS_NAME[] = L"Window Class";
	WNDCLASSEX winClass = { 0 };
	winClass.cbSize = sizeof(winClass);
	winClass.style = CS_OWNDC;
	winClass.lpfnWndProc = WindowProc;
	winClass.cbClsExtra = 0;
	winClass.cbWndExtra = 0;
	winClass.hInstance = hInstance;
	winClass.hIcon = nullptr;
	winClass.hCursor = nullptr;
	winClass.hbrBackground = nullptr;
	winClass.lpszMenuName = nullptr;
	winClass.lpszClassName = CLASS_NAME;
	winClass.hIconSm = nullptr;
	RegisterClassEx(&winClass);

	int width = 1280;
	int height = 720;

	int centerScreenX = GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2;
	int centerScreenY = GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2;

	RECT wr;
	wr.left = centerScreenX;
	wr.top = centerScreenY;
	wr.right = wr.left + width;
	wr.bottom = wr.top + height;
	AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

	// tworzenie okna
	HWND window = CreateWindowEx(
		0,
		CLASS_NAME,
		L"Silnik 3D",
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		wr.left,
		wr.top,
		wr.right - wr.left,
		wr.bottom - wr.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr);
	ShowWindow(window, SW_SHOW);

	engine = new Engine(window, keyboard, mouse, 1280, 720);

	RAWINPUTDEVICE rid;
	rid.usUsagePage = 0x01;
	rid.usUsage = 0x02;
	rid.dwFlags = 0;
	rid.hwndTarget = NULL;

	RegisterRawInputDevices(&rid, 1, sizeof(rid));

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			engine->mainLoop();
		}
	}

	return 0;
}