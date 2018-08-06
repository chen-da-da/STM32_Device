#include "isp_user.h"
#include "string.h"
//#include "hwconfig.h"
u8 UartSendBuffer[256];
pFunction Jump_To_Application;
uint32_t JumpAddress;

uint32_t g_apromSize, g_apromAddr,g_apromCs,g_dataFlashAddr, g_dataFlashSize;
/*******************************************************************************
* Function Name  : Bootloader_App
* Description    : ��תAPP����
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Send_Status(void)
{
	u8 Status=0;
	Status=Read_Iap_Flag();
	memset(UartSendBuffer,CMD_IAP_STATUS,2);				//��DMA����
	UartSendBuffer[0]=CMD_IAP_STATUS;
	if(Status==IAP_FLAG)			UartSendBuffer[1]=IAP_FLAG;
	else if(Status==APP_FLAG)		UartSendBuffer[1]=APP_FLAG;
	else							UartSendBuffer[1]=ERR_FLAG;
	Send_Handler(2);
//    Delay_MS(10);
	DMA_Cmd(DMA1_Channel4, DISABLE); //��ֹ��û�����ļ��رգ��ͽ�ȥ��APP

}
/*******************************************************************************
* Function Name  : Bootloader_App
* Description    : ��תAPP����
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Bootloader_App(void)
{
	if(Read_Iap_Flag()==APP_FLAG){
		 if (((*(__IO uint32_t*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000)
	      { /* Jump to user application */
	         JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 0x4);
	         Jump_To_Application = (pFunction) JumpAddress;
	          /* Initialize user application's Stack Pointer */
	         __set_MSP(*(__IO uint32_t*) ApplicationAddress);
	         Jump_To_Application();
	      }
	}	
}
/*******************************************************************************
* Function Name  : int ParseCmd(u8 *buffer, uint16_t len)
* Description    : ����APP����
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int ParseCmd(u8 *buffer, uint16_t len)
{
	static uint32_t StartAddress=ApplicationAddress,rcksum;
	uint8_t *pSrc,*response;
	uint8_t	lcmd;
	pSrc =buffer;
	lcmd =buffer[0]; //inpw(pSrc);
	UART_MemDealHead(buffer,(len-1));						
	memset(UartSendBuffer, 0, len);
	if(lcmd==CMD_WRITER_ID){
		
		UartSendBuffer[0]=lcmd;
		UartSendBuffer[1]=0;
		Send_Handler(2);	
	}
	else if(lcmd==CMD_IAP_VERSION){
		response = UartSendBuffer;
		ReadData(IAP_VERSION_ADDRESS,IAP_VERSION_ADDRESS+32,(uint32_t*)(response));
		UART_MemDealBack(response,(len-1));	
		UartSendBuffer[0]=lcmd;
		Send_Handler(32);	
	}
	else if(lcmd==CMD_IAP_FILE){
		g_apromSize=*(uint32_t*)(pSrc);//��ȡ�´��ĳ���
		g_apromAddr=*(uint32_t*)(pSrc+4);//��ȡ�´���ʼ��ַ
		g_apromCs=*(uint32_t*)(pSrc+8);//��ȡChecksumֵ
		EraseForPage(ApplicationAddress, g_apromSize);
		StartAddress=ApplicationAddress;
		rcksum=0;
	}
	else if(lcmd==CMD_IAP_TRANS){	
		response = UartSendBuffer;
		if(g_apromSize<(len-1)){
		WriteData(StartAddress, StartAddress + g_apromSize, (uint32_t*)(pSrc));
		ReadData(StartAddress,StartAddress+g_apromSize,(uint32_t*)(response));
		rcksum+=Checksum(UartSendBuffer, g_apromSize);
		}
		else{
		WriteData(StartAddress, StartAddress +(len-1), (uint32_t*)(pSrc));
		ReadData(StartAddress,StartAddress+(len-1),(uint32_t*)(response));
		rcksum+=Checksum(UartSendBuffer, (len-1));
		g_apromSize-=(len-1);
		}
		StartAddress+=(len-1);
	}
	else if(lcmd==CMD_CANCEL_FILE){
		u32 OlderCs=0,NewCs=0;
		response = UartSendBuffer;
		ReadData(IAP_ERASEAP_ADDRESS,IAP_ERASEAP_ADDRESS + IAP_VERSION_SIZE,(uint32_t*)(response));
		EraseForPage(IAP_ERASEAP_ADDRESS, FMC_FLASH_PAGE_SIZE);
		App_Iap_Flag(APP_FLAG);														//תAPP
		OlderCs+=Checksum(UartSendBuffer, IAP_VERSION_SIZE);						
		WriteData(IAP_ERASEAP_ADDRESS, IAP_ERASEAP_ADDRESS + IAP_VERSION_SIZE, (uint32_t*)(response));
		ReadData(IAP_ERASEAP_ADDRESS,IAP_ERASEAP_ADDRESS + IAP_VERSION_SIZE,(uint32_t*)(response));
		NewCs+=Checksum(UartSendBuffer, IAP_VERSION_SIZE);
		if(OlderCs==NewCs){															//���±�־����ɹ�
			UartSendBuffer[0]=lcmd;
			UartSendBuffer[1]=RST_CORRECT;
			Send_Handler(2);
			FLASH_Lock();
//			SoftReset();									//�����λ
		}else{
			UartSendBuffer[0]=lcmd;
			UartSendBuffer[1]=RST_WRONG;
			Send_Handler(2);
		}	
	}
	else if(lcmd==CMD_IAP_RESULT){
		UartSendBuffer[0]=lcmd;
		UartSendBuffer[1]=(rcksum==g_apromCs)?RST_CORRECT:RST_WRONG;	
		Send_Handler(2);
	}
	else if(lcmd==CMD_IAP_MVERSION){
		EraseForPage(IAP_VERSION_ADDRESS, FMC_FLASH_PAGE_SIZE);
		WriteData(IAP_VERSION_ADDRESS, IAP_VERSION_SIZE + 32, (uint32_t*)(pSrc));
	}
	else if(lcmd==CMD_IAP_TEST){
		u8 ID_CS=0;
		#ifdef WRITEID
		ID_CS+=Checksum(pSrc, (FT_ID_LENGTH-1));
		*(pSrc+(FT_ID_LENGTH-1))=ID_CS;
		EraseForPage(FT_ID_ADDRESS, FMC_FLASH_PAGE_SIZE);
		WriteData(FT_ID_ADDRESS, FT_ID_ADDRESS + FT_ID_LENGTH, (uint32_t*)(pSrc));
		response = UartSendBuffer;
		ReadData(FT_ID_ADDRESS,FT_ID_ADDRESS + FT_ID_LENGTH,(uint32_t*)(response));
		Send_Handler(FT_ID_LENGTH);
		#endif
		
		#ifdef ROMTEST
		StartAddress=ApplicationAddress;
		response = UartSendBuffer;
		ReadData(StartAddress,StartAddress+(len-1),(uint32_t*)(response));
		rcksum+=Checksum(UartSendBuffer, (len-1));
		Send_Handler(len-1);	
		#endif
		#ifdef IAPTEST
		u32 OlderCs=0,NewCs=0;
		response = UartSendBuffer;
		ReadData(IAP_ERASEAP_ADDRESS,IAP_ERASEAP_ADDRESS + IAP_VERSION_SIZE,(uint32_t*)(response));
		EraseForPage(IAP_ERASEAP_ADDRESS, FMC_FLASH_PAGE_SIZE);
		App_Iap_Flag(IAP_FLAG);														//תAPP
		OlderCs+=Checksum(UartSendBuffer, IAP_VERSION_SIZE);						
		WriteData(IAP_ERASEAP_ADDRESS, IAP_ERASEAP_ADDRESS + IAP_VERSION_SIZE, (uint32_t*)(response));
		ReadData(IAP_ERASEAP_ADDRESS,IAP_ERASEAP_ADDRESS + IAP_VERSION_SIZE,(uint32_t*)(response));
		NewCs+=Checksum(UartSendBuffer, IAP_VERSION_SIZE);
		if(OlderCs==NewCs){
			Send_Handler(IAP_VERSION_SIZE);
		}
		#endif

		#ifdef IAPERAES
		u32 OlderCs=0,NewCs=0,NewCs1=0,BackCs1=0,BackCs2=0;;
		response = UartSendBuffer;
		ReadData(IAP_ERASEAP_ADDRESS,IAP_ERASEAP_ADDRESS + IAP_VERSION_SIZE,(uint32_t*)(response));
		//�Ȳ���IAP_ERASEAP_ADDRESS���һҳ
		EraseForPage(0x08002000, FMC_FLASH_PAGE_SIZE);
		App_Iap_Flag(IAP_FLAG);		
		WriteData(0x08002000, 0x08002000 + IAP_VERSION_SIZE, (uint32_t*)(response));
		ReadData(0x08002000,0x08002000 + IAP_VERSION_SIZE,(uint32_t*)(response));
		BackCs1+=Checksum(UartSendBuffer, IAP_VERSION_SIZE);
		//�ٲ�����ǰҳ��ȷ���Ƿ��Ǵӵ�ǰҳ�ĵ�ַ��ʼ�������ҳ�����ַ
		EraseForPage(IAP_ERASEAP_ADDRESS, FMC_FLASH_PAGE_SIZE);		//������ҳ
		App_Iap_Flag(IAP_FLAG);														//תAPP
		OlderCs+=Checksum(UartSendBuffer, IAP_VERSION_SIZE);						
		WriteData(IAP_ERASEAP_ADDRESS, IAP_ERASEAP_ADDRESS + IAP_VERSION_SIZE, (uint32_t*)(response));
		ReadData(IAP_ERASEAP_ADDRESS,IAP_ERASEAP_ADDRESS + IAP_VERSION_SIZE,(uint32_t*)(response));
		NewCs+=Checksum(UartSendBuffer, IAP_VERSION_SIZE);
		//��ȡ��һҳ��д��ֵ���ж��Ƿ��Ǵ���ʼ��ַ��ʼ������2K
		ReadData(0x08002000,0x08002000 + IAP_VERSION_SIZE,(uint32_t*)(response));
		BackCs2+=Checksum(UartSendBuffer, IAP_VERSION_SIZE);
		//��ʼ��ַ����0x200��������ǰҳ���ٶ�ȡ
		EraseForPage(IAP_ERASEAP_ADDRESS+0x200, FMC_FLASH_PAGE_SIZE);
		ReadData(IAP_ERASEAP_ADDRESS,IAP_ERASEAP_ADDRESS + IAP_VERSION_SIZE,(uint32_t*)(response));
		NewCs1+=Checksum(UartSendBuffer, IAP_VERSION_SIZE);
		
		//ҳ����Ԥ�е�ǰ��������ʼ��ַ����һҳ��������ҳ������
		//���ԣ���������������Ļ�������Ӳ������
		//������IAP�ı�־�͵���ռ��1ҳ2K�Ŀռ䡣
		if(OlderCs==NewCs){
			Send_Handler(IAP_VERSION_SIZE);
		}
		#endif
		
	}
    return 0;
}

/*******************************************************************************
* Function Name  : void EraseForFile(size_t filelen)
* Description    : Ϊд�ļ������ռ�
* Input          : �ļ��ĳ���
* Return         : None.
*******************************************************************************/
void EraseForPage(u32 FlashDestination, u32 filelen)
 {
     //u32 FlashDestination = (u32)ApplicationAddress; 
     u32 EraseCounter = 0x00,NbrOfPage = 0,PageSize=FMC_FLASH_PAGE_SIZE;
     FLASH_Status FLASHStatus = FLASH_COMPLETE;
	 FLASH_Unlock();				//ȡ��flash����
     NbrOfPage = FLASH_PagesMask(filelen);
     for (EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
     {
           FLASHStatus = FLASH_ErasePage(FlashDestination + (PageSize * EraseCounter));
     }
	 FLASHStatus = FLASH_ErasePage((u32)IAP_VERSION_ADDRESS);	  //�����汾�ŷ���
	 while(FLASHStatus != FLASH_COMPLETE);//�ȴ����
 }

/*******************************************************************************
* Function Name  : void ReadData(u32 addr_start, u32 addr_end, u32* data)
* Description    : Read data from flash
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void ReadData(u32 addr_start, u32 addr_end, u32* data)    
{
    u32 rLoop;

    for ( rLoop = addr_start; rLoop < addr_end; rLoop += 4 ) {
        *data = inpw(rLoop);
         data++;
    }
    return;
}

/*******************************************************************************
* Function Name  : void WriteData(u32 addr_start, u32 addr_end, u32 *data)
* Description    : Write data into flash
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void WriteData(u32 addr_start, u32 addr_end, u32 *data)  
{
    unsigned int wLoop;

    for ( wLoop = addr_start; wLoop < addr_end; wLoop+=4 ) {
        FLASH_ProgramWord(wLoop, *data);
        data++;
    }

}
/*******************************************************************************
* Function Name  : void UART_MemDealBack(uint8_t *dest, int32_t size)
* Description    : ��������
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void UART_MemDealBack(uint8_t *dest, int32_t size)
{
    while(size--) dest[size+1]=dest[size];
}
/*******************************************************************************
* Function Name  : void UART_MemDealHead(uint8_t *dest, int32_t size)
* Description    : ��������
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void UART_MemDealHead(uint8_t *dest, int32_t size)
{
    while(size--) *dest++ = *(dest+1);
}

/*******************************************************************************
* Function Name  : u32 FLASH_PagesMask(u32 Size)
* Description    : ����flash��ҳ��
* Input          : �ļ��ĳ���
* Return         : None.
*******************************************************************************/
u32 FLASH_PagesMask(u32 Size)
{
  u32 PageNum = 0x0;
  u32 size = Size;
  if ((size % PAGE_SIZE) != 0)		PageNum = (size / PAGE_SIZE) + 1;
  else								PageNum = size / PAGE_SIZE;
  return PageNum;
}

u8 Check_Data(u8* buf,int len)
{
	u8 flag=0;							//�����
	u32 Cs=0,Cstemp=0;
	Cs=Checksum(buf,len);				//���㵽֡��CSֵ
	Cstemp=(u16)buf[len-2]<<8;
	Cstemp|=buf[len-1];
	if((u16)Cs==Cstemp)		flag=1;

	return flag;
}
/*******************************************************************************
* Function Name  : Set_System
* Description    : Configures Main system clocks & power.
* Input          : None.
* Return         : None.
*******************************************************************************/
u32  Checksum(u8* buf, int len)
{
    int i;
    u32 cs;

    for(cs = 0 , i = 0 ; i < len-2; i++) {
        cs += buf[i];
    }
    return (cs);
}


//u16 CalCheckSum(u32 start, u32 len)
//{
//    int i;
//    u32 lcksum = 0;
//
//    for(i = 0; i < len; i+=FMC_FLASH_PAGE_SIZE) {
//        ReadData(start + i, start + i + FMC_FLASH_PAGE_SIZE, (uint32_t*)aprom_buf);
//        if(len - i >= FMC_FLASH_PAGE_SIZE)
//            lcksum += Checksum(aprom_buf, FMC_FLASH_PAGE_SIZE);
//        else
//            lcksum += Checksum(aprom_buf, len - i);
//    }
//
//    return lcksum;
//}  


/*******************************************************************************
* Function Name  :void Send_Handler(u16 BufferCnt)
* Description    : �ϴ�����
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Send_Handler(u16 BufferCnt)
{
	USART_ITConfig(USART1, USART_IT_TC, ENABLE);
	DMA_Cmd(DMA1_Channel4, DISABLE);						//�رմ򿪷���
    DMA1_Channel4->CNDTR=BufferCnt;//BUFFER_SIZE;						//DMA���ط��ͳ�ʼ����
	DMA_Cmd(DMA1_Channel4,ENABLE);
	

}

/*******************************************************************************
* Function Name  : void App_Iap_Flag(u8 AppIapflag)
* Description    : APP_IAP��־����
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/

void App_Iap_Flag(u8 AppIapflag)
{
	u8 *ptr=UartSendBuffer;
	if(AppIapflag==APP_FLAG)			*(u32*)ptr=APP_SET;
	else if(AppIapflag==IAP_FLAG)		*(u32*)ptr=IAP_SET;
}
/*******************************************************************************
* Function Name  : u8 Read_Iap_Flag(void)
* Description    : ��ȡIAP״̬
* Input          : None.
* Output         : None.
* Return         : APP״̬��IAP ״̬������״̬
*******************************************************************************/
u8 Read_Iap_Flag(void)
{
	u32 Iap_App;
	u8 status=0;
	ReadData(IAP_ERASEAP_ADDRESS,IAP_ERASEAP_ADDRESS + IAP_APP_OFFSET,&Iap_App);
	if(Iap_App==APP_SET)			status=APP_FLAG;
	else if(Iap_App==IAP_SET)		status=IAP_FLAG;
	else		status=0;

	return status;
}



