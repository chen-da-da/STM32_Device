#include "key.h"
#include "ftcomm.h"
#include "wificomm.h"
#include "lcd.h"

u8 TIM2_num;

void Key_Test(void)
{
	u8 flag,mode;
	mode=Work_Key_Scan();
	if(mode==0x01)				//KEY1�Ĺ���1
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
//Function Description: �ϵ�ɨ�谴��
//Input:				
//Output:
//Return:   
//*********************************************
u8 Startup_Key_Scan(void)
{
	u8 flag=0,KeyCnt=10;				//100ms��Ϊ��
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
//Function Description: ���������а���ɨ��
//Input:				
//Output:
//Return:   
//*********************************************
u8 Work_Key_Scan(void)
{
	u8 mode=0,Read_Key;
	if(TIM2_num>=2){							//10msɨ��һ��
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
//Function Description: ����ɨ��
//Input:				
//Output:
//Return:   
//*********************************************
u8 keyscan(void)
{
	static u8 Key_State   = 0;        //����״̬
    static u8 Key_Prev    = 0;        //��һ�ΰ���
    static u16 Key_Delay   = 0;        //��������ʱ��
    static u8 Key_Series  = 0;    //��־������ʼ
    u8 Key_Press  = NO_KEY;           //����ֵ
    u8 Key_Return = NO_KEY;           //��������ֵ
	Key_Press=Get_Key();        
	switch (Key_State){
		case 0:  					//����״̬��ʼ��
		if(Key_Press!=NO_KEY){
			Key_State =1;
			Key_Prev =Key_Press;   	//���水��״̬
		}
		break;
		case 1:					 		//����ȷ��״̬1
		if(Key_Press==Key_Prev){	  	//ȷ�����ϴΰ�����ͬ
		
			 Key_State = 2;
			 Key_Return =KEY_DOWN|Key_Prev;
		}
		else{  //����̧��,�Ƕ���,����Ӧ����
            Key_State = 0;
        }
		break;
		case 2://�����ͷ�̬
        if (Key_Press == NO_KEY ){//�����ͷ���
            Key_State = 0;
            Key_Delay = 0;
            if(Key_Series==0){
				Key_Return  = KEY_UP | Key_Prev;      //�̰���̧�ַ��ذ���̧��ֵ
            }
            Key_Series  = 0;
            break;
        }
        if ( Key_Press ==Key_Prev ){
            Key_Delay++;
            if ((Key_Series==1) && (Key_Delay>KEY_SERIES_DELAY)){
                Key_Delay  = 0;
                Key_Return = KEY_LIAN | Key_Press;  //����������ֵ
                Key_Prev   = Key_Press;           //��ס�ϴεİ���.
                break;
            }
            if (Key_Delay>KEY_SERIES_FLAG){
                Key_Series = 1;
                Key_Delay  = 0;
                Key_Return = KEY_LONG | Key_Prev;   //���س������ֵ
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
//Function Description: ��ð���ֵ
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












