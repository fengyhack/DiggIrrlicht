#include "SimpleGame.h"

Size::Size(int w, int h)
{
	width = w;
	height = h;
}

int Size::Width() const
{
	return width;
}

int Size::Height() const
{
	return height;
}


///
// Ä¬ÈÏÇý¶¯DirectX9
///

Game::Game(E_DRIVER_TYPE driverType, Size windowSize,
	FullScreen::Status fullScreen,	BufferedShadows::Status bufferedShadow, 
	VerticalSync::Status verticalSync, AntiAlias::Status antiAlias, 
	PlayMusic::Status playMusic)
{
	m_irrParams.DriverType = driverType;
	u32 w = windowSize.Width();
	u32 h = windowSize.Height();
	if (driverType == EDT_SOFTWARE)
	{
		w = 640;
		h = 480;
		fullScreen = FullScreen::OFF;
	}
	m_irrParams.WindowSize = dimension2d<u32>(w, h);
	m_irrParams.Fullscreen = (fullScreen == FullScreen::ON);
	m_irrParams.Bits = 32;
	m_irrParams.Stencilbuffer = (bufferedShadow == BufferedShadows::ON);
	m_irrParams.Vsync = (verticalSync == VerticalSync::ON);
	m_irrParams.AntiAlias = (antiAlias == AntiAlias::ON);
	m_irrParams.EventReceiver = this;
	m_irrDevice = createDeviceEx(m_irrParams);

	if (m_irrDevice == NULL)
	{
		MessageBox(NULL, TEXT("Failed to create irrlicht device!"), TEXT("Failure"), MB_OK);
		return;
	}

	m_videoDriver = m_irrDevice->getVideoDriver();
	m_sceneManager = m_irrDevice->getSceneManager();
	m_guiEnv = m_irrDevice->getGUIEnvironment();
	m_fileSystem = m_irrDevice->getFileSystem();
	timer = m_irrDevice->getTimer();

	m_playMusic = playMusic;
	m_soundPlayed = NULL;
	m_ballSound = NULL;
	m_impactSound = NULL;

	m_currentScene = -2;
	m_bgColor = SColor(255, 192, 192, 255);
	m_statusText = NULL;
	m_InOutFader = NULL;
	m_quakeLevelMesh = NULL;
	m_quakeLevelNode = NULL;
	m_skyBoxNode = NULL;
	m_model_1 = NULL;
	m_model_2 = NULL;
	m_campFire = NULL;
	m_metaSelector = NULL;
	m_mapSelector = NULL;
	m_sceneStartTime = 0;
	m_sceneDurationTime = 0;
	m_Impacts = NULL;

	if (m_playMusic == PlayMusic::ON)
	{
		m_soundEngine = createIrrKlangDevice();
	}
	else
	{
		m_soundEngine = NULL;
	}
}

Game::~Game()
{
	//
}

void Game::SetBkMusic(const char* pszMusicFile)
{
	ISoundSource* soundSource = m_soundEngine->getSoundSource(pszMusicFile);
	m_soundEngine->setSoundVolume(1.0f);
	m_soundPlayed = m_soundEngine->play2D(soundSource, true, false, false, true);
	ISoundEffectControl* fx = m_soundPlayed->getSoundEffectControl();
	if (fx != NULL)
	{
		//fx->enableDistortionSoundEffect(-10.0f, 5.0f);
		fx->enableChorusSoundEffect(30.0f, 20.0f);
		fx->enableWavesReverbSoundEffect(0.0f, 0.0f, 200.0f);
	}

	soundSource->drop();
}

