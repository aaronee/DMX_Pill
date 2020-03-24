/*
 * it.c
 *
 *  Created on: Mar. 13, 2020
 *      Author: khoahuynh
 */
#include "main.h"
#include "it.h"
//#include "DMX_data_stream.h"

uint16_t cnt = 0;

void SysTick_Handler(void)
{
	HAL_SYSTICK_IRQHandler();
	HAL_IncTick();
}

void USART1_IRQHandler(void)
{
	HAL_UART_IRQHandler(&huart1);
	cnt++;
	if (cnt==514)
	{
		__HAL_UART_CLEAR_FLAG(&huart1, UART_FLAG_TC);
		cnt = 0;
		DMX_Start(&hDMX);
		/*Must set the TIM pior > this ISR
		 * Because PACKET_Start will trigger
		 * TIM ISR, so if TIM ISR is higher
		 * it can run TIM ISR and complete the task.
		 *
		 * OTW, it will trapped in PACKET_Start while() loop
		 *
		 * */
	}
}
void TIM4_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim4);
}


