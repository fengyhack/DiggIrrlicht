#include "GameLauncher.h"


GameLauncher::GameLauncher()
{
	startButton = NULL;
	launcherDevice = NULL;
	selected = 2;
	start = false;
	fullScreen = FullScreen::OFF;  //È«ÆÁ
	playMusic = PlayMusic::ON;  //±³¾°ÒôÀÖ
	shadows = BufferedShadows::OFF;  //¶¯Ì¬ÒõÓ°
	transparent = true; //±³¾°Í¸Ã÷
	vSync = VerticalSync::OFF;   //´¹Ö±Í¬²½
	antiAlias = AntiAlias::OFF;  //¿¹¾â³Ý
}

bool GameLauncher::Run(E_DRIVER_TYPE& outDriver, 
	FullScreen::Status& outFullScreen,
	BufferedShadows::Status& outBufferedShadows,
	VerticalSync::Status&outVerticalSync, AntiAlias::Status& outAntiAlias,
	PlayMusic::Status& outPlayMusic)
{
	E_DRIVER_TYPE driverType = EDT_DIRECT3D9;

	launcherDevice = createDevice(driverType,dimension2d<u32>(512, 384), 16, false, false, false, this);

	IFileSystem* m_fileSystem = launcherDevice->getFileSystem();
    m_fileSystem->addFileArchive("Assets/irrlicht.dat");
	IVideoDriver* driver = launcherDevice->getVideoDriver();
	ISceneManager* smgr = launcherDevice->getSceneManager();
	IGUIEnvironment* m_guiEnv = launcherDevice->getGUIEnvironment();

	 stringw caption = "SimpleGame (Irrlicht Engine ";
	caption += launcherDevice->getVersion();
	caption += ")";
	launcherDevice->setWindowCaption(caption.c_str());

	// set new Skin
	IGUISkin* skin = m_guiEnv->createSkin(EGST_BURNING_SKIN);
	m_guiEnv->setSkin(skin);
	skin->drop();

	// load font
	IGUIFont* font = m_guiEnv->getFont("Assets/fonthaettenschweiler.bmp");
	if (font)
	{
		m_guiEnv->getSkin()->setFont(font);
	}

	const s32 leftX = 260;
	IGUITabControl* tabctrl = m_guiEnv->addTabControl(rect<int>(leftX, 10, 512 - 10, 384 - 10),0, true, true);
	IGUITab* optTab = tabctrl->addTab(L"Settings");
	IGUITab* aboutTab = tabctrl->addTab(L"About");

	IGUIListBox* box = m_guiEnv->addListBox(rect<int>(10, 10, 220, 120), optTab, 1);
	box->addItem(L"OpenGL 1.5");
	box->addItem(L"Direct3D 8.1");
	box->addItem(L"Direct3D 9.0c");
	box->addItem(L"Irrlicht Software Renderer 1.0");
	box->setSelected(selected);

	startButton = m_guiEnv->addButton(rect<int>(30, 295, 200, 324), optTab, 2, L"Start Game");

	const s32 d = 50;

	m_guiEnv->addCheckBox((fullScreen==FullScreen::ON), rect<int>(20, 60 + d, 130, 85 + d),optTab, 3, L"Fullscreen");
	m_guiEnv->addCheckBox((playMusic==PlayMusic::ON), rect<int>(20, 85 + d, 245, 110 + d),optTab, 4, L"Music & Sfx");
	m_guiEnv->addCheckBox((shadows==BufferedShadows::ON), rect<int>(20, 110 + d, 135, 135 + d),optTab, 5, L"Realtime shadows");
	m_guiEnv->addCheckBox((vSync==VerticalSync::ON), rect<int>(20, 160 + d, 230, 185 + d),optTab, 7, L"Vertical synchronisation");
	m_guiEnv->addCheckBox((antiAlias==AntiAlias::ON), rect<int>(20, 185 + d, 230, 210 + d),optTab, 8, L"Antialiasing");

	// add about text

	const wchar_t* text = L"This is the tech demo of the Irrlicht engine. To start, "\
		L"select a video driver which works best with your hardware and press 'Start Demo'.\n"\
		L"What you currently see is displayed using the Burning Software Renderer (Thomas Alten).\n"\
		L"The Irrlicht Engine was written by me, Nikolaus Gebhardt. The models, "\
		L"maps and textures were placed at my disposal by B.Collins, M.Cook and J.Marton. The music was created by "\
		L"M.Rohde and is played back by irrKlang.\n"\
		L"For more informations, please visit the homepage of the Irrlicht engine:\nhttp://irrlicht.sourceforge.net";

	m_guiEnv->addStaticText(text, rect<int>(10, 10, 230, 320),true, true, aboutTab);

	// add md2 model

	IAnimatedMesh* mesh = smgr->getMesh("Assets/faerie.md2");
	IAnimatedMeshSceneNode* modelNode = smgr->addAnimatedMeshSceneNode(mesh);
	if (modelNode)
	{
		modelNode->setPosition(vector3df(0.f, 0.f, -5.f));
		modelNode->setMaterialTexture(0, driver->getTexture("Assets/faerie2.bmp"));
		modelNode->setMaterialFlag(EMF_LIGHTING, true);
		modelNode->getMaterial(0).Shininess = 50.f;
		modelNode->getMaterial(0).NormalizeNormals = true;
		modelNode->setMD2Animation(EMAT_STAND);
	}

	// set ambient light (no sun light in the catacombs)
	smgr->setAmbientLight(SColorf(0.2f, 0.2f, 0.2f));

	ILightSceneNode *light;
	ISceneNodeAnimator* anim;
	ISceneNode* bill;

	enum eLightParticle
	{
		LIGHT_NONE,
		LIGHT_GLOBAL,
		LIGHT_RED,
		LIGHT_BLUE
	};

	vector3df lightDir[2] = {vector3df(0.f, 0.1f, 0.4f),vector3df(0.f, 0.1f, -0.4f)};

	struct SLightParticle
	{
		eLightParticle type;
		u32 dir;
	};
	const SLightParticle lightParticle[] =
	{
		//LIGHT_GLOBAL,0,
		{ LIGHT_RED, 0 },
		{ LIGHT_BLUE, 0 },
		{ LIGHT_RED, 1 },
		{ LIGHT_BLUE, 1 },
		{ LIGHT_NONE, 0 }
	};

	const SLightParticle *l = lightParticle;
	while (l->type != LIGHT_NONE)
	{
		switch (l->type)
		{
		case LIGHT_GLOBAL:
			// add illumination from the background
			light = smgr->addLightSceneNode(0, vector3df(10.f, 40.f, -5.f),SColorf(0.2f, 0.2f, 0.2f), 90.f);
			break;
		case LIGHT_RED:
			// add light nearly red
			light = smgr->addLightSceneNode(0, vector3df(0, 1, 0),SColorf(0.8f, 0.f, 0.f, 0.0f), 30.0f);
			// attach red billboard to the light
			bill = smgr->addBillboardSceneNode(light, dimension2d<f32>(10, 10));
			if (bill)
			{
				bill->setMaterialFlag(EMF_LIGHTING, false);
				bill->setMaterialType(EMT_TRANSPARENT_ADD_COLOR);
				bill->setMaterialTexture(0, driver->getTexture("Assets/particlered.bmp"));
			}
			// add fly circle animator to the light
			anim = smgr->createFlyCircleAnimator(vector3df(0.f, 0.f, -5.f), 20.f,0.002f, lightDir[l->dir]);
			light->addAnimator(anim);
			anim->drop();
			break;
		case LIGHT_BLUE:
			// add light nearly blue
			light = smgr->addLightSceneNode(0, vector3df(0, 1, 0),SColorf(0.f, 0.0f, 0.8f, 0.0f), 30.0f);
			// attach blue billboard to the light
			bill = smgr->addBillboardSceneNode(light, dimension2d<f32>(10, 10));
			if (bill)
			{
				bill->setMaterialFlag(EMF_LIGHTING, false);
				bill->setMaterialType(EMT_TRANSPARENT_ADD_COLOR);
				bill->setMaterialTexture(0, driver->getTexture("Assets/portal1.bmp"));
			}
			// add fly circle animator to the light
			anim = smgr->createFlyCircleAnimator(vector3df(0.f, 0.f, -5.f), 20.f,-0.002f, lightDir[l->dir], 0.5f);
			light->addAnimator(anim);
			anim->drop();
			break;
		case LIGHT_NONE:
			break;
		}
		l += 1;
	}

	// create a fixed camera
	smgr->addCameraSceneNode(0, vector3df(45, 0, 0), vector3df(0, 0, 10));


	// irrlicht logo and background
	// add irrlicht logo
	bool oldMipMapState = driver->getTextureCreationFlag(ETCF_CREATE_MIP_MAPS);
	driver->setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, false);

	m_guiEnv->addImage(driver->getTexture("Assets/logo.jpg"),
		position2d<s32>(5, 5));

	ITexture* irrlichtBack = driver->getTexture("Assets/demoback.jpg");

	driver->setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, oldMipMapState);

	// query original skin color
	GetOriginalSkinColor();

	// set transparency
	SetTransparency();

	// draw all

	while (launcherDevice->run())
	{
		if (launcherDevice->isWindowActive())
		{
			driver->beginScene(false, true, SColor(0, 0, 0, 0));

			if (irrlichtBack)
			{
				driver->draw2DImage(irrlichtBack, position2d<int>(0, 0));
			}

			smgr->drawAll();
			m_guiEnv->drawAll();
			driver->endScene();
		}
	}

	launcherDevice->drop();

	outFullScreen = fullScreen;
	outPlayMusic = playMusic;
	outBufferedShadows = shadows;
	outVerticalSync = vSync;
	outAntiAlias = antiAlias;

	switch (selected)
	{
	case 0:	outDriver = EDT_OPENGL; break;
	case 1:	outDriver = EDT_DIRECT3D8; break;
	case 2:	outDriver = EDT_DIRECT3D9; break;
	case 3:	outDriver = EDT_SOFTWARE; break;
	}

	return start;
}