void Game::Run()
{
	m_fileSystem->addFileArchive("Assets/irrlicht.dat");
	m_fileSystem->addFileArchive("Assets/map-20kdm2.pk3");
	m_irrDevice->setWindowCaption(L"SimpleGame (Irrlicht Engine)");
	m_sceneManager->setAmbientLight(SColorf(0x00c0c0c0));
	
	m_sceneStartTime = timer->getTime();
	while (m_irrDevice->run())
	{
		if (m_irrDevice->isWindowActive())
		{
			// update 3D position for sound engine
			ICameraSceneNode* cam = m_sceneManager->getActiveCamera();
			if (cam)
			{
				m_soundEngine->setListenerPosition(cam->getAbsolutePosition(), cam->getTarget());
			}	

			s32 now = timer->getTime();
			if (now - m_sceneStartTime > m_sceneDurationTime && m_sceneDurationTime != -1)
			{
				SwitchToNextScene();
			}

			CreateParticleImpacts();

			m_videoDriver->beginScene(m_sceneDurationTime != -1, true, m_bgColor);

			m_sceneManager->drawAll();
			m_guiEnv->drawAll();
			m_videoDriver->endScene();

			wchar_t tmp[255] = { 0 };
			swprintf(tmp, 255, L"Driver:%ls  FPS:%3d  Triangles:%0.3f MIO/S",
				m_videoDriver->getName(), m_videoDriver->getFPS(),
				1.0f*(m_videoDriver->getPrimitiveCountDrawn(1)) / 1000000.f);
			m_statusText->setText(tmp);
		}
	}

	m_soundEngine->drop();
	m_irrDevice->drop();
}


bool Game::OnEvent(const SEvent& evt)
{
	if (m_irrDevice == NULL)
	{
		return false;
	}
	bool bKeyPressed = (evt.EventType == EET_KEY_INPUT_EVENT) && 
	                              (evt.KeyInput.PressedDown == false);

	if (bKeyPressed && evt.KeyInput.Key == KEY_ESCAPE)
	{
		// user wants to quit.
		if (m_currentScene < 3)
		{
			m_sceneDurationTime = 0;
		}
		else
		{
			m_irrDevice->closeDevice();
		}
	}
	else
	{
		if (bKeyPressed &&	evt.KeyInput.Key == KEY_F9)
		{
			IImage* image = m_videoDriver->createScreenShot();
			u32 tm=timer->getRealTime();
			char fn[256] = { 0 };
			sprintf_s(fn, "Screenshot_%d.jpg", tm);
			if (image)
			{
				m_videoDriver->writeImageToFile(image, fn);
				image->drop();
			}
		}
		else
		{
			return true;
			ICameraSceneNode* camNode = m_sceneManager->getActiveCamera();
			if (camNode)
			{
				camNode->OnEvent(evt);
				return true;
			}
		}
	}

	return false;
}


