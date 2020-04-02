/*
 * DMX_Pill_GUI.c
 *
 *  Created on: Mar. 31, 2020
 *      Author: khoahuynh
 */

#include "DMX_Pill_GUI.h"

uint8_t cursor_pos = 0;
uint8_t menu_pos = 0;
uint8_t DMX_channel[3]= {0x30,0x30,0x30};
uint8_t customChar[8] = {0};
uint8_t gflag = 0;

char gbuff[80] = {0};

/*FUNC PROTOTYPE --- FUNC PROTOTYPE --- FUNC PROTOTYPE --- FUNC PROTOTYPE --- FUNC PROTOTYPE --- FUNC PROTOTYPE*/
void GUI_nav(uint8_t sw, uint8_t nav);
void menu_init(uint8_t menu_pos);

/*FUNCTION --- FUNCTION --- FUNCTION --- FUNCTION --- FUNCTION --- FUNCTION --- FUNCTION --- FUNCTION --- FUNCTION*/
void GUI_nav(uint8_t sw, uint8_t nav)
{
	if(menu_pos != 0x01)
		menu_init(0x01);

	//Determine DMX set DMX channel on screen

	if (sw == SW1)
	{
		if(nav == sw_cc)
		{
			cursor_pos++;
			if (cursor_pos > 12)
			{
				cursor_pos = 12;
			}
		}

		else
		{

			cursor_pos--;
			if (cursor_pos < 10)
			{
				cursor_pos = 10;
			}

		}
	}
	if (sw == SW2)
	{
		if(nav == sw_cc)
		{
			DMX_channel[cursor_pos-10]++;
			if (DMX_channel[cursor_pos-10] > 0x39)
			{
				DMX_channel[cursor_pos-10] = 0x39;
			}
		}
		else
		{
			DMX_channel[cursor_pos-10]--;
			if (DMX_channel[cursor_pos-10] < 0x30)
			{
				DMX_channel[cursor_pos-10] = 0x30;
			}
		}
		LCD_write_byte(DMX_channel[cursor_pos-10]);
	}

	//Write percentage value to screen
	uint16_t DMXchannel = (DMX_channel[0]-0x30)*100;
	DMXchannel += (DMX_channel[1]-0x30)*10;
	DMXchannel += (DMX_channel[2]-0x30);
	sprintf(gbuff,"%u",hDMX.DATA_percent[DMXchannel]);
	if(hDMX.DATA_percent[DMXchannel] < 100)
	{
		LCD_setCursor(1,1);
		if(hDMX.DATA_percent[DMXchannel] < 10)
			LCD_write("  ");
		else
			LCD_write(" ");
	}
	else // hDMX.DATA_percent[DMXchannel] == 100
	{
		LCD_setCursor(1,1);
	}

	LCD_write(gbuff);
	//Write DMX value percentage bar to screen
	if (hDMX.DATA_percent[DMXchannel] == 0)
	{
		LCD_setCursor(1,8);
		LCD_write_byte(0x02);
	}

	//Eof function, set the cursor back for next call
	LCD_setCursor(0,cursor_pos);
}

void menu_init(uint8_t menupos)
{
	if (menupos == 0x01)
	{
		gflag = 5; // set gflag
		cursor_pos = 12;
		LCD_setCursor(0,0);
		sprintf(gbuff,"Channel  <000>");
		LCD_write(gbuff);
		LCD_setCursor(1,0);
		sprintf(gbuff,"   0%%");
		LCD_write(gbuff);
		LCD_home();
		LCD_setCursor(0,12);
		LCD_cursor_on();
		menu_pos = 0x01;

		sprintf(gbuff,"%c%c%c%c%c%c%c%c",  0x80,0x80,0x80,0x80,0x10,0x80,0x80,0x80);
		LCD_custom((uint8_t *)gbuff,0);
		sprintf(gbuff,"%c%c%c%c%c%c%c%c",0x80,0x80,0x80,0x10,0x18,0x10,0x80,0x80);
		LCD_custom((uint8_t *)gbuff,1);
		sprintf(gbuff,"%c%c%c%c%c%c%c%c",0x80,0x80,0x10,0x18,0x1C,0x18,0x10,0x80);
		LCD_custom((uint8_t *)gbuff,2);
		sprintf(gbuff,"%c%c%c%c%c%c%c%c",0x80,0x10,0x18,0x1C,0x1E,0x1C,0x18,0x10);
		LCD_custom((uint8_t *)gbuff,3);
		sprintf(gbuff,"%c%c%c%c%c%c%c%c",0x80,0x18,0x1C,0x1E,0x1F,0x1E,0x1C,0x18);
		LCD_custom((uint8_t *)gbuff,4);
		sprintf(gbuff,"%c%c%c%c%c%c%c%c",0x80,0x1C,0x1E,0x1F,0x1F,0x1F,0x1E,0x1C);
		LCD_custom((uint8_t *)gbuff,5);
		sprintf(gbuff,"%c%c%c%c%c%c%c%c",0x80,0x1E,0x1F,0x1F,0x1F,0x1F,0x1F,0x1E);
		LCD_custom((uint8_t *)gbuff,6);
		sprintf(gbuff,"%c%c%c%c%c%c%c%c",0x80,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F);
		LCD_custom((uint8_t *)gbuff,7);

	}
}














