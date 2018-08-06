/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#include "stm32f10x.h"
#include "spi_sd.h"

/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */

#define BLOCK_SIZE            512 /* Block Size in Bytes */
/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
		SD_Error  Status;
	/* Supports only single drive */
	if (pdrv)
	{
		return STA_NOINIT;
	}
/*-------------------------- SD Init ----------------------------- */
  	Status = SD_Init();
	if (Status!=SD_RESPONSE_NO_ERROR )
	{
		return STA_NOINIT;
	}
	else
	{
		return RES_OK;
	}
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	if (count > 1)
	{
		SD_ReadMultiBlocks(buff, sector, BLOCK_SIZE, count); //*BLOCK_SIZE
	
	    /* Wait until end of DMA transfer */
//	    while(SD_GetStatus() != SD_RESPONSE_NO_ERROR);

	}
	else
	{
		
		SD_ReadBlock(buff, sector, BLOCK_SIZE);	  //*BLOCK_SIZE;2G卡是Adress操作，8G是扇区操作

	    /* Wait until end of DMA transfer */
//	    while(SD_GetStatus() != SD_RESPONSE_NO_ERROR);

	}
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	if (count > 1)
	{
		SD_WriteMultiBlocks((uint8_t *)buff, sector, BLOCK_SIZE, count);  // *BLOCK_SIZE
		
//	    while(SD_GetStatus() != SD_RESPONSE_NO_ERROR); //等待sdio到sd卡传输结束
	}
	else
	{
		SD_WriteBlock((uint8_t *)buff,sector, BLOCK_SIZE);		//	*BLOCK_SIZE
		
//	    while(SD_GetStatus() != SD_RESPONSE_NO_ERROR); //等待sdio到sd卡传输结束
	}
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	switch(cmd)
		{
			case CTRL_SYNC:
				res=RES_OK;
				break;
			case GET_SECTOR_SIZE:
				*(DWORD*)buff=512;
				res=RES_OK;
				break;
			case GET_BLOCK_SIZE:   		//获取擦除块大小（以扇区为单位）
				*(DWORD*)buff=1;
				res=RES_OK;
				break;
			case GET_SECTOR_COUNT:
				*(DWORD*)buff =SD_GetSectorCount();
				res=RES_OK;
				break;
		  default:
		    res = RES_PARERR;
        break;
				
		}
	return res;
}


/*-----------------------------------------------------------------------*/
/* Get current time                                                      */
/*-----------------------------------------------------------------------*/ 
DWORD get_fattime(void)
{

 	return 0;

} 



