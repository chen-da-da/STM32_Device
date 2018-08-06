#include "ftcomm.h"
#include "malloc.h"
#include "fft_test.h"
#include "lcd.h"

//接收缓存
USART_RX USART_RX_Table[2];	//USART_RX_Table[0]表示FT接收,USART_RX_Table[1]表示Wifi接收
FT_OUTTIME_FLAG	FtOutTimeFlag[4];//USART1~USART4的超时机制;
FT_ID_NAME		FtIdName[4];
FT_WORK_FLAG	FtWorkFlag[4];
volatile u8 App_Comm_Status;		//- - -- -- ok/ng over/work   // ---- --  1/0 1/0
const char UpdataPassInfo[6][17]={
"ReadStatus: PASS",\
"ResetIap:   PASS",\
"SendCfg:    PASS",\
"LoadCode:   PASS",\
"UpdataCode: PASS",\
"ResetApp:   PASS"
};
const char UpdataErrInfo[6][17]={
"ReadStatus: ERR",\
"ResetIap:   ERR",\
"SendCfg:    ERR",\
"LoadCode:   ERR",\
"UpdataCode: ERR",\
"ResetApp:   ERR"
};

const CommOrderTable  FtOrderTable[11]={
	Fun_ReadFt_Status,
	Fun_ReadFt_OutCome
//	Fun_ReadFt_ID,
	//Fun_ReadFt_Test						//测试通讯
};
const APPOrderTable  AppOrder[11]={
	APP_RepReadStatus,
	APP_RepResetIAP,
	APP_RepSendCfg,
	App_LoadCode,
	APP_RepReadOutCome,
	APP_RepResetAPP
};

/*******************************************************************************
* Function Name  : int ftParseCmd(u8 *buffer, uint16_t len)
* Description    : ft 通讯命令
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void FtParseCmd(void)
{
	u8 *pSrc;
	u8 cmd,flag_status=0;
	UART_NUM UartNum=USART_1;
	u32 len;
	
	pSrc=&USART_RX_Table[0].data.USART_RX_BUF[0];		//指针偏移至数据缓存区
	cmd=*pSrc;
	len=USART_RX_Table[0].len;							//接收到数据长度
	UartNum=USART_RX_Table[0].uartn;					//接收到的串口编号
	
	flag_status=Check_Data(pSrc,len);		 			//判断接收数据有效性判断
	if((cmd>=CMD_FT_STATUS)&&(cmd<=CMD_FT_END))			//当命令在区间范围内才进行通讯返回
	{
		if(flag_status==0x01)
		{
			Fun_Cmd_FtParse(cmd,UartNum);				//处理数据	
			//USARTX_SendByte(UART_5,USART_RX_Table[0].data.USART_RX_BUF,USART_RX_Table[0].len);
		}
	}
	else if((cmd>=CMD_IAP_FILE)&&(cmd<=CMD_IAP_ALL))	//当命令在区间范围内才进行通讯返回
	{
		App_Comm_Status|=1<<0;								//APP程序接收完成
		App_Comm_Status|=flag_status<<1;					//接收数据是否正确
	}
	
}
/*******************************************************************************
* Function Name  : void Fun_Cmd_FtParse(u8 cmd)
* Description    : 命令处理函数
* Input          : None.
* Output         : None.
* Return         : None.注意:程序中并没有对超时标志进行清除,只是在警告完成后清除
*******************************************************************************/
u8 Fun_Cmd_FtParse(u8 cmd,UART_NUM USART_X)
{
	u8 static CMD_FT_STATUS_Flag[4]={0};					
	//u8 TableNum;
	//TableNum=cmd-CMD_FT_STATUS;
	if(cmd==CMD_FT_STATUS)							//读取Ft的测试状态
	{
		CMD_FT_STATUS_Flag[USART_X]=Fun_ReadFt_Status(USART_X);
	}
	else if(cmd==CMD_FT_OUTCOME)					//读取Ft的测试结果
	{
		if(((CMD_FT_STATUS_Flag[USART_X]==Ft_OutCome)&&(UartX_Read_OutTime(USART_X))==0))//未超时	并且状态是读结果状态	
		{	
			CMD_FT_STATUS_Flag[USART_X]=Ft_Start;	//清除状态
			Disable_Ft_OutTime(USART_X);			//取消超时
			Set_Ft_Done(USART_X);
			if(Fun_ReadFt_OutCome(USART_X)==1)		Set_Ft_Status(USART_X,FT_TEST_DONE_OK);
			else									Set_Ft_Status(USART_X,FT_TEST_DONE_SDERR);
		}
		else
		{
			Set_Ft_Status(USART_X,FT_TEST_COMM_ERR);
		}
	}
	
	//RegularFlag=FtOrderTable[TableNum](cmd,UART_5);
	return 1;
}

