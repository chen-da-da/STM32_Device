#include "fft_test.h"
#include "string.h"
	 

FATFS *fs[1];  		//逻辑磁盘工作区.	 
FIL *file;	  		//文件1
FIL *ftemp;	  		//文件2.
UINT br,bw;			//读写变量
FILINFO fileinfo;	//文件信息
DIR dir;  			//目录
u8 *fatbuf;			//SD卡数据缓存区
u8 SectorBuffer[4102]; //读数据缓存区间	

//////////////////////////////////////////////////////////////////////////////////////////
const  char path[24][40]={
"0:BANK0/FLAG.txt","0:BANK0/BIN.txt","0:BANK0/CFG.txt","0:BANK0/CFGBAK.txt","0:BANK0/OTHER.txt",\
"0:BANK1/FLAG.txt","0:BANK1/BIN.txt","0:BANK1/CFG.txt","0:BANK1/CFGBAK.txt","0:BANK1/OTHER.txt",\
"0:BANK2/FLAG.txt","0:BANK2/BIN.txt","0:BANK2/CFG.txt","0:BANK2/CFGBAK.txt","0:BANK2/OTHER.txt",\
"0:BANK3/FLAG.txt","0:BANK3/BIN.txt","0:BANK3/CFG.txt","0:BANK3/CFGBAK.txt","0:BANK3/OTHER.txt",\
"0:PUBANK/FTAPP.txt","0:PUBANK/CRAPP.txt","0:PUBANK/FTCFG.txt","0:PUBANK/CRCFG.txt"
};
const char filepath[5][20]={
"0:BANK0","0:BANK1","0:BANK2","0:BANK3","0:PUBANK"
};
/////////////////////////////////////////////////////////////////////////////////////////

//向SD卡的固定区域添加内容
//向BANK0中的flag添加内容
u8 Fm_AddData(u8*path)
{ 
	u32 filesize=0;
	u8 res=0xff,textFileBuffer[512]={0};
	memset(textFileBuffer,0xAA,1);								//只赋值512个AA
//	exfuns_init();  			//分配内存
//    f_mount(fs[0],"0:",0);    	//将文件系统设置到0区
	if(mf_open(path, FA_OPEN_EXISTING | FA_READ)==FR_OK) //打开文件读
	{																	 
		 filesize=mf_size();											//读取当前文件内数据的长度
		 #if CFG_PRINTF
		 printf("\r\nfilesize:%d \r\n",filesize);  						//显示出当前文件长度
		 #endif
		 if(mf_open(path, FA_OPEN_EXISTING | FA_WRITE)==FR_OK) //打开文件写
		 {
		 	 mf_lseek(1);										//指针指向文本尾部，此处最好添加长度限制，确保写满
		 	 res=mf_write(textFileBuffer, 1);  							//写入512个数据；直接写入SD卡
			 mf_close();												//写完关闭文件
			 //校验下数据对不对;输出来看看
			 mf_open(path, FA_OPEN_EXISTING | FA_READ);
			 filesize=mf_size();											//更新当前文件内数据的长度
			 #if CFG_PRINTF
		 	 printf("\r\nfilesize:%d \r\n",filesize);  						//显示出当前文件长度
		 	 #endif
			 mf_lseek(0);													//指针指向文件头
			 mf_read(filesize);
			 mf_close();
		 }
	}
	return res;
}

//写入文件的内容
//filesize<=4096Byte
//写入缓存：SectorBuffer[4096]
u8 Fm_Writefile(u8*path,u8* pbuf,u32 offset,u16 writesize)
{
	u8 res=0xff;
//	exfuns_init();  			//分配内存
  //  f_mount(fs[0],"0:",0);    	//将文件系统设置到0区
	if(mf_open(path, FA_OPEN_EXISTING | FA_WRITE)==FR_OK) //打开文件读
	{
		mf_lseek(offset);			   //从文件的偏移地址开始写入
		res=mf_write(pbuf, writesize); //最大不能大于512
		mf_close();
	}
	return res;	
}

