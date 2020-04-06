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
#include "STM32_ST7032.h"
#include <stdio.h>
#include <string.h>

/*GLOBAL VARIABLE --- GLOBAL VARIABLE --- GLOBAL VARIABLE --- GLOBAL VARIABLE --- GLOBAL VARIABLE */

UART_HandleTypeDef huart1 = {0};
I2C_HandleTypeDef hi2c1 = {0};
TIM_HandleTypeDef htim4 = {0};
TIM_HandleTypeDef htim3 = {0};
TIM_HandleTypeDef htim2 = {0};
TIM_HandleTypeDef htim1 = {0};

DMX512_HandleTypeDef hDMX = {0};
uint8_t sw_cc = 0; // testing
uint8_t sw_flag = FALSE;
uint8_t contrast = 7; // 7/15 level
uint8_t backlit = 30; // 100-30 = 70%

char mesg[80] = "Hello";

/* create bell shape character to (5pixel x 8pixel)
 * 0x04: x-x-x-0-0-1-0-0
 * 0x0E: x-x-x-0-1-1-1-0
 * 0x0E: x-x-x-0-1-1-1-0
 * 0x0E: x-x-x-0-1-1-1-0
 * 0x0E: x-x-x-0-1-1-1-0
 * 0x0F: x-x-x-1-1-1-1-1
 * 0x04: x-x-x-0-0-1-0-0
 * 0x00: x-x-x-0-0-0-0-0
 * */
uint8_t bell[8] = {0x04,0x0E,0x0E,0x0E,0x0E,0x1F,0x04,0x00};

/*MAIN --- MAIN --- MAIN --- MAIN --- MAIN --- MAIN --- MAIN --- MAIN --- MAIN --- MAIN --- MAIN --- MAIN*/
int main(void)
{
	/* Sof Initialize code */
 	HAL_Init();
	GPIO_Init();
	SYSCLK_Init();
	UART_Init();
	TIM_Init();
	I2C_Init();
	/* Eof Initialize code */

	/* Sof variable define */

	//DMX struct
	hDMX.huart = &huart1;
	hDMX.htim = &htim4;
	hDMX.GPIOx = GPIOB;
	hDMX.GPIO_Pin = GPIO_PIN_6;
	//LCD struct
	ST7032_InitTypeDef MIDAS = {0};
	MIDAS.LCD_hi2c = &hi2c1;
	MIDAS.LCD_htim_backlight = &htim1;
	MIDAS.TIM_channel_backlight = TIM_CHANNEL_2;
	MIDAS.i2cAddr = 0x3E;
	MIDAS.num_col = 16;
	MIDAS.num_lines = 2;
	/* Eof variable define */

	/* Sof user code */
	LCD_ST7032_Init(&MIDAS);
	LCD_backlight(backlit);

	DMX_Start(&hDMX);

	HAL_TIM_Encoder_Start_IT(&htim2, TIM_CHANNEL_ALL); // interrupt when channel 2 fires
	HAL_TIM_Encoder_Start_IT(&htim3, TIM_CHANNEL_ALL); // interrupt when channel 2 fires

	GUI_startup();

	/* Eof user code */
	while(1)
	{
		if (sw_flag)
		{
			switch (sw_flag)
			{
			case SW1:
				if(sw_cc)
				{
					GUI_nav(SW1,sw_cc);
				}
				else
				{
					GUI_nav(SW1,!sw_cc);
				}
				break;
			case SW2:
				if(sw_cc)
				{
					GUI_nav(SW2,sw_cc);
				}
				else
				{
					GUI_nav(SW2,!sw_cc);
				}
				break;
			}
			HAL_Delay(5); // wait to void the 2nd channel interrupt from sw
			sw_flag = FALSE;
		}
	}
	return 0;
}
/*SYSTEM INIT --- SYSTEM INIT --- SYSTEM INIT --- SYSTEM INIT --- SYSTEM INIT --- SYSTEM INIT --- SYSTEM INIT*/

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
	__HAL_RCC_HSI_DISABLE(); // Disable HSI

}
void GPIO_Init(void)
{
	/* Set up port D , pin PD0 as OSC in, PD1 as OSC out*/
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

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
	/*TIM4---TIM4---TIM4---TIM4---TIM4---TIM4---TIM4---TIM4---TIM4---TIM4*/
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
	htim3.Init.Period = 2048;

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

	/*TIM2---TIM2---TIM2---TIM2---TIM2---TIM2---TIM2---TIM2---TIM2---TIM2*/
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 0; // APB1 = 6Mhz => TIM4 clk = 6*2 = 12 Mhz
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 2048;

	TIM_Encoder_InitTypeDef TIM2C_param;
	TIM2C_param.EncoderMode = TIM_ENCODERMODE_TI12;
	TIM2C_param.IC1Polarity = TIM_ICPOLARITY_BOTHEDGE;
	TIM2C_param.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	TIM2C_param.IC1Prescaler = TIM_ICPSC_DIV1;
	TIM2C_param.IC1Filter = 0xF;
	TIM2C_param.IC2Polarity = TIM_ICPOLARITY_BOTHEDGE;
	TIM2C_param.IC2Selection = TIM_ICSELECTION_DIRECTTI;
	TIM2C_param.IC2Prescaler = TIM_ICPSC_DIV1;
	TIM2C_param.IC2Filter = 0xF;

	if (HAL_TIM_Encoder_Init(&htim2,&TIM2C_param) != HAL_OK)
	{
		Error_Handler();
	}

	/*TIM1---TIM1---TIM1---TIM1---TIM1---TIM1---TIM1---TIM1---TIM1---TIM1*/
	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 49;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = 1999;
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

	if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
	{
		Error_Handler();
	}
	TIM_OC_InitTypeDef TIM1PWM_param;
	TIM1PWM_param.OCMode = TIM_OCMODE_PWM1;
	TIM1PWM_param.Pulse = 500;
	TIM1PWM_param.OCFastMode = TIM_OCFAST_DISABLE;
	TIM1PWM_param.OCPolarity = TIM_OCPOLARITY_HIGH;
	if (HAL_TIM_PWM_ConfigChannel(&htim1,&TIM1PWM_param,TIM_CHANNEL_2) != HAL_OK) // PA9
	{
		Error_Handler();
	}
}
void I2C_Init (void)
{
	hi2c1.Instance = I2C1;
	hi2c1.Init.ClockSpeed = 200000; // 200Khz
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = 0x01;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT; // 7bit address

	if (HAL_I2C_Init(&hi2c1) != HAL_OK)
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
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			sw_cc = TRUE;
		}
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
		{
			sw_cc = FALSE;
		}
		sw_flag = SW1;
	}
	if (htim->Instance == TIM2)
	{
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			sw_cc = TRUE;
		}
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
		{
			sw_cc = FALSE;
		}
		sw_flag = SW2;
	}
}