/*******************************************************************************
* Function Name  : void Fun_ReadFt_ID(u8 cmd)
* Description    : 读取FT ID
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
u8 Fun_ReadFt_ID(UART_NUM USART_X)
{
	
	return 0;
}

/*******************************************************************************
* Function Name  : void Fun_ReadFt_Status(u8 cmd)
* Description    : 读取FT状态函数
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
u8 Fun_ReadFt_Status(UART_NUM USART_X)
{
	u8 NextStatus[4]={0};
	u8 static Ft_Status[4]={0};
/*	if(UartX_Read_OutTime(USART_X)==0x01)						//先判断是否超时
	{
		Disable_Ft_OutTime(USART_X);							//关闭超时机制
		Ft_Status[USART_X]=Ft_Start;							//初始为Ft_start状态
	}*/
	switch(Ft_Status[USART_X])
	{
		case Ft_Start:
			if(USART_RX_Table[0].data.ftStatus.status==FT_TEST_START)
			{
				Enable_Ft_Work(USART_X);							//使能工作启动状态
				Enable_Ft_OutTime(USART_X);							//使能超时机制
				NextStatus[USART_X]=++Ft_Status[USART_X];			//读取下一步的ft状态				
			}
		break;
		case Ft_Busy:
			if(USART_RX_Table[0].data.ftStatus.status==FT_TEST_BUSY)
			{
				NextStatus[USART_X]=++Ft_Status[USART_X];			//读取下一步的ft状态
			}
		break;
		case Ft_End:
			if(USART_RX_Table[0].data.ftStatus.status==FT_TEST_END)
			{
				NextStatus[USART_X]=++Ft_Status[USART_X];			//读取下一步的ft状态
				Ft_Status[USART_X]=Ft_Start;						//初始状态
			}
		break;
		default:	break;
	}										
	return NextStatus[USART_X];
}

