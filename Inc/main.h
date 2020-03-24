/*
 * main.h
 *
 *  Created on: Mar. 13, 2020
 *      Author: khoahuynh
 */

#ifndef INC_MAIN_H_
#define INC_MAIN_H_

#include "stm32f103xb.h"
#include "stm32f1xx_hal.h"
#include "DMX_data_stream.h"

void SYSCLK_Init(void);
void GPIO_Init(void);
void Error_Handler(void);
void UART_Init(void);
void TIM_Init(void);
void DMX_Init(void);

extern 	UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim4;
extern DMX512_HandleTypeDef hDMX;
uint8_t DMXIO_state; // 0 = UART mode ; 1 = Digital OP mode


#endif /* INC_MAIN_H_ */