void Game::SwitchToNextScene()
{
	++m_currentScene;
	if (m_currentScene > 3)
		m_currentScene = 1;

	ISceneNodeAnimator* sa = 0;
	ICameraSceneNode* camera = 0;

	camera = m_sceneManager->getActiveCamera();
	if (camera)
	{
		m_sceneManager->setActiveCamera(0);
		camera->remove();
		camera = 0;
	}

	switch (m_currentScene)
	{
	case -1: // loading screen
		m_sceneDurationTime = 0;
		CreateLoadingScreen();
		break;

	case 0: // load scene
		m_sceneDurationTime = 0;
		LoadSceneData();
		break;

	case 1: // panorama camera
	{
				++m_currentScene;
				//camera = m_sceneManager->addCameraSceneNode(0, vector3df(0,0,0), vector3df(-586,708,52));
				//camera->setTarget(vector3df(0,400,0));

				array<core::vector3df> points;

				points.push_back(vector3df(-931.473755f, 138.300003f, 987.279114f)); // -49873
				points.push_back(vector3df(-847.902222f, 136.757553f, 915.792725f)); // -50559
				points.push_back(vector3df(-748.680420f, 152.254501f, 826.418945f)); // -51964
				points.push_back(vector3df(-708.428406f, 213.569580f, 784.466675f)); // -53251
				points.push_back(vector3df(-686.217651f, 288.141174f, 762.965576f)); // -54015
				points.push_back(vector3df(-679.685059f, 365.095612f, 756.551453f)); // -54733
				points.push_back(vector3df(-671.317871f, 447.360107f, 749.394592f)); // -55588
				points.push_back(vector3df(-669.468445f, 583.335632f, 747.711853f)); // -56178
				points.push_back(vector3df(-667.611267f, 727.313232f, 746.018250f)); // -56757
				points.push_back(vector3df(-665.853210f, 862.791931f, 744.436096f)); // -57859
				points.push_back(vector3df(-642.649597f, 1026.047607f, 724.259827f)); // -59705
				points.push_back(vector3df(-517.793884f, 838.396790f, 490.326050f)); // -60983
				points.push_back(vector3df(-474.387299f, 715.691467f, 344.639984f)); // -61629
				points.push_back(vector3df(-444.600250f, 601.155701f, 180.938095f)); // -62319
				points.push_back(vector3df(-414.808899f, 479.691406f, 4.866660f)); // -63048
				points.push_back(vector3df(-410.418945f, 429.642242f, -134.332687f)); // -63757
				points.push_back(vector3df(-399.837585f, 411.498383f, -349.350983f)); // -64418
				points.push_back(vector3df(-390.756653f, 403.970093f, -524.454407f)); // -65005
				points.push_back(vector3df(-334.864227f, 350.065491f, -732.397400f)); // -65701
				points.push_back(vector3df(-195.253387f, 349.577209f, -812.475891f)); // -66335
				points.push_back(vector3df(16.255573f, 363.743134f, -833.800415f)); // -67170
				points.push_back(vector3df(234.940964f, 352.957825f, -820.150696f)); // -67939
				points.push_back(vector3df(436.797668f, 349.236450f, -816.914185f)); // -68596
				points.push_back(vector3df(575.236206f, 356.244812f, -719.788513f)); // -69166
				points.push_back(vector3df(594.131042f, 387.173828f, -609.675598f)); // -69744
				points.push_back(vector3df(617.615234f, 412.002899f, -326.174072f)); // -70640
				points.push_back(vector3df(606.456848f, 403.221954f, -104.179291f)); // -71390
				points.push_back(vector3df(610.958252f, 407.037750f, 117.209778f)); // -72085
				points.push_back(vector3df(597.956909f, 395.167877f, 345.942200f)); // -72817
				points.push_back(vector3df(587.383118f, 391.444519f, 566.098633f)); // -73477
				points.push_back(vector3df(559.572449f, 371.991333f, 777.689453f)); // -74124
				points.push_back(vector3df(423.753204f, 329.990051f, 925.859741f)); // -74941
				points.push_back(vector3df(247.520050f, 252.818954f, 935.311829f)); // -75651
				points.push_back(vector3df(114.756012f, 199.799759f, 805.014160f));
				points.push_back(vector3df(96.783348f, 181.639481f, 648.188110f));
				points.push_back(vector3df(97.865623f, 138.905975f, 484.812561f));
				points.push_back(vector3df(99.612457f, 102.463669f, 347.603210f));
				points.push_back(vector3df(99.612457f, 102.463669f, 347.603210f));
				points.push_back(vector3df(99.612457f, 102.463669f, 347.603210f));

				m_sceneDurationTime = (points.size() - 3) * 1000;

				camera = m_sceneManager->addCameraSceneNode(0, points[0], vector3df(0, 400, 0));
				//camera->setTarget(core::vector3df(0,400,0));

				sa = m_sceneManager->createFollowSplineAnimator(timer->getTime(), points);
				camera->addAnimator(sa);
				sa->drop();

				m_model_1->setVisible(false);
				m_model_2->setVisible(false);
				m_campFire->setVisible(false);
				m_InOutFader->fadeIn(7000);
	}
		break;

	case 2:	// down fly anim camera
		camera = m_sceneManager->addCameraSceneNode(0, vector3df(100, 40, -80), vector3df(844, 670, -885));
		sa = m_sceneManager->createFlyStraightAnimator(vector3df(94, 1002, 127), vector3df(108, 15, -60), 10000, true);
		camera->addAnimator(sa);
		m_sceneDurationTime = 9900;
		m_model_1->setVisible(true);
		m_model_2->setVisible(false);
		m_campFire->setVisible(false);
		sa->drop();
		break;

	case 3: // interactive, go around
	{
				m_model_1->setVisible(true);
				m_model_2->setVisible(true);
				m_campFire->setVisible(true);
				m_sceneDurationTime = -1;

				SKeyMap keyMap[9];
				keyMap[0].Action = EKA_MOVE_FORWARD;
				keyMap[0].KeyCode = KEY_UP;
				keyMap[1].Action = EKA_MOVE_FORWARD;
				keyMap[1].KeyCode = KEY_KEY_W;

				keyMap[2].Action = EKA_MOVE_BACKWARD;
				keyMap[2].KeyCode = KEY_DOWN;
				keyMap[3].Action = EKA_MOVE_BACKWARD;
				keyMap[3].KeyCode = KEY_KEY_S;

				keyMap[4].Action = EKA_STRAFE_LEFT;
				keyMap[4].KeyCode = KEY_LEFT;
				keyMap[5].Action = EKA_STRAFE_LEFT;
				keyMap[5].KeyCode = KEY_KEY_A;

				keyMap[6].Action = EKA_STRAFE_RIGHT;
				keyMap[6].KeyCode = KEY_RIGHT;
				keyMap[7].Action = EKA_STRAFE_RIGHT;
				keyMap[7].KeyCode = KEY_KEY_D;

				keyMap[8].Action = EKA_JUMP_UP;
				keyMap[8].KeyCode = KEY_KEY_J;

				camera = m_sceneManager->addCameraSceneNodeFPS(0, 100.0f, .4f, -1, keyMap, 9, false, 3.f);
				camera->setPosition(vector3df(108, 140, -140));
				camera->setFarValue(5000.0f);

				ISceneNodeAnimatorCollisionResponse* collider =
					m_sceneManager->createCollisionResponseAnimator(
					m_metaSelector, camera, vector3df(25, 50, 25),
					vector3df(0, m_quakeLevelMesh ? -10.f : 0.0f, 0),
					vector3df(0, 45, 0), 0.005f);

				camera->addAnimator(collider);
				collider->drop();
	}
		break;
	}

	m_sceneStartTime = timer->getTime();
}