/*******************************************************************************
* Function Name  : void Fun_ReadFt_OutCome(u8 cmd)
* Description    : 读取FT结果
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
u8 Fun_ReadFt_OutCome(UART_NUM USART_X)
{
	u8 Ft_test_Flag=0x88,res=0x00,PathNum=0;
	u32 Datalen=0;
	u8 WirteFlag[3]={0x88};
	Ft_test_Flag=USART_RX_Table[0].data.OutComeFlag.outflag;
	PathNum=USART_X*5;
	if(Fm_Readfilesize((u8*)path[PathNum],&Datalen)==FR_OK)
	{
		  if(Fm_Writefile((u8*)path[PathNum],&Ft_test_Flag,Datalen,1)==FR_OK)
		  {
		  	USARTX_SendByte(UART_5,WirteFlag,3);					//写成功标志，测试使用
		  	res=1;
		  }
	}
	else
	{
		 res=0;
	}	

	return res;
}
/*******************************************************************************
* Function Name  : void Fun_ReadFt_OutCome(u8 cmd)
* Description    : 读取FT结果
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
u8 Fun_ReadFt_Test(u8 cmd,u8 flag)
{
/*	u8* buf;
	u8* response;
	u16 Cs=0;
	response=&USART_RX_Table[0].data.USART_RX_BUF[0];
	buf=mymalloc(128);						//申请内存
	MemCopy(buf,response,128);
	if(flag==DATA_VAILD)
	{
		USARTX_SendByte(USART_RX_Table[0].uartn,buf,USART_RX_Table[0].len);
	}
	else
	{
		buf[0]=cmd;
		Cs+=cmd;
		buf[1]=0x55;
		Cs+=0x55;
		buf[2]=Cs>>8;
		buf[3]=Cs;
		USARTX_SendByte(USART_RX_Table[0].uartn,buf,4);
	}
	myfree(buf);							//释放内存	*/
	return 0;	
}
/*******************************************************************************
* Function Name  : void Enable_Ft_OutTime(void)
* Description    : 启动超时机制
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Enable_Ft_OutTime(UART_NUM uart_num)
{
	FtOutTimeFlag[uart_num].TestFlag=0x01;
	FtOutTimeFlag[uart_num].TestCnt=0;
	FtOutTimeFlag[uart_num].TestCntMax=6000;				// 30S
}
/*******************************************************************************
* Function Name  : void Enable_Ft_OutTime(void)
* Description    : 启动工作标志
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Enable_Ft_Work(UART_NUM uart_num)
{
	FtWorkFlag[uart_num].WorkStart=0x01;
}

/*******************************************************************************
* Function Name  : void Enable_Ft_OutTime(void)
* Description    : 关闭超时机制
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Disable_Ft_OutTime(UART_NUM uart_num)
{
	FtOutTimeFlag[uart_num].TestFlag=0x00;
	FtOutTimeFlag[uart_num].TestCnt=0;
	FtOutTimeFlag[uart_num].TestCntMax=0;				
}
/*******************************************************************************
* Function Name  : void Disable_Ft_Work(UART_NUM uart_num)
* Description    : 清除工作标志
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Disable_Ft_Work(UART_NUM uart_num)
{
	FtWorkFlag[uart_num].WorkStart=0x00;
	FtWorkFlag[uart_num].WorkDone=0x00;
	FtWorkFlag[uart_num].WorkStatus=0x00;
}

/*******************************************************************************
* Function Name  : void Set_Ft_Done(UART_NUM uart_num)
* Description    : 测试1PCS完成
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Set_Ft_Done(UART_NUM uart_num)
{
	FtWorkFlag[uart_num].WorkStart=0x00;
	FtWorkFlag[uart_num].WorkDone=0x01;
}
/*******************************************************************************
* Function Name  : void Set_Ft_Status(UART_NUM uart_num,u8 status)
* Description    : 设置状态:0x00表示等待状态中,0x1表示成功;0x02表示写入SD卡失败
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Set_Ft_Status(UART_NUM uart_num,u8 status)
{
	FtWorkFlag[uart_num].WorkStatus=status;
}
/*******************************************************************************
* Function Name  : void Clear_Ft_Status(UART_NUM uart_num)
* Description    : 清除状态:此状态共外部使用，有外部函数清除，只在测试完成中置起
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Clear_Ft_Status(UART_NUM uart_num)
{
	FtWorkFlag[uart_num].WorkStatus=0x00;
}

/*******************************************************************************
* Function Name  : void Clear_Ft_Status(UART_NUM uart_num)
* Description    : 清除状态:此状态共外部使用，有外部函数清除，只在测试完成中置起
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
u8 Read_Ft_Status(UART_NUM uart_num)
{
	return(FtWorkFlag[uart_num].WorkStatus);
}
/*******************************************************************************
* Function Name  : void Test_UART1_AppUpdata(void)
* Description    : 测试使用串口1下载ft的App
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Test_UART1_AppUpdata(void)
{
	u8 Err;
//	if(APP_RepReadStatus(USART_1)==1)
//	{
//		USARTX_SendByte(UART_5,USART_RX_Table[0].data.USART_RX_BUF,USART_RX_Table[0].len);
//	} 
	if(Fun_Cmd_AppUpdata(USART_1,&Err)==0)
	{
		USARTX_SendByte(UART_5,&Err,3);
	}
}

/*******************************************************************************
* Function Name  : void Test_UART1_AppUpdata(void)
* Description    : 测试使用串口1下载ft的App
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Test_UART3_AppUpdata(void)
{
	u8 Err;
//	if(APP_ResetIAP(USART_3)==1)
//	{
//		USARTX_SendByte(UART_5,USART_RX_Table[0].data.USART_RX_BUF,USART_RX_Table[0].len);
//	} 
	if(Fun_Cmd_AppUpdata(USART_3,&Err)==0)
	{
		Clear_Screen(1,10,256,2);
		Display_String_GB2312(1,10,"FT程序更新失败!",0);
		USARTX_SendByte(UART_5,USART_RX_Table[0].data.USART_RX_BUF,USART_RX_Table[0].len); //用来测试使用
		USARTX_SendByte(UART_5,&Err,3);
	}
	else
	{
		Clear_Screen(1,10,256,2);
		Display_String_GB2312(1,10,"FT程序更新成功!",0);
	}
}

/*******************************************************************************
* Function Name  : void Fun_Cmd_AppUpdata(UART_NUM USART_X)
* Description    : 命令处理函数
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
u8 Fun_Cmd_AppUpdata(UART_NUM USART_X,u8* Err)
{
	u8 flag=0,TableNum=0,percentage=0;
	*Err=0x00;
	for(TableNum=0;TableNum<IAP_LOAD_NUM;TableNum++)
	{
		Display_Roll(1,12,' ',percentage,1);						//显示当前的更新过程
		percentage+=20;
		if(AppOrder[TableNum](USART_X)==0)	break;
		#ifdef APP_UPDATA_TEST										//测试条件下才使用
		else
		{
			Clear_Screen(1,(TableNum+1),256,1);
			Dispaly_5x8_string(1,(TableNum+1),(u8*)UpdataPassInfo[TableNum],0);
			USARTX_SendByte(UART_5,USART_RX_Table[0].data.USART_RX_BUF,USART_RX_Table[0].len); //用来测试使用
		}
		#endif
	}
	if(TableNum==IAP_LOAD_NUM)		flag=1;
	else							*Err=TableNum;		//返回错误项目
	return flag;
}

/*******************************************************************************
* Function Name  : u8 APP_RepReadStatus(void)
* Description    : 多次通讯
* Input          : None.
* Return         : None
*******************************************************************************/
u8 APP_RepResetIAP(UART_NUM USART_X)
{
	u8 flag=1,i,RepCnt=3;
	for(i=0;i<RepCnt;i++)
	{
		if(APP_ResetIAP(USART_X)==0x01)	break;
	}
	if(i==RepCnt)	flag=0;
	Delay_MS(50);  					//上电复位过程延时
	return flag;
}

