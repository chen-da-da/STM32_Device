#ifndef __ISP_USER_H
#define __ISP_USER_H
#include "stm32f10x.h"
#include "stm32f10x_flash.h"

#ifdef STM32F10X_HD
  #define PAGE_SIZE                         (0x800)
  #define FLASH_SIZE                        (0x80000) /* 512K */
#elif  define  STM32F10X_MD
  #define PAGE_SIZE                         (0x400)
  #define FLASH_SIZE                        (0x20000) /* 128K */
#endif 

#define WRITEID								//写入唯一的ID标号							
//#define IAPERAES							//测试FLASH
//#define IAPTEST							//测试IAP
//#define ROMTEST							//测试ROM读取
#define inpw(port)            (*((volatile unsigned long *)(port)))


#define RST_CORRECT   				0x77   //正确或者为空
#define RST_WRONG     				0x88   //错误或者非空



#define	CMD_IAP_FILE				0x80
#define	CMD_IAP_TRANS				0x81
#define	CMD_WRITER_ID				0x82
#define	CMD_CANCEL_FILE				0x83
#define	CMD_IAP_RESULT				0x84
#define	CMD_IAP_VERSION				0x85
#define CMD_IAP_MVERSION			0x86//修改IAP模式
#define CMD_IAP_TEST				0x87//测试模式

#define CMD_IAP_STATUS				0x88


typedef  void (*pFunction)(void);
#define ApplicationAddress 			0x08002800
#define IAP_ERASEAP_ADDRESS			(ApplicationAddress-(2*FMC_FLASH_PAGE_SIZE))	//写IAP擦除区域
#define IAP_VERSION_ADDRESS			IAP_ERASEAP_ADDRESS							//写IAP版本
#define FT_ID_ADDRESS				(ApplicationAddress-FMC_FLASH_PAGE_SIZE)
#define FT_ID_LENGTH				0x10			//前15Byte+CS


#define FMC_FLASH_PAGE_SIZE			0x800	
#define IAP_VERSION_SIZE			0x20		//32Byte
#define IAP_APP_OFFSET				0x04		//IAP_APP标志存放偏移长度
#define APP_SET						0x99887766
#define IAP_SET						0x44332211
#define APP_FLAG					0x55
#define IAP_FLAG					0xAA
#define ERR_FLAG					0x88



u32  Checksum(u8* buf, int len);
void Bootloader_App(void);
u32 FLASH_PagesMask(u32 Size);
void UART_MemDealBack(uint8_t *dest, int32_t size);
void UART_MemDealHead(uint8_t *dest, int32_t size);
void EraseForPage(u32 FlashDestination, u32 filelen);
void WriteData(u32 addr_start, u32 addr_end, u32 *data);
void ReadData(u32 addr_start, u32 addr_end, u32* data);
int ParseCmd(u8 *buffer, uint16_t len);
void Send_Handler(u16 BufferCnt);
void UART_MemDealBack(uint8_t *dest, int32_t size);
void UART_MemDealHead(uint8_t *dest, int32_t size);
void App_Iap_Flag(u8 AppIapflag);
u8 Read_Iap_Flag(void);
void Send_Status(void);


#endif

