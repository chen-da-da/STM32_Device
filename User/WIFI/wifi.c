#include "wifi.h"

u8 TCP_STA = 0;	//TCP连接状态标志
u8 IPString[16];

/**
**进入AT命令模式
**/
void EnterATMode(void)
{
	Delay_MS(20);
	Send_AT("+++");
	Delay_MS(20);				//必须延时20ms
	Send_AT("a");
	Delay_MS(20);
}

/**
**退出AT命令模式
**/
void ExitATMode(void)
{
	Delay_MS(50);
	Send_AT("AT+ENTM\r");
	Delay_MS(20);
}

/**
**获取WiFi模块IP
**/
void GetIP(void)
{
	//Delay_MS(100);
	EnterATMode();
	Send_AT("AT+WANN\r");
	ExitATMode();
}

/**
**获取TCP连接状态
**返回值：已连接返回1，未连接返回0
**/
u8 GetTCP_STA()
{
	//Delay_MS(100);
	u8 flag;
	EnterATMode();
	Send_AT("AT+TCPLK\n");
	ExitATMode();
	flag = TCP_STA;
	TCP_STA = 0;
	return flag;
}

/**
**重启WiFi模块
**/
void WIFIRestart()
{
	Delay_MS(100);
	EnterATMode();
	Send_AT("AT+Z\n");
	ExitATMode();
}

/**
**发送AT命令
**/
void Send_AT(uint8_t *str)
{
	Usart_SendString(UART5, str);
}

/*****************  发送字符串 **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, uint8_t *str)
{
	unsigned int k=0;
    do 
    {
        Usart_SendByte( pUSARTx, *(str + k) );
        k++;
    } while(*(str + k)!='\0');
}

/*****************  发送一个字符 **********************/
static void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch )
{
	/* 发送一个字节数据到USART1 */
	USART_SendData(pUSARTx,ch);
		
	/* 等待发送完毕 */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