/*******************************************************************************
* Function Name  : u8 APP_ResetAPP(void)
* Description    : 复位为IAP
* Input          : None.
* Return         : None.
*******************************************************************************/
u8 APP_ResetIAP(UART_NUM USART_X)
{
	u8 buf[3], flag=0;								//申请一个临时缓存
	buf[0]=CMD_IAP_MVERSION;
	buf[1]=0x00;
	buf[2]=CMD_IAP_MVERSION;
	USARTX_SendByte(USART_X,buf,3);
	flag=APP_Readback_Status(CMD_IAP_MVERSION,RST_CORRECT);
	return flag;
}

/*******************************************************************************
* Function Name  : u8 APP_RepReadStatus(void)
* Description    : 多次通讯
* Input          : None.
* Return         : None
*******************************************************************************/
u8 APP_RepResetAPP(UART_NUM USART_X)
{
	u8 flag=1,i,RepCnt=3;
	for(i=0;i<RepCnt;i++)
	{
		if(APP_ResetAPP(USART_X)==0x01)	break;
	}
	if(i==RepCnt)	flag=0;
	return flag;
}

/*******************************************************************************
* Function Name  : u8 APP_ResetAPP(void)
* Description    : 复位为APP
* Input          : None.
* Return         : None.
*******************************************************************************/
u8 APP_ResetAPP(UART_NUM USART_X)
{
	u8 buf[3],flag=0;								//申请一个临时缓存
	buf[0]=CMD_CANCEL_FILE;
	buf[1]=0x00;
	buf[2]=CMD_CANCEL_FILE;
	USARTX_SendByte(USART_X,buf,3);
	flag=APP_Readback_Status(CMD_CANCEL_FILE,RST_CORRECT);
	
	return flag;
}
/*******************************************************************************
* Function Name  : u8 APP_RepReadStatus(void)
* Description    : 多次通讯
* Input          : None.
* Return         : None
*******************************************************************************/
u8 APP_RepReadOutCome(UART_NUM USART_X)
{
	u8 flag=1,i,RepCnt=3;
	for(i=0;i<RepCnt;i++)
	{
		if(APP_ReadOutCome(USART_X)==0x01)	break;
	}
	if(i==RepCnt)	flag=0;
	return flag;
}

/*******************************************************************************
* Function Name  : APP_ReadOutCome
* Description    : 读取APP下载结果
* Input          : None.
* Return         : None.
*******************************************************************************/
u8 APP_ReadOutCome(UART_NUM USART_X)
{
	u8 buf[3],flag=0;								//申请一个临时缓存
	buf[0]=CMD_IAP_RESULT;
	buf[1]=0x00;
	buf[2]=CMD_IAP_RESULT;
	USARTX_SendByte(USART_X,buf,3);
	flag=APP_Readback_Status(CMD_IAP_RESULT,RST_CORRECT);
	return flag;
}

