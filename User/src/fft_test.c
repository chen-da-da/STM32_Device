#include "fft_test.h"
#include "string.h"
	 

FATFS *fs[1];  		//�߼����̹�����.	 
FIL *file;	  		//�ļ�1
FIL *ftemp;	  		//�ļ�2.
UINT br,bw;			//��д����
FILINFO fileinfo;	//�ļ���Ϣ
DIR dir;  			//Ŀ¼
u8 *fatbuf;			//SD�����ݻ�����
u8 SectorBuffer[4102]; //�����ݻ�������	

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

//��SD���Ĺ̶������������
//��BANK0�е�flag�������
u8 Fm_AddData(u8*path)
{ 
	u32 filesize=0;
	u8 res=0xff,textFileBuffer[512]={0};
	memset(textFileBuffer,0xAA,1);								//ֻ��ֵ512��AA
//	exfuns_init();  			//�����ڴ�
//    f_mount(fs[0],"0:",0);    	//���ļ�ϵͳ���õ�0��
	if(mf_open(path, FA_OPEN_EXISTING | FA_READ)==FR_OK) //���ļ���
	{																	 
		 filesize=mf_size();											//��ȡ��ǰ�ļ������ݵĳ���
		 #if CFG_PRINTF
		 printf("\r\nfilesize:%d \r\n",filesize);  						//��ʾ����ǰ�ļ�����
		 #endif
		 if(mf_open(path, FA_OPEN_EXISTING | FA_WRITE)==FR_OK) //���ļ�д
		 {
		 	 mf_lseek(1);										//ָ��ָ���ı�β�����˴������ӳ������ƣ�ȷ��д��
		 	 res=mf_write(textFileBuffer, 1);  							//д��512�����ݣ�ֱ��д��SD��
			 mf_close();												//д��ر��ļ�
			 //У�������ݶԲ���;���������
			 mf_open(path, FA_OPEN_EXISTING | FA_READ);
			 filesize=mf_size();											//���µ�ǰ�ļ������ݵĳ���
			 #if CFG_PRINTF
		 	 printf("\r\nfilesize:%d \r\n",filesize);  						//��ʾ����ǰ�ļ�����
		 	 #endif
			 mf_lseek(0);													//ָ��ָ���ļ�ͷ
			 mf_read(filesize);
			 mf_close();
		 }
	}
	return res;
}

//д���ļ�������
//filesize<=4096Byte
//д�뻺�棺SectorBuffer[4096]
u8 Fm_Writefile(u8*path,u8* pbuf,u32 offset,u16 writesize)
{
	u8 res=0xff;
//	exfuns_init();  			//�����ڴ�
  //  f_mount(fs[0],"0:",0);    	//���ļ�ϵͳ���õ�0��
	if(mf_open(path, FA_OPEN_EXISTING | FA_WRITE)==FR_OK) //���ļ���
	{
		mf_lseek(offset);			   //���ļ���ƫ�Ƶ�ַ��ʼд��
		res=mf_write(pbuf, writesize); //����ܴ���512
		mf_close();
	}
	return res;	
}

//��ȡ�ļ�������
//filesize<=4096Byte
//��ȡ���棺SectorBuffer[4096]
u8 Fm_Readfile(u8*path,u32 offset,u32 filesize)
{
	u8 res=0xff;
//	exfuns_init();  			//�����ڴ�
  //  f_mount(fs[0],"0:",0);    	//���ļ�ϵͳ���õ�0��
	if(mf_open(path, FA_OPEN_EXISTING | FA_READ)==FR_OK) //���ļ���
	{
		mf_lseek(offset);			   //���ļ���ƫ�Ƶ�ַ��ʼ��ȡ
		res=mf_read(filesize);			   //��ȡfilesize�ļ��ĳ���
		mf_close();
	}
	return res;	
}

