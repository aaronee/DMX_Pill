/*
 * msp.c
 *
 *  Created on: Mar. 13, 2020
 *      Author: khoahuynh
 */
#include "main.h"

void HAL_MspInit(void)
{
	// HERE WE DO LOW LEVEL PROCESSOR SPECIFIC INIT - chap5vid3
	//1. Set up the priority grouping of the ARM cortex mx processor
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4); // set interrupt priority grouping - chap5vid4
	//2. Enable the required system exceptions of the ARM cortex mx processor
	SCB->SHCSR |= 0x07 << 16; // Enable usagefault-memoryfault-busfault system exceptions- chap5vid5
	//3. Set up the priority if the system exceptions
	HAL_NVIC_SetPriority(MemoryManagement_IRQn,0,0);// - chap5vid5
	HAL_NVIC_SetPriority(BusFault_IRQn,0,0);// - chap5vid5
	HAL_NVIC_SetPriority(UsageFault_IRQn,0,0);// - chap5vid5

//	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_RCC_AFIO_CLK_ENABLE(); // ENABLE AFIO to mapping pins alternate function
}
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	if (huart->Instance == huart1.Instance)
	{
		// Enable clock for USART1 + USART1IO
		__HAL_RCC_USART1_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
		// Init io
		GPIO_InitTypeDef USART1IO_param;
		USART1IO_param.Pin = GPIO_PIN_6;
		USART1IO_param.Mode = GPIO_MODE_AF_PP;
		USART1IO_param.Pull = GPIO_NOPULL;
		USART1IO_param.Speed = GPIO_SPEED_FREQ_MEDIUM;

		HAL_GPIO_Init(GPIOB,&USART1IO_param);
		__HAL_AFIO_REMAP_USART1_ENABLE();
		// Enable Interrupt
		HAL_NVIC_EnableIRQ(USART1_IRQn);
		HAL_NVIC_SetPriority(USART1_IRQn,15,0);
	}

}
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == htim4.Instance)
	{
		// Enable clock for TIM4
		__HAL_RCC_TIM4_CLK_ENABLE();
		// Enable Interrupt
		HAL_NVIC_EnableIRQ(TIM4_IRQn);
		HAL_NVIC_SetPriority(TIM4_IRQn,14,0);
	}

	if (htim->Instance == htim3.Instance)
	{
		// Enable clk for TIM3 and GPIOB
		__HAL_RCC_TIM3_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
		// Config IO for TIM3C(1-2-3-4)
		GPIO_InitTypeDef TIM3IO_param;
		TIM3IO_param.Mode = GPIO_MODE_AF_INPUT;
		TIM3IO_param.Pin = GPIO_PIN_6 | GPIO_PIN_7;
		TIM3IO_param.Pull = GPIO_PULLDOWN;
		TIM3IO_param.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOA,&TIM3IO_param);
		// Enable Interrupt
		HAL_NVIC_EnableIRQ(TIM3_IRQn);
		HAL_NVIC_SetPriority(TIM3_IRQn,15,0);

	}

}
