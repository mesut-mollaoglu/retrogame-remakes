#include "GameController.h"
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#pragma warning(disable:4996)

GameController* controller;
Graphics* graphics;
Sprite* spriteLoader;
HWND windowHandle = nullptr;

MSG Structures::Window::message = { 0 };

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_DESTROY) { PostQuitMessage(0); controller->Unload(); return 0; }
	if (message == WM_KEYDOWN) { BaseStateMachine::lastKeyHit = (char)wParam; }
	DefWindowProc(hwnd, message, wParam, lParam);
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"ClassName";
	wc.lpszMenuName = nullptr;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClassEx(&wc);
	RECT rect = { 0, 0, 300, 600 };
	AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, false, WS_EX_OVERLAPPEDWINDOW);
	windowHandle = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"ClassName", L"Tetris", WS_OVERLAPPEDWINDOW, 100, 0, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, hInstance, nullptr);
	SaveSystem::FileInit("Score.txt");
	graphics = new Graphics();
	controller = new GameController();
	if (!graphics->InitGraphics(windowHandle)) {
		delete graphics;
		return -1;
	}
	ShowWindow(windowHandle, nCmdShow);
	controller->Load();
	Structures::Window::message.message = WM_NULL;
	while (Structures::Window::message.message != WM_QUIT) {
		if (PeekMessage(&Structures::Window::message, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&Structures::Window::message);
			DispatchMessage(&Structures::Window::message);
		}
		else {
			controller->Render();
			controller->Update();
		}
	}
	return 0;
}