//��ȡ�ļ��ĳ���
u8 Fm_Readfilesize(u8*path,u32* filesize)
{
	u8 res=0xff;
//	exfuns_init();  			//�����ڴ�
 //   f_mount(fs[0],"0:",0);    	//���ļ�ϵͳ���õ�0��
	if(mf_open(path, FA_OPEN_EXISTING | FA_READ)==FR_OK) //���ļ���
	{
		*filesize=mf_size();
		mf_close();
		res=0x00;				//�򿪳ɹ�
	}
	return res;	
}

//����5���ļ��У�BANK0~BANK4
//BANK0~BANK3������һ��Flag.txt+BIN.txt+CFG.txt+CFGBAK.txt+Other.txt
//PUBLICBANK����һ��FTAPP+CRAPP
u8 Fm_CreatAllFile(void)
{
	u8 path_num=0,res=0x00;
//	res=exfuns_init();  			//�����ڴ�
//   res=f_mount(fs[0],"0:",0);    	//���ļ�ϵͳ���õ�0��
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


//����5���ļ��У�BANK0~BANK4
//BANK0~BANK3������һ��Flag.txt+BIN.txt+CFG.txt+CFGBAK.txt+Other.txt
//PUBLICBANK����һ��FTAPP+CRAPP
u8 Fm_CreatFile(u8*pathfile,u8*path)
{
	 u8 res=0xFF;
//	 exfuns_init();  			//�����ڴ�
//     f_mount(fs[0],"0:",0);    	//���ļ�ϵͳ���õ�0��
	 mf_mkdir(pathfile);									//�����ļ���
	 res=mf_open(path, FA_OPEN_ALWAYS | FA_WRITE);
	 if(res==FR_OK)		//�ļ����´����ļ�bin1.bin
	 {
	 	#if CFG_PRINTF
	 	printf("\r\n �����ɹ�\r\n");
	 	#endif
	 }
	 mf_close(); 
	 return res;	 
}

//ɾ�������ļ�
//ɾ��BANK0~BANK3�е�Flag.txt+BIN.txt+CFG.txt+CFGBAK.txt+Other.txt
//����ɾ��PUBLICBANKFTAPP+CRAPP
u8 Fm_DeleteAllFile(void)
{
	u8 path_num,res=0x00;
//	exfuns_init();  			//�����ڴ�
//   f_mount(fs[0],"0:",0);    	//���ļ�ϵͳ���õ�0��
	for(path_num=0;path_num<22;path_num++)
	{
		if(mf_unlink((u8*)path[path_num])!=FR_OK)
		{
			res=0xff;							 //���󷵻ر�־
			break;	
		}			
	}
	return res;	 
}

//ɾ���ļ�
//ɾ��BANK0~BANK3�е�Flag.txt+BIN.txt+CFG.txt+CFGBAK.txt+Other.txt
//����ɾ��PUBLICBANKFTAPP+CRAPP
u8 Fm_DeleteFile(u8*path)
{
	 u8 res=0xFF;
//	 exfuns_init();  			//�����ڴ�
//     f_mount(fs[0],"0:",0);    	//���ļ�ϵͳ���õ�0��
	 res=mf_unlink(path);
	 if(res==FR_OK)				//�ļ�ɾ���ɹ�
	 {
	 	#if CFG_PRINTF
	 	printf("\r\n ɾ���ɹ�\r\n");
	 	#endif
	 } 
	 return res;	 
}

//�Ѳ��ļ����µ��ļ�
//����BANK0~BANK3�е�Flag.txt+BIN.txt+CFG.txt+CFGBAK.txt+Other.txt
//��������PUBLICBANKFTAPP+CRAPP
u8 Fm_scanFile(u8*pathfile)
{
	 u8 res=0xFF;
//	 exfuns_init();  			//�����ڴ�
//     f_mount(fs[0],"0:",0);    	//���ļ�ϵͳ���õ�0��
	 res=mf_scan_files(pathfile);
	 if(res==FR_OK)				//�ļ�ɾ���ɹ�
	 {
	 	#if CFG_PRINTF
	 	printf("\r\n �����ɹ�\r\n");
	 	#endif
	 } 
	 return res;	 
}

//��ȡSD����ָ������������
//sec�����������ַ���
//sector��С:512Byte
void SD_readsector(u32 sector)
{
	u8 *buf;
	#if CFG_PRINTF
	u16 i;
	#endif
	buf=mymalloc(512);						//�����ڴ�
	if(SD_ReadBlock(buf,sector*512,512)==0)	//��ȡ0����������
	{	
		#if CFG_PRINTF
		printf("sector: %d \r\n",sector);
		for(i=0;i<512;i++)printf("%x ",buf[i]);//��ӡ��������    
		printf("\r\nDATA ENDED\r\n");
		#endif
	}
	myfree(buf);							//�ͷ��ڴ�	
}
//������� 

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
//Ϊexfuns�����ڴ�
//����ֵ:0,�ɹ�
//1,ʧ��
u8 exfuns_init(void)
{
	fs[0]=(FATFS*)mymalloc(sizeof(FATFS));	//Ϊ����0�����������ڴ�	
	//fs[1]=(FATFS*)mymalloc(sizeof(FATFS));	//Ϊ����1�����������ڴ�
	file=(FIL*)mymalloc(sizeof(FIL));		//Ϊfile�����ڴ�
	ftemp=(FIL*)mymalloc(sizeof(FIL));		//Ϊftemp�����ڴ�
	fatbuf=(u8*)mymalloc(512);				//Ϊfatbuf�����ڴ�
	if(fs[0]&&file&&ftemp&&fatbuf)return 0;  //������һ��ʧ��,��ʧ��.
	else return 1;	
}
 
//�õ�����ʣ������
//drv:���̱��("0:"/"1:")
//total:������	 ����λKB��
//free:ʣ������	 ����λKB��
//����ֵ:0,����.����,�������
u8 exf_getfree(u8 *drv,u32 *total,u32 *free)
{
	FATFS *fs1;
	u8 res;
    u32 fre_clust=0, fre_sect=0, tot_sect=0;
    //�õ�������Ϣ�����д�����
    res =(u32)f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect=(fs1->n_fatent-2)*fs1->csize;	//�õ���������
	    fre_sect=fre_clust*fs1->csize;			//�õ�����������	   
#if FF_MAX_SS!=512				  				//������С����512�ֽ�,��ת��Ϊ512�ֽ�
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		*total=tot_sect>>1;	//��λΪKB
		*free=fre_sect>>1;	//��λΪKB 
 	}
	return res;
}

