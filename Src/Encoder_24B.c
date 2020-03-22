/*
 * Encoder_24B.c
 *
 *  Created on: Mar. 13, 2020
 *      Author: AaronEE
 *      Email: khoahuynh.ld@gmail.com
 *      File: ECW1J-24B-BC0024 driver for STM32F103C8T6/BluePill board
 *      	Required
 *      Goal: 1/ receive signals from A/B/Com Pins, De-bound and return value of encoder
 */

#include "main.h"
#include "Encoder_24B.h"

uint32_t  cw_count = 0;
uint32_t  ccw_count = 0;


