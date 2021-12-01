/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Main Process
 * Author: Huawei LiteOS Team
 * Create: 2021-12-01
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

#include "main.h"
#include "sys_init.h"
#include "los_base.h"
#include "los_task_pri.h"
#include "los_typedef.h"
#include "los_sys.h"

#include "fsl_port.h"

#include "los_tick_pri.h"

void HardwareInit(void)
{
    // Set Port B Pin 39/40 as UART0 RX/TX
    CLOCK_EnableClock(kCLOCK_PortB);
    PORT_SetPinMux(PORTB, 16U, kPORT_MuxAlt3);  /* PORTB16 (pin 39) is configured as UART0_RX */
    PORT_SetPinMux(PORTB, 17U, kPORT_MuxAlt3);  /* PORTB17 (pin 40) is configured as UART0_TX */

    // Set Port C Pin 46/49 as UART1 RX/TX
    CLOCK_EnableClock(kCLOCK_PortC);
    PORT_SetPinMux(PORTC, 3U, kPORT_MuxAlt3);   /* PORTC3 (pin 46) is configured as UART1_RX */
    PORT_SetPinMux(PORTC, 4U, kPORT_MuxAlt3);   /* PORTC4 (pin 49) is configured as UART1_TX */

    SystemClock_Config();
    uart_early_init();

    g_sys_mem_addr_end = __LOS_HEAP_ADDR_END__;
    dwt_delay_init(SystemCoreClock);
}

int32_t main(void)
{
    OsSetMainTask();
    OsCurrTaskSet(OsGetMainTask());

    HardwareInit();

    PRINT_RELEASE("\n********Hello Huawei LiteOS********\n"
                    "\nLiteOS Kernel Version : %s\n"
                    "Processor : %s (@ %d Mhz)\n"
                    "Build date : %s %s\n\n"
                    "**********************************\n",
                    HW_LITEOS_KERNEL_VERSION_STRING,
		    LOS_CpuInfo(), (SystemCoreClock / 1000000),
		    __DATE__, __TIME__);

    uint32_t ret = OsMain();
    if (ret != LOS_OK) {
        return LOS_NOK;
    }

    OsStart();

    return LOS_OK;
}
