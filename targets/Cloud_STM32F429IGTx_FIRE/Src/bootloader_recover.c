/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: Targets Stm32f429 Src Bootloader Recover
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "hal_flash.h"
#include "hal_spi_flash.h"
#include "usart.h"
#include "board_ota.h"
#include "recover_image.h"

void SysTick_Handler(void)
{
    HAL_IncTick();
}

void _Error_Handler(char *file, int line)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /** Configure the main internal regulator output voltage
     */
    __HAL_RCC_PWR_CLK_ENABLE();

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /** Initializes the CPU, AHB and APB busses clocks
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = 16;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 180;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 4;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        _Error_Handler(__FILE__, __LINE__);
    }

    /** Activate the Over-Drive mode
    */
    if (HAL_PWREx_EnableOverDrive() != HAL_OK) {
        _Error_Handler(__FILE__, __LINE__);
    }

    /** Initializes the CPU, AHB and APB busses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
        _Error_Handler(__FILE__, __LINE__);
    }

    SystemCoreClockUpdate();
}

static int flash_read(flash_type_e flash_type, void *buf, int32_t len, uint32_t offset)
{
    switch (flash_type) {
        case FLASH_OLDBIN_READ:
            return hal_flash_read(buf, len, OTA_DEFAULT_IMAGE_ADDR + offset);
        case FLASH_PATCH:
            return hal_spi_flash_read(buf, len, OTA_IMAGE_DOWNLOAD_ADDR + offset);
        case FLASH_UPDATE_INFO:
            return hal_spi_flash_read(buf, len, OTA_FLAG_ADDR1);
        default:
            printf("wrong flash type detected %d\n", flash_type);
            return -1;
    }
}

static int flash_write(flash_type_e flash_type, const void *buf, int32_t len, uint32_t offset)
{
    switch (flash_type) {
        case FLASH_NEWBIN_WRITE:
            return hal_spi_flash_erase_write(buf, len, OTA_IMAGE_DIFF_UPGRADE_ADDR + offset);
        case FLASH_PATCH:
            return hal_spi_flash_erase_write(buf, len, OTA_IMAGE_DOWNLOAD_ADDR + offset);
        case FLASH_UPDATE_INFO:
            return hal_spi_flash_erase_write(buf, len, OTA_FLAG_ADDR1);
        default:
            return -1;
    }
}

static int register_info(void)
{
    recover_info_s info;
    recover_assist_s assist;
    recover_flash_s flash;

    info.max_new_image_size = OTA_IMAGE_DIFF_UPGRADE_SIZE;
    info.max_old_image_size = OTA_IMAGE_DIFF_UPGRADE_SIZE;
    info.max_patch_size = OTA_IMAGE_DOWNLOAD_SIZE;
    info.old_image_addr = OTA_DEFAULT_IMAGE_ADDR;
    info.new_image_addr = OTA_IMAGE_DIFF_UPGRADE_ADDR;
    info.patch_addr = OTA_IMAGE_DOWNLOAD_ADDR;
    info.flash_erase_unit = 0x1000;
    info.recover_on_oldimage = 0;

    assist.func_printf = printf;
    assist.func_malloc = malloc;
    assist.func_free = free;

    flash.func_flash_read = flash_read;
    flash.func_flash_write = flash_write;

    return recover_init(&info, &assist, &flash);
}

static int jump(uint32_t oldbin_size)
{
    int ret;

    printf("info: begin to jump to application\n");
    ret = jumpToApp();
    if (ret != 0) {
        printf("warning: jump to app failed, try to roll back now\n");
        (void)recover_set_update_fail();
        ret = ImgRollbackCopy(oldBinSize);
        if (ret != 0) {
            printf("fatal: roll back failed, system start up failed\n");
            _Error_Handler(__FILE__, __LINE__);
        }
    }
    printf("info: begin to try to jump to application again\n");
    ret = jumpToApp();
    if (ret != 0) {
        printf("fatal: roll back succeed, system start up failed\n");
        _Error_Handler(__FILE__, __LINE__);
    }

    return ret;
}

int main(void)
{
    int ret;
    recover_upgrade_type_e upgradeType = RECOVER_UPGRADE_NONE;
    uint32_t newBinSize = 0;
    uint32_t oldBinSize = 0;

    SystemClock_Config();  // system clock initialize
    MX_USART1_UART_Init(); // usart1 initialize
    hal_spi_flash_config(); // spi flash initialize

    printf("bootloader begin\n");

    ret = register_info();
    if (ret != 0) {
        printf("warning: recover register failed\n");
    }

    printf("info: begin to process upgrade\n");
    ret = recover_image(&upgrade_type, &newbin_size, &oldbin_size);
    if (oldBinSize == 0) {
        oldBinSize = OTA_IMAGE_DOWNLOAD_SIZE;
    }
    if (ret == 0) {
        switch (upgradeType) {
            case RECOVER_UPGRADE_NONE:
                printf("info: normal start up\n");
                break;
            case RECOVER_UPGRADE_FULL:
                printf("info: full upgrade\n");
                ret = ImgUpdateCopy(oldBinSize, newBinSize, OTA_IMAGE_DOWNLOAD_ADDR);
                if (ret != 0) {
                    printf("warning: [full] copy newimage to inner flash failed\n");
                    (void)recover_set_update_fail();
                }
                break;
            case RECOVER_UPGRADE_DIFF:
                printf("info: diff upgrade\n");
                ret = ImgUpdateCopy(oldBinSize, newBinSize, OTA_IMAGE_DIFF_UPGRADE_ADDR);
                if (ret != 0) {
                    printf("warning: [diff] copy newimage to inner flash failed\n");
                    (void)recover_set_update_fail();
                }
                break;
            default:
                break;
        }
    } else {
        printf("warning: upgrade failed with ret %d\n", ret);
    }

    ret = jump(oldBinSize);

    return ret;
}
