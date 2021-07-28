/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: ARM generic timer.
 * Author: Huawei LiteOS Team
 * Create: 2018-10-06
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

#include "los_tick_pri.h"
#include "gic_common.h"
#include "arch/regs.h"

#define STRING_COMB(x, y, z)        x ## y ## z

#ifdef LOSCFG_ARCH_SECURE_MONITOR_MODE
#define TIMER_REG(reg)              STRING_COMB(TIMER_REG_, CNTPS, reg)
#else
#define TIMER_REG(reg)              STRING_COMB(TIMER_REG_, CNTP, reg)
#endif
#define TIMER_REG_CTL               TIMER_REG(_CTL)     /* 32 bits */
#define TIMER_REG_TVAL              TIMER_REG(_TVAL)    /* 32 bits */
#define TIMER_REG_CVAL              TIMER_REG(_CVAL)    /* 64 bits */
#define TIMER_REG_CT                TIMER_REG(CT)       /* 64 bits */

#ifdef __LP64__

#define TIMER_REG_CNTFRQ            cntfrq_el0

/* CNTP AArch64 registers */
#define TIMER_REG_CNTP_CTL          cntp_ctl_el0
#define TIMER_REG_CNTP_TVAL         cntp_tval_el0
#define TIMER_REG_CNTP_CVAL         cntp_cval_el0
#define TIMER_REG_CNTPCT            cntpct_el0

/* CNTPS AArch64 registers */
#define TIMER_REG_CNTPS_CTL         cntps_ctl_el1
#define TIMER_REG_CNTPS_TVAL        cntps_tval_el1
#define TIMER_REG_CNTPS_CVAL        cntps_cval_el1
#define TIMER_REG_CNTPSCT           cntpct_el0

#define READ_TIMER_REG32(reg)       AARCH64_SYSREG_READ(reg)
#define READ_TIMER_REG64(reg)       AARCH64_SYSREG_READ(reg)
#define WRITE_TIMER_REG32(reg, val) AARCH64_SYSREG_WRITE(reg, (UINT64)(val))
#define WRITE_TIMER_REG64(reg, val) AARCH64_SYSREG_WRITE(reg, val)

#else /* Aarch32 */

#define TIMER_REG_CNTFRQ            CP15_REG(c14, 0, c0, 0)

/* CNTP AArch32 registers */
#define TIMER_REG_CNTP_CTL          CP15_REG(c14, 0, c2, 1)
#define TIMER_REG_CNTP_TVAL         CP15_REG(c14, 0, c2, 0)
#define TIMER_REG_CNTP_CVAL         CP15_REG64(c14, 2)
#define TIMER_REG_CNTPCT            CP15_REG64(c14, 0)

/* CNTPS AArch32 registers are banked and accessed through CNTP */
#define CNTPS CNTP

#define READ_TIMER_REG32(reg)       ARM_SYSREG_READ(reg)
#define READ_TIMER_REG64(reg)       ARM_SYSREG64_READ(reg)
#define WRITE_TIMER_REG32(reg, val) ARM_SYSREG_WRITE(reg, val)
#define WRITE_TIMER_REG64(reg, val) ARM_SYSREG64_WRITE(reg, val)

#endif

#ifdef LOSCFG_PLATFORM_RASPBERRY32_PI3B
#define OS_CYCLE_PER_TICK LOSCFG_BASE_CORE_TICK_PER_SECOND
#else
#define OS_CYCLE_PER_TICK (GET_SYS_CLOCK() / LOSCFG_BASE_CORE_TICK_PER_SECOND)
#endif
UINT32 HalClockFreqRead(VOID)
{
    return READ_TIMER_REG32(TIMER_REG_CNTFRQ);
}

VOID HalClockFreqWrite(UINT32 freq)
{
    WRITE_TIMER_REG32(TIMER_REG_CNTFRQ, freq);
}

STATIC_INLINE UINT32 TimerCtlRead(VOID)
{
    return READ_TIMER_REG32(TIMER_REG_CTL);
}

STATIC_INLINE VOID TimerCtlWrite(UINT32 cntpCtl)
{
    WRITE_TIMER_REG32(TIMER_REG_CTL, cntpCtl);
}

