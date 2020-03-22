/*
 * DMX_data_stream.c
 *
 *  Created on: Mar. 13, 2020
 *      Author: AaronEE
 *      Email: khoahuynh.ld@gmail.com
 *      File: DMX data stream control code
 *      Goal: 1/ Controlling data stream and timing based on article
 *      		& ULTRATEC POCKET CONSOLE DMX CONTROLLER - CLF-4463	reference signal
 *
 *      		https://web.archive.org/web/20170923181853/http://www.dmx512-online.com/packt.html
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

uint8_t DMX_frame[513] = {0};

/* This function is used to set the value of a DMX channel when called */
void FRAME_Write(uint8_t dmx_channel,uint8_t dmx_value)
{
	DMX_frame[dmx_channel] = dmx_value;
}
/* This function takes care the MTBP-BREAK-MAB sequence to start a new packet
 * Set MTBP = 500 usec - HIGH
 * Set BREAK = 100 usec	- LOW
 * Set MAB = 8 usec
 * */
void PACKET_start(void)
{

}