/*******************************************************************************
* Function Name  : u8 APP_RepReadStatus(void)
* Description    : 多次通讯
* Input          : None.
* Return         : None
*******************************************************************************/
u8 APP_RepReadStatus(UART_NUM USART_X)
{
	u8 flag=1,i,RepCnt=3;
	for(i=0;i<RepCnt;i++)
	{
		if(APP_ReadStatus(USART_X)==0x01)	break;
	}
	if(i==RepCnt)	flag=0;
	return flag;
}
/*******************************************************************************
* Function Name  : APP_ReadStatus
* Description    : 读取当前FT板的状态
* Input          : None.
* Return         : None.
*******************************************************************************/
u8 APP_ReadStatus(UART_NUM USART_X)
{
	u8 buf[3],flag=0;								//申请一个临时缓存
	buf[0]=CMD_IAP_STATUS;
	buf[1]=0x00;
	buf[2]=CMD_IAP_STATUS;
	USARTX_SendByte(USART_X,buf,3);
	flag=APP_Readback_MulStatus(CMD_IAP_STATUS,APP_FLAG,IAP_FLAG);
	return flag;
}
/*******************************************************************************
* Function Name  : u8 APP_RepSendCfg(void)
* Description    : 多次通讯
* Input          : None.
* Return         : None
*******************************************************************************/
u8 APP_RepSendCfg(UART_NUM USART_X)
{
	u8 flag=1,i,RepCnt=3;
	for(i=0;i<RepCnt;i++)
	{
		if(APP_SendCfg(USART_X)==0x01)	break;
	}
	if(i==RepCnt)	flag=0;
	return flag;
}
/*******************************************************************************
* Function Name  : u8 APP_SendCfg(void)
* Description    : 发送更新APP相关内容
* Input          : None.
* Return         : None.Cmd+lenght+起始地址+CodeCs值+CS:12+3
*******************************************************************************/
u8 APP_SendCfg(UART_NUM USART_X)
{
	u8 flag=0;
	if(Fm_Readfile((u8*)path[PUBANK_FTCFG],0,12)==FR_OK)
	{
		BufferAdd(SectorBuffer,12);
	}
	UART_MemDealBack(SectorBuffer,12);	//数组右移一位,首位配置为CMD
	SectorBuffer[0]=CMD_IAP_FILE;					//CMD添入首位	
	USARTX_SendByte(USART_X,SectorBuffer,(12+3));
	flag=APP_Readback_Status(CMD_IAP_FILE,RST_CORRECT);
	return flag;
}
/*******************************************************************************
* Function Name  : u8 App_LoadCode(u32 All_Len)
* Description    : 发送Code
* Input          : None.
* Return         : None:All_len:Code总长度，这个长度上位机会给出
*******************************************************************************/
u8 App_LoadCode(UART_NUM USART_X)
{
	u32 CodeLen;
	u8 flag=0;
	if(Fm_Readfilesize((u8*)path[PUBANK_FTAPP],&CodeLen)==0)
	{
		  flag=App_SendCode(USART_X,CodeLen);
	}else
	{
		flag=0;
	}
	return flag;
}
/*******************************************************************************
* Function Name  : u8 App_SendCode(u32 All_Len)
* Description    : 发送Code
* Input          : None.
* Return         : None:All_len:Code总长度，这个长度上位机会给出
*******************************************************************************/
u8 App_SendCode(UART_NUM USART_X,u32 All_Len)
{
	u8 flag=1;
	u16 Cycle,left,i,CycTimeOut=3;//最大超过3次错误则错
	u32 Offset=0;
	Cycle=All_Len/4096;
	left=All_Len%4096;
	if(Cycle==0)
	{
		
		for(i=0;i<CycTimeOut;i++)
		{
			if(Fm_Readfile((u8*)path[PUBANK_FTAPP],0,left)==FR_OK)
			{
				BufferAdd(SectorBuffer,left);
			}
			if(APP_SendPackage(SectorBuffer,USART_X,left)==1)	break;
		}
		if(i==CycTimeOut)			return 0;
	}
	else
	{
		while(Cycle--)
		{
			
			for(i=0;i<CycTimeOut;i++)
			{
				if(Fm_Readfile((u8*)path[PUBANK_FTAPP],Offset,4096)==FR_OK)
				{
					BufferAdd(SectorBuffer,4096);
				}
				else
				{
					while(1);
				}
				if(APP_SendPackage(SectorBuffer,USART_X,4096)==1)	{Offset+=4096;break;}	
			}
			if(i==CycTimeOut)			return 0;
		}
		if(left>0)
		{
			
			for(i=0;i<CycTimeOut;i++)
			{
				if(Fm_Readfile((u8*)path[PUBANK_FTAPP],Offset,left)==FR_OK)
				{
					BufferAdd(SectorBuffer,left);
				}
				else
				{
					while(1);
				}
				if(APP_SendPackage(SectorBuffer,USART_X,left)==1)	break;
			}
			if(i==CycTimeOut)			return 0;
		}
	}
	return flag;
}

