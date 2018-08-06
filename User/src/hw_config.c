#include "hw_config.h"
#include <stdio.h>
#include"prt_usart1.h"
#include "fft_test.h"
#include "ftcomm.h"
#include "Lcd.h"

//#define		UART1_SD_TEST
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
const UartSendTable  UartTable_Send[5]=
{
	USART1_SendData,
	USART2_SendData,
	USART3_SendData,
	UART4_SendData,
	UART5_SendData
};

const UartClrTable  UartTable_Clr[5]=
{
	USART1_ClearFlag,
	USART2_ClearFlag,
	USART3_ClearFlag,
	UART4_ClearFlag,
	UART5_ClearFlag
};

BuzzerSetInfo	BuzzerSet;
/*******************************************************************************
* Function Name  : Set_System
* Description    : Configures Main system clocks & power.
* Input          : None.
* Return         : None.
*******************************************************************************/
void Set_System(void){
  
	SystemInit();
	NVIC_Configuration();
	Timer2_Configuration();
	GPIO_Configuration();
	UARTX_Configuration(115200);
	LED_ON(LEDALL);
	LCD_init();
	Buzzer_Out(1);							//输出1S							
	while(mf_ftfsInit()!=0x00);			   //等待SD卡完成初始化
	LED_OFF(LEDALL);
	LCD_Display_Test();
	USARTX_ON(UART_ALL);	
}

/*******************************************************************************
* Function Name  : Set_System
* Description    : Sets the vector table location and Offset.
* Input          : None.
* Return         : None.
*******************************************************************************/
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef	NVIC_InitStructure;
	//NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	//NVIC Config
	NVIC_InitStructure.NVIC_IRQChannel=USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//NVIC Config
	NVIC_InitStructure.NVIC_IRQChannel=USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//NVIC Config
	NVIC_InitStructure.NVIC_IRQChannel=USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//NVIC Config
	NVIC_InitStructure.NVIC_IRQChannel=UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//NVIC Config
	NVIC_InitStructure.NVIC_IRQChannel=UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	
    //NVIC Config
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;			//低抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

/*******************************************************************************
* Function Name  : void UARTX_Configuration(u32 BaudRate)
* Description    : Configures UART1.
* Input          : None.
* Return         : None.
*******************************************************************************/
void UARTX_Configuration(u32 BaudRate)
{
	UART1_Configuration(BaudRate); 
	UART2_Configuration(BaudRate);
	UART3_Configuration(BaudRate);
	UART4_Configuration(BaudRate);
	UART5_Configuration(BaudRate);
}
/*******************************************************************************
* Function Name  : void UART1_Configuration(u32 BaudRate)
* Description    : Configures UART1.
* Input          : None.
* Return         : None.
*******************************************************************************/
void UART1_Configuration(u32 BaudRate) 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	//TX->PA9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//RX->PA10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	

	USART_InitStructure.USART_BaudRate = BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;//可编程位，USART_Parity_Odd(奇)USART_Parity_Even(偶)校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure); 
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//开启空闲中断:一帧数据接收中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//一字节中断
	USART_Cmd(USART1, DISABLE);                    //关闭串口
	//USART_ClearFlag(USART1, USART_FLAG_TC);
}

/*******************************************************************************
* Function Name  : void UART2_Configuration(u32 BaudRate)
* Description    : Configures UART1.
* Input          : None.
* Return         : None.
*******************************************************************************/
void UART2_Configuration(u32 BaudRate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
//	NVIC_InitTypeDef	NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	//TX->PA2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//RX->PA3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;//可编程位，USART_Parity_Odd(奇)USART_Parity_Even(偶)校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure); 
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);//开启空闲中断:一帧数据接收中断
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//一字节中断
	USART_Cmd(USART2, DISABLE);                    //关闭串口 
	//USART_ClearFlag(USART2, USART_FLAG_TC);
}

