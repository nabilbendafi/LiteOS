/*----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2021. All rights reserved.
 * Description: User Task Implementation
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

#include "sys_init.h"

#include "demo_entry.h"
#include "fsl_port.h"

uint32_t LED_Init(void)
{
    // Enable GPIO Port C clock
    CLOCK_EnableClock(kCLOCK_PortC);

    // Set Port C Pin 5 as GPIO
    PORT_SetPinMux(PORTC, 5U, kPORT_MuxAsGpio);  /* PORTC5 (pin 50) is configured as GPIO */

    /* Define the init structure for the output LED pin */
    gpio_pin_config_t led_config = {
        kGPIO_DigitalOutput,
        1
    };

    GPIO_PinInit(GPIOC, 5U, &led_config);

    return 0;
}

uint32_t LED_Blink(void)
{
    while(1) {
	GPIO_TogglePinsOutput(GPIOC, 1 << 5);
        LOS_TaskDelay(2000);
    }

}

STATIC UINT32 LED_TaskCreate(VOID)
{
    UINT32 taskId;
    TSK_INIT_PARAM_S LEDTask;

    LED_Init();

    (VOID)memset_s(&LEDTask, sizeof(TSK_INIT_PARAM_S), 0, sizeof(TSK_INIT_PARAM_S));
    LEDTask.pfnTaskEntry = (TSK_ENTRY_FUNC)LED_Blink;
    LEDTask.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    LEDTask.pcName = "LED_Blink";
    LEDTask.usTaskPrio = LOSCFG_BASE_CORE_TSK_DEFAULT_PRIO;
    LEDTask.uwResved = LOS_TASK_STATUS_DETACHED;
    return LOS_TaskCreate(&taskId, &LEDTask);
}
void app_init(void)
{
    LED_TaskCreate();
    DemoEntry();
}
