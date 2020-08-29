#include <Windows.h>
#include <irrlicht.h>
using namespace irr;
using namespace core;
using namespace video;
using namespace scene;

#pragma comment(lib,"irrlicht.lib")

HWND InitMainWindow(HINSTANCE hInst);
void IrrlichtColorFade(void);

INT APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//创建并初始化主窗口
	//HWND hWnd = InitMainWindow(hInstance);
	//ShowWindow(hWnd, SW_HIDE); //隐藏主窗口
	//SetCursor(NULL); //影藏光标
	//UpdateWindow(hWnd); //更新

	// 颜色渐变窗口, 渲染循环
	try
	{
		IrrlichtColorFade();
	}
	catch (LPCWSTR except)
	{
		//MessageBox(hWnd, except, L"Exception", MB_OK);
	}

	//DestroyWindow(hWnd);
	return 0;
}

HWND InitMainWindow(HINSTANCE hInst)
{
	LPCWSTR pszWin = L"HidenMainWindow";
	LPCWSTR pszClx = L"MainWindowClass";

	WNDCLASS wndClx;
	wndClx.hInstance = hInst;
	//wndClx.hbrBackground = CreateSolidBrush(RGB(200, 250, 200));
	//wndClx.hCursor = LoadCursor(hInstance, IDC_ARROW);
	//wndClx.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wndClx.lpszClassName = pszClx;
	//wndClx.lpszMenuName = NULL;
	wndClx.lpfnWndProc = DefWindowProc; // 默认消息处理回调函数
	//wndClx.style = CS_HREDRAW | CS_VREDRAW;
	//wndClx.cbClsExtra = 0;
	//wndClx.cbWndExtra = 0;

	RegisterClass(&wndClx);

	return CreateWindow(pszClx, pszWin, WS_POPUP, 0, 0, 100, 100, NULL, NULL, hInst, NULL);
}

void IrrlichtColorFade(void)
{
	IrrlichtDevice* device = createDevice(EDT_DIRECT3D9, dimension2d<u32>(800, 600), 32U);
	if (device == NULL) throw(L"Failed to create device!!");

	device->setWindowCaption(L"Color Fade In/Out");
	IVideoDriver* driver = device->getVideoDriver();
	ITimer* timer = device->getTimer();
	f32 bg_r = 255.0f;
	f32 bg_g = 0.0f;
	f32 bg_b = 255.0f;
	bool fadeOut = true;
	int lastFPS = -1;
	u32 then = timer->getTime();
	const f32 fadeRate = 0.05f;

	while (device->run())
	{
		const u32 now = timer->getTime();
		const f32 frameDeltaTime = (f32)(now - then);
		then = now;
		if (bg_r <= 0.0f) fadeOut = false;
		else if (bg_r >= 255.0f) fadeOut = true;

		if (fadeOut)
		{
			bg_r -= fadeRate * frameDeltaTime;
			bg_g += fadeRate * frameDeltaTime;
			bg_b -= fadeRate * frameDeltaTime;
		}
		else
		{
			bg_r += fadeRate * frameDeltaTime;
			bg_g -= fadeRate * frameDeltaTime;
			bg_b += fadeRate * frameDeltaTime;
		}

		driver->beginScene(true, true, SColor(255, (u32)bg_r, (u32)bg_g, (u32)bg_b));
		driver->endScene();
	}

	device->drop();
}