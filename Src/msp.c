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


