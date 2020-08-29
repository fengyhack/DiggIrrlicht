#include "GameLauncher.h"
#include "SimpleGame.h"

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR strCmdLine, INT nCmdShow)
{
	GameLauncher launcher;
	FullScreen::Status fullScreen = FullScreen::OFF;
	PlayMusic::Status playMusic = PlayMusic::ON;
	BufferedShadows::Status shadows = BufferedShadows::OFF;
	VerticalSync::Status vSync = VerticalSync::OFF;
	AntiAlias::Status antiAlias = AntiAlias::OFF;
	E_DRIVER_TYPE driverType = video::EDT_DIRECT3D9;
	bool ret=launcher.Run(driverType,fullScreen, shadows, vSync, antiAlias, playMusic);
	if (ret)
	{
		Game* myGame = new Game(EDT_DIRECT3D9, Size(900, 640), 
			fullScreen, shadows,vSync, antiAlias, playMusic);
		myGame->SetBkMusic("Assets/Background.mp3");
		myGame->Run();
		delete myGame;
	}
	return 0;
}