/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2021. All rights reserved.
 * Description: Sota Implementation
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

#include "nb_demo.h"
#include "stdio.h"
#include "los_swtmr.h"
#include "osdepends/atiny_osdep.h"
#include "sota/sota.h"
#include "sota/sota_hal.h"
#include "ota_port.h"
#include "hal_spi_flash.h"
#include "at_main.h"

#define DEVICE_VER         "V0.0"
#define SOTA_BUF_LEN       (512+16)
#define SOTA_MAX_TIME_OUT  5000

extern int nb_send_str(const char* buf, int len);

static int8_t g_demoSoftBuf[SOTA_BUF_LEN];
static uint16_t g_demoSotaTimer;

int ReadVerson(char* buf, uint32_t len)
{
    (void)memcpy_s(buf, strlen(DEVICE_VER), DEVICE_VER, strlen(DEVICE_VER));
    return 0;
}

static int32_t SotaCmdMatch(const char *buf, char* featurestr, int len)
{
    if (strstr(buf, featurestr) != NULL) {
        if (sota_parse((const int8_t *)buf, len, g_demoSoftBuf, SOTA_BUF_LEN) == SOTA_OK) {
            return 0;
        }
    }
    return -1;
}

static int32_t SotaCallback(void *arg, int8_t* buf, int32_t buflen)
{
    int ret;
    ret = sota_process(arg, g_demoSoftBuf, buflen);

    switch (ret) {
        case SOTA_UPDATED: {
            LOS_SwtmrStop(g_demoSotaTimer);
            atiny_reboot();
            break;
        }
        case SOTA_DOWNLOADING:
        case SOTA_UPDATING: {
            LOS_SwtmrStart(g_demoSotaTimer);
            break;
        }
        case SOTA_EXIT:
        case SOTA_WRITE_FLASH_FAILED: {
            LOS_SwtmrStop(g_demoSotaTimer);
            break;
        }
        default:
            break;
    }
    return 0;
}

#define LOG_BUF_SIZE (256)
int SotaLog(const char *fmt, ...)
{
    int ret;
    char str_buf[LOG_BUF_SIZE] = {0};
    va_list list;

    memset(str_buf, 0, LOG_BUF_SIZE);
    va_start(list, fmt);
    ret = vsnprintf(str_buf, LOG_BUF_SIZE, fmt, list);
    va_end(list);

    printf("%s", str_buf);

    return ret;
}

void NBIoT_SotaDemo(void)
{
    sota_arg_s flash_op = {
       .get_ver = ReadVerson,
       .sota_send = nb_send_str,
       .sota_malloc = at_malloc,
       .sota_printf = sota_log,
       .sota_free = at_free,
    };
    hal_get_ota_opt(&flash_op.ota_info);
    flash_op.ota_info.key.rsa_N = "C94BECB7BCBFF459B9A71F12C3CC0603B11F0D3A366A226FD3E"
                                  "73D453F96EFBBCD4DFED6D9F77FD78C3AB1805E1BD3858131AC"
                                  "B5303F61AF524F43971B4D429CB847905E68935C1748D0096C1"
                                  "A09DD539CE74857F9FDF0B0EA61574C5D76BD9A67681AC6A9DB"
                                  "1BB22F17120B1DBF3E32633DCE34F5446F52DD7335671AC3A1F"
                                  "21DC557FA4CE9A4E0E3E99FED33A0BAA1C6F6EE53EDD742284D"
                                  "6582B51E4BF019787B8C33C2F2A095BEED11D6FE68611BD0082"
                                  "5AF97DB985C62C3AE0DC69BD7D0118E6D620B52AFD514AD5BFA"
                                  "8BAB998332213D7DBF5C98DC86CB8D4F98A416802B892B8D6BE"
                                  "E5D55B7E688334B281E4BEDDB11BD7B374355C5919BA5A9A1C9"
                                  "1F";
    flash_op.ota_info.key.rsa_E = "10001";
    hal_init_ota();
#if (LOSCFG_BASE_CORE_SWTMR_ALIGN == YES)
    LOS_SwtmrCreate(SOTA_MAX_TIME_OUT, LOS_SWTMR_MODE_NO_SELFDELETE, (SWTMR_PROC_FUNC)sota_timeout_handler,
                    &g_demoSotaTimer, 1, OS_SWTMR_ROUSES_ALLOW, OS_SWTMR_ALIGN_SENSITIVE);
#else
    LOS_SwtmrCreate(SOTA_MAX_TIME_OUT, LOS_SWTMR_MODE_NO_SELFDELETE, (SWTMR_PROC_FUNC)sota_timeout_handler, &g_demoSotaTimer, 1);
#endif
    sota_init(&flash_op);
    (void)at.oob_register("+NNMI:", strlen("+NNMI:"), SotaCallback, SotaCmdMatch);
}
