#include "key.h"
#include "ftcomm.h"
#include "wificomm.h"
#include "lcd.h"

u8 TIM2_num;

void Key_Test(void)
{
	u8 flag,mode;
	mode=Work_Key_Scan();
	if(mode==0x01)				//KEY1的功能1
	{
		Clear_Screen(1,1,256,12);
		flag=Read_Current_FtID();
		if(flag>0)
		{
			Delay_MS(1);
		}
	}
	else if(mode==3)			
	{
		Clear_Screen(1,1,256,12);
		Test_UART3_AppUpdata();
	}
	else if(mode==2)
	{
		FT_Info_Dispaly();
	}
	FT_Work_Dispaly();
	
}
//*********************************************
//Function Name:u8 Startup_Key_Scan(void)
//Function Description: 上电扫描按键
//Input:				
//Output:
//Return:   
//*********************************************
u8 Startup_Key_Scan(void)
{
	u8 flag=0,KeyCnt=10;				//100ms均为低
	while((KEY1_STATUS==0)&&KeyCnt)
	{
		Delay_MS(10);
		KeyCnt--;
	}
	if(KeyCnt==0)	flag=1;
	return flag;
}

//*********************************************
//Function Name:Work_Key_Scan(void)
//Function Description: 工作过程中按键扫描
//Input:				
//Output:
//Return:   
//*********************************************
u8 Work_Key_Scan(void)
{
	u8 mode=0,Read_Key;
	if(TIM2_num>=2){							//10ms扫描一次
		TIM2_num=0;
		Read_Key=keyscan();
		if(Read_Key==(KEY_UP|KEY0_DOWN))		mode=1;
		else if(Read_Key==(KEY_UP|KEY1_DOWN))	mode=2;
		else if(Read_Key==(KEY_LONG|KEY0_DOWN)) mode=3;
	}
	return mode;
}
//*********************************************
//Function Name:u8 u8 keyscan(void)
//Function Description: 按键扫描
//Input:				
//Output:
//Return:   
//*********************************************
u8 keyscan(void)
{
	static u8 Key_State   = 0;        //按键状态
    static u8 Key_Prev    = 0;        //上一次按键
    static u16 Key_Delay   = 0;        //按键连发时间
    static u8 Key_Series  = 0;    //标志连发开始
    u8 Key_Press  = NO_KEY;           //按键值
    u8 Key_Return = NO_KEY;           //按键返回值
	Key_Press=Get_Key();        
	switch (Key_State){
		case 0:  					//按键状态初始化
		if(Key_Press!=NO_KEY){
			Key_State =1;
			Key_Prev =Key_Press;   	//保存按键状态
		}
		break;
		case 1:					 		//按键确认状态1
		if(Key_Press==Key_Prev){	  	//确认与上次按键相同
		
			 Key_State = 2;
			 Key_Return =KEY_DOWN|Key_Prev;
		}
		else{  //按键抬起,是抖动,不响应按键
            Key_State = 0;
        }
		break;
		case 2://按键释放态
        if (Key_Press == NO_KEY ){//按键释放了
            Key_State = 0;
            Key_Delay = 0;
            if(Key_Series==0){
				Key_Return  = KEY_UP | Key_Prev;      //短按键抬手返回按键抬起值
            }
            Key_Series  = 0;
            break;
        }
        if ( Key_Press ==Key_Prev ){
            Key_Delay++;
            if ((Key_Series==1) && (Key_Delay>KEY_SERIES_DELAY)){
                Key_Delay  = 0;
                Key_Return = KEY_LIAN | Key_Press;  //返回连发的值
                Key_Prev   = Key_Press;           //记住上次的按键.
                break;
            }
            if (Key_Delay>KEY_SERIES_FLAG){
                Key_Series = 1;
                Key_Delay  = 0;
                Key_Return = KEY_LONG | Key_Prev;   //返回长按后的值
                break;
            }
        }

    	default :
        	break;
   }
   return Key_Return;
} 

//*********************************************
//Function Name:u8 Get_Key(void)
//Function Description: 获得按键值
//Input:				
//Output:
//Return:   
//*********************************************
u8 Get_Key(void)
{
    if (KEY0_STATUS==0) return  KEY0_DOWN;
	if (KEY1_STATUS==0) return  KEY1_DOWN;
    return NO_KEY;
}