void Game::LoadSceneData()
{
	// load quake level

	// Quake3 Shader controls Z-Writing
	m_sceneManager->getParameters()->setAttribute(scene::ALLOW_ZWRITE_ON_TRANSPARENT, true);

	m_quakeLevelMesh = (IQ3LevelMesh*)(m_sceneManager->getMesh("maps/20kdm2.bsp"));

	if (m_quakeLevelMesh)
	{
		u32 i;

		//move all quake level meshes (non-realtime)
		matrix4 m;
		m.setTranslation(vector3df(-1300, -70, -1249));

		for (i = 0; i != quake3::E_Q3_MESH_SIZE; ++i)
			m_sceneManager->getMeshManipulator()->transform(m_quakeLevelMesh->getMesh(i), m);

		m_quakeLevelNode = m_sceneManager->addOctreeSceneNode(
			m_quakeLevelMesh->getMesh(quake3::E_Q3_MESH_GEOMETRY));
		if (m_quakeLevelNode)
		{
			//m_quakeLevelNode->setPosition(core::vector3df(-1300,-70,-1249));
			m_quakeLevelNode->setVisible(true);

			// create map triangle selector
			m_mapSelector = m_sceneManager->createOctreeTriangleSelector(m_quakeLevelMesh->getMesh(0),
				m_quakeLevelNode, 128);

			// if not using shader and no gamma it's better to use more lighting, because
			// quake3 level are usually dark
			m_quakeLevelNode->setMaterialType(EMT_LIGHTMAP_M4);

			// set additive blending if wanted
			m_quakeLevelNode->setMaterialType(EMT_LIGHTMAP_ADD);
		}

		// the additional mesh can be quite huge and is unoptimized
		IMesh * additional_mesh = m_quakeLevelMesh->getMesh(quake3::E_Q3_MESH_ITEMS);

		for (i = 0; i != additional_mesh->getMeshBufferCount(); ++i)
		{
			IMeshBuffer *meshBuffer = additional_mesh->getMeshBuffer(i);
			const SMaterial &material = meshBuffer->getMaterial();

			//! The ShaderIndex is stored in the material parameter
			s32 shaderIndex = (s32)material.MaterialTypeParam2;

			// the meshbuffer can be rendered without additional support, or it has no shader
			const quake3::IShader *shader = m_quakeLevelMesh->getShader(shaderIndex);
			if (0 == shader)
			{
				continue;
			}
			// Now add the MeshBuffer(s) with the current Shader to the Manager
			m_sceneManager->addQuake3SceneNode(meshBuffer, shader);
		}
	}

	// load sydney model and create 2 instances

	IAnimatedMesh* mesh = 0;
	mesh = m_sceneManager->getMesh("Assets/sydney.md2");
	if (mesh)
	{
		m_model_1 = m_sceneManager->addAnimatedMeshSceneNode(mesh);
		if (m_model_1)
		{
			m_model_1->setMaterialTexture(0, m_videoDriver->getTexture("Assets/spheremap.jpg"));
			m_model_1->setPosition(vector3df(100, 40, -80));
			m_model_1->setScale(vector3df(2, 2, 2));
			m_model_1->setMD2Animation(EMAT_STAND);
			m_model_1->setMaterialFlag(EMF_LIGHTING, false);
			m_model_1->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);
			m_model_1->setMaterialType(EMT_SPHERE_MAP);
			m_model_1->addShadowVolumeSceneNode();
		}

		m_model_2 = m_sceneManager->addAnimatedMeshSceneNode(mesh);
		if (m_model_2)
		{
			m_model_2->setPosition(vector3df(180, 15, -60));
			m_model_2->setScale(vector3df(2, 2, 2));
			m_model_2->setMD2Animation(EMAT_RUN);
			m_model_2->setMaterialTexture(0, m_videoDriver->getTexture("Assets/sydney.bmp"));
			m_model_2->setMaterialFlag(EMF_LIGHTING, true);
			m_model_2->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);
			m_model_2->addShadowVolumeSceneNode();
		}
	}

	ISceneNodeAnimator* anim = 0;

	// create sky box
	m_videoDriver->setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, false);
	m_skyBoxNode = m_sceneManager->addSkyBoxSceneNode(
		m_videoDriver->getTexture("Assets/irrlicht2_up.jpg"),
		m_videoDriver->getTexture("Assets/irrlicht2_dn.jpg"),
		m_videoDriver->getTexture("Assets/irrlicht2_lf.jpg"),
		m_videoDriver->getTexture("Assets/irrlicht2_rt.jpg"),
		m_videoDriver->getTexture("Assets/irrlicht2_ft.jpg"),
		m_videoDriver->getTexture("Assets/irrlicht2_bk.jpg"));
	m_videoDriver->setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, true);

	// create walk-between-portals animation

	vector3df waypoint[2];
	waypoint[0].set(-150, 40, 100);
	waypoint[1].set(350, 40, 100);

	if (m_model_2)
	{
		anim = m_sceneManager->createFlyStraightAnimator(waypoint[0], waypoint[1], 2000, true);
		m_model_2->addAnimator(anim);
		anim->drop();
	}

	// create animation for portals;

	array<video::ITexture*> textures;
	for (s32 g = 1; g < 8; ++g)
	{
		stringc tmp("Assets/portal");
		tmp += g;
		tmp += ".bmp";
		ITexture* t = m_videoDriver->getTexture(tmp);
		textures.push_back(t);
	}

	anim = m_sceneManager->createTextureAnimator(textures, 100);

	// create portals

	IBillboardSceneNode* bill = 0;

	for (int r = 0; r < 2; ++r)
	{
		bill = m_sceneManager->addBillboardSceneNode(0, dimension2d<f32>(100, 100),
			waypoint[r] + vector3df(0, 20, 0));
		bill->setMaterialFlag(EMF_LIGHTING, false);
		bill->setMaterialTexture(0, m_videoDriver->getTexture("Assets/portal1.bmp"));
		bill->setMaterialType(EMT_TRANSPARENT_ADD_COLOR);
		bill->addAnimator(anim);
	}

	anim->drop();

	// create cirlce flying dynamic light with transparent billboard attached

	ILightSceneNode* light = 0;

	light = m_sceneManager->addLightSceneNode(0,
		vector3df(0, 0, 0), SColorf(1.0f, 1.0f, 1.f, 1.0f), 500.f);

	anim = m_sceneManager->createFlyCircleAnimator(
		vector3df(100, 150, 80), 80.0f, 0.0005f);

	light->addAnimator(anim);
	anim->drop();

	bill = m_irrDevice->getSceneManager()->addBillboardSceneNode(
		light, dimension2d<f32>(40, 40));
	bill->setMaterialFlag(EMF_LIGHTING, false);
	bill->setMaterialTexture(0, m_videoDriver->getTexture("Assets/particlewhite.bmp"));
	bill->setMaterialType(EMT_TRANSPARENT_ADD_COLOR);

	// create meta triangle selector with all triangles selectors in it.
	m_metaSelector = m_sceneManager->createMetaTriangleSelector();
	m_metaSelector->addTriangleSelector(m_mapSelector);

	// create camp fire

	m_campFire = m_sceneManager->addParticleSystemSceneNode(false);
	m_campFire->setPosition(vector3df(100, 120, 600));
	m_campFire->setScale(vector3df(2, 2, 2));

	IParticleEmitter* em = m_campFire->createBoxEmitter(
		aabbox3d<f32>(-7, 0, -7, 7, 1, 7),
		vector3df(0.0f, 0.06f, 0.0f),
		80, 100, SColor(1, 255, 255, 255), SColor(1, 255, 255, 255), 800, 2000);

	em->setMinStartSize(dimension2d<f32>(20.0f, 10.0f));
	em->setMaxStartSize(dimension2d<f32>(20.0f, 10.0f));
	m_campFire->setEmitter(em);
	em->drop();

	IParticleAffector* paf = m_campFire->createFadeOutParticleAffector();
	m_campFire->addAffector(paf);
	paf->drop();

	m_campFire->setMaterialFlag(EMF_LIGHTING, false);
	m_campFire->setMaterialFlag(EMF_ZWRITE_ENABLE, false);
	m_campFire->setMaterialTexture(0, m_videoDriver->getTexture("Assets/fireball.bmp"));
	m_campFire->setMaterialType(EMT_TRANSPARENT_ADD_COLOR);
}


