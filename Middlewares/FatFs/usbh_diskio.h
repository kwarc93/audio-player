/*
 * usbh_diskio.h
 *
 *  Created on: 30.09.2017
 *      Author: Kwarc
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBH_DISKIO_H
#define __USBH_DISKIO_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Disk IO Driver structure definition
  */
typedef struct
{
  DSTATUS (*disk_initialize) (BYTE);                     		/*!< Initialize Disk Drive                     */
  DSTATUS (*disk_status)     (BYTE);                     		/*!< Get Disk Status                           */
  DRESULT (*disk_read)       (BYTE, BYTE*, DWORD, UINT);       	/*!< Read Sector(s)                            */
  DRESULT (*disk_write)      (BYTE, const BYTE*, DWORD, UINT); 	/*!< Write Sector(s) when _USE_WRITE = 0       */
  DRESULT (*disk_ioctl)      (BYTE, BYTE, void*);              	/*!< I/O control operation when _USE_IOCTL = 1 */

}ff_diskio_drv_t;

extern const ff_diskio_drv_t  USBH_Driver;

#endif /* __USBH_DISKIO_H */


