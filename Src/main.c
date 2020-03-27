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
TIM_HandleTypeDef htim4 = {0};
TIM_HandleTypeDef htim3 = {0};
DMX512_HandleTypeDef hDMX = {0};
char mesg[20] = {0}; // testing
uint32_t value = 0; // testing

int main(void)
{
	/* Sof Initialize code */
	HAL_Init();
	GPIO_Init();
	SYSCLK_Init();
	UART_Init();
	TIM_Init();
	/*Eof Initialize code*/
	hDMX.huart = &huart1;
	hDMX.htim = &htim4;
	hDMX.GPIOx = GPIOB;
	hDMX.GPIO_Pin = GPIO_PIN_6;
	DMX_Write(&hDMX,1,255);

	DMX_Start(&hDMX);

	while(1);
	return 0;
}
/*SYSTEM INIT			SYSTEM INIT			SYSTEM INIT			SYSTEM INIT			SYSTEM INIT*/
void Error_Handler(void)
{
	while(1);
}
void SYSCLK_Init(void)
{
	RCC_OscInitTypeDef OSCinit_param = {0};
	OSCinit_param.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	OSCinit_param.HSEState = RCC_HSE_ON;
	OSCinit_param.PLL.PLLState = RCC_PLL_ON;
	OSCinit_param.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	OSCinit_param.PLL.PLLMUL = RCC_PLL_MUL6; // HSE x6 = 48MHz
	/*SYSCLK = 8MHz HSE PD0/PD1*/
	if (HAL_RCC_OscConfig(&OSCinit_param) != HAL_OK)
	{
		Error_Handler();
	}
	RCC_ClkInitTypeDef CLKinit_param = {0};
	CLKinit_param.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
								RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	CLKinit_param.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK; // 48M
	CLKinit_param.AHBCLKDivider = RCC_SYSCLK_DIV1;	// 48M
	CLKinit_param.APB1CLKDivider = RCC_HCLK_DIV8;	// 48M/8 = 6M
	CLKinit_param.APB2CLKDivider = RCC_HCLK_DIV8;	//
	if( HAL_RCC_ClockConfig(&CLKinit_param,FLASH_LATENCY_1) != HAL_OK)
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
void TIM_Init(void)
{
	htim4.Instance = TIM4;
	htim4.Init.Prescaler = 0; // APB1 = 6Mhz => TIM4 clk = 6*2 = 12 Mhz
	htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim4.Init.Period = 0xFFFE; // 65535 - 1

	if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
	{
		Error_Handler();
	}

	/*TIM3---TIM3---TIM3---TIM3---TIM3---TIM3---TIM3---TIM3---TIM3---TIM3*/
	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 0; // APB1 = 6Mhz => TIM4 clk = 6*2 = 12 Mhz
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 2048; // 65535 - 1

	TIM_Encoder_InitTypeDef TIM3C_param;
	TIM3C_param.EncoderMode = TIM_ENCODERMODE_TI12;
	TIM3C_param.IC1Polarity = TIM_ICPOLARITY_BOTHEDGE;
	TIM3C_param.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	TIM3C_param.IC1Prescaler = TIM_ICPSC_DIV1;
	TIM3C_param.IC1Filter = 0xF;
	TIM3C_param.IC2Polarity = TIM_ICPOLARITY_BOTHEDGE;
	TIM3C_param.IC2Selection = TIM_ICSELECTION_DIRECTTI;
	TIM3C_param.IC2Prescaler = TIM_ICPSC_DIV1;
	TIM3C_param.IC2Filter = 0xF;

	if (HAL_TIM_Encoder_Init(&htim3,&TIM3C_param) != HAL_OK)
	{
		Error_Handler();
	}
}

/*ISR CALLBACK			ISR CALLBACK			ISR CALLBACK			ISR CALLBACK*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == hDMX.htim->Instance)
	{
		IBM_Start(&hDMX);
	}
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == hDMX.huart->Instance)
	{
		DMX_Start(&hDMX);
	}
}
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM3)
	{
		sprintf(mesg,"%lu \r\n",1+(TIM3->CNT)/4);
		HAL_UART_Transmit(&huart1, (uint8_t *)mesg, strlen(mesg), HAL_MAX_DELAY);
	}
}


