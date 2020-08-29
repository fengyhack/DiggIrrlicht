// 请先设置irrlicht引用路径
#include <irrlicht.h> 
using namespace irr;
using namespace core;
using namespace video;

#pragma comment(lib,"irrlicht.lib")

int main(void)
{
	// 设置Console窗口的标题与窗口背景/前景颜色
	system("TITLE Irrlicht Demo - Color FadeIn/FadeOut");
	system("COLOR 9E");

	//创建(虚拟)设备(引擎)
	IrrlichtDevice* device = createDevice(EDT_DIRECT3D9, dimension2d<u32>(800, 600), 32U, false, false, false, NULL);
	//设置渲染窗口标题
	device->setWindowCaption(L"Color Fade In/Out");
	if (device != NULL)
	{
		//视频渲染驱动
		IVideoDriver* driver = device->getVideoDriver();
		if (driver != NULL)
		{
			//计时器
			ITimer* timer = device->getTimer();
			f32 bg_r = 255.0f;
			f32 bg_g = 0.0f;
			f32 bg_b = 255.0f;
			bool fadeOut = true;
			u32 then = timer->getTime();
			const f32 fadeRate = 0.05f; //颜色渐变速率

			//渲染循环
			while (device->run())
			{
				//当前帧开始时刻
				const u32 now = timer->getTime();
				//逝去时间
				const f32 frameDeltaTime = (f32)(now - then);
				then = now;
				if (bg_r <= 0.0f)
				{
					fadeOut = false; //改为淡入
				}
				else if (bg_r >= 255.0f)
				{
					fadeOut = true; //改为淡入
				} // bg_r??fadeOut
				else
				{
					// 0<=bg_r<=255, do nothing.
				} // bg_r??fadeOut

				//窗口颜色淡入/淡出渐变
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

				//渲染
				driver->beginScene(true, true, SColor(255, (u32)bg_r, (u32)bg_g, (u32)bg_b));
				driver->endScene();
			} // while(running)
		} // driver OK
		else
		{
			printf("Failed to get video driver!\n");
		} // driver NULL

		//卸载设备
		device->drop();
	} // device engine OK
	else
	{
		printf("Irrlicht engine failure!\n");
	} // device engine NULL

	system("pause");
	return 0;
}