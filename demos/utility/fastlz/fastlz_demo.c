/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Fastlz Demo Implementation
 * Author: Huawei LiteOS Team
 * Create: 2021-05-07
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

#include "fastlz_demo.h"
#include "stdio.h"
#include "los_task.h"
#include "unistd.h"
#include "fcntl.h"
#include "fastlz_shellcmd.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define FASTLZ_TASK_STACK_SIZE 0x1000
#define FASTLZ_TASK_PRIORITY   8
#define COMPRESS_LEVEL         2

STATIC UINT32 g_demoTaskId;

STATIC INT32 DemoTaskEntry(VOID)
{
    INT32 i;
    INT32 ret;
    INT32 fd = -1;
    const CHAR *sourceFile = "/fatfs/fastlz_source.txt";
    const CHAR *cmprsFile = "/fatfs/fastlz_cmprs.txt";
    const CHAR *dcmprsFile = "/fatfs/fastlz_decmprs.txt";

    printf("Fastlz demo task start to run.\n");

    fd = open(sourceFile, O_CREAT | O_RDWR | O_TRUNC, 0664);    /* 0664: File Permissions */
    if (fd < 0) {
        printf("Open file %s failed.\n", sourceFile);
        return -1;
    }
    for (i = 0; i < 100; i++) {                                 /* 100: Cycle 100 times */
        write(fd, "fastlz test case", strlen("fastlz test case"));
    }
    close(fd);

    ret = FastlzCompress(sourceFile, cmprsFile, COMPRESS_LEVEL);
    if (ret < 0) {
        printf("Compress file failed.\n");
    } else {
        ret = FastlzDcompress(cmprsFile, dcmprsFile);
        if (ret < 0) {
            printf("Deompress file failed.\n");
        }
    }

    printf("Fastlz demo task finished.\n");
    return 0;
}

VOID FastlzDemoTask(VOID)
{
    UINT32 ret;
    TSK_INIT_PARAM_S taskInitParam;

    ret = memset_s(&taskInitParam, sizeof(TSK_INIT_PARAM_S), 0, sizeof(TSK_INIT_PARAM_S));
    if (ret != EOK) {
        return;
    }
    taskInitParam.usTaskPrio = FASTLZ_TASK_PRIORITY;
    taskInitParam.pcName = "FastlzDemoTask";
    taskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)DemoTaskEntry;
    taskInitParam.uwStackSize = FASTLZ_TASK_STACK_SIZE;
    ret = LOS_TaskCreate(&g_demoTaskId, &taskInitParam);
    if (ret != LOS_OK) {
        printf("Create Fastlz demo task failed.\n");
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
