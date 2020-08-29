//
#ifndef __SIMPLE_GAME_H_INCLUDED__
#define __SIMPLE_GAME_H_INCLUDED__

#include <windows.h>

// Irrlicht dependencies
#include <irrlicht.h>
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;
#pragma comment(lib,"irrlicht.lib")

// IrrKlang audio support
#include <irrKlang.h>	
using namespace irrklang;
#pragma comment (lib, "irrKlang.lib")

#include "Parameters.h"
using namespace parameters;

typedef struct particleImpact
{
	u32 when;
	vector3df pos;
	vector3df outVector;
} SParticleImpact;

class Size
{
public:
	Size(int w, int h);
	int Width() const;
	int Height() const;
private:
	int width;
	int height;
};

class Game :public IEventReceiver
{
public:
	const int CAMERA_COUNT=7;

	Game(E_DRIVER_TYPE driverType, Size windowSize,  FullScreen::Status fullScreen, 
		BufferedShadows::Status bufferedShadow, VerticalSync::Status verticalSync, 
		AntiAlias::Status antiAlias, PlayMusic::Status playMusic);

	void SetBkMusic(const char* pszMusicFile);

	~Game();
	void Run();
	virtual bool OnEvent(const SEvent& evt);
private:
	void CreateLoadingScreen();
	void LoadSceneData();
	void SwitchToNextScene();
	void Shoot();
	void CreateParticleImpacts();

	SIrrlichtCreationParameters m_irrParams;
	IrrlichtDevice* m_irrDevice;
	IVideoDriver*  m_videoDriver;
	ISceneManager* m_sceneManager;
	IGUIEnvironment* m_guiEnv;
	IFileSystem* m_fileSystem;
	ITimer*  timer;

	PlayMusic::Status m_playMusic;
	ISoundEngine* m_soundEngine;
	ISound* m_soundPlayed;
	ISoundSource* m_ballSound;
	ISoundSource* m_impactSound;

	int m_currentScene;
	SColor m_bgColor;

	IGUIStaticText* m_statusText;
	IGUIInOutFader* m_InOutFader;

	IQ3LevelMesh* m_quakeLevelMesh;
	ISceneNode* m_quakeLevelNode;
	ISceneNode* m_skyBoxNode;
	IAnimatedMeshSceneNode* m_model_1;
	IAnimatedMeshSceneNode* m_model_2;
	IParticleSystemSceneNode* m_campFire;

	IMetaTriangleSelector* m_metaSelector;
	ITriangleSelector* m_mapSelector;

	s32 m_sceneStartTime;
	s32 m_sceneDurationTime;

	array<SParticleImpact> m_Impacts;
};

#endif