void Game::CreateLoadingScreen()
{
	dimension2d<u32> size = m_videoDriver->getScreenSize();

	m_irrDevice->getCursorControl()->setVisible(false);

	// setup loading screen

	m_bgColor.set(255, 90, 90, 156);

	// create in fader

	m_InOutFader = m_guiEnv->addInOutFader();
	m_InOutFader->setColor(m_bgColor, SColor(0, 230, 230, 230));

	// irrlicht logo
	m_guiEnv->addImage(m_videoDriver->getTexture("Assets/logo.jpg"), position2d<s32>(5, 5));

	// loading text

	const int lwidth = size.Width - 20;
	const int lheight = 16;

	rect<int> pos(10, size.Height - lheight - 10, 10 + lwidth, size.Height - 10);

	m_guiEnv->addImage(pos);
	m_statusText = m_guiEnv->addStaticText(L"Loading...", pos, true);
	m_statusText->setOverrideColor(SColor(255, 205, 200, 200));

	// load bigger font

	m_guiEnv->getSkin()->setFont(m_guiEnv->getFont("Assets/fonthaettenschweiler.bmp"));

	// set new font color

	m_guiEnv->getSkin()->setColor(EGDC_BUTTON_TEXT, SColor(255, 100, 100, 100));
}


void Game::Shoot()
{
	ICameraSceneNode* camera = m_sceneManager->getActiveCamera();

	if (!camera || !m_mapSelector)
		return;

	SParticleImpact impact;
	impact.when = 0;

	// get line of camera

	vector3df start = camera->getPosition();
	vector3df end = (camera->getTarget() - start);
	end.normalize();
	start += end*8.0f;
	end = start + (end * camera->getFarValue());

	triangle3df triangle;

	line3d<f32> line(start, end);

	// get intersection point with map
	ISceneNode* hitNode;
	if (m_sceneManager->getSceneCollisionManager()->getCollisionPoint(line, 
	            m_mapSelector, end, triangle, hitNode))
	{
		// collides with wall
		vector3df out = triangle.getNormal();
		out.setLength(0.03f);

		impact.when = 1;
		impact.outVector = out;
		impact.pos = end;
	}
	else
	{
		// doesnt collide with wall
		vector3df start = camera->getPosition();
		vector3df end = (camera->getTarget() - start);
		end.normalize();
		start += end*8.0f;
		end = start + (end * camera->getFarValue());
	}

	// create fire ball
	ISceneNode* node = 0;
	node = m_sceneManager->addBillboardSceneNode(0, dimension2d<f32>(25, 25), start);

	node->setMaterialFlag(EMF_LIGHTING, false);
	node->setMaterialTexture(0, m_videoDriver->getTexture("Assets/fireball.bmp"));
	node->setMaterialType(EMT_TRANSPARENT_ADD_COLOR);

	f32 length = (f32)(end - start).getLength();
	const f32 speed = 0.6f;
	u32 time = (u32)(length / speed);

	ISceneNodeAnimator* anim = 0;

	// set flight line

	anim = m_sceneManager->createFlyStraightAnimator(start, end, time);
	node->addAnimator(anim);
	anim->drop();

	anim = m_sceneManager->createDeleteAnimator(time);
	node->addAnimator(anim);
	anim->drop();

	if (impact.when)
	{
		// create impact note
		impact.when = m_irrDevice->getTimer()->getTime() + (time - 100);
		m_Impacts.push_back(impact);
	}

	// play sound
	if (m_ballSound)
		m_soundEngine->play2D(m_ballSound);
}


