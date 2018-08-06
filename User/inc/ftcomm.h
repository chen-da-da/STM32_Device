#ifndef _FTCOMM
#define _FTCOMM
#include "stm32f10x.h"
#include "hw_config.h"


#define UART_BUFFER_SIZE			4096

#define DATA_VAILD					0x01
#define	DATA_INVAILD				0x00

#define CMD_FT_STATUS				0x90
#define CMD_FT_OUTCOME				0x91
#define CMD_FT_TEST					0x93

#define CMD_FT_END					0x9F


#define	CMD_IAP_FILE				0x80
#define	CMD_IAP_TRANS				0x81
#define	CMD_WRITER_ID				0x82
#define	CMD_CANCEL_FILE				0x83
#define	CMD_IAP_RESULT				0x84
#define	CMD_IAP_VERSION				0x85
#define CMD_IAP_MVERSION			0x86//修改IAP模式
#define CMD_IAP_TEST				0x87//测试模式
#define CMD_IAP_STATUS				0x88
#define CMD_FT_ID					0x89//读取ID
#define CMD_IAP_ALL					0x8F



#define APP_FLAG					0x55
#define IAP_FLAG					0xAA
#define ERR_FLAG					0x88

#define FT_TEST_START				0x11
#define FT_TEST_BUSY				0x22
#define FT_TEST_END					0x33

#define RST_CORRECT   				0x77   //正确或者为空
#define RST_WRONG     				0x88   //错误或者非空
#define IAP_LOAD_NUM				6	  //完成一次下载需要的命令数


#define FT_USART					0
#define APP_UPDATA_TEST					//测试下载APP


#define FT_TEST_DONE_OK				1
#define FT_TEST_DONE_SDERR			2
#define FT_TEST_COMM_ERR			3


#pragma pack(1)							//使用1个字节对齐
////////////////////////////////////////////
typedef struct{
	u8 cmd;
	u8 outflag;
	u16 Cs;
}OutComeStatus;							//良品/不良品信息
////////////////////////////////////////////
typedef struct{
	u8 index;
	u16 Bindata;
}BinInfo;									
typedef struct{
	u8 cmd;
	BinInfo BinTable[64];
	u16 Cs;
}OutComeBin;							//Bin信息
////////////////////////////////////////////
typedef struct{
	u8 cmd;
	u8 status;
	u16 Cs;
}OutFtStatus;							//ft Status
////////////////////////////////////////////
typedef struct{
	u8 cmd;
	u8 status[32];					
	u16 Cs;
}OutFtID;								//ft_ID
////////////////////////////////////////////
typedef struct
{
	UART_NUM	uartn;					//串口编号
	u16	len;							//接收到的数据长度:最大是4096Byte
	union _data
	{
		u8  USART_RX_BUF[UART_BUFFER_SIZE];			//接收缓存
		OutComeStatus		OutComeFlag;			//良品信息
		OutComeBin			BinData;				//Bin输出结果
		OutFtStatus			ftStatus;				//ft输出状态
		OutFtID				FtID;					//ft板的ID编号
	}data;	
}USART_RX;


typedef struct
{
	u8 TestFlag;						//启动超时标志
	u8 WarnFlag;						//警告标志
	u8 OutTimeFlag;						//超时标志								
	u16 TestCnt;
	u16 TestCntMax;
}FT_OUTTIME_FLAG;

typedef struct
{
	u8 Version[6];								
	u16 IDindex;
}FT_ID_NAME;	

typedef struct
{
	u8 WorkStart;								
	u8 WorkDone;
	u8 WorkStatus;
}FT_WORK_FLAG;	

#pragma pack()

typedef u8 (*CommOrderTable)(UART_NUM);		   			//重命名函数指针
typedef u8 (*APPOrderTable)(UART_NUM);		   			//重命名函数指针
typedef enum {Ft_Start,Ft_Busy,Ft_End,Ft_OutCome}FT_TEST_STATUS;
void FtParseCmd(void);

u8 Fun_Cmd_FtParse(u8 cmd,UART_NUM USART_X);
u8 Fun_ReadFt_ID(UART_NUM USART_X);
u8 Fun_ReadFt_Status(UART_NUM USART_X);
u8 Fun_ReadFt_OutCome(UART_NUM USART_X);
u8 Fun_ReadFt_Test(u8 cmd,u8 flag);

void Enable_Ft_OutTime(UART_NUM uart_num);
void Disable_Ft_OutTime(UART_NUM uart_num);

void Test_UART1_AppUpdata(void);
void Test_UART3_AppUpdata(void);

u8 Fun_Cmd_AppUpdata(UART_NUM USART_X,u8* Err);
u8 APP_RepResetIAP(UART_NUM USART_X);

u8 APP_RepResetAPP(UART_NUM USART_X);
u8 APP_RepReadOutCome(UART_NUM USART_X);
u8 APP_RepReadStatus(UART_NUM USART_X);
u8 APP_RepSendCfg(UART_NUM USART_X);
u8 App_LoadCode(UART_NUM USART_X);

u8 APP_ReadStatus(UART_NUM USART_X);
u8 APP_SendPackage(u8* buf,UART_NUM USART_X,u32 len);
u8 APP_ReadStatus(UART_NUM USART_X);
u8 APP_ReadOutCome(UART_NUM USART_X);
u8 APP_ResetIAP(UART_NUM USART_X);
u8 APP_ResetAPP(UART_NUM USART_X);
u8 App_SendCode(UART_NUM USART_X,u32 All_Len);
u8 APP_SendCfg(UART_NUM USART_X);

u8 APP_Readback_Status(u8 Cmd,u8 status);
u8 APP_Readback_MulStatus(u8 Cmd,u8 status1,u8 status2);

void Enable_Ft_Work(UART_NUM uart_num);
void Disable_Ft_Work(UART_NUM uart_num);
void Set_Ft_Done(UART_NUM uart_num);
void Set_Ft_Status(UART_NUM uart_num,u8 status);
void Clear_Ft_Status(UART_NUM uart_num);
u8 Read_Ft_Status(UART_NUM uart_num);


u8 Check_Data(u8* buf,u32 len);
u32  Checksum(u8* buf, u32 len);
void MemCopy(u8 *pdest,const u8*src,u32 len);
u16 CalCheckSum(u8*buf,u32 len);
void ReloadChecksum(u8* buf,u32 len);
u8 APP_Null(void);

void UART_MemDealBack(uint8_t *dest, int32_t size);
void UART_MemDealHead(uint8_t *dest, int32_t size);
void UartX_Set_OutTime(void);
u8 UartX_Read_OutTime(UART_NUM USART_X);
u8 APP_Readback_MulData(u8 Cmd,u8*puf,u32 len);

extern USART_RX USART_RX_Table[];
extern volatile u8 App_Comm_Status;
extern FT_OUTTIME_FLAG	FtOutTimeFlag[];
extern FT_ID_NAME	FtIdName[];



#endif



