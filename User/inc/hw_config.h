#ifndef __HWCONFIG_H
#define __HWCONFIG_H
#include "stm32f10x.h"
#include <stdio.h>
#include<string.h>

//#define UART4_DR_ADR	0x40004C00+0x04
#define LCD_ALL_PORT			(GPIO_Pin_8 |GPIO_Pin_9|GPIO_Pin_10 |GPIO_Pin_11|GPIO_Pin_12 |GPIO_Pin_13)
#define LCD_ALL					 GPIOD

#define LED_0					GPIO_Pin_7
#define LED_1					GPIO_Pin_8
#define LED_2					GPIO_Pin_9
#define LED_3					GPIO_Pin_10
#define LED_PORT				GPIOE

#define KEY_0					GPIO_Pin_14
#define KEY_1					GPIO_Pin_15
#define KEY_PORT				GPIOE

#define BUZZEROut_Close()			GPIO_ResetBits(GPIOF,GPIO_Pin_15)
#define BUZZEROut_Open()			GPIO_SetBits(GPIOF,GPIO_Pin_15)

typedef enum _LED_Num{LED0,LED1,LED2,LED3,LEDALL}LED_Num;
typedef enum _UART_NUM{USART_1,USART_2,USART_3,UART_4,UART_5,UART_ALL}UART_NUM;
#pragma pack(1)		
typedef struct
{
	u8 BuzzerTimerCntFlag;
	u16 BuzzerTimerCnt;
	u16 BuzzerTimerCntMAX;	
}BuzzerSetInfo;
#pragma pack()
typedef void (*UartSendTable)(u16);		   		//重命名函数指针
typedef void (*UartClrTable)(void);		   		//重命名函数指针
void Set_System(void);
void NVIC_Configuration(void);
void LED_OFF(LED_Num nLED);
void LED_ON(LED_Num nLED);
 void USARTX_OFF(UART_NUM nUsart);
  void USARTX_ON(UART_NUM nUsart);
void GPIO_Configuration(void);
void USARTX_SendByte(UART_NUM UartNum,u8* PData,u32 Length);
void UARTX_Configuration(u32 BaudRate);
void UART1_Configuration(u32 BaudRate);
void UART2_Configuration(u32 BaudRate);
void UART3_Configuration(u32 BaudRate);
void UART4_Configuration(u32 BaudRate);
void UART5_Configuration(u32 BaudRate);
void USART1_SendData(u16 ch);
void USART2_SendData(u16 ch);
void USART3_SendData(u16 ch);
void UART4_SendData(u16 ch);
void UART5_SendData(u16 ch);
void USART1_ClearFlag(void);
void USART2_ClearFlag(void);
void USART3_ClearFlag(void);
void UART4_ClearFlag(void);
void UART5_ClearFlag(void);
void Timer2_Configuration(void);
void Buzzer_Out(u8 CntS);
void ErrOut(void);

void SoftReset(void);
void Delay_MS(u16 num);
void Delay(u32 DelayTime);

void Buzzer_Delay_Set(void);
u8 Read_Buzzer_Status(void);


extern BuzzerSetInfo	BuzzerSet;




#endif

