/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: Fat Fs Spi Hal Implementation
 * Author: Huawei LiteOS Team
 * Create: 2013-01-01
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */

/* Includes ----------------------------------------------------------------- */
#include <stdio.h>
#include <string.h>

#if defined(__GNUC__) || defined(__CC_ARM)
#include "fcntl.h"
#include <los_printf.h>
#endif

#include "fs/los_vfs.h"
#include "fs/los_fatfs.h"
#include "los_hwi.h"
#include "hal_spi_flash.h"

#define SPI_FLASH_ID             0xEF4019
#define SPI_FLASH_SECTOR_SIZE    (4 * 1024)

static DSTATUS Stm32f4FatfsStatus(BYTE lun)
{
    DSTATUS status = STA_NOINIT;

    if (hal_spi_flash_get_id() == SPI_FLASH_ID) {
        status &= ~STA_NOINIT;
    }
    return status;
}

static DSTATUS Stm32f4FatfsInit(BYTE lun)
{
    DSTATUS status = STA_NOINIT;
    hal_spi_flash_config();
    hal_spi_flash_wake_up();
    status = Stm32f4FatfsStatus(lun);
    return status;
}

static DRESULT Stm32f4FatfsRead(BYTE lun, BYTE *buff, DWORD sector, UINT count)
{
    int ret;
    ret = hal_spi_flash_read(buff, count * SPI_FLASH_SECTOR_SIZE, FF_PHYS_ADDR + sector * SPI_FLASH_SECTOR_SIZE);
    if (ret != 0) {
        return RES_ERROR;
    }
    return RES_OK;
}

static DRESULT Stm32f4FatfsWrite(BYTE lun, const BYTE *buff, DWORD sector, UINT count)
{
    int ret;
    ret = hal_spi_flash_erase_write(buff, count * SPI_FLASH_SECTOR_SIZE, FF_PHYS_ADDR + sector * SPI_FLASH_SECTOR_SIZE);
    if (ret != 0) {
        return RES_ERROR;
    }
    return RES_OK;
}

static DRESULT Stm32f4FatfsIoctl(BYTE lun, BYTE cmd, void *buff)
{
    DRESULT res = RES_PARERR;

    switch (cmd) {
        case GET_SECTOR_COUNT:
            *(DWORD *)buff = FF_PHYS_SIZE / SPI_FLASH_SECTOR_SIZE;
            break;
        case GET_SECTOR_SIZE:
            *(WORD *)buff = SPI_FLASH_SECTOR_SIZE;
            break;
        case GET_BLOCK_SIZE:
            *(DWORD *)buff = 1;
            break;
        default:
            break;
    }
    res = RES_OK;
    return res;
}

static struct diskio_drv g_fatfsConfig = {
    Stm32f4FatfsInit,
    Stm32f4FatfsStatus,
    Stm32f4FatfsRead,
    Stm32f4FatfsWrite,
    Stm32f4FatfsIoctl
};

DWORD get_fattime(void)
{
    return 0;
}

struct diskio_drv* FatfsConfigGet(void)
{
    return &g_fatfsConfig;
}

void FatfsDriverInit(int needErase)
{
    if (needErase) {
        hal_spi_flash_config();
        hal_spi_flash_erase(FF_PHYS_ADDR, FF_PHYS_SIZE);
    }
}

/* Private functions -------------------------------------------------------- */
