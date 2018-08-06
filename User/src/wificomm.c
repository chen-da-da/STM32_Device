#include "wificomm.h"
#include "lcd.h"
#include "ftcomm.h"

extern u8 TCP_STA;
extern u8 IPString[16];
u8 Sendbuff[4096];		//发送缓存

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
* Description    : wifi通讯命令
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
	flag_status = Check_Data(pSrc, USART_RX_Table[1].len);		 //接收数据有效性判断
	Fun_Cmd_WifiParse(cmd, flag_status);		 //处理数据
	//return 0;
}

/*******************************************************************************
* Function Name  : void Fun_Cmd_WifiParse(u8 cmd,UART_SEND_NUM uartn,u8 flag)
* Description    : 命令处理函数
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
* Description    : 显示当前FT信息
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
		Display_String_GB2312(1,9,"状态:",0);
		Clear_Ft_Status(USART_3);
		Display_String_GB2312(49,9,"正常",0);
		Display_String_GB2312(1,11,"良品数:",0);
		Display_8x16_Data(65,11,PassCnt,Dec,0);
	}
	else if((Read_Ft_Status(USART_3))>FT_TEST_DONE_OK)
	{
		Clear_Screen(1,9,256,4);
		Display_String_GB2312(1,9,"状态:",0);
		Clear_Ft_Status(USART_3);
		Display_String_GB2312(33,9,"异常",0);
	}	
}

/*******************************************************************************
* Function Name  : void FT_Work_Dispaly(void)
* Description    : 显示当前FT信息
* Input          : None.
* Return         : 
*******************************************************************************/
void FT_Info_Dispaly(void)
{
	Clear_Screen(1,1,256,12);
	Dispaly_8x16_string(1,1,"SC93F8433:40000PCS",0);//显示IC型号,IC总数
	Dispaly_8x16_string(1,3,"28152030-Q-1829",0);//显示IC批次，生产日期
	Dispaly_8x16_string(1,5,"SC92F7320-V2.3-V2.3",0);//测试程序名称
	Dispaly_8x16_string(1,7,"CRC0X4D88A29E-20180316",0);
}

/*******************************************************************************
* Function Name  : void Dispaly_Ft_ID(UART_NUM USART_X)
* Description    : 显示当前FT的ID，如果未读出成功，显示NUll
* Input          : None.
* Return         : 显示格式:UARTN _FT_ID:XXX;显示成功加黑背景
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
* Description    : 显示当前FT的ID，如果未读出成功，显示NUll
* Input          : None.
* Return         : 显示格式:UARTN _FT_ID:XXX;显示成功加黑背景
*******************************************************************************/
u8 Dispaly_Ft_ID(UART_NUM USART_X)
{
	u8 *disptr,flagId=0;
	u8 FTIDStringTable[12]={0x55,0x41,0x52,0x54,0x32,0x5f,0x46,0x54,0x5f,0x49,0x44,0x3a};	
	if(APP_RepReadID(USART_X)==1)			//如果读取成功，显示出来
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
* Description    : 多次通讯
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
* Description    : 读取FT ID:Cmd+(8Byte+Cs(8Byte))+Cs(通讯)=13Byte
* Input          : None.  V X . X _ 0 (16Bit编号)
* Output         : None.
* Return         : None.  编号是小端模式，存储不方便，还是从Buffer读取会更好
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
**WiFi帧处理函数
**/
void CMD_WiFiHandle(u8 *pBuff)
{
	u8 i;
	 //获取IP
    if((pBuff[4] == 'D'&&pBuff[5] == 'H'))
      {
		//从接收缓存取出IP地址
        do
         {
            IPString[i] = pBuff[i+9];
             i++;
         } while(pBuff[i+9] != 0x2c);
		 //Dispaly_8x16_string(1,10,"Connect",0);
       }
     //判断WiFi连接状态
     else if(pBuff[4] == 'o')
        {
			//WiFi已连接
            if(pBuff[5] == 'n')
            {
				//Dispaly_8x16_string(1,10,"Connect",0);
				TCP_STA = 1;	//将TCP连接标志置1
            }
			//WiFi未连接
            else
            {
				//Dispaly_8x16_string(1,10,"Disconnect",0);
				TCP_STA = 0;	//将TCP连接标志置0
            }
        }
}

/**
**处理读命令函数
**/
u8 CMD_ReadHandle(UART_NUM USART_X)
{
	u8 CMD_Type;
	//CMD_Type = pBuff[1];
	 //判断校验和是否正确
        //if(Check_Data(pBuff,len))
       // {
			CMD_ReadDataRight(CMD_Type);
       // }
		//校验错误
       // else
       // {
			//CMD_ReadDataError();
        //}
	return 0;
}

/**
**接收数据正确处理
**/
void CMD_ReadDataRight(u8 CMD_Type)
{
	//switch(CMD_Type)
	//{
		//只读良率回复
		//case TYPE_SET:
			SDCount("0:/BANK2/FLAG.TXT");			//统计良品、不良品数量
			Framing();			//组成发送帧，并发送
			//break;
		
		//收到上位机接收正确反馈
		//case TYPE_REC_RIGHT:
			//break;
		
		//收到上位机接收错误反馈
		//case TYPE_REC_ERROR:
			//CMD_ReadDataError();
			//break;
	//}
}

/**
**读SD卡函数
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
	PASS Pass;		//PASS联合体变量
	NG Ng;			//NG联合体变量
	NULL_ Null_;	//NULL联合体变量
	
	res = f_open(&fil,path, FA_OPEN_EXISTING |FA_READ);
	data_len = f_size(&fil);		//获取SD卡数据长度

	Pass.Passnum = Ng.NGnum = Null_.NUII_num= 0;
	
	slice = data_len/4096;		//获得分片数
	last = data_len%4096;
	if(slice >0)
	{
		for(i = 0;i < slice; i++)
		{
			res = f_read(&fil,SDbuff,4096,&bww);  //读取SD卡4096byte到缓存
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
**组帧函数
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
	//Add_Scheck(Sendbuff,74);		//加校验和
	ReloadChecksum(Sendbuff,76);
	//Usart_SendStr_length(UART5, Sendbuff, 76);
	USARTX_SendByte(UART_5,Sendbuff,76);
}