//��·���µ��ļ�
//path:·��+�ļ���
//mode:��ģʽ
//����ֵ:ִ�н��
u8 mf_open(u8*path,u8 mode)
{
	u8 res;	 
	res=f_open(file,(const TCHAR*)path,mode);//���ļ���
	return res;
} 


//�ر��ļ�
//����ֵ:ִ�н��
u8 mf_close(void)
{
	f_close(file);
	return 0;
}

//ͬ���ļ�
//FF_FS_READONLY==0
//��д����������д��SD
//����Close
//Ƶ��д�뽨��ʹ��
u8 mf_sync(void)
{
	u8 res;
	res=f_sync(file);
	return res;	
} 

//��������
//len:�����ĳ���
//����ֵ:ִ�н��
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
		if(res)	//�����ݳ�����
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
	if(tlen)printf("\r\nReaded data len:%d\r\n",tlen);//���������ݳ���
	printf("Read data over\r\n");	
	#endif
	return res;
}

//д������
//dat:���ݻ�����
//len:д�볤��
//����ֵ:ִ�н��
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

//��Ŀ¼
 //path:·��
//����ֵ:ִ�н��
u8 mf_opendir(u8* path)
{
	return f_opendir(&dir,(const TCHAR*)path);	
}
//�ر�Ŀ¼ 
//����ֵ:ִ�н��
u8 mf_closedir(void)
{
	return f_closedir(&dir);	
}


 //�����ļ�
 //path:·��
 //����ֵ:ִ�н��
