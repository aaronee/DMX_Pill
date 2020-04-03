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
uint8_t eflag = 0;

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
	if(gflag) // if gflag == 1
	{
		if (sw == SW1)
		{
			if(nav == sw_cc)
			{
				cursor_pos++;
				if (cursor_pos > 12)
				{
					gflag++;
					if (gflag == 5)
					{
						gflag = 0;
						LCD_setCursor(0,9);
						LCD_write_byte(0x20);
						LCD_setCursor(0,13);
						LCD_write_byte(0x20);
						LCD_setCursor(1,7);
						LCD_write_byte(0x05);
						LCD_setCursor(1,15);
						LCD_write_byte(0x06);
						LCD_cursor_off(); // jump out DMX channel select => adjust DMX value
					}
					cursor_pos = 12;
				}
			}

			else
			{

				cursor_pos--;
				if (gflag > 1)
				{
					gflag = 1;
				}
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
	}
	//calculate which DMX channel is on the screen
	uint16_t DMXchannel = (DMX_channel[0]-0x30)*100;
	DMXchannel += (DMX_channel[1]-0x30)*10;
	DMXchannel += (DMX_channel[2]-0x30);

	if(!gflag) // if gflag == 0 ; this is DMX adjust mode
	{
		if (sw == SW1)
		{
			if(nav == sw_cc)
			{
				// do nothing
			}

			else
			{
				eflag--; // exit flag = 1, 1 count back => turn to DMX channel select mode
				if (!eflag)
				{
					eflag = 1;
					gflag = 1;
					LCD_setCursor(0,9);
					LCD_write_byte(0x3C);
					LCD_setCursor(0,13);
					LCD_write_byte(0x3E);
					LCD_setCursor(1,7);
					LCD_write_byte(0x20);
					LCD_setCursor(1,15);
					LCD_write_byte(0x20);
					LCD_cursor_on();
				}

			}
		}
		if (sw == SW2)
		{
			uint8_t	temp = hDMX.DATA_percent[DMXchannel];

			if(nav == sw_cc)
			{
				DMX_Write_per(&hDMX,DMXchannel,++temp);
			}
			else
			{
				DMX_Write_per(&hDMX,DMXchannel,--temp);
			}
		}
	}
	//Write percentage value to screen
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
	uint8_t fullblock = hDMX.DATA_frame[DMXchannel]/37;
	uint8_t count = 0;
	LCD_setCursor(1,8);
	for (count=0; count < fullblock; count++)
	{
		LCD_write_byte(0x04);
	}
	count = fullblock; // store back the value of temp
	count *= 37;
	count = hDMX.DATA_frame[DMXchannel] - count;
	if (count >=0 && count<35)
	{
		if(count<7)
		{
			LCD_write_byte(0x20);
		}
		if(count >=7 && count< 14)
		{
			LCD_write_byte(0x00);
		}
		if(count >= 14 && count < 21)
		{
			LCD_write_byte(0x01);
		}
		if(count >= 21 && count < 28)
		{
			LCD_write_byte(0x02);
		}
		if(count >= 28)
		{
			LCD_write_byte(0x03);
		}
		uint8_t cnt = 0;
		for(cnt = 0; cnt < (6-fullblock); cnt++)
			LCD_write_byte(0x20);
	}
	if (hDMX.DATA_frame[DMXchannel] > 253)
	{
		LCD_setCursor(1,14);
		LCD_write_byte(0x04);
	}
//	if (hDMX.DATA_frame[DMXchannel] <= 7)
//	{
//		LCD_setCursor(1,8);
//		LCD_write("       ");
//	}


	//Eof function, set the cursor back for next call
	LCD_setCursor(0,cursor_pos);
}

void menu_init(uint8_t menupos)
{
	if (menupos == 0x01)
	{
		gflag = 1; // set gflag
		eflag = 5;
		cursor_pos = 12;
		LCD_setCursor(0,0);
		sprintf(gbuff,"Channel  <000>");
		LCD_write(gbuff);
		LCD_setCursor(1,0);
		sprintf(gbuff,"   0%%           ");
		LCD_write(gbuff);
		LCD_home();
		LCD_setCursor(0,12);
		LCD_cursor_on();
		menu_pos = 0x01;

		sprintf(gbuff,"%c%c%c%c%c%c%c%c",0x80,0x10,0x10,0x10,0x10,0x10,0x10,0x80);
		LCD_custom((uint8_t *)gbuff,0);
		sprintf(gbuff,"%c%c%c%c%c%c%c%c",0x80,0x18,0x18,0x18,0x18,0x18,0x18,0x80);
		LCD_custom((uint8_t *)gbuff,1);
		sprintf(gbuff,"%c%c%c%c%c%c%c%c",0x80,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x80);
		LCD_custom((uint8_t *)gbuff,2);
		sprintf(gbuff,"%c%c%c%c%c%c%c%c",0x80,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x80);
		LCD_custom((uint8_t *)gbuff,3);
		sprintf(gbuff,"%c%c%c%c%c%c%c%c",0x80,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x80);
		LCD_custom((uint8_t *)gbuff,4);
		sprintf(gbuff,"%c%c%c%c%c%c%c%c",0x80,0x01,0x03,0x06,0x06,0x03,0x01,0x80);
		LCD_custom((uint8_t *)gbuff,5);
		sprintf(gbuff,"%c%c%c%c%c%c%c%c",0x80,0x10,0x18,0x0C,0x0C,0x18,0x10,0x80);
		LCD_custom((uint8_t *)gbuff,6);

	}
}














