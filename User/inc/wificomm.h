#ifndef _WIFICOMM
#define _WIFICOMM
#include "stm32f10x.h"
#include "hw_config.h"
#include "ftcomm.h"
#include "wifi.h"
#include "fft_test.h"


#define WIFI_USART				1
#define CMD_Read_ID				0x10
#define CMD_READ 				0x16				//只读良率命令
#define CMD_SET					0x10				//上位机握手+设置机台编号

typedef union
{
	u32 Passnum;
	u8 Passarr[4];
}PASS;

typedef union
{
	u32 NGnum;
	u8 NGarr[4];
}NG;

typedef union
{
	u32 NUII_num;
	u8 NULL_arr[4];
}NULL_;

#define WIFI_BUFFER_SIZE		4096
void FT_Info_Dispaly(void);
void FT_Work_Dispaly(void);
u8 Read_Current_FtID(void);
u8 Dispaly_Ft_ID(UART_NUM USART_X);
u8 APP_RepReadID(UART_NUM USART_X);
u8 Fun_Read_ID(UART_NUM USART_X );

void Fun_Cmd_WifiParse(u8 cmd,u8 flag);
void WifiParseCmd(void);
void CMD_WiFiHandle(u8 *pBuff);
void SDCount(const TCHAR *path);
void Framing(void);
void CMD_ReadDataRight(u8 CMD_Type);
u8 CMD_ReadHandle(UART_NUM USART_X);

#endif



