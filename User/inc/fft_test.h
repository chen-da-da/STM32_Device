
#ifndef _FFTTEST_H
#define	_FFTTEST_H
#include "stm32f10x.h"
//#include "spi_sd.h"
//#include "prt_usart1.h"	
#include "ff.h"
#include "malloc.h"

#define CFG_PRINTF		0

u8 Fm_CreatFile(u8*pathfile,u8*path);
u8 Fm_CreatAllFile(void);
u8 Fm_AddData(u8*path);
u8 Fm_Readfile(u8*path,u32 offset,u32 filesize);
u8 Fm_DeleteFile(u8*path);
u8 Fm_DeleteAllFile(void);
u8 Fm_scanFile(u8*pathfile);
u32 BufferAdd(u8* pBuffer,u32 BufferLength);

u8 exf_getfree(u8 *drv,u32 *total,u32 *free);
u8 mf_open(u8*path,u8 mode);
u8 mf_close(void);
u8 mf_read(u16 len);
u8 exfuns_init(void);
u8 mf_write(u8*dat,u16 len);
u8 mf_opendir(u8* path);
u8 mf_closedir(void);
u8 mf_scan_files(u8 * path);
u8 mf_readdir(void);
u32 mf_showfree(u8 *drv);
u8 mf_lseek(u32 offset);
u32 mf_tell(void);
u32 mf_size(void);
u8 mf_mkdir(u8*pname);
u8 mf_fmkfs(u8* path,u8* work ,u32 au);
u8 mf_unlink(u8 *pname);
u8 mf_rename(u8 *oldname,u8* newname);
void mf_getlabel(u8 *path);
void mf_setlabel(u8 *path);
void mf_gets(u16 size);
u8 mf_putc(u8 c);
u8 mf_puts(u8*c);
u8 mf_sync(void);
void SD_readsector(u32 sector);
u8 mf_ftfsInit(void);
u8 Fm_Readfilesize(u8*path,u32* filesize);
u32 ReadFileChecksum(u8*path);
u8 Fm_Writefile(u8*path,u8* pbuf,u32 offset,u16 writesize);

extern const char path[24][40];
extern const char filepath[5][20];
extern u8 SectorBuffer[];
extern  FATFS *fs[1];

typedef enum
{
	BANK0_FLAG,BANK0_BIN,BANK0_CFG,BANK0_CFGBAK,BANK0_OTHER,\
	BANK1_FLAG,BANK1_BIN,BANK1_CFG,BANK1_CFGBAK,BANK1_OTHER,\
	BANK2_FLAG,BANK2_BIN,BANK2_CFG,BANK2_CFGBAK,BANK2_OTHER,\
	BANK3_FLAG,BANK3_BIN,BANK3_CFG,BANK3_CFGBAK,BANK3_OTHER,\
	PUBANK_FTAPP,PUBANK_CRAPP,PUBANK_FTCFG,PUBANK_CRCFG
}epath;

#endif
