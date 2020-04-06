/*
 * main.h
 *
 *  Created on: Mar. 13, 2020
 *      Author: khoahuynh
 */

#ifndef INC_MAIN_H_
#define INC_MAIN_H_

#define TRUE 1
#define FALSE 0
#define SW2 2
#define SW1 1

#include "stm32f103xb.h"
#include "stm32f1xx_hal.h"
#include "DMX_data_stream.h"
#include "DMX_Pill_GUI.h"

void SYSCLK_Init(void);
void GPIO_Init(void);
void Error_Handler(void);
void UART_Init(void);
void TIM_Init(void);
void I2C_Init(void);

extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim1;
extern I2C_HandleTypeDef hi2c1;
extern DMX512_HandleTypeDef hDMX;
extern uint8_t sw_cc;
extern uint8_t contrast;
extern uint8_t backlit;


#endif /* INC_MAIN_H_ */
