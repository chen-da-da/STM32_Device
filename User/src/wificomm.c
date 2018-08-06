#include "wificomm.h"
#include "lcd.h"
#include "ftcomm.h"

extern u8 TCP_STA;
extern u8 IPString[16];
u8 Sendbuff[4096];		//���ͻ���

const CommOrderTable  WifiOrderTable[10]={
	Fun_Read_ID,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	CMD_ReadHandle
};


/*******************************************************************************
* Function Name  : int WifiParseCmd(u8 *buffer,UART_SEND_NUM uartn, u16 len)
* Description    : wifiͨѶ����
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void WifiParseCmd(void)
{
	u8 *pSrc,*response;
	u8	cmd,flag_status = 0;
	pSrc =USART_RX_Table[1].data.USART_RX_BUF;
	cmd = *pSrc;
	if(cmd == '+')
	{
		CMD_WiFiHandle(pSrc);
	}
	flag_status = Check_Data(pSrc, USART_RX_Table[1].len);		 //����������Ч���ж�
	Fun_Cmd_WifiParse(cmd, flag_status);		 //��������
	//return 0;
}

/*******************************************************************************
* Function Name  : void Fun_Cmd_WifiParse(u8 cmd,UART_SEND_NUM uartn,u8 flag)
* Description    : �������
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Fun_Cmd_WifiParse(u8 cmd,u8 flag)
{
	u8 TableNum;
	TableNum=cmd-CMD_Read_ID;
	WifiOrderTable[TableNum](UART_5);
}

/*******************************************************************************
* Function Name  : void FT_Work_Dispaly(void)
* Description    : ��ʾ��ǰFT��Ϣ
* Input          : None.
* Return         : 
*******************************************************************************/
void FT_Work_Dispaly(void)
{
	u32 static PassCnt=0;
	
	if((Read_Ft_Status(USART_3))==FT_TEST_DONE_OK)
	{
		PassCnt++;
		Clear_Screen(1,9,256,4);
		Display_String_GB2312(1,9,"״̬:",0);
		Clear_Ft_Status(USART_3);
		Display_String_GB2312(49,9,"����",0);
		Display_String_GB2312(1,11,"��Ʒ��:",0);
		Display_8x16_Data(65,11,PassCnt,Dec,0);
	}
	else if((Read_Ft_Status(USART_3))>FT_TEST_DONE_OK)
	{
		Clear_Screen(1,9,256,4);
		Display_String_GB2312(1,9,"״̬:",0);
		Clear_Ft_Status(USART_3);
		Display_String_GB2312(33,9,"�쳣",0);
	}	
}

/*******************************************************************************
* Function Name  : void FT_Work_Dispaly(void)
* Description    : ��ʾ��ǰFT��Ϣ
* Input          : None.
* Return         : 
*******************************************************************************/
void FT_Info_Dispaly(void)
{
	Clear_Screen(1,1,256,12);
	Dispaly_8x16_string(1,1,"SC93F8433:40000PCS",0);//��ʾIC�ͺ�,IC����
	Dispaly_8x16_string(1,3,"28152030-Q-1829",0);//��ʾIC���Σ���������
	Dispaly_8x16_string(1,5,"SC92F7320-V2.3-V2.3",0);//���Գ�������
	Dispaly_8x16_string(1,7,"CRC0X4D88A29E-20180316",0);
}