//读取文件的内容
//filesize<=4096Byte
//读取缓存：SectorBuffer[4096]
u8 Fm_Readfile(u8*path,u32 offset,u32 filesize)
{
	u8 res=0xff;
//	exfuns_init();  			//分配内存
  //  f_mount(fs[0],"0:",0);    	//将文件系统设置到0区
	if(mf_open(path, FA_OPEN_EXISTING | FA_READ)==FR_OK) //打开文件读
	{
		mf_lseek(offset);			   //从文件的偏移地址开始读取
		res=mf_read(filesize);			   //读取filesize文件的长度
		mf_close();
	}
	return res;	
}

//读取文件的长度
u8 Fm_Readfilesize(u8*path,u32* filesize)
{
	u8 res=0xff;
//	exfuns_init();  			//分配内存
 //   f_mount(fs[0],"0:",0);    	//将文件系统设置到0区
	if(mf_open(path, FA_OPEN_EXISTING | FA_READ)==FR_OK) //打开文件读
	{
		*filesize=mf_size();
		mf_close();
		res=0x00;				//打开成功
	}
	return res;	
}

//创建5个文件夹：BANK0~BANK4
//BANK0~BANK3都包含一个Flag.txt+BIN.txt+CFG.txt+CFGBAK.txt+Other.txt
//PUBLICBANK包含一个FTAPP+CRAPP
u8 Fm_CreatAllFile(void)
{
	u8 path_num=0,res=0x00;
//	res=exfuns_init();  			//分配内存
//   res=f_mount(fs[0],"0:",0);    	//将文件系统设置到0区
	for(path_num=0;path_num<24;path_num++)
	{
		if(path_num%5==0)
		{
			res=mf_mkdir((u8*)filepath[path_num/5]);
				
		}
		res=mf_open((u8*)path[path_num], FA_OPEN_ALWAYS | FA_WRITE);
		if(res!=FR_OK)
		{
			res=0xFF;
			break;
		}
	}
	mf_close();
	return res;	 
}


//创建5个文件夹：BANK0~BANK4
//BANK0~BANK3都包含一个Flag.txt+BIN.txt+CFG.txt+CFGBAK.txt+Other.txt
//PUBLICBANK包含一个FTAPP+CRAPP
u8 Fm_CreatFile(u8*pathfile,u8*path)
{
	 u8 res=0xFF;
//	 exfuns_init();  			//分配内存
//     f_mount(fs[0],"0:",0);    	//将文件系统设置到0区
	 mf_mkdir(pathfile);									//创建文件夹
	 res=mf_open(path, FA_OPEN_ALWAYS | FA_WRITE);
	 if(res==FR_OK)		//文件夹下创建文件bin1.bin
	 {
	 	#if CFG_PRINTF
	 	printf("\r\n 创建成功\r\n");
	 	#endif
	 }
	 mf_close(); 
	 return res;	 
}

//删除所有文件
//删除BANK0~BANK3中的Flag.txt+BIN.txt+CFG.txt+CFGBAK.txt+Other.txt
//或者删除PUBLICBANKFTAPP+CRAPP
u8 Fm_DeleteAllFile(void)
{
	u8 path_num,res=0x00;
//	exfuns_init();  			//分配内存
//   f_mount(fs[0],"0:",0);    	//将文件系统设置到0区
	for(path_num=0;path_num<22;path_num++)
	{
		if(mf_unlink((u8*)path[path_num])!=FR_OK)
		{
			res=0xff;							 //错误返回标志
			break;	
		}			
	}
	return res;	 
}

//删除文件
//删除BANK0~BANK3中的Flag.txt+BIN.txt+CFG.txt+CFGBAK.txt+Other.txt
//或者删除PUBLICBANKFTAPP+CRAPP
u8 Fm_DeleteFile(u8*path)
{
	 u8 res=0xFF;
//	 exfuns_init();  			//分配内存
//     f_mount(fs[0],"0:",0);    	//将文件系统设置到0区
	 res=mf_unlink(path);
	 if(res==FR_OK)				//文件删除成功
	 {
	 	#if CFG_PRINTF
	 	printf("\r\n 删除成功\r\n");
	 	#endif
	 } 
	 return res;	 
}

