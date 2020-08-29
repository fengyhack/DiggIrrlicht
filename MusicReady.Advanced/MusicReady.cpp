#include <Windows.h>
#include <irrlicht.h>
#include <irrKlang.h>
using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace irrklang;

#pragma comment(lib,"irrlicht.lib")
#pragma comment(lib,"irrKlang.lib")

HWND InitMainWindow(HINSTANCE hInst);
void IrrlichtMusiLang(const char* pszFileName);

INT APIENTRY WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance, 
	LPSTR lpCmdLine, 
	int nCmdShow)
{
	//创建并初始化主窗口
	HWND hWnd = InitMainWindow(hInstance); 
	ShowWindow(hWnd, SW_HIDE); //隐藏主窗口
	SetCursor(NULL); //影藏光标
	UpdateWindow(hWnd);

	try
	{
		IrrlichtMusiLang("Assets/Background.mp3");
	}
	catch (PWSTR except)
	{
		MessageBox(hWnd, except, TEXT("Exception"), MB_OK);
	}

	DestroyWindow(hWnd);
	return 0;
}

HWND InitMainWindow(HINSTANCE hInst)
{
	LPCWSTR pszWin = L"HidenMainWindow";
	LPCWSTR pszClx = L"MainWindowClass";

	WNDCLASS wndClx;
	wndClx.hInstance = hInst;
	wndClx.lpszClassName = pszClx;
	wndClx.lpfnWndProc = DefWindowProc; // 默认消息处理回调函数

	RegisterClass(&wndClx);

	return CreateWindow(pszClx, pszWin, WS_POPUP, 0, 0, 100, 100, NULL, NULL, hInst, NULL);
}

void IrrlichtMusiLang(const char* pszFileName)
{
	IrrlichtDevice* device = createDevice(EDT_DIRECT3D9, dimension2d<u32>(800, 600), 32U);
	if (device == NULL) throw(L"Failed to create device!!");

	////////////////////////////////////////////////////////////////////////////////
	// Play Music
	ISoundEngine* engine = createIrrKlangDevice();
	if (engine == NULL) throw (L"An error ocurred in \'createIrrKlangDevice\'");
	ISound* music = engine->play2D(pszFileName, true, false, true, ESM_AUTO_DETECT, true);
	if (music == NULL) throw (L"An error ocurred in \'play2DMusic\'");
	ISoundEffectControl* fx = fx = music->getSoundEffectControl();
	if (fx == NULL) throw(L"This device or sound does not support sound effects.");
	fx->enableWavesReverbSoundEffect();
	// Play Music
	////////////////////////////////////////////////////////////////////////////////

	device->setWindowCaption(L"Win32 window with irr music");
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

	music->drop(); // 释放资源
	engine->drop(); // 撤出引擎
	device->drop();
}