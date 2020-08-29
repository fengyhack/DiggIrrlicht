// Win32Windowmint main

#include <Windows.h>
#include <irrlicht.h> 
using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
#pragma comment(lib,"irrlicht.lib")


HWND CreateMainWindow(HINSTANCE hInstance, LPCSTR lpszMainWindow, WNDPROC lpfnWinProc,
	DWORD windowStyle = WS_SYSMENU, int windowWidth = 800, int windowHeight = 600);

LRESULT CALLBACK MyWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pszCmdLine, INT nCmdShow)
{
	LPCSTR lpszMyWindow = "DrawOverlay";
	HWND hMainWindow = NULL;

	hMainWindow = CreateMainWindow(hInstance, lpszMyWindow, MyWindowProc);

	SIrrlichtCreationParameters params;
	params.DriverType = EDT_DIRECT3D9;
	params.WindowId = reinterpret_cast<void*>(hMainWindow);

	IrrlichtDevice* device = createDeviceEx(params);
	IVideoDriver* driver = device->getVideoDriver();
	ITexture* image = driver->getTexture("Assets/forza.png");

	ShowWindow(hMainWindow, SW_SHOW);
	UpdateWindow(hMainWindow);

	MSG msg = { 0 };
	while (msg.message!=WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			driver->beginScene(true, true, SColor(255, 255, 150, 100));

			driver->draw2DImage(image, position2d<s32>(200, 80),
				rect<s32>(0, 0, 400, 400), 0, SColor(200, 255, 0, 0), true);

			for (int i = 0; i < 5; ++i)
			{
				for (int j = 0; j < 5; ++j)
				{
					driver->draw2DImage(image, position2d<s32>(i * 150 + 30, j * 150 + 10),
						rect<s32>(0, 0, 128, 128), 0, SColor(255, 255, 175 + 20 * i, 55 + 50 * j), true);
				}
			}

			driver->endScene();
		}
	}

	device->closeDevice();
	device->drop();

	UnregisterClass(lpszMyWindow, hInstance);

	return 0;
}


HWND CreateMainWindow(HINSTANCE hInstance, LPCSTR lpszMainWindow,
	WNDPROC lpfnWinProc, DWORD windowStyle, int windowWidth, int windowHeight)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = lpfnWinProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = lpszMainWindow;
	wcex.hIconSm = 0;

	RegisterClassEx(&wcex);

	HWND hMainWindow = CreateWindow(
		lpszMainWindow, lpszMainWindow, windowStyle,
		0, 0, windowWidth, windowHeight,
		NULL, NULL, hInstance, NULL);

	return hMainWindow;
}

LRESULT CALLBACK MyWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}