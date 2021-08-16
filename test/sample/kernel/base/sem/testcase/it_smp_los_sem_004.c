/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Sem Test Case
 * Author: Huawei LiteOS Team
 * Create: 2021-06-02
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
 * HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ---------------------------------------------------------------------------
 */
 
#include "it_los_sem.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#ifdef LOSCFG_KERNEL_SMP
static VOID TaskF01(VOID)
{
    UINT32 ret;

    ICUNIT_ASSERT_EQUAL_VOID(g_testCount, 0, g_testCount);

    ret = LOS_SemPend(g_usSemID, LOS_WAIT_FOREVER);
    ICUNIT_ASSERT_EQUAL_VOID(ret, LOS_OK, ret);

    TestDumpCpuid();
    LOS_AtomicInc(&g_testCount);

    return;
}

static VOID TaskF02(VOID)
{
    UINT32 ret;

    ret = LOS_SemDelete(g_usSemID);
    ICUNIT_ASSERT_EQUAL_VOID(ret, LOS_OK, ret);

    TestDumpCpuid();
    LOS_AtomicInc(&g_testCount);

    return;
}

static UINT32 TestCase(VOID)
{
    UINT32 ret, otherCpuid;
    TSK_INIT_PARAM_S testTask;

    g_testCount = 0;

    ret = LOS_SemCreate(0, &g_usSemID);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    ret = LOS_SemPost(g_usSemID);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT1);

    otherCpuid = (ArchCurrCpuid() + 1) % (LOSCFG_KERNEL_CORE_NUM);

    TEST_TASK_PARAM_INIT_AFFI(testTask, "smp_sem_tsk004A", TaskF01,
        TASK_PRIO_TEST - 1, CPUID_TO_AFFI_MASK(ArchCurrCpuid())); // current cpu
    ret = LOS_TaskCreate(&g_testTaskID01, &testTask);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT1);

    ICUNIT_GOTO_EQUAL(g_testCount, 1, g_testCount, EXIT2);

    TEST_TASK_PARAM_INIT_AFFI(testTask, "smp_sem_tsk004A", TaskF02,
        TASK_PRIO_TEST - 1, CPUID_TO_AFFI_MASK(otherCpuid)); // othercpu
    ret = LOS_TaskCreate(&g_testTaskID02, &testTask);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT2);

    TestAssertBusyTaskDelay(100, 0x2); // delay enough time 100
    ICUNIT_GOTO_EQUAL(g_testCount, 0x2, g_testCount, EXIT);
EXIT:
    (VOID)LOS_TaskDelete(g_testTaskID02);
EXIT2:
    (VOID)LOS_TaskDelete(g_testTaskID01);
EXIT1:
    (VOID)LOS_SemDelete(g_usSemID);

    return LOS_OK;
}

/**
* @ingroup TEST_SEM
* @par TestCase_Number
* ItSmpLosSem004
* @par TestCase_TestCase_Type
* Function test
* @brief Test interface LOS_SemCreate/LOS_SemPost/LOS_SemDelete.
* @par TestCase_Pretreatment_Condition
* NA.
* @par TestCase_Test_Steps
* step1: create and post sem(init value:0), and create task1 and task2;\n
* step2: task1 pend sem, and task2 delete sem;\n
* @par TestCase_Expected_Result
* task1 pend sem successfully.
* @par TestCase_Level
* Level 0
* @par TestCase_Automated
* true
* @par TestCase_Remark
* null
*/

VOID ItSmpLosSem004(VOID)
{
    TEST_ADD_CASE("ItSmpLosSem004", TestCase, TEST_LOS, TEST_SEM, TEST_LEVEL0, TEST_FUNCTION);
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
