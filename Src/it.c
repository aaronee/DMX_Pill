/*
 * it.c
 *
 *  Created on: Mar. 13, 2020
 *      Author: khoahuynh
 */
#include "main.h"
#include "it.h"


void SysTick_Handler(void)
{
	HAL_SYSTICK_IRQHandler();
	HAL_IncTick();
}

void USART1_IRQHandler(void)
{
	HAL_UART_IRQHandler(&huart1);
}