//搜查文件夹下的文件
//搜索BANK0~BANK3中的Flag.txt+BIN.txt+CFG.txt+CFGBAK.txt+Other.txt
//或者搜索PUBLICBANKFTAPP+CRAPP
u8 Fm_scanFile(u8*pathfile)
{
	 u8 res=0xFF;
//	 exfuns_init();  			//分配内存
//     f_mount(fs[0],"0:",0);    	//将文件系统设置到0区
	 res=mf_scan_files(pathfile);
	 if(res==FR_OK)				//文件删除成功
	 {
	 	#if CFG_PRINTF
	 	printf("\r\n 搜索成功\r\n");
	 	#endif
	 } 
	 return res;	 
}

//读取SD卡的指定扇区的内容
//sec：扇区物理地址编号
//sector大小:512Byte
void SD_readsector(u32 sector)
{
	u8 *buf;
	#if CFG_PRINTF
	u16 i;
	#endif
	buf=mymalloc(512);						//申请内存
	if(SD_ReadBlock(buf,sector*512,512)==0)	//读取0扇区的内容
	{	
		#if CFG_PRINTF
		printf("sector: %d \r\n",sector);
		for(i=0;i<512;i++)printf("%x ",buf[i]);//打印扇区数据    
		printf("\r\nDATA ENDED\r\n");
		#endif
	}
	myfree(buf);							//释放内存	
}
//数组求和 

u32  BufferAdd(u8* pBuffer,u32 BufferLength)
{
	u32 BufferCs=0;
	while (BufferLength--)
	{
		BufferCs+=*pBuffer;
		pBuffer++;
	}
	return 	BufferCs;
}
///////////////////////////////////////////////////////////////////////////////////////
//为exfuns申请内存
//返回值:0,成功
//1,失败
u8 exfuns_init(void)
{
	fs[0]=(FATFS*)mymalloc(sizeof(FATFS));	//为磁盘0工作区申请内存	
	//fs[1]=(FATFS*)mymalloc(sizeof(FATFS));	//为磁盘1工作区申请内存
	file=(FIL*)mymalloc(sizeof(FIL));		//为file申请内存
	ftemp=(FIL*)mymalloc(sizeof(FIL));		//为ftemp申请内存
	fatbuf=(u8*)mymalloc(512);				//为fatbuf申请内存
	if(fs[0]&&file&&ftemp&&fatbuf)return 0;  //申请有一个失败,即失败.
	else return 1;	
}
 
//得到磁盘剩余容量
//drv:磁盘编号("0:"/"1:")
//total:总容量	 （单位KB）
//free:剩余容量	 （单位KB）
//返回值:0,正常.其他,错误代码
u8 exf_getfree(u8 *drv,u32 *total,u32 *free)
{
	FATFS *fs1;
	u8 res;
    u32 fre_clust=0, fre_sect=0, tot_sect=0;
    //得到磁盘信息及空闲簇数量
    res =(u32)f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect=(fs1->n_fatent-2)*fs1->csize;	//得到总扇区数
	    fre_sect=fre_clust*fs1->csize;			//得到空闲扇区数	   
#if FF_MAX_SS!=512				  				//扇区大小不是512字节,则转换为512字节
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		*total=tot_sect>>1;	//单位为KB
		*free=fre_sect>>1;	//单位为KB 
 	}
	return res;
}

//打开路径下的文件
//path:路径+文件名
//mode:打开模式
//返回值:执行结果
u8 mf_open(u8*path,u8 mode)
{
	u8 res;	 
	res=f_open(file,(const TCHAR*)path,mode);//打开文件夹
	return res;
} 


//关闭文件
//返回值:执行结果
u8 mf_close(void)
{
	f_close(file);
	return 0;
}

