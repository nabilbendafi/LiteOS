/*----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: Main
 * Author: Huawei LiteOS Team
 * Create: 2020-12-10
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

#include "los_task_pri.h"
#include "arch/canary.h"
#include "los_swtmr_pri.h"
#include "los_config.h"
#include "los_printf.h"
#include "los_atomic.h"
#include "gic_common.h"
#include "uart.h"

Atomic ncpu = 1;

LITE_OS_SEC_TEXT_INIT void osSystemInfo(void)
{
    PRINT_RELEASE("\n********Hello Huawei LiteOS********\n\n"
	        "LiteOS Kernel Version : %s\n"
            "Processor   : %s"
#ifdef LOSCFG_KERNEL_SMP
            " * %d\n"
            "Run Mode    : SMP\n"
#else
            "\n"
            "Run Mode    : UP\n"
#endif
            "GIC Rev     : %s\n"
            "build time  : %s %s\n\n"
            "**********************************\n",
			HW_LITEOS_KERNEL_VERSION_STRING,
            LOS_CpuInfo(),
#ifdef LOSCFG_KERNEL_SMP
            LOSCFG_KERNEL_SMP_CORE_NUM,
#endif
            HalIrqVersion(), __DATE__,__TIME__);
}

LITE_OS_SEC_TEXT_INIT int secondary_cpu_start(void)
{
    OsCurrTaskSet(OsGetMainTask());

    /* increase cpu counter */
    LOS_AtomicInc(&ncpu);

    HalIrqInitPercpu();
#ifdef LOSCFG_BASE_CORE_SWTMR
    OsSwtmrInit();
#endif
    OsIdleTaskCreate();
    OsStart();

    while(1){
        __asm volatile("wfi");
    }
}

#ifdef LOSCFG_KERNEL_SMP
LITE_OS_SEC_TEXT_INIT VOID release_secondary_cores(void)
{
    /* send SGI to wakeup APs */
    (void)HalIrqSendIpi(0x00, 0x0F);
    *(int*)SYS_FLAGSSET = 0x10000;

    /* wait until all APs are ready */
    while (LOS_AtomicRead(&ncpu) < LOSCFG_KERNEL_CORE_NUM) {
        asm volatile("wfe");
    }
}
#endif

LITE_OS_SEC_TEXT_INIT int main(void)
{
    UINT32 ret = LOS_OK;

#ifdef __GNUC__
    ArchStackGuardInit();
#endif
    OsSetMainTask();
    OsCurrTaskSet(OsGetMainTask());

    /* early init uart output */
    uart_early_init();

    /* system and chip info */
    osSystemInfo();

    PRINTK("\nmain core booting up...\n");
    ret = OsMain();
    if (ret != LOS_OK) {
        return LOS_NOK;
    }

#ifdef LOSCFG_KERNEL_SMP
    PRINTK("releasing %u secondary cores\n", LOSCFG_KERNEL_SMP_CORE_NUM - 1);
    release_secondary_cores();
#endif

    OsStart();

    while (1) {
        __asm volatile("wfi");
    }
}