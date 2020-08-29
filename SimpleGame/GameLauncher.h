#ifndef __GAMELAUNCHER_H_INCLUDED__
#define __GAMELAUNCHER_H_INCLUDED__

#include <irrlicht.h>
using namespace irr;
using namespace core;
using namespace io;
using namespace video;
using namespace gui;
using namespace scene;
#pragma comment(lib,"irrlicht.lib")

#include "Parameters.h"
using namespace parameters;

class GameLauncher :	public IEventReceiver
{
public:
	GameLauncher();

	bool Run(E_DRIVER_TYPE& outDriver, FullScreen::Status& outFullScreen,
		BufferedShadows::Status& outBufferedShadows,
		VerticalSync::Status&outVerticalSync, AntiAlias::Status& outAntiAlias, 
		PlayMusic::Status& outPlayMusic);

	virtual bool OnEvent(const SEvent& event);

private:

	void SetTransparency();

	IGUIButton* startButton;
	IrrlichtDevice *launcherDevice;
	s32 selected;
	bool start;
	bool transparent;

	FullScreen::Status fullScreen;
	PlayMusic::Status playMusic;
	BufferedShadows::Status shadows;
	VerticalSync::Status vSync;
	AntiAlias::Status antiAlias;

	IAnimatedMesh* quakeLevel;
	ISceneNode* lightMapNode;
	ISceneNode* dynamicNode;

	SColor SkinColor[EGDC_COUNT];
	void GetOriginalSkinColor();
};

#endif