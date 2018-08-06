/******************** (C) COPYRIGHT 2009 STMicroelectronics ********************
* File Name          : stm32f10x_it.c
* Author             : MCD Application Team
* Version            : V3.0.1
* Date               : 04/27/2009
* Description        : Main Interrupt Service Routines.
*                      This file provides template for all exceptions handler
*                      and peripherals interrupt service routine.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"	
#include "ftcomm.h"
#include "wificomm.h"
#include "key.h"
#include "hw_Config.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/*******************************************************************************
* Function Name  : NMI_Handler
* Description    : This function handles NMI exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NMI_Handler(void)
{
}

/*******************************************************************************
* Function Name  : HardFault_Handler
* Description    : This function handles Hard Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : MemManage_Handler
* Description    : This function handles Memory Manage exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : BusFault_Handler
* Description    : This function handles Bus Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : UsageFault_Handler
* Description    : This function handles Usage Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : SVC_Handler
* Description    : This function handles SVCall exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SVC_Handler(void)
{
}

/*******************************************************************************
* Function Name  : DebugMon_Handler
* Description    : This function handles Debug Monitor exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DebugMon_Handler(void)
{
}

/*******************************************************************************
* Function Name  : PendSV_Handler
* Description    : This function handles PendSVC exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PendSV_Handler(void)
{
}

/*******************************************************************************
* Function Name  : SysTick_Handler
* Description    : This function handles SysTick Handler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*            STM32F10x Peripherals Interrupt Handlers                        */
/******************************************************************************/

/*******************************************************************************
* Function Name  : USB_LP_CAN1_RX0_IRQHandler
* Description    : This function handles USB Low Priority or CAN RX0 interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_LP_CAN1_RX0_IRQHandler(void)
{

}




/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/*******************************************************************************
* Function Name  : PPP_IRQHandler
* Description    : This function handles PPP interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM2_IRQHandler(void)
{	
	if(TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET){	
		TIM2_num++;
		Buzzer_Delay_Set();				//报警
		UartX_Set_OutTime();			//接收Ft板超时判断
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1 );
	}
}

/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : This function handles TIM3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM3_IRQHandler(void)
{

}



void DMA2_Channel4_5_IRQHandler(void)
{

}

/*******************************************************************************
* Function Name  : USART2_IRQHandler
* Description    : This function handles UART4 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART1_IRQHandler(void)  
{
	u16 static USART1_RX_Len=0;
	
	if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)        
	{
		   USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		   USART_RX_Table[0].data.USART_RX_BUF[USART1_RX_Len]=USART_ReceiveData(USART1);
		   USART1_RX_Len++;
	}
	else if(USART_GetFlagStatus(USART1,USART_FLAG_IDLE)!=RESET)
	{					
	   USART1->SR;
	   USART1->DR;	
	   USART_RX_Table[0].len=USART1_RX_Len;
	   USART_RX_Table[0].uartn=USART_1;
	   USART1_RX_Len=0;
	   FtParseCmd(); 
	}
}
/*******************************************************************************
* Function Name  : USART2_IRQHandler
* Description    : This function handles USART2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART2_IRQHandler(void)  
{
	u16 static USART2_RX_Len=0;
	if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)        
	{
		   USART_ClearITPendingBit(USART2,USART_IT_RXNE);
		   USART_RX_Table[0].data.USART_RX_BUF[USART2_RX_Len]=USART_ReceiveData(USART2);
		   USART2_RX_Len++;
	}
	else if(USART_GetFlagStatus(USART2,USART_FLAG_IDLE)!=RESET)
	{					
	   USART2->SR;
	   USART2->DR;	
	   USART_RX_Table[0].len=USART2_RX_Len;
	   USART_RX_Table[0].uartn=USART_2;
	   USART2_RX_Len=0;					
	   FtParseCmd(); 
	}
}

/*******************************************************************************
* Function Name  : USART3_IRQHandler
* Description    : This function handles USART3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART3_IRQHandler(void)  
{
	u16 static USART3_RX_Len=0;
	if(USART_GetITStatus(USART3,USART_IT_RXNE)!=RESET)        
	{
		   USART_ClearITPendingBit(USART3,USART_IT_RXNE);
		   USART_RX_Table[0].data.USART_RX_BUF[USART3_RX_Len]=USART_ReceiveData(USART3);
		   USART3_RX_Len++;
	}
	else if(USART_GetFlagStatus(USART3,USART_FLAG_IDLE)!=RESET)
	{					
	   USART3->SR;
	   USART3->DR;	
	   USART_RX_Table[0].len=USART3_RX_Len;
	   USART_RX_Table[0].uartn=USART_3;
	   USART3_RX_Len=0;					
	   FtParseCmd(); 
	}
}

/*******************************************************************************
* Function Name  : UART4_IRQHandler
* Description    : This function handles UART4 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART4_IRQHandler(void)  
{
	u16 static USART4_RX_Len=0;
	if(USART_GetITStatus(UART4,USART_IT_RXNE)!=RESET)        
	{
		   USART_ClearITPendingBit(UART4,USART_IT_RXNE);
		   USART_RX_Table[0].data.USART_RX_BUF[USART4_RX_Len]=USART_ReceiveData(UART4);
		   USART4_RX_Len++;
	}
	else if(USART_GetFlagStatus(UART4,USART_FLAG_IDLE)!=RESET)
	{					
	   UART4->SR;
	   UART4->DR;	
	   USART_RX_Table[0].len=USART4_RX_Len;
	   USART_RX_Table[0].uartn=UART_4;
	   USART4_RX_Len=0;					
	   FtParseCmd(); 
	}
}

/*******************************************************************************
* Function Name  : UART5_IRQHandler
* Description    : This function handles UART5 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART5_IRQHandler(void)  
{
	u16 static USART5_RX_Len=0;
	if(USART_GetITStatus(UART5,USART_IT_RXNE)!=RESET)        
	{
		   USART_ClearITPendingBit(UART5,USART_IT_RXNE);
		   USART_RX_Table[1].data.USART_RX_BUF[USART5_RX_Len]=USART_ReceiveData(UART5);
		   USART5_RX_Len++;
	}
	else if(USART_GetFlagStatus(UART5,USART_FLAG_IDLE)!=RESET)
	{					
	   UART5->SR;
	   UART5->DR;	
	   USART_RX_Table[1].len=USART5_RX_Len;
	   USART_RX_Table[1].uartn=UART_5;
	   USART5_RX_Len=0;					
	   WifiParseCmd(); 
	}
}
/*void PPP_IRQHandler(void)
{
}*/

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
