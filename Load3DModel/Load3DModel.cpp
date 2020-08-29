#include <irrlicht.h>
using namespace irr;
using namespace core;
using namespace video;
using namespace scene;

#pragma comment(lib,"irrlicht.lib")

int main(void)
{
	//设置控制台窗口的标题与窗口颜色
	system("TITLE Irrlicht Demo - StaticMesh");
	system("COLOR E0");

	//用3ds max等导出的*.obj格式模型文件
	const char* meshFile = "Assets/BMW.3DS";
	try
	{
		IrrlichtDevice* device = createDevice(EDT_DIRECT3D9, dimension2d<u32>(800, 600), 32U);
		device->setWindowCaption(L"BMW Model");

		if (device == NULL)
		{
			//抛出异常(字符串型, char*)
			throw("An error occurred in function \'createDevice\'");
		}
		IVideoDriver* driver = device->getVideoDriver();
		ISceneManager* smgr = device->getSceneManager();

		//载入模型
		IMesh* mesh = smgr->getMesh(meshFile);
		IMeshSceneNode* node = smgr->addMeshSceneNode(mesh);
		node->setMaterialFlag(EMF_LIGHTING, false);
		node->setPosition(vector3df(0, 0, 0));

		//添加摄像机,设置观察点
		smgr->addCameraSceneNode(0, vector3df(120, 50, 50), vector3df(10, 30, 20));

		//渲染循环
		while (device->run())
		{
			driver->beginScene(true, true, SColor(255, 0, 0, 0));
			smgr->drawAll();
			driver->endScene();
		}
		device->drop();
	}
	catch (char* pszError)
	{
		puts(pszError);
	}

	return 0;
}