/*******************************************************************************
* Function Name  : void UART3_Configuration(u32 BaudRate)
* Description    : Configures UART1.
* Input          : None.
* Return         : None.
*******************************************************************************/
void UART3_Configuration(u32 BaudRate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
//	NVIC_InitTypeDef	NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	//TX->PB10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//RX->PB11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;//可编程位，USART_Parity_Odd(奇)USART_Parity_Even(偶)校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure); 
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);//开启空闲中断:一帧数据接收中断
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//一字节中断
	USART_Cmd(USART3, DISABLE);                    //关闭串口 
	//USART_ClearFlag(USART3, USART_FLAG_TC);
}

/*******************************************************************************
* Function Name  : void UART4_Configuration(u32 BaudRate)
* Description    : Configures UART1.
* Input          : None.
* Return         : None.
*******************************************************************************/
void UART4_Configuration(u32 BaudRate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
//	NVIC_InitTypeDef	NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
	//TX->PC10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//RX->PC11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;//可编程位，USART_Parity_Odd(奇)USART_Parity_Even(偶)校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART4, &USART_InitStructure); 
	USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);//开启空闲中断:一帧数据接收中断
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//一字节中断
	USART_Cmd(UART4, DISABLE);                    //关闭串口 
	//USART_ClearFlag(UART4, USART_FLAG_TC);
}

/*******************************************************************************
* Function Name  : void UART5_Configuration(u32 BaudRate)
* Description    : Configures UART1.
* Input          : None.
* Return         : None.
*******************************************************************************/
void UART5_Configuration(u32 BaudRate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
//	NVIC_InitTypeDef	NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);
	//TX->PC12
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//RX->PD2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;//可编程位，USART_Parity_Odd(奇)USART_Parity_Even(偶)校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART5, &USART_InitStructure); 
	USART_ITConfig(UART5, USART_IT_IDLE, ENABLE);//开启空闲中断:一帧数据接收中断
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//一字节中断
	USART_Cmd(UART5, DISABLE);                    //关闭串口 
	//USART_ClearFlag(UART5, USART_FLAG_TC);
}
/*******************************************************************************
* Function Name  : void USARTX_SendByte(UART_SEND_NUM UartNum,u8* PData,u8 Length)
* Description    : USARTX发送多BYTE数据
* Input          : None.
* Return         : None.
*******************************************************************************/
void USARTX_SendByte(UART_NUM UartNum,u8* PData,u32 Length)
{
	u16 i=0;
	ReloadChecksum(PData,Length);			//Cmd+数据+CS
	UartTable_Clr[UartNum]();				//发送前清发送寄存器状态，防止第一个Byte丢失
	while(Length--)
	{
		UartTable_Send[UartNum]((u8)PData[i++]);
	} 
}
/*******************************************************************************
* Function Name  : void USART1_ClearFlag(void)
* Description    : USARTX发送前清状态
* Input          : None.
* Return         : None.
*******************************************************************************/
void USART1_ClearFlag(void)
{
	USART_ClearFlag(USART1, USART_FLAG_TC);
}

/*******************************************************************************
* Function Name  : void USART1_SendData(u16 ch)
* Description    : USART1发送1BYTE.
* Input          : None.
* Return         : None.
*******************************************************************************/
void USART1_SendData(u16 ch)
{
	u16 i;
	USART_SendData(USART1, (u8)ch);
	for(i=0;i<3000;i++)
	{
		if(USART_GetFlagStatus(USART1, USART_FLAG_TC) != RESET)
		{
			break;
		}
	}
}

/*******************************************************************************
* Function Name  : void USART2_ClearFlag(void)
* Description    : USARTX发送前清状态
* Input          : None.
* Return         : None.
*******************************************************************************/
void USART2_ClearFlag(void)
{
	USART_ClearFlag(USART2, USART_FLAG_TC);
}

/*******************************************************************************
* Function Name  : void USART2_SendData(u16 ch)
* Description    : USART2发送1BYTE.
* Input          : None.
* Return         : None.
*******************************************************************************/
void USART2_SendData(u16 ch)
{
	u16 i;
	USART_SendData(USART2, (u8)ch);
	for(i=0;i<3000;i++)
	{
		if(USART_GetFlagStatus(USART2, USART_FLAG_TC) != RESET)
		{
			break;
		}
	}
}
/*******************************************************************************
* Function Name  : void USART3_ClearFlag(void)
* Description    : USARTX发送前清状态
* Input          : None.
* Return         : None.
*******************************************************************************/
void USART3_ClearFlag(void)
{
	USART_ClearFlag(USART3, USART_FLAG_TC);
}