/*******************************************************************************
* Function Name  : void Dispaly_Ft_ID(UART_NUM USART_X)
* Description    : ��ʾ��ǰFT��ID�����δ�����ɹ�����ʾNUll
* Input          : None.
* Return         : ��ʾ��ʽ:UARTN _FT_ID:XXX;��ʾ�ɹ��Ӻڱ���
*******************************************************************************/
u8 Read_Current_FtID(void)
{
	u8 CycId,IdFlag=0;
	for(CycId=0;CycId<4;CycId++)
	{
		IdFlag|=(Dispaly_Ft_ID((UART_NUM)CycId))<<CycId;
	}
	return IdFlag;
}
/*******************************************************************************
* Function Name  : void Dispaly_Ft_ID(UART_NUM USART_X)
* Description    : ��ʾ��ǰFT��ID�����δ�����ɹ�����ʾNUll
* Input          : None.
* Return         : ��ʾ��ʽ:UARTN _FT_ID:XXX;��ʾ�ɹ��Ӻڱ���
*******************************************************************************/
u8 Dispaly_Ft_ID(UART_NUM USART_X)
{
	u8 *disptr,flagId=0;
	u8 FTIDStringTable[12]={0x55,0x41,0x52,0x54,0x32,0x5f,0x46,0x54,0x5f,0x49,0x44,0x3a};	
	if(APP_RepReadID(USART_X)==1)			//�����ȡ�ɹ�����ʾ����
	{
		flagId=1;
		disptr=&FtIdName[USART_X].Version[0];
		FTIDStringTable[4]=0x30|(USART_X+1);
		Clear_Screen(1,((USART_X<<1)+1),256,2);
		Dispaly_8x16_string(1,((USART_X<<1)+1),FTIDStringTable,0);
		Display_String_Data_8x16(97,((USART_X<<1)+1),disptr,FtIdName[USART_X].IDindex,Dec,1);
	}
	else
	{
		Clear_Screen(1,((USART_X<<1)+1),256,2);
		FTIDStringTable[4]=0x30|(USART_X+1);
		Dispaly_8x16_string(1,((USART_X<<1)+1),FTIDStringTable,0);
		Dispaly_8x16_string(97,((USART_X<<1)+1),"NULL",0);
	}
	return flagId;
}
/*******************************************************************************
* Function Name  : u8 APP_RepReadStatus(void)
* Description    : ���ͨѶ
* Input          : None.
* Return         : None
*******************************************************************************/
u8 APP_RepReadID(UART_NUM USART_X)
{
	u8 flag=1,i,RepCnt=3;
	for(i=0;i<RepCnt;i++)
	{
		if(Fun_Read_ID(USART_X)==0x01)	break;
	}
	if(i==RepCnt)	flag=0;
	return flag;
}

/*******************************************************************************
* Function Name  : void Fun_ReadFt_ID(u8 cmd)
* Description    : ��ȡFT ID:Cmd+(8Byte+Cs(8Byte))+Cs(ͨѶ)=13Byte
* Input          : None.  V X . X _ 0 (16Bit���)
* Output         : None.
* Return         : None.  �����С��ģʽ���洢�����㣬���Ǵ�Buffer��ȡ�����
*******************************************************************************/
u8 Fun_Read_ID(UART_NUM USART_X )
{
	u8* ftPtr;
	u8 buf[3],flag=0;
	u8 Ft_ID_Buf[16]={0};
	ftPtr=&FtIdName[USART_X].Version[0];
	buf[0]=CMD_FT_ID;
	buf[1]=0x00;
	buf[2]=CMD_FT_ID;
	USARTX_SendByte(USART_X,buf,3);
	if(APP_Readback_MulData(CMD_FT_ID,Ft_ID_Buf,13)==1)
	{
		if(Check_Data(&Ft_ID_Buf[1],10)==1)
		{
			MemCopy(ftPtr, &Ft_ID_Buf[1], 8);
			flag=1;
		}
	}
	return flag;
}

/**
**WiFi֡������
**/
void CMD_WiFiHandle(u8 *pBuff)
{
	u8 i;
	 //��ȡIP
    if((pBuff[4] == 'D'&&pBuff[5] == 'H'))
      {
		//�ӽ��ջ���ȡ��IP��ַ
        do
         {
            IPString[i] = pBuff[i+9];
             i++;
         } while(pBuff[i+9] != 0x2c);
		 //Dispaly_8x16_string(1,10,"Connect",0);
       }
     //�ж�WiFi����״̬
     else if(pBuff[4] == 'o')
        {
			//WiFi������
            if(pBuff[5] == 'n')
            {
				//Dispaly_8x16_string(1,10,"Connect",0);
				TCP_STA = 1;	//��TCP���ӱ�־��1
            }
			//WiFiδ����
            else
            {
				//Dispaly_8x16_string(1,10,"Disconnect",0);
				TCP_STA = 0;	//��TCP���ӱ�־��0
            }
        }
}