void  Game::CreateParticleImpacts()
{
	u32 now = timer->getTime();
	int  iSize = m_Impacts.size();
	for (int i = 0; i<iSize; ++i)
	{
		if (now > m_Impacts[i].when)
		{
			// create smoke particle system
			IParticleSystemSceneNode* ps = 0;
			ps = m_sceneManager->addParticleSystemSceneNode(false, 0, -1, m_Impacts[i].pos);
			ps->setParticleSize(dimension2d<f32>(10.0f, 10.0f));

			aabbox3d<f32> box = aabbox3d<f32>(-5, -5, -5, 5, 5, 5);
			SColor cmin = SColor(50, 255, 255, 255);
			SColor cmax = SColor(50, 255, 255, 255);
			IParticleEmitter* pe = ps->createBoxEmitter(box, m_Impacts[i].outVector, 
			            20, 40, cmin, cmax, 1200, 1600, 20);

			ps->setEmitter(pe);
			pe->drop();

			IParticleAffector* pf = m_campFire->createFadeOutParticleAffector();
			ps->addAffector(pf);
			pf->drop();

			ps->setMaterialFlag(EMF_LIGHTING, false);
			ps->setMaterialFlag(EMF_ZWRITE_ENABLE, false);
			ps->setMaterialTexture(0, m_videoDriver->getTexture("Assets/smoke.bmp"));
			ps->setMaterialType(EMT_TRANSPARENT_ADD_COLOR);

			ISceneNodeAnimator* anim = m_sceneManager->createDeleteAnimator(2000);
			ps->addAnimator(anim);
			anim->drop();

			// play impact sound
			if (m_soundEngine)
			{
				ISound* sound = m_soundEngine->play3D(m_impactSound, m_Impacts[i].pos, false, false, true);

				if (sound)
				{
					// adjust max value a bit to make to sound of an impact louder
					sound->setMinDistance(400);
					sound->drop();
				}
			}

			// delete entry
			m_Impacts.erase(i);
			--i;
		}
	}
}