/*******************************************************************************
* Function Name  : void USART3_SendData(u16 ch)
* Description    : USART3发送1BYTE.
* Input          : None.
* Return         : None.
*******************************************************************************/
void USART3_SendData(u16 ch)
{
	u16 i;
	USART_SendData(USART3, (u8)ch);
	for(i=0;i<3000;i++)
	{
		if(USART_GetFlagStatus(USART3, USART_FLAG_TC) != RESET)
		{
			break;
		}
	}
}
/*******************************************************************************
* Function Name  : void USART4_ClearFlag(void)
* Description    : USARTX发送前清状态
* Input          : None.
* Return         : None.
*******************************************************************************/
void UART4_ClearFlag(void)
{
	USART_ClearFlag(UART4, USART_FLAG_TC);
}

/*******************************************************************************
* Function Name  : void USART1_SendData(u16 ch)
* Description    : USART1发送1BYTE.
* Input          : None.
* Return         : None.
*******************************************************************************/
void UART4_SendData(u16 ch)
{
	u16 i;
	USART_SendData(UART4, (u8)ch);
	for(i=0;i<3000;i++)
	{
		if(USART_GetFlagStatus(UART4, USART_FLAG_TC) != RESET)
		{
			break;
		}
	}
}

/*******************************************************************************
* Function Name  : void USART5_ClearFlag(void)
* Description    : USARTX发送前清状态
* Input          : None.
* Return         : None.
*******************************************************************************/
void UART5_ClearFlag(void)
{
	USART_ClearFlag(UART5, USART_FLAG_TC);
}


/*******************************************************************************
* Function Name  : void USART1_SendData(u16 ch)
* Description    : USART1发送1BYTE.
* Input          : None.
* Return         : None.
*******************************************************************************/
void UART5_SendData(u16 ch)
{
	u16 i;
	USART_SendData(UART5, (u8)ch);
	for(i=0;i<3000;i++)
	{
		if(USART_GetFlagStatus(UART5, USART_FLAG_TC) != RESET)
		{
			break;
		}
	}
}

///*******************************************************************************
//* Function Name  : fputc
//* Description    :重定向C的printf到USART4
//* Input          : None.
//* Return         : None.
//*******************************************************************************/
//int fputc(int ch,FILE *f)
//{
//	USART_SendData(UART4, (u8) ch);//将Printf内容发向串口
//	while(USART_GetFlagStatus(UART4, USART_FLAG_TC)==RESET);
//	return(ch);
//}

/*******************************************************************************
* Function Name  :GPIO_Configuration
* Description    :
* Input          : None.
* Return         : None.
*******************************************************************************/
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|\
							RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOF,ENABLE);
//LCD:PD8~PD13
//字库：PA4~PA7
	GPIO_InitStructure.GPIO_Pin = LCD_ALL_PORT ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LCD_ALL, &GPIO_InitStructure);
	GPIO_SetBits(LCD_ALL, LCD_ALL_PORT);						//LCD驱动初始为高
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, (GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7));
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_6);
	
 ////////////////////////////////////////////////////////////////////////////// 
 //蜂鸣器
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	BUZZEROut_Close();
//LED:PE7~PE10
	GPIO_InitStructure.GPIO_Pin = LED_0|LED_1|LED_2|LED_3 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LED_PORT, &GPIO_InitStructure);
 	GPIO_ResetBits(LED_PORT,(LED_0|LED_1|LED_2|LED_3));
 //Key:PE14~PE15
 	GPIO_InitStructure.GPIO_Pin = KEY_0|KEY_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(KEY_PORT, &GPIO_InitStructure);
	GPIO_SetBits(KEY_PORT,(KEY_0|KEY_1));

 	
}

