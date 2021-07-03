/* *
 * *****************************************************************************
 * @file    tim.c
 * @brief   This file provides code for the configuration
 * of the TIM instances.
 * *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 * opensource.org/licenses/BSD-3-Clause
 *
 * *****************************************************************************
 */

/* Includes ------------------------------------------------------------------ */
#include "tim.h"

/* USER CODE BEGIN 0 */
#include "los_hwi.h"

#define TIMER3_RELOAD 50000
/* USER CODE END 0 */


/* TIM3 init function */
void MX_TIM3_Init(void)
{
	LL_GPTIM_InitTypeDef		InitStructer;	 

	InitStructer.Prescaler			   = 7200 - 1;										  //��Ƶϵ��8000
	InitStructer.CounterMode		   = LL_GPTIM_COUNTER_DIRECT_UP;					 //���ϼ���
	InitStructer.Autoreload 		   = 50000 - 1;										   //�Զ���װ��ֵ1000
	InitStructer.ClockDivision		   = LL_GPTIM_CLOCKDIVISION_DIV1;					//�������˲�����
	InitStructer.AutoreloadState	   = DISABLE;									   //�Զ���װ�ؽ�ֹpreload
	LL_GPTIM_Init(GPTIM0, &InitStructer);

	NVIC_DisableIRQ(GPTIM0_IRQn);
	NVIC_SetPriority(GPTIM0_IRQn,0);//�ж����ȼ�����
	NVIC_EnableIRQ(GPTIM0_IRQn);	

	LL_GPTIM_ClearFlag_UPDATE(GPTIM0);	//����������жϱ�־λ

	LL_GPTIM_EnableIT_UPDATE(GPTIM0); //�����������ж�

//	LL_GPTIM_EnableCounter(GPTIM0); //ʹ�ܶ�ʱ��

}

void GPTIM0_IRQHandler(void)
{
	if (( LL_GPTIM_IsEnabledIT_UPDATE(GPTIM0) == SET ) &&(LL_GPTIM_IsActiveFlag_UPDATE(GPTIM0)==SET))
	{	 
		LL_GPTIM_ClearFlag_UPDATE(GPTIM0);

		
	}			
}


/* *
 * @brief This function handles TIM3 global interrupt.
 */
void TIM3_IRQHandler(void)
{
    /* USER CODE BEGIN TIM3_IRQn 0 */

    /* USER CODE END TIM3_IRQn 0 */
    GPTIM0_IRQHandler();
    /* USER CODE BEGIN TIM3_IRQn 1 */

    /* USER CODE END TIM3_IRQn 1 */
}

/* USER CODE BEGIN 1 */

UINT64 Timer3Getcycle(VOID)
{
    static UINT64 bacCycle;
    static UINT64 cycleTimes;
    UINT64 swCycles = LL_GPTIM_GetCounter(GPTIM0);

    if (swCycles <= bacCycle) {
        cycleTimes++;
    }
    bacCycle = swCycles;
    return swCycles + cycleTimes * TIMER3_RELOAD;
}

VOID StmTimerInit(VOID)
{
    MX_TIM3_Init();
}

VOID StmTimerHwiCreate(VOID)
{
    UINT32 ret;

    ret = LOS_HwiCreate(TIM_IRQ, 0, 0, TIM3_IRQHandler, 0); // 16: cortex-m irq num shift
    if (ret != 0) {
        printf("ret of TIM3 LOS_HwiCreate = %#x\n", ret);
        return;
    }
    LL_GPTIM_EnableCounter(GPTIM0); //ʹ�ܶ�ʱ��
}

UINT64 StmGetTimerCycles(Timer_t num)
{
    UINT64 cycles = 0;

    switch (num) {
        case 3:
            cycles = Timer3Getcycle();
            break;
        default:
            printf("Wrong number of TIMER.\n");
    }
    return cycles;
}

/* USER CODE END 1 */

/* *********************** (C) COPYRIGHT STMicroelectronics *****END OF FILE*** */
