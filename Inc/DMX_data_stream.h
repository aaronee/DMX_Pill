/*
 * DMX_data_stream.h
 *
 *  Created on: Mar. 21, 2020
 *      Author: khoahuynh
 */

#ifndef INC_DMX_DATA_STREAM_H_
#define INC_DMX_DATA_STREAM_H_

typedef struct {
	TIM_HandleTypeDef *htim;
	UART_HandleTypeDef *huart;
	GPIO_TypeDef *GPIOx; // GPIO Port for UART
	uint16_t GPIO_Pin;	// Pin for UART TX
	uint8_t DATA_frame[513];
	uint8_t DATA_percent[513];

} DMX512_HandleTypeDef;

extern void DMX_Write(DMX512_HandleTypeDef *hDMX512, uint16_t dmx_channel,uint8_t dmx_value);
extern void IBM_Start(DMX512_HandleTypeDef *hDMX512);
extern void DMX_Start(DMX512_HandleTypeDef *hDMX512);


#endif /* INC_DMX_DATA_STREAM_H_ */