//同步文件
//FF_FS_READONLY==0
//读写操作将缓存写入SD
//类似Close
//频繁写入建议使用
u8 mf_sync(void)
{
	u8 res;
	res=f_sync(file);
	return res;	
} 

//读出数据
//len:读出的长度
//返回值:执行结果
u8 mf_read(u16 len)
{
	u16 i,t;
	u8 res=0;
	u16 tlen=0;
	#if CFG_PRINTF
	printf("\r\nRead file data is:\r\n");
	#endif
	for(i=0;i<len/512;i++)
	{
		res=f_read(file,fatbuf,512,&br);
		if(res)
		{
			#if CFG_PRINTF
			printf("Read Error:%d\r\n",res);
			#endif
			break;
		}else
		{
			for(t=0;t<br;t++)
			{
				SectorBuffer[t+tlen]=fatbuf[t];
				#if CFG_PRINTF
				printf("%d ",fatbuf[t]);
				#endif
			} 
			tlen+=br;
		}
	}
	if(len%512)
	{
		res=f_read(file,fatbuf,len%512,&br);
		if(res)	//读数据出错了
		{
			#if CFG_PRINTF
			printf("\r\nRead Error:%d\r\n",res); 
			#endif
		}else
		{
			for(t=0;t<br;t++)
			{
				SectorBuffer[t+tlen]=fatbuf[t];
				#if CFG_PRINTF
				printf("%d ",fatbuf[t]);
				#endif
			} 
			tlen+=br;
		}	 
	}
	#if CFG_PRINTF
	if(tlen)printf("\r\nReaded data len:%d\r\n",tlen);//读到的数据长度
	printf("Read data over\r\n");	
	#endif
	return res;
}

//写入数据
//dat:数据缓存区
//len:写入长度
//返回值:执行结果
u8 mf_write(u8*dat,u16 len)
{			    
	u8 res;	   					   
	#if CFG_PRINTF
	printf("\r\nBegin Write file...\r\n");
	printf("Write data len:%d\r\n",len);
	#endif
	res=f_write(file,dat,len,&bw);
	#if CFG_PRINTF
	if(res)
	{
		printf("Write Error:%d\r\n",res);   
	}else printf("Writed data len:%d\r\n",bw);
	printf("Write data over.\r\n");
	#endif
	return res;
}

//打开目录
 //path:路径
//返回值:执行结果
u8 mf_opendir(u8* path)
{
	return f_opendir(&dir,(const TCHAR*)path);	
}
//关闭目录 
//返回值:执行结果
u8 mf_closedir(void)
{
	return f_closedir(&dir);	
}


 //遍历文件
 //path:路径
 //返回值:执行结果
u8 mf_scan_files(u8 * path)
{
	FRESULT res;
		  
    char *fn;   /* This function is assuming non-Unicode cfg. */
#if FF_USE_LFN
 	fileinfo.lfsize = _MAX_LFN * 2 + 1;
	fileinfo.lfname = mymalloc(fileinfo.lfsize);
#endif		  

    res = f_opendir(&dir,(const TCHAR*)path); //打开一个目录
    if (res == FR_OK) 
	{	
		#if CFG_PRINTF
		printf("\r\n"); 
		#endif
		while(1)
		{
	        res = f_readdir(&dir, &fileinfo);                   //读取目录下的一个文件
	        if (res != FR_OK || fileinfo.fname[0] == 0) break;  //错误了/到末尾了,退出
	        //if (fileinfo.fname[0] == '.') continue;             //忽略上级目录
#if FF_USE_LFN
        	fn = *fileinfo.lfname ? fileinfo.lfname : fileinfo.fname;
#else							   
        	fn = fileinfo.fname;
#endif	                                              /* It is a file. */
			#if CFG_PRINTF
			printf("%s/", path);//打印路径	
			printf("%s\r\n",  fn);//打印文件名	
			#endif
		} 
    }	  
	//myfree(fileinfo.lfname);
	myfree(fileinfo.fname);
    return res;	  
}

//打读取文件夹
//返回值:执行结果
u8 mf_readdir(void)
{
	u8 res;
	char *fn;			 
#if FF_USE_LFN
 	fileinfo.lfsize = FF_MAX_LFN * 2 + 1;
	fileinfo.lfname = mymalloc(fileinfo.lfsize);
#endif		  
	res=f_readdir(&dir,&fileinfo);//读取一个文件的信息
	if(res!=FR_OK||fileinfo.fname[0]==0)
	{
		myfree(fileinfo.fname);
		return res;//读完了.
	}
#if FF_USE_LFN
	fn=*fileinfo.lfname ? fileinfo.lfname : fileinfo.fname;
#else
	fn=fileinfo.fname;
#endif	
	#if CFG_PRINTF
	printf("\r\n DIR info:\r\n");

	printf("dir.id:%d\r\n",dir.obj.id);
	printf("dir.dptr:%d\r\n",dir.dptr);
//	printf("dir.sclust:%d\r\n",dir.sclust);
	printf("dir.clust:%d\r\n",dir.clust);
	printf("dir.sect:%d\r\n",dir.sect);	  

	printf("\r\n");
	printf("File Name is:%s\r\n",fn);
	printf("File Size is:%d\r\n",fileinfo.fsize);
	printf("File data is:%d\r\n",fileinfo.fdate);
	printf("File time is:%d\r\n",fileinfo.ftime);
	printf("File Attr is:%d\r\n",fileinfo.fattrib);
	printf("\r\n");
	#endif
	//myfree(fileinfo.lfname);
	myfree(fileinfo.fname);
	return 0;
}

//显示剩余容量
//drv:盘符
//返回值:剩余容量(字节)
u32 mf_showfree(u8 *drv)
{
	FATFS *fs1;
	u8 res;
    u32 fre_clust=0, fre_sect=0, tot_sect=0;
    //得到磁盘信息及空闲簇数量
    res = f_getfree((const TCHAR*)drv,(DWORD*)&fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect = (fs1->n_fatent - 2) * fs1->csize;//得到总扇区数
	    fre_sect = fre_clust * fs1->csize;			//得到空闲扇区数	   
#if FF_MAX_SS!=512
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		if(tot_sect<20480)//总容量小于10M
		{	
		    /* Print free space in unit of KB (assuming 512 bytes/sector) */
		    #if CFG_PRINTF
		    printf("\r\n磁盘总容量:%d KB\r\n"
		           "可用空间:%d KB\r\n",
		           tot_sect>>1,fre_sect>>1);
		           #endif
		}else
		{
		    /* Print free space in unit of KB (assuming 512 bytes/sector) */
		    #if CFG_PRINTF
		    printf("\r\n磁盘总容量:%d MB\r\n"
		           "可用空间:%d MB\r\n",
		           tot_sect>>11,fre_sect>>11);
		           #endif
		}
	}
	return fre_sect;
}


//文件读写指针偏移
//offset:相对首地址的偏移量
//返回值:执行结果.
u8 mf_lseek(u32 offset)
{
	return f_lseek(file,offset);
}
//读取文件当前读写指针的位置.
//返回值:位置
u32 mf_tell(void)
{
	return f_tell(file);
}
//读取文件大小
//返回值:文件大小
u32 mf_size(void)
{
	return f_size(file);
}

//创建目录
//pname:目录路径+名字
//返回值:执行结果
u8 mf_mkdir(u8*pname)
{
	return f_mkdir((const TCHAR *)pname);
}

//格式化
//path:磁盘路径，比如"0:"、"1:"
//mode:模式
//au:簇大小,默认可写0
//返回值:执行结果
u8 mf_fmkfs(u8* path,u8* work ,u32 au)
{
//	return 0;//f_mkfs((const TCHAR*)path,mode,au);//格式化,drv:盘符;mode:模式;au:簇大小
	return 	f_mkfs((const TCHAR* )path, FM_FAT, au, work,512);
} 
//删除文件/目录
//pname:文件/目录路径+名字
//返回值:执行结果
u8 mf_unlink(u8 *pname)
{
	return  f_unlink((const TCHAR *)pname);
}

//修改文件/目录名字(如果目录不同,还可以移动文件哦!)
//oldname:之前的名字
//newname:新名字
//返回值:执行结果
u8 mf_rename(u8 *oldname,u8* newname)
{
	return  f_rename((const TCHAR *)oldname,(const TCHAR *)newname);
}

//获取盘符（磁盘名字）
//path:磁盘路径，比如"0:"、"1:"  
void mf_getlabel(u8 *path)
{
	u8 buf[20]={0};
	u32 sn=0;
	u8 res;
	res=f_getlabel ((const TCHAR *)path,(TCHAR *)buf,(DWORD*)&sn);
	if(res==FR_OK)
	{
		#if CFG_PRINTF
		printf("\r\n磁盘%s 的盘符为:%s\r\n",path,buf);
		printf("磁盘%s 的序列号:%X\r\n\r\n",path,sn); 
		#endif
	}else 
	{
		#if CFG_PRINTF
		printf("\r\n获取失败，错误码:%X\r\n",res);
		#endif
	}
}

//设置盘符（磁盘名字），最长11个字符！！，支持数字和大写字母组合以及汉字等
//path:磁盘号+名字，比如"0:ALIENTEK"、"1:OPENEDV"  
void mf_setlabel(u8 *path)
{
	u8 res;
	res=f_setlabel ((const TCHAR *)path);
	if(res==FR_OK)
	{
		#if CFG_PRINTF
		printf("\r\n磁盘盘符设置成功:%s\r\n",path);
		#endif
	}else
	{
		#if CFG_PRINTF
		printf("\r\n磁盘盘符设置失败，错误码:%X\r\n",res);
		#endif
	}
} 

//从文件里面读取一段字符串
//size:要读取的长度
void mf_gets(u16 size)
{
 	TCHAR* rbuf;
	rbuf=f_gets((TCHAR*)fatbuf,size,file);
	if(*rbuf==0)return  ;//没有数据读到
	else
	{
		#if CFG_PRINTF
		printf("\r\nThe String Readed Is:%s\r\n",rbuf); 
		#endif
	}			    	
}

//需要_USE_STRFUNC>=1
//写一个字符到文件
//c:要写入的字符
//返回值:执行结果
u8 mf_putc(u8 c)
{
	return f_putc((TCHAR)c,file);
}
//写字符串到文件
//c:要写入的字符串
//返回值:写入的字符串长度
u8 mf_puts(u8*c)
{
	return f_puts((TCHAR*)c,file);
}
//ftfs初始化
u8 mf_ftfsInit(void)
{	
	u8 res=0xff;
	exfuns_init();  			//分配内存
    res=f_mount(fs[0],"0:",1);    	//将文件系统设置到0区
    return res;
}

/*******************************************************************************
* Function Name  : u32 ReadFileChecksum(u8*path)
* Description    : 读取某一个文件的数据累加和
* Input          : None.
* Return         : None.
*******************************************************************************/
u32 ReadFileChecksum(u8*path)
{
	u32 CodeLen,Offset=0,CodeCs=0;
	u16 Cycle,left;
	if(Fm_Readfilesize(path,&CodeLen)==0)
	{
		Cycle=CodeLen/4096;
		left=CodeLen%4096;
		if(Cycle==0)
		{
			if(Fm_Readfile(path,0,left)==FR_OK)
			{
				CodeCs=BufferAdd(SectorBuffer,left);
			}
		}
		else
		{
			while(Cycle--)
			{
				if(Fm_Readfile(path,Offset,4096)==FR_OK)
				{
					CodeCs+=BufferAdd(SectorBuffer,4096);
					Offset+=4096;
				}	
			}
			if(left>0)
			{
				if(Fm_Readfile(path,Offset,left)==FR_OK)
				{
					CodeCs+=BufferAdd(SectorBuffer,left);
				}
			}
		}
	}
	return CodeCs;
}



 