/*******************************************************************************
* Function Name  : u8 APP_SendCfg(void)
* Description    : 发送更新APP相关内容
* Input          : None.
* Return         : None.Cmd+Code+CS;(Code最大是4096一包)
*******************************************************************************/
u8 APP_SendPackage(u8* buf,UART_NUM USART_X,u32 len)  //len:下传的Code数据长度
{
	u8 flag=0;
	UART_MemDealBack(buf,len);				//数组右移一位,首位配置为CMD
	*buf=CMD_IAP_TRANS;						//CMD添入首位	
	USARTX_SendByte(USART_X,buf,(len+3)); //发送CMD+数据+CS(CS在发送函数内部会处理)
	flag=APP_Readback_Status(CMD_IAP_TRANS,RST_CORRECT);
	return flag;
}
/*******************************************************************************
* Function Name  : u8 APP_Readback_Status(u8 Cmd,u8 status)
* Description    : APP命令回读是否正确，最大超时2S
* Input          : None.
* Return         : None.
*******************************************************************************/
u8 APP_Readback_Status(u8 Cmd,u8 status)
{
	u8 App_Cnt=200;
	u8 flag=0;
	
	while(((App_Comm_Status&(1<<0))==0)&&(App_Cnt))
	{
	//	LED_ON(LED0);
		Delay_MS(10);
		App_Cnt--;
	//	LED_OFF(LED0);
	}
	if(App_Cnt==0)			return  0;					//超时过程，此过程错误，说明数据接收异常
	else
	{
		App_Comm_Status&=~(1<<0);						//清完成标志
		if((App_Comm_Status&(1<<1))==(1<<1))			//数据校验正确
		{
			App_Comm_Status&=~(1<<1);					//清数据校验标志
			if(USART_RX_Table[0].data.USART_RX_BUF[0]==Cmd)	
			{
				if(USART_RX_Table[0].data.USART_RX_BUF[1]==status)
				{
					flag=0x01;
				}
			}
		}
	}
	return flag;
}

/*******************************************************************************
* Function Name  : u8 APP_Readback_MulStatus(u8 Cmd,u8 status1,u8 status2)
* Description    : 多参数APP命令回读是否正确，最大超时2S
* Input          : None.
* Return         : None.
*******************************************************************************/
u8 APP_Readback_MulStatus(u8 Cmd,u8 status1,u8 status2)
{
	u32 App_Cnt=200;
	u8 flag=0;
	while(((App_Comm_Status&(1<<0))==0)&&(App_Cnt))
	{
	//	LED_ON(LED0);
		Delay_MS(10);
		App_Cnt--;
	//	LED_OFF(LED0);
	}
	if(App_Cnt==0)			return  0;				//超时过程，此过程错误，说明数据没有接收完成
	else
	{
		App_Comm_Status&=~(1<<0);						//清完成标志
		if((App_Comm_Status&(1<<1))==(1<<1))			//数据校验正确
		{
			App_Comm_Status&=~(1<<1);					//清数据校验标志
			if(USART_RX_Table[0].data.USART_RX_BUF[0]==Cmd)	
			{
				if((USART_RX_Table[0].data.USART_RX_BUF[1]==status1)||(USART_RX_Table[0].data.USART_RX_BUF[1]==status2))
				{
					flag=0x01;
				}
			}
		}
	}
	return flag;
}

