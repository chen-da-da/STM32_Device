#ifndef __WIFI_H
#define __WIFI_H

#include "hw_config.h"

void EnterATMode(void);	//����AT����ģʽ
void ExitATMode(void);	//�˳�AT����ģʽ
void GetIP(void);		//��ȡIP
u8 GetTCP_STA(void);	//��ȡTCP����״̬
void WIFIRestart(void);	//����WiFiģ��

void Send_AT(uint8_t *str);
void Usart_SendString( USART_TypeDef * pUSARTx, uint8_t *str);
static void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch );

#endif

