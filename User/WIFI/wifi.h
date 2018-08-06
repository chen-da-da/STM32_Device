#ifndef __WIFI_H
#define __WIFI_H

#include "hw_config.h"

void EnterATMode(void);	//进入AT命令模式
void ExitATMode(void);	//退出AT命令模式
void GetIP(void);		//获取IP
u8 GetTCP_STA(void);	//获取TCP连接状态
void WIFIRestart(void);	//重启WiFi模块

void Send_AT(uint8_t *str);
void Usart_SendString( USART_TypeDef * pUSARTx, uint8_t *str);
static void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch );

#endif

