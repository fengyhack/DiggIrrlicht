#include <irrlicht.h>
using namespace irr;
using namespace core;
using namespace video;
#pragma comment(lib, "Irrlicht.lib")

#include <iostream>
#include <exception>
using namespace std;

int main()
{
	try
	{
		IrrlichtDevice* device = createDevice(EDT_DIRECT3D9, dimension2d<u32>(800, 600), 32U);
		IVideoDriver* driver = device->getVideoDriver();
		ITexture* image = driver->getTexture("Assets/ahha.jpg");
		while (device->run())
		{
			driver->beginScene(true, true, SColor(255, 255, 255, 255));
			for (int i = 0; i < 5; ++i)
			{
				for (int j = 0; j < 5;++j)
				{
					driver->draw2DImage(image, position2d<s32>(i * 130,j * 120), 
						rect<s32>(0, 0, 128, 128), 0, SColor(255, 255, 255, 255), true);
				}
			}
			driver->draw2DImage(image, position2d<s32>(400, 20),
				rect<s32>(0, 0, 128, 128), 0, SColor(85,
				255, 0, 0), true);
			driver->draw2DImage(image, position2d<s32>(400, 170),
				rect<s32>(0, 0, 128, 128), 0, SColor(170,
				0, 255, 0), true);
			driver->draw2DImage(image, position2d<s32>(400, 320),
				rect<s32>(0, 0, 128, 128), 0, SColor(255,
				0, 0, 255), true);
			driver->draw2DImage(image, rect<s32>(50, 50, 300, 450),
				rect<s32>(0, 0, 128, 128), 0, 0, true);
			driver->endScene();
		}
		device->drop();
	}
	catch (exception ex)
	{
		cout << ex.what() << endl;
	}
	return 0;
}