/**
**����������
**/
u8 CMD_ReadHandle(UART_NUM USART_X)
{
	u8 CMD_Type;
	//CMD_Type = pBuff[1];
	 //�ж�У����Ƿ���ȷ
        //if(Check_Data(pBuff,len))
       // {
			CMD_ReadDataRight(CMD_Type);
       // }
		//У�����
       // else
       // {
			//CMD_ReadDataError();
        //}
	return 0;
}

/**
**����������ȷ����
**/
void CMD_ReadDataRight(u8 CMD_Type)
{
	//switch(CMD_Type)
	//{
		//ֻ�����ʻظ�
		//case TYPE_SET:
			SDCount("0:/BANK2/FLAG.TXT");			//ͳ����Ʒ������Ʒ����
			Framing();			//��ɷ���֡��������
			//break;
		
		//�յ���λ��������ȷ����
		//case TYPE_REC_RIGHT:
			//break;
		
		//�յ���λ�����մ�����
		//case TYPE_REC_ERROR:
			//CMD_ReadDataError();
			//break;
	//}
}

/**
**��SD������
**
**/
void SDCount(const TCHAR *path)
{
	u8 res;
	u16 i,j;
	FIL fil;
	UINT bww;
	u32 data_len,slice,last;
	u8 SDbuff[4096*2];
	PASS Pass;		//PASS���������
	NG Ng;			//NG���������
	NULL_ Null_;	//NULL���������
	
	res = f_open(&fil,path, FA_OPEN_EXISTING |FA_READ);
	data_len = f_size(&fil);		//��ȡSD�����ݳ���

	Pass.Passnum = Ng.NGnum = Null_.NUII_num= 0;
	
	slice = data_len/4096;		//��÷�Ƭ��
	last = data_len%4096;
	if(slice >0)
	{
		for(i = 0;i < slice; i++)
		{
			res = f_read(&fil,SDbuff,4096,&bww);  //��ȡSD��4096byte������
			for(j = 0; j < 4096; j++)
			{
				if(SDbuff[j] == 0x55) Pass.Passnum++;
				else if(SDbuff[j] == 0xaa) Ng.NGnum++;
				else if(SDbuff[j] == 0x88) Null_.NUII_num++;
			}
		}
	}
	res = f_read(&fil,SDbuff,last,&bww);
	res = f_close(&fil);
	for(i = 0;i < last;i++)
	{
		if(SDbuff[i] == 0x55) Pass.Passnum++;
		else if(SDbuff[i] == 0xaa) Ng.NGnum++;
		else if(SDbuff[i] == 0x88) Null_.NUII_num++;
	}
	
}

/**
**��֡����
**/
void Framing()
{
	Sendbuff[0] = CMD_READ;	
	//Sendbuff[1] = TYPE_SET;
	Sendbuff[2] = 0x00;
	Sendbuff[3] = 0x00;
	Sendbuff[4] = 0x4c;
	Sendbuff[5] = 0x00;
	Sendbuff[6] = 0x55;
	Sendbuff[7] = 0x22;
	//Sendbuff[8] = Get_FTMifstate();
	//Sendbuff[9] = Get_FTCount();
	//.....
	//Add_Scheck(Sendbuff,74);		//��У���
	ReloadChecksum(Sendbuff,76);
	//Usart_SendStr_length(UART5, Sendbuff, 76);
	USARTX_SendByte(UART_5,Sendbuff,76);
}

