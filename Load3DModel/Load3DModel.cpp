#include <irrlicht.h>
using namespace irr;
using namespace core;
using namespace video;
using namespace scene;

#pragma comment(lib,"irrlicht.lib")

int main(void)
{
	//���ÿ���̨���ڵı����봰����ɫ
	system("TITLE Irrlicht Demo - StaticMesh");
	system("COLOR E0");

	//��3ds max�ȵ�����*.obj��ʽģ���ļ�
	const char* meshFile = "Assets/BMW.3DS";
	try
	{
		IrrlichtDevice* device = createDevice(EDT_DIRECT3D9, dimension2d<u32>(800, 600), 32U);
		device->setWindowCaption(L"BMW Model");

		if (device == NULL)
		{
			//�׳��쳣(�ַ�����, char*)
			throw("An error occurred in function \'createDevice\'");
		}
		IVideoDriver* driver = device->getVideoDriver();
		ISceneManager* smgr = device->getSceneManager();

		//����ģ��
		IMesh* mesh = smgr->getMesh(meshFile);
		IMeshSceneNode* node = smgr->addMeshSceneNode(mesh);
		node->setMaterialFlag(EMF_LIGHTING, false);
		node->setPosition(vector3df(0, 0, 0));

		//��������,���ù۲��
		smgr->addCameraSceneNode(0, vector3df(120, 50, 50), vector3df(10, 30, 20));

		//��Ⱦѭ��
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