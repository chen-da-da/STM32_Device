/******************** (C) COPYRIGHT 2009 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team
* Version            : V3.0.1
* Date               : 04/27/2009
* Description        : Custom HID demo main file
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "prt_usart1.h"	
#include "ff.h"
#include "fft_test.h"
#include "lcd.h"
#include "hw_config.h"
#include "ftcomm.h"
#include "key.h"
#include "wifi.h"
//7.24  UART口硬件RX接10K上拉，否则会误进入串口中断
//		Malloc的空间修改为最大申请4K
//7.25  堆栈空间扩展为0x400，申请的大数组时 否则临时变量会修改全局变量。
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
//	Status = SD_Init();
//	if(Status==0)
//	{
//		Sector=SD_GetSectorCount_8G();
//		MAXnM=Sector>>11;			//当前的MB 
//		Delay_MS(1);
//	}

//	LCD_init();
//	Fm_CreatFile((u8*)filepath[BANK0_FLAG],(u8*)path[BANK0_FLAG]);
//	Fm_scanFile((u8*)filepath[BANK0_FLAG]);
//	if(Fm_DeleteFile((u8*)path[BANK0_FLAG])==0)
//	{
//		 Fm_CreatFile((u8*)filepath[BANK0_FLAG],(u8*)path[BANK0_FLAG]);
//	}
//	if(Fm_CreatAllFile()==0)
//	{
//		printf("r\n操作成功\r\n");
//	}

//	printf("r\n操作成功\r\n");
//	Fm_AddData((u8*)path[BANK0_FLAG]);
//	if(Fm_Readfile((u8*)path[BANK0_FLAG],0,256)==FR_OK)
//	{
//		BufferAdd(SectorBuffer,256);
//		APP_SendPackage(SectorBuffer,256);
//	//	printf("r\n读取成功\r\n");
////	#if CFG_PRINTF
//	//	printf("Cs:%d\r\n",BufferAdd(SectorBuffer,128));
////		#endif
//	}
//	Sector=ReadFileChecksum((u8*)path[PUBANK_FTCFG]);
//	Test_UART1_AppUpdata();
//	Sector=ReadFileChecksum((u8*)path[PUBANK_FTAPP]);

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : main.
* Description    : main routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int main(void){
	Set_System();
//	Fm_CreatAllFile();	//创建5个文件夹
	
	while (1)
	{	
		Key_Test();
		ErrOut();
	}		   
}

/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while(1)
  {
  }
}

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
