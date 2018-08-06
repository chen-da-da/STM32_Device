#ifndef _KEY
#define _KEY
#include "hw_config.h"
#include "wifi.h"

extern u8 TIM2_num;

#define KEY_SERIES_FLAG     100      //按键连发开始所需时间长度
#define KEY_SERIES_DELAY    5       //按键连发的时间间隔长度

														 
#define KEY_DOWN        0xA0	 //按下按键
#define KEY_LONG        0xB0	 //长按键
#define KEY_LIAN        0xC0	 //连续按键（长按住）
#define KEY_UP          0xD0	 //释放按键
#define NO_KEY          0x00	 //无按键
#define KEY0_DOWN       0X01
#define KEY1_DOWN		0x02
#define	KEY0_STATUS		GPIO_ReadInputDataBit(KEY_PORT, KEY_0)
#define KEY1_STATUS		GPIO_ReadInputDataBit(KEY_PORT, KEY_1)

u8 Get_Key(void);
u8 keyscan(void);
u8 Startup_Key_Scan(void);
u8 Work_Key_Scan(void);
void Key_Test(void);





#endif



