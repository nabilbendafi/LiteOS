/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Usart Init Implementation
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

#include "fsl_uart.h"
#include "uart.h"
#include "los_hwi.h"
#include "platform.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

uart_config_t config;

void UartInit(void) {
    UART_GetDefaultConfig(&config);

    config.enableTx = true;
    config.enableRx = true;

    UART_Init(UART0, &config, CLOCK_GetFreq(UART0_CLK_SRC));
}

void UartWriteChar(const char c) {
    UART_WriteBlocking(UART0, (uint8_t *)&c, 1);
}

uint8_t UartReadChar(void) {
    uint8_t ch;

    UART_ReadBlocking(UART0, &ch, sizeof(uint8_t));

    return ch;
}
 
void UartHandler(void) {
    uart_getc();
    UART0->CFIFO |= UART_CFIFO_RXFLUSH_MASK;

    LOS_HwiClear(NUM_HAL_INTERRUPT_UART);
}

int32_t UartHwi(void) {
    int32_t ret;

    ret = LOS_HwiCreate(NUM_HAL_INTERRUPT_UART, 0, 0, UartHandler, NULL);
    if (ret != LOS_OK) {
        PRINT_ERR("%s,%d, uart interrupt created error:%x\n", __FUNCTION__, __LINE__, ret);
	return ret;
    } else {
        /* Enable RX interrupt. */
        UART_EnableInterrupts(UART0, kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable);

        LOS_HwiEnable(NUM_HAL_INTERRUPT_UART);
    }

    return LOS_OK;
}

UartControllerOps g_genericUart = {
    .uartInit = UartInit,
    .uartWriteChar = UartWriteChar,
    .uartReadChar = UartReadChar,
    .uartHwiCreate = UartHwi,
};

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