/*******************************************************************************
* Function Name  : void Timer2_Configuration(void)
* Description    : Configures the Timer2 5ms
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void Timer2_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    TIM_TimeBaseStructure.TIM_Prescaler=(8-1);      //8分频
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period=45000;       //5ms定时
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
    
    TIM_ITConfig(TIM2,TIM_IT_CC1,ENABLE);
    TIM_ClearITPendingBit(TIM2,TIM_IT_CC1);
    TIM_Cmd(TIM2,ENABLE); 	

}
//*********************************************
//Function Name: void ErrOut(void)
//Function Description: 
//Input:		
//Output:	
//Return:BuzzerTimerCnt:	
//*********************************************
void ErrOut(void)
{
	u8 i;
	if(Read_Buzzer_Status()==0)					//空闲状态下才报警
	{
		for(i=0;i<4;i++)
		{
			if(FtOutTimeFlag[i].WarnFlag==1)
			{
				FtOutTimeFlag[i].WarnFlag=0;				//清除当前的Ft测试警告
				Buzzer_Out(1);							//ft测试异常报警
				break;
			}
		}
	}
}

//*********************************************
//Function Name: void Buzzer_Out(u8 CntS)
//Function Description: Buzzer输出Ns
//Input:		
//Output:	
//Return:BuzzerTimerCnt:单位5ms,1S=200Cnt;,最大支持65536/200	
//*********************************************
void Buzzer_Out(u8 CntS)
{
	BUZZEROut_Close();							//关闭BUZZER
	BuzzerSet.BuzzerTimerCnt=0;
	BuzzerSet.BuzzerTimerCntMAX=(u16)CntS*200;
	if(BuzzerSet.BuzzerTimerCntMAX>0)
	{
		BuzzerSet.BuzzerTimerCntFlag=1;
		BUZZEROut_Open();
	}
}
//*********************************************
//Function Name: void LED_ON(LED_Num nLED)
//Function Description: Led On_OFF
//Input:		
//Output:	//capy
//Return:	Led1-Pb5,Led2-PE0,Led3-PB1,Led4-Pc0
//*********************************************
 void LED_OFF(LED_Num nLED)
 {
   switch(nLED)
   {
	 case LED0: 	GPIO_WriteBit(LED_PORT, LED_0, Bit_RESET);
	               	break;
	 case LED1: 	GPIO_WriteBit(LED_PORT, LED_1, Bit_RESET);
	               	break;
	 case LED2:  	GPIO_WriteBit(LED_PORT, LED_2, Bit_RESET);
	               	break;
	 case LED3:  	GPIO_WriteBit(LED_PORT, LED_3, Bit_RESET);
	               	break;
	 case LEDALL:  	GPIO_WriteBit(LED_PORT, LED_0, Bit_RESET);
	 				GPIO_WriteBit(LED_PORT, LED_1, Bit_RESET);
					GPIO_WriteBit(LED_PORT, LED_2, Bit_RESET);
					GPIO_WriteBit(LED_PORT, LED_3, Bit_RESET);
	               	break;			   
	  default :    	break;
	}
 }

 //*********************************************
//Function Name: void LED_OFF(LED_Num nLED)
//Function Description: Led On_OFF
//Input:		
//Output:	//capy
//Return:	Led1-Pb5,Led2-PE0,Led3-PB1,Led4-Pc0
//*********************************************

void LED_ON(LED_Num nLED)
{
   switch(nLED)
   {
	 case LED0: 	GPIO_WriteBit(LED_PORT, LED_0, Bit_SET);
	               	break;
	 case LED1: 	GPIO_WriteBit(LED_PORT, LED_1, Bit_SET);
	               	break;
	 case LED2:  	GPIO_WriteBit(LED_PORT, LED_2, Bit_SET);
	               	break;
	 case LED3:  	GPIO_WriteBit(LED_PORT, LED_3, Bit_SET);
	               	break;
	 case LEDALL:  	GPIO_WriteBit(LED_PORT, LED_0, Bit_SET);
	 				GPIO_WriteBit(LED_PORT, LED_1, Bit_SET);
					GPIO_WriteBit(LED_PORT, LED_2, Bit_SET);
					GPIO_WriteBit(LED_PORT, LED_3, Bit_SET);
	               	break;			   
	  default :    	break;
	}
 
}

//*********************************************
//Function Name: void LED_ON(LED_Num nLED)
//Function Description: Led On_OFF
//Input:		
//Output:	//capy
//Return:	Led1-Pb5,Led2-PE0,Led3-PB1,Led4-Pc0
//*********************************************
 void USARTX_OFF(UART_NUM nUsart)
 {
   switch(nUsart)
   {
	 case USART_1: 	USART_Cmd(USART1, DISABLE);
	               	break;
	 case USART_2: 	USART_Cmd(USART2, DISABLE);
	               	break;
	 case USART_3:  USART_Cmd(USART3, DISABLE);
	               	break;
	 case UART_4:  	USART_Cmd(UART4, DISABLE);
	               	break;
	 case UART_5:  	USART_Cmd(UART5, DISABLE);
	               	break;
	 case UART_ALL: USART_Cmd(USART1, DISABLE);
	 				USART_Cmd(USART2, DISABLE);
					USART_Cmd(USART3, DISABLE);
					USART_Cmd(UART4, DISABLE);
					USART_Cmd(UART5, DISABLE);
	               	break;			   
	  default :    	break;
	}
 }

 //*********************************************
//Function Name: void LED_OFF(LED_Num nLED)
//Function Description: Led On_OFF
//Input:		
//Output:	//capy
//Return:	Led1-Pb5,Led2-PE0,Led3-PB1,Led4-Pc0
//*********************************************

void USARTX_ON(UART_NUM nUsart)
{
   switch(nUsart)
   {
	 case USART_1: 	USART_Cmd(USART1, ENABLE);USART_ClearFlag(USART1, USART_FLAG_TC);
	               	break;
	 case USART_2: 	USART_Cmd(USART2, ENABLE);USART_ClearFlag(USART2, USART_FLAG_TC);
	               	break;
	 case USART_3:  USART_Cmd(USART3, ENABLE);USART_ClearFlag(USART3, USART_FLAG_TC);
	               	break;
	 case UART_4:  	USART_Cmd(UART4, ENABLE);USART_ClearFlag(UART4, USART_FLAG_TC);
	               	break;
	 case UART_5:  	USART_Cmd(UART5, ENABLE);USART_ClearFlag(UART5, USART_FLAG_TC);
	               	break;
	 case UART_ALL: USART_Cmd(USART1, ENABLE);USART_ClearFlag(USART1, USART_FLAG_TC);
	 				USART_Cmd(USART2, ENABLE);USART_ClearFlag(USART2, USART_FLAG_TC);
					USART_Cmd(USART3, ENABLE);USART_ClearFlag(USART3, USART_FLAG_TC);
					USART_Cmd(UART4, ENABLE);USART_ClearFlag(UART4, USART_FLAG_TC);
					USART_Cmd(UART5, ENABLE);USART_ClearFlag(UART5, USART_FLAG_TC);
	               	break;			   
	  default :    	break;
	}
 }
/*******************************************************************************
* Function Name  : void SoftReset(void) 
* Description    : 软件复位
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SoftReset(void) 
{ 
	Delay_MS(10);		 	//延时一下
	__set_FAULTMASK(1);  	// 关闭所有中断
	NVIC_SystemReset();		// 复位 
} 


/*******************************************************************************
* Function Name  : void Delay_MS(u16 num)
* Description    :  MS延时
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Delay_MS(u16 num)
{
	u16 nCount;
	while(num--)
	{
		nCount = 8000;
		for(; nCount!= 0;nCount--);
	}
}
/*******************************************************************************
* Function Name  : void Delay(u32 DelayTime)
* Description    : us延时
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Delay(u32 DelayTime)
{
	for(;DelayTime!=0;DelayTime--);
}

/*******************************************************************************
* Function Name  : void Buzzer_Delay_Set(void)
* Description    : Buzzer延时
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Buzzer_Delay_Set(void)
{
	if(BuzzerSet.BuzzerTimerCntFlag==1)
	{
		if(++BuzzerSet.BuzzerTimerCnt>=BuzzerSet.BuzzerTimerCntMAX)
		{
			BuzzerSet.BuzzerTimerCntFlag=0;
			BUZZEROut_Close();
		}
	}	
}

/*******************************************************************************
* Function Name  : void Read_Buzzer_Status(void)
* Description    : Buzzer状态
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
u8 Read_Buzzer_Status(void)
{
	return (BuzzerSet.BuzzerTimerCntFlag);
}



  


