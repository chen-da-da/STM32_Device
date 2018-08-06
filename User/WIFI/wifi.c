#include "wifi.h"

u8 TCP_STA = 0;	//TCP����״̬��־
u8 IPString[16];

/**
**����AT����ģʽ
**/
void EnterATMode(void)
{
	Delay_MS(20);
	Send_AT("+++");
	Delay_MS(20);				//������ʱ20ms
	Send_AT("a");
	Delay_MS(20);
}

/**
**�˳�AT����ģʽ
**/
void ExitATMode(void)
{
	Delay_MS(50);
	Send_AT("AT+ENTM\r");
	Delay_MS(20);
}

/**
**��ȡWiFiģ��IP
**/
void GetIP(void)
{
	//Delay_MS(100);
	EnterATMode();
	Send_AT("AT+WANN\r");
	ExitATMode();
}

/**
**��ȡTCP����״̬
**����ֵ�������ӷ���1��δ���ӷ���0
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
**����WiFiģ��
**/
void WIFIRestart()
{
	Delay_MS(100);
	EnterATMode();
	Send_AT("AT+Z\n");
	ExitATMode();
}

/**
**����AT����
**/
void Send_AT(uint8_t *str)
{
	Usart_SendString(UART5, str);
}

/*****************  �����ַ��� **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, uint8_t *str)
{
	unsigned int k=0;
    do 
    {
        Usart_SendByte( pUSARTx, *(str + k) );
        k++;
    } while(*(str + k)!='\0');
}

/*****************  ����һ���ַ� **********************/
static void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch )
{
	/* ����һ���ֽ����ݵ�USART1 */
	USART_SendData(pUSARTx,ch);
		
	/* �ȴ�������� */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