STATIC_INLINE UINT64 TimerCvalRead(VOID)
{
    return READ_TIMER_REG64(TIMER_REG_CVAL);
}

STATIC_INLINE VOID TimerCvalWrite(UINT64 cval)
{
    WRITE_TIMER_REG64(TIMER_REG_CVAL, cval);
}

STATIC_INLINE VOID TimerTvalWrite(UINT32 tval)
{
    WRITE_TIMER_REG32(TIMER_REG_TVAL, tval);
}

UINT64 HalClockGetCycles(VOID)
{
    UINT64 cntpct;

    cntpct = READ_TIMER_REG64(TIMER_REG_CT);
    return cntpct;
}

LITE_OS_SEC_TEXT VOID HalTickEntry(VOID)
{
    TimerCtlWrite(0);

    OsTickHandler();

    /*
     * use last cval to generate the next tick's timing is
     * absolute and accurate. DO NOT use tval to drive the
     * generic time in which case tick will be slower.
     */
    TimerCvalWrite(TimerCvalRead() + OS_CYCLE_PER_TICK);
    TimerCtlWrite(1);
}

LITE_OS_SEC_TEXT_INIT VOID HalClockInit(VOID)
{
    UINT32 ret;
    SET_SYS_CLOCK(HalClockFreqRead());
    ret = LOS_HwiCreate(OS_TICK_INT_NUM, OS_HWI_PRIO_LOWEST, 0, HalTickEntry, 0);
    if (ret != LOS_OK) {
        PRINT_ERR("%s, %d create tick irq failed, ret:0x%x\n", __FUNCTION__, __LINE__, ret);
    }
}

LITE_OS_SEC_TEXT_INIT VOID HalClockStart(VOID)
{
    (void)HalIrqUnmask(OS_TICK_INT_NUM);

    /* triggle the first tick */
    TimerCtlWrite(0);
    TimerTvalWrite(OS_CYCLE_PER_TICK);
    TimerCtlWrite(1);
}

VOID HalDelayUs(UINT32 usecs)
{
    UINT64 cycles = (UINT64)usecs * GET_SYS_CLOCK() / OS_SYS_US_PER_SECOND;
    UINT64 deadline = HalClockGetCycles() + cycles;

    while (HalClockGetCycles() < deadline) {
        __asm__ volatile ("nop");
    }
}

UINT64 hi_sched_clock(VOID)
{
    return LOS_CurrNanosec();
}

UINT32 HalClockGetTickTimerCycles(VOID)
{
    UINT64 cval = TimerCvalRead();
    UINT64 cycles = HalClockGetCycles();

    return (UINT32)((cval > cycles) ? (cval - cycles) : 0);
}

VOID HalClockTickTimerReload(UINT32 cycles)
{
    (void)HalIrqMask(OS_TICK_INT_NUM);
    (void)HalIrqClear(OS_TICK_INT_NUM);

    TimerCtlWrite(0);
    TimerCvalWrite(HalClockGetCycles() + cycles);
    TimerCtlWrite(1);

    (void)HalIrqUnmask(OS_TICK_INT_NUM);
}

UINT32 HalGetRemainingCycles(UINT32 *ticks)
{
    UINT64 cval = TimerCvalRead();
    UINT64 cycles = HalClockGetCycles();
    UINT32 remainingCycles;
    /* Stop the systick timer */
    TimerCtlWrite(0);
    /* If the tick interrupt has arrived, update tick count and sleep ticks */
    if (cval <= cycles) {
        if (ticks != NULL) {
            (*ticks)--;
        }
        remainingCycles = (OS_CYCLE_PER_TICK - (UINT32)(cycles - cval));
    } else {
        remainingCycles = (UINT32)(cval - cycles);
    }
    return remainingCycles;
}

VOID HalTickCheckAndAdjust(UINT32 ticks)
{
    UINT32 remainingCycles = HalGetRemainingCycles(&ticks);
    UINT64 sleepCycles = (UINT64)(ticks - 1) * OS_CYCLE_PER_TICK;
    HalClockTickTimerReload((UINT32)(sleepCycles + remainingCycles));
}