bool GameLauncher::OnEvent(const SEvent& evt)
{
	if (evt.EventType == EET_KEY_INPUT_EVENT &&	evt.KeyInput.Key == KEY_F9 &&evt.KeyInput.PressedDown == false)
	{
		IImage* image = launcherDevice->getVideoDriver()->createScreenShot();
		if (image)
		{
			launcherDevice->getVideoDriver()->writeImageToFile(image, "screenshot_main.jpg");
			image->drop();
		}
	}
	else
	if (evt.EventType == EET_MOUSE_INPUT_EVENT &&evt.MouseInput.Event == EMIE_RMOUSE_LEFT_UP)
	{
		rect<s32> r(evt.MouseInput.X, evt.MouseInput.Y, 0, 0);
		IGUIContextMenu* menu = launcherDevice->getGUIEnvironment()->addContextMenu(r, 0, 45);
		menu->addItem(L"transparent menus", 666, transparent == false);
		menu->addItem(L"solid menus", 666, transparent == true);
		menu->addSeparator();
		menu->addItem(L"Cancel");
	}
	else
	if (evt.EventType == EET_GUI_EVENT)
	{
		s32 id = evt.GUIEvent.Caller->getID();
		switch (id)
		{
		case 45: // context menu
			if (evt.GUIEvent.EventType == EGET_MENU_ITEM_SELECTED)
			{
				s32 s = ((IGUIContextMenu*)evt.GUIEvent.Caller)->getSelectedItem();
				if (s == 0 || s == 1)
				{
					transparent = !transparent;
					SetTransparency();
				}
			}
			break;
		case 1:
			if (evt.GUIEvent.EventType == EGET_LISTBOX_CHANGED ||
				evt.GUIEvent.EventType == EGET_LISTBOX_SELECTED_AGAIN)
			{
				selected = ((IGUIListBox*)evt.GUIEvent.Caller)->getSelected();
				//startButton->setEnabled(selected != 4);
				startButton->setEnabled(true);
			}
			break;
		case 2:
			if (evt.GUIEvent.EventType == EGET_BUTTON_CLICKED)
			{
				launcherDevice->closeDevice();
				start = true;
			}
		case 3:
			if (evt.GUIEvent.EventType == EGET_CHECKBOX_CHANGED)
				fullScreen = (((IGUICheckBox*)evt.GUIEvent.Caller)->isChecked()) ? (FullScreen::ON) : (FullScreen::OFF);
			break;
		case 4:
			if (evt.GUIEvent.EventType == EGET_CHECKBOX_CHANGED)
				playMusic = (((IGUICheckBox*)evt.GUIEvent.Caller)->isChecked()) ? (PlayMusic::ON) : (PlayMusic::OFF);
			break;
		case 5:
			if (evt.GUIEvent.EventType == EGET_CHECKBOX_CHANGED)
				shadows = (((IGUICheckBox*)evt.GUIEvent.Caller)->isChecked()) ? (BufferedShadows::ON) : (BufferedShadows::OFF);
			break;
		case 6:
			if (evt.GUIEvent.EventType == EGET_CHECKBOX_CHANGED)
				vSync = (((IGUICheckBox*)evt.GUIEvent.Caller)->isChecked()) ? (VerticalSync::ON) : (VerticalSync::OFF);
			break;
		case 7:
			if (evt.GUIEvent.EventType == EGET_CHECKBOX_CHANGED)
				antiAlias = (((IGUICheckBox*)evt.GUIEvent.Caller)->isChecked())?  (AntiAlias::ON) : (AntiAlias::OFF);
			break;
		}
	}

	return false;
}


void GameLauncher::GetOriginalSkinColor()
{
	IGUISkin * skin = launcherDevice->getGUIEnvironment()->getSkin();
	for (s32 i = 0; i<EGDC_COUNT; ++i)
	{
		SkinColor[i] = skin->getColor((EGUI_DEFAULT_COLOR)i);
	}

}


void GameLauncher::SetTransparency()
{
	IGUISkin * skin = launcherDevice->getGUIEnvironment()->getSkin();

	for (u32 i = 0; i<EGDC_COUNT; ++i)
	{
		SColor col = SkinColor[i];

		if (false == transparent)
			col.setAlpha(255);

		skin->setColor((EGUI_DEFAULT_COLOR)i, col);
	}
}
