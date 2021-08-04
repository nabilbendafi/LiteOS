/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Usart Implementation
 * Author: Huawei LiteOS Team
 * Create: 2021-08-04
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

#include "usart.h"
#include "los_hwi.h"
#include "platform.h"
#include "metal/tty.h"
#include "metal/machine/platform.h"

VOID UsartWrite(const CHAR c)
{
    metal_tty_putc(c);
}

UINT8 UsartRead(VOID)
{
    INT32 ch;
    metal_tty_getc(&ch);
    return (UINT8)ch;
}

STATIC VOID UartHandler(VOID)
{
    (VOID)uart_getc();
}

INT32 UsartHwi(VOID)
{
    (VOID)LOS_HwiCreate(NUM_HAL_INTERRUPT_UART, 1, 0, UartHandler, NULL);
    (VOID)LOS_HwiEnable(NUM_HAL_INTERRUPT_UART);
    *(UINT32 volatile *)(METAL_SIFIVE_UART0_10013000_BASE_ADDRESS + METAL_SIFIVE_UART0_IE) |= (1 << 1);
    return LOS_OK;
}

UartControllerOps g_armGenericUart = {
    .uartInit = NULL,
    .uartWriteChar = UsartWrite,
    .uartReadChar = UsartRead,
    .uartHwiCreate = UsartHwi
};