/*******************************************************************************
* Function Name  : u8 APP_Readback_MulData(u8 Cmd,u8*puf,u32 len)
* Description    : 多数据APP命令回读是否正确，最大超时为100ms
* Input          : None.
* Return         : None.
*******************************************************************************/
u8 APP_Readback_MulData(u8 Cmd,u8*puf,u32 len)
{
	u32 App_Cnt=10;//200;
	u8 flag=0;
	while(((App_Comm_Status&(1<<0))==0)&&(App_Cnt))
	{
	//	LED_ON(LED0);
		Delay_MS(10);
		App_Cnt--;
	//	LED_OFF(LED0);
	}
	if(App_Cnt==0)			return  0;				//超时过程，此过程错误，说明数据没有接收完成
	else
	{
		App_Comm_Status&=~(1<<0);						//清完成标志
		if((App_Comm_Status&(1<<1))==(1<<1))			//数据校验正确
		{
			App_Comm_Status&=~(1<<1);					//清数据校验标志
			if(USART_RX_Table[0].data.USART_RX_BUF[0]==Cmd)	
			{
				MemCopy(puf,&USART_RX_Table[0].data.USART_RX_BUF[0],len);
				return 1;
			}
		}
	}
	return flag;
}

/*******************************************************************************
* Function Name  : u8 Check_Data(u8* buf,int len)
* Description    : 检查数据是否有效
* Input          : None.
* Return         : None.
*******************************************************************************/
u8 Check_Data(u8* buf,u32 len)
{
	u8 flag=0;							//默认是错误状态
	u32 Cs=0,Cstemp=0;
	if(len<3)				return 0;	//有效数据判断
	Cs=Checksum(buf,len);				//计算到帧的CS值
	Cstemp=(u16)buf[len-2]<<8;
	Cstemp|=buf[len-1];
	if((u16)Cs==Cstemp)		flag=1;
	return flag;
}


/*******************************************************************************
* Function Name  : Checksum
* Description    : 计算CS的值
* Input          : None.
* Return         : None.
*******************************************************************************/
u32  Checksum(u8* buf, u32 len)
{
    u16 i;
    u32 cs;
    for(cs = 0 , i = 0 ; i < len-2; i++) {
        cs += buf[i];
    }
    return (cs);
}

u16 CalCheckSum(u8*buf,u32 len)
{
    int i;
    u32 lcksum = 0;

    for(i = 0; i < len; i++) {
        lcksum += buf[i];
    }

    return lcksum;
}  

void ReloadChecksum(u8* buf,u32 len)
{
	u32 Cs=0;
	Cs=CalCheckSum(buf,(len-2));			//计算Cmd+数据
	buf[len-2]=(u16)Cs>>8;
	buf[len-1]=(u16)Cs;
}
/*******************************************************************************
* Function Name  : void MemCopy(void*pdest,const void*src,u32 n)
* Description    : 数组赋值
* Input          : None.
* Return         : None.
*******************************************************************************/
void MemCopy(u8 *pdest,const u8*src,u32 len)
{
	while(len--)
	{
		*pdest=*src;
		pdest++;
		src++;
	}
}

/*******************************************************************************
* Function Name  : void UART_MemDealBack(uint8_t *dest, int32_t size)
* Description    : 数组右移
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
* Description    : 数组左移
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void UART_MemDealHead(uint8_t *dest, int32_t size)
{
    while(size--) *dest++ = *(dest+1);
}


/*******************************************************************************
* Function Name  : void UartX_Set_OutTime(void)
* Description    : 接收数据超时判断
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void UartX_Set_OutTime(void)
{
	u8 uartnum;
	for(uartnum=0;uartnum<UART_5;uartnum++)
	{
		if(FtOutTimeFlag[uartnum].TestFlag==1)
		{
			if(++FtOutTimeFlag[uartnum].TestCnt>=FtOutTimeFlag[uartnum].TestCntMax)
			{
				FtOutTimeFlag[uartnum].TestFlag=0;
				FtOutTimeFlag[uartnum].WarnFlag=1;			//警告标志
				FtOutTimeFlag[uartnum].OutTimeFlag=1;		//超时标志
			}
		}
	}
	
}


/*******************************************************************************
* Function Name  : u8 UartX_Read_OutTime(UART_NUM USART_X)
* Description    : 接收数据超时判断
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
u8 UartX_Read_OutTime(UART_NUM USART_X)
{
	return (FtOutTimeFlag[USART_X].OutTimeFlag);
}


u8 APP_Null(void)
{
	return 0;
}



