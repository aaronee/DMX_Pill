/*
 * main.c
 *
 *  Created on: Mar. 13, 2020
 *      Author: khoahuynh
 */
#include "main.h"
#include <string.h>
#include <stdio.h>

void SYSCLK_Init(void);
void GPIO_Init(void);
void Error_Handler(void);

int main(void)
{
	HAL_Init();
	GPIO_Init();
	SYSCLK_Init();

	while(1)
	{
		HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
		HAL_Delay(500);
	}
	return 0;
}
void Error_Handler(void)
{
	while(1);
}
void SYSCLK_Init(void)
{
	RCC_OscInitTypeDef OSCinit_param = {0};
	OSCinit_param.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	OSCinit_param.HSEState = RCC_HSE_ON;
	OSCinit_param.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&OSCinit_param) != HAL_OK)
	{
		Error_Handler();
	}
	RCC_ClkInitTypeDef CLKinit_param = {0};
	CLKinit_param.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
								RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	CLKinit_param.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
	CLKinit_param.AHBCLKDivider = RCC_SYSCLK_DIV1;
	CLKinit_param.APB1CLKDivider = RCC_HCLK_DIV1;
	CLKinit_param.APB2CLKDivider = RCC_HCLK_DIV1;
	if( HAL_RCC_ClockConfig(&CLKinit_param,FLASH_LATENCY_0) != HAL_OK)
	{
		Error_Handler();
	}
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
}
void GPIO_Init(void)
{
	/* Set up port D , pin PD0 as OSC in, PD1 as OSC out*/
	__HAL_RCC_GPIOD_CLK_ENABLE();

	/*Remap Pin using FIO->MAPR register - different with STM32L4R5ZI*/
//	AFIO->MAPR = AFIO_MAPR_USART1_REMAP;

	/* Set up led PC13*/
	__HAL_RCC_GPIOC_CLK_ENABLE();
	GPIO_InitTypeDef LEDIO_param = {0};
	LEDIO_param.Pin = GPIO_PIN_13;
	LEDIO_param.Mode = GPIO_MODE_OUTPUT_PP;
	LEDIO_param.Pull = GPIO_NOPULL;
	LEDIO_param.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC,&LEDIO_param);
}



