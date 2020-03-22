/*
 * main.c
 *
 *  Created on: Mar. 13, 2020
 *      Author: AaronEE
 *      Email: khoahuynh.ld@gmail.com
 *      File: Main code for DMX Pill
 *      Goal: 1/ Sending basic DMX512 signals
 */
#include "main.h"
#include "DMX_data_stream.h"

UART_HandleTypeDef huart1 = {0};

int main(void)
{
	/* Sof Reset Value code*/
	/* Eof Reset Value code*/

	/* Sof Initialize code */
	HAL_Init();
	GPIO_Init();
	SYSCLK_Init();
	UART_Init();
	/*Eof Initialize code*/
	while(1)
	{
		if(HAL_UART_GetState(&huart1) != HAL_UART_STATE_BUSY_TX)
		{
			if(HAL_UART_Transmit_IT(&huart1,DMX_frame,sizeof(DMX_frame)) != HAL_OK)
			{
				Error_Handler();
			}
		}
//		HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
//		HAL_Delay(500);
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
void UART_Init(void)
{
//	USE_HAL_UART_REGISTER_CALLBACKS = 1;
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 250000;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_2;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;

	if(HAL_UART_Init(&huart1) != HAL_OK)
	{
		Error_Handler();
	}

}
