/*
 * DMX_data_stream.c
 *
 *  Created on: Mar. 13, 2020
 *      Author: AaronEE
 *      Email: khoahuynh.ld@gmail.com
 *      File: DMX data stream control code, based on HAL library
 *      	This file was written for STM32F103C8 controller, using STM32_HAL library
 *      Required: 1 channel Compare output TIM
 *      Goal: 1/ Controlling data stream and timing based on article
 *       		https://web.archive.org/web/20170923181853/http://www.dmx512-online.com/packt.html
 *
 *      		& ULTRATEC POCKET CONSOLE DMX CONTROLLER - CLF-4463	reference signal
 *
 *      				MIN		|	TYP		| 	MAX		|	Unit	|	Logic
 *      		BREAK :	88		|	88		|	1000000	|	usec	|	Low
 *      		MAB   :			|	8		|			|	usec	|	High
 *      FRAME WIDTH	  :			|	44		|			|	usec	|	L/H
 *    START/STOP/DATA :			|	4		|			|	usec	|	L/H
 *    			MTBF  :	0		|	NS		|	1000000 |	usec	|	High
 *    			MTBP  : 0		|	NS		|	1000000 |	usec	|	High
 *    			*NS = Non specify
 *    			*MTBF/MTBP =  Mark time between frame/packet
 */

#include "main.h"
#include "DMX_data_stream.h"


uint8_t IBM = 0; // 0 - Idle state ; 1 - BREAK state ; 2 - MAB state
uint8_t IBM_done = 0; // IBM not done yet

/* This function is used to set the value of a DMX channel when called */
void DMX_Write(DMX512_HandleTypeDef *hDMX512, uint8_t dmx_channel,uint8_t dmx_value)
{
	hDMX512->DATA_frame[dmx_channel] = dmx_value;
}
/* This function will generate IBM sequence on TX pin
 * [Inter-packet Idle(MTBP) - BREAK - MARK after BREAK(MAB)] sequence to start a new packet
 * Set MTBP = 600 usec - HIGH
 * Set BREAK = 100 usec	- LOW
 * Set MAB = 18 usec
 * @param: TIM_HandleTypeDef with TIM clk at 12Mhz / Prescaler 0 / BASE mode
 * */
void IBM_Start(DMX512_HandleTypeDef *hDMX512)
{
	switch (IBM)
	{
	case 0:
		__HAL_TIM_SET_AUTORELOAD(hDMX512->htim, 6000);
		HAL_GPIO_WritePin(hDMX512->GPIOx,hDMX512->GPIO_Pin,GPIO_PIN_SET);
		IBM = 1;
		break;
	case 1:
		__HAL_TIM_SET_AUTORELOAD(hDMX512->htim, 1200);
		HAL_GPIO_WritePin(hDMX512->GPIOx,hDMX512->GPIO_Pin,GPIO_PIN_RESET);
		IBM = 2;
		break;
	case 2:
		HAL_GPIO_WritePin(hDMX512->GPIOx,hDMX512->GPIO_Pin,GPIO_PIN_SET);
		IBM = 0;
		HAL_TIM_Base_Stop_IT(hDMX512->htim); // Stop TIM - End IBM - begin sending DMX FRAMEs
		break;
	default:
		break;
	}
}

void DMX_Start(DMX512_HandleTypeDef *hDMX512)
{
	while(HAL_UART_GetState(hDMX512->huart) != HAL_UART_STATE_BUSY_TX)
	{
		/*Start of mode switching*/
		GPIO_InitTypeDef USART1IO_param;
		USART1IO_param.Pin = hDMX512->GPIO_Pin;
		USART1IO_param.Mode = GPIO_MODE_OUTPUT_PP;
		USART1IO_param.Pull = GPIO_NOPULL;
		USART1IO_param.Speed = GPIO_SPEED_FREQ_MEDIUM;

		HAL_GPIO_Init(hDMX512->GPIOx,&USART1IO_param);
		/*End of mode swithching*/
		/*Start sequence begin*/
		if(!IBM_done)
		{
			HAL_TIM_Base_Start_IT(hDMX512->htim); // Start TIM and implement PACKET_start IN ISR
			IBM_done = 1; // set Start SEQ flag done
		}
		if(TIM4->CR1 == 0) // check when the IBM is done - TIM4 is disable is OK
		{
			/*Start of mode switching*/
			GPIOB->CRL |= 1U << 27U;
			GPIOB->CRL |= 1U << 24U;
			/*End of mode swithching*/
			if(HAL_UART_Transmit_IT(hDMX512->huart,hDMX512->DATA_frame,sizeof(hDMX512->DATA_frame)) != HAL_OK)
			{
				Error_Handler();
			}
			IBM_done = 0; // reset Start SEQ flag
		}

	}
}
