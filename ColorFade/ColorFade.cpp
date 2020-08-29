// ��������irrlicht����·��
#include <irrlicht.h> 
using namespace irr;
using namespace core;
using namespace video;

#pragma comment(lib,"irrlicht.lib")

int main(void)
{
	// ����Console���ڵı����봰�ڱ���/ǰ����ɫ
	system("TITLE Irrlicht Demo - Color FadeIn/FadeOut");
	system("COLOR 9E");

	//����(����)�豸(����)
	IrrlichtDevice* device = createDevice(EDT_DIRECT3D9, dimension2d<u32>(800, 600), 32U, false, false, false, NULL);
	//������Ⱦ���ڱ���
	device->setWindowCaption(L"Color Fade In/Out");
	if (device != NULL)
	{
		//��Ƶ��Ⱦ����
		IVideoDriver* driver = device->getVideoDriver();
		if (driver != NULL)
		{
			//��ʱ��
			ITimer* timer = device->getTimer();
			f32 bg_r = 255.0f;
			f32 bg_g = 0.0f;
			f32 bg_b = 255.0f;
			bool fadeOut = true;
			u32 then = timer->getTime();
			const f32 fadeRate = 0.05f; //��ɫ��������

			//��Ⱦѭ��
			while (device->run())
			{
				//��ǰ֡��ʼʱ��
				const u32 now = timer->getTime();
				//��ȥʱ��
				const f32 frameDeltaTime = (f32)(now - then);
				then = now;
				if (bg_r <= 0.0f)
				{
					fadeOut = false; //��Ϊ����
				}
				else if (bg_r >= 255.0f)
				{
					fadeOut = true; //��Ϊ����
				} // bg_r??fadeOut
				else
				{
					// 0<=bg_r<=255, do nothing.
				} // bg_r??fadeOut

				//������ɫ����/��������
				if (fadeOut)
				{
					bg_r -= fadeRate * frameDeltaTime;
					bg_g += fadeRate * frameDeltaTime;
					bg_b -= fadeRate * frameDeltaTime;
				} // fadeOut/In
				else
				{
					bg_r += fadeRate * frameDeltaTime;
					bg_g -= fadeRate * frameDeltaTime;
					bg_b += fadeRate * frameDeltaTime;
				} // fadeOut/In

				//��Ⱦ
				driver->beginScene(true, true, SColor(255, (u32)bg_r, (u32)bg_g, (u32)bg_b));
				driver->endScene();
			} // while(running)
		} // driver OK
		else
		{
			printf("Failed to get video driver!\n");
		} // driver NULL

		//ж���豸
		device->drop();
	} // device engine OK
	else
	{
		printf("Irrlicht engine failure!\n");
	} // device engine NULL

	system("pause");
	return 0;
}