u8 mf_scan_files(u8 * path)
{
	FRESULT res;
		  
    char *fn;   /* This function is assuming non-Unicode cfg. */
#if FF_USE_LFN
 	fileinfo.lfsize = _MAX_LFN * 2 + 1;
	fileinfo.lfname = mymalloc(fileinfo.lfsize);
#endif		  

    res = f_opendir(&dir,(const TCHAR*)path); //��һ��Ŀ¼
    if (res == FR_OK) 
	{	
		#if CFG_PRINTF
		printf("\r\n"); 
		#endif
		while(1)
		{
	        res = f_readdir(&dir, &fileinfo);                   //��ȡĿ¼�µ�һ���ļ�
	        if (res != FR_OK || fileinfo.fname[0] == 0) break;  //������/��ĩβ��,�˳�
	        //if (fileinfo.fname[0] == '.') continue;             //�����ϼ�Ŀ¼
#if FF_USE_LFN
        	fn = *fileinfo.lfname ? fileinfo.lfname : fileinfo.fname;
#else							   
        	fn = fileinfo.fname;
#endif	                                              /* It is a file. */
			#if CFG_PRINTF
			printf("%s/", path);//��ӡ·��	
			printf("%s\r\n",  fn);//��ӡ�ļ���	
			#endif
		} 
    }	  
	//myfree(fileinfo.lfname);
	myfree(fileinfo.fname);
    return res;	  
}

//���ȡ�ļ���
//����ֵ:ִ�н��
u8 mf_readdir(void)
{
	u8 res;
	char *fn;			 
#if FF_USE_LFN
 	fileinfo.lfsize = FF_MAX_LFN * 2 + 1;
	fileinfo.lfname = mymalloc(fileinfo.lfsize);
#endif		  
	res=f_readdir(&dir,&fileinfo);//��ȡһ���ļ�����Ϣ
	if(res!=FR_OK||fileinfo.fname[0]==0)
	{
		myfree(fileinfo.fname);
		return res;//������.
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

//��ʾʣ������
//drv:�̷�
//����ֵ:ʣ������(�ֽ�)
u32 mf_showfree(u8 *drv)
{
	FATFS *fs1;
	u8 res;
    u32 fre_clust=0, fre_sect=0, tot_sect=0;
    //�õ�������Ϣ�����д�����
    res = f_getfree((const TCHAR*)drv,(DWORD*)&fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect = (fs1->n_fatent - 2) * fs1->csize;//�õ���������
	    fre_sect = fre_clust * fs1->csize;			//�õ�����������	   
#if FF_MAX_SS!=512
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		if(tot_sect<20480)//������С��10M
		{	
		    /* Print free space in unit of KB (assuming 512 bytes/sector) */
		    #if CFG_PRINTF
		    printf("\r\n����������:%d KB\r\n"
		           "���ÿռ�:%d KB\r\n",
		           tot_sect>>1,fre_sect>>1);
		           #endif
		}else
		{
		    /* Print free space in unit of KB (assuming 512 bytes/sector) */
		    #if CFG_PRINTF
		    printf("\r\n����������:%d MB\r\n"
		           "���ÿռ�:%d MB\r\n",
		           tot_sect>>11,fre_sect>>11);
		           #endif
		}
	}
	return fre_sect;
}


//�ļ���дָ��ƫ��
//offset:����׵�ַ��ƫ����
//����ֵ:ִ�н��.
u8 mf_lseek(u32 offset)
{
	return f_lseek(file,offset);
}
//��ȡ�ļ���ǰ��дָ���λ��.
//����ֵ:λ��
u32 mf_tell(void)
{
	return f_tell(file);
}
//��ȡ�ļ���С
//����ֵ:�ļ���С
u32 mf_size(void)
{
	return f_size(file);
}

//����Ŀ¼
//pname:Ŀ¼·��+����
//����ֵ:ִ�н��
u8 mf_mkdir(u8*pname)
{
	return f_mkdir((const TCHAR *)pname);
}

//��ʽ��
//path:����·��������"0:"��"1:"
//mode:ģʽ
//au:�ش�С,Ĭ�Ͽ�д0
//����ֵ:ִ�н��
u8 mf_fmkfs(u8* path,u8* work ,u32 au)
{
//	return 0;//f_mkfs((const TCHAR*)path,mode,au);//��ʽ��,drv:�̷�;mode:ģʽ;au:�ش�С
	return 	f_mkfs((const TCHAR* )path, FM_FAT, au, work,512);
} 
//ɾ���ļ�/Ŀ¼
//pname:�ļ�/Ŀ¼·��+����
//����ֵ:ִ�н��
u8 mf_unlink(u8 *pname)
{
	return  f_unlink((const TCHAR *)pname);
}

//�޸��ļ�/Ŀ¼����(���Ŀ¼��ͬ,�������ƶ��ļ�Ŷ!)
//oldname:֮ǰ������
//newname:������
//����ֵ:ִ�н��
u8 mf_rename(u8 *oldname,u8* newname)
{
	return  f_rename((const TCHAR *)oldname,(const TCHAR *)newname);
}

//��ȡ�̷����������֣�
//path:����·��������"0:"��"1:"  
void mf_getlabel(u8 *path)
{
	u8 buf[20]={0};
	u32 sn=0;
	u8 res;
	res=f_getlabel ((const TCHAR *)path,(TCHAR *)buf,(DWORD*)&sn);
	if(res==FR_OK)
	{
		#if CFG_PRINTF
		printf("\r\n����%s ���̷�Ϊ:%s\r\n",path,buf);
		printf("����%s �����к�:%X\r\n\r\n",path,sn); 
		#endif
	}else 
	{
		#if CFG_PRINTF
		printf("\r\n��ȡʧ�ܣ�������:%X\r\n",res);
		#endif
	}
}

//�����̷����������֣����11���ַ�������֧�����ֺʹ�д��ĸ����Լ����ֵ�
//path:���̺�+���֣�����"0:ALIENTEK"��"1:OPENEDV"  
void mf_setlabel(u8 *path)
{
	u8 res;
	res=f_setlabel ((const TCHAR *)path);
	if(res==FR_OK)
	{
		#if CFG_PRINTF
		printf("\r\n�����̷����óɹ�:%s\r\n",path);
		#endif
	}else
	{
		#if CFG_PRINTF
		printf("\r\n�����̷�����ʧ�ܣ�������:%X\r\n",res);
		#endif
	}
} 

//���ļ������ȡһ���ַ���
//size:Ҫ��ȡ�ĳ���
void mf_gets(u16 size)
{
 	TCHAR* rbuf;
	rbuf=f_gets((TCHAR*)fatbuf,size,file);
	if(*rbuf==0)return  ;//û�����ݶ���
	else
	{
		#if CFG_PRINTF
		printf("\r\nThe String Readed Is:%s\r\n",rbuf); 
		#endif
	}			    	
}

//��Ҫ_USE_STRFUNC>=1
//дһ���ַ����ļ�
//c:Ҫд����ַ�
//����ֵ:ִ�н��
u8 mf_putc(u8 c)
{
	return f_putc((TCHAR)c,file);
}
//д�ַ������ļ�
//c:Ҫд����ַ���
//����ֵ:д����ַ�������
u8 mf_puts(u8*c)
{
	return f_puts((TCHAR*)c,file);
}
//ftfs��ʼ��
u8 mf_ftfsInit(void)
{	
	u8 res=0xff;
	exfuns_init();  			//�����ڴ�
    res=f_mount(fs[0],"0:",1);    	//���ļ�ϵͳ���õ�0��
    return res;
}

/*******************************************************************************
* Function Name  : u32 ReadFileChecksum(u8*path)
* Description    : ��ȡĳһ���ļ��������ۼӺ�
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



 

