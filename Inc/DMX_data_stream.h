/*
 * DMX_data_stream.h
 *
 *  Created on: Mar. 21, 2020
 *      Author: khoahuynh
 */

#ifndef INC_DMX_DATA_STREAM_H_
#define INC_DMX_DATA_STREAM_H_

extern void FRAME_Write(uint8_t ,uint8_t);
extern void PACKET_start(void);
extern uint8_t DMX_frame[513];

#endif /* INC_DMX_DATA_STREAM_H_ */

