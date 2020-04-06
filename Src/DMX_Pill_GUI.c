/*
 * DMX_Pill_GUI.c
 *
 *  Created on: Mar. 31, 2020
 *      Author: khoahuynh
 */

#include "DMX_Pill_GUI.h"

uint8_t cursor_pos = 0;
uint8_t menu_pos = 0x00;
uint8_t access_call = 0x02;
uint8_t DMX_channel[3]= {0x30,0x30,0x30};
uint8_t customChar[8] = {0};
uint8_t gflag = 0;	// GUI flag
uint8_t gflagc = 0;	// GUI flag child
uint8_t eflag = 0;	// exit flag
char cbuff[80] = {0};
char sbuff[5][16] = {0};

/*FUNC PROTOTYPE --- FUNC PROTOTYPE --- FUNC PROTOTYPE --- FUNC PROTOTYPE --- FUNC PROTOTYPE --- FUNC PROTOTYPE*/
void GUI_nav(uint8_t sw, uint8_t nav);
void GUI_1ch(uint8_t sw, uint8_t nav);
void GUI_main(uint8_t sw, uint8_t nav);
void GUI_abt(uint8_t sw, uint8_t nav);
void GUI_set(uint8_t sw, uint8_t nav);
void GUI_set_ctrast(uint8_t sw, uint8_t nav);
void GUI_set_blit(uint8_t sw, uint8_t nav);
void menu_init(uint8_t menu_pos);
void GUI_startup(void);

/*FUNCTION --- FUNCTION --- FUNCTION --- FUNCTION --- FUNCTION --- FUNCTION --- FUNCTION --- FUNCTION --- FUNCTION*/
void GUI_1ch(uint8_t sw, uint8_t nav)
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
				eflag = 5; // reset exit flag if turn sw1 dial cw
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
				if (gflag > 1) // reset gflag to 1 if sw1 ever turn ccw
				{
					gflag = 1;
				}
				if (cursor_pos < 10)
				{
					eflag--;
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
					eflag = 5;
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
	sprintf(cbuff,"%u",hDMX.DATA_percent[DMXchannel]);
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

	LCD_write(cbuff);

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

	if (eflag == 0 && gflag) // exit flag triggered, DMX channel sellect is still on
	{
		LCD_cursor_off();
		// set flag for next function
		eflag = 0;
		gflag = 0;
		cursor_pos = 0;
		access_call = 0x02;
		GUI_nav(sw,nav);// trigger main screen to write back correct position
	}
}
void menu_init(uint8_t menupos)
{
	switch (menupos)
	{
	case 0x01: // 1 channel adjust screen init

		LCD_clear();
		LCD_home();

		//reset flags

		//Init screen with respective menu
		LCD_setCursor(0,0);
		sprintf(cbuff,"Channel  <000>");
		LCD_write(cbuff);
		LCD_setCursor(1,0);
		sprintf(cbuff,"   0%%           ");
		LCD_write(cbuff);
		LCD_home();
		LCD_setCursor(0,12);
		LCD_cursor_on();
		menu_pos = 0x01; // Mark menu position now at 0x01

		sprintf(cbuff,"%c%c%c%c%c%c%c%c",0x80,0x10,0x10,0x10,0x10,0x10,0x10,0x80);
		LCD_custom((uint8_t *)cbuff,0);
		sprintf(cbuff,"%c%c%c%c%c%c%c%c",0x80,0x18,0x18,0x18,0x18,0x18,0x18,0x80);
		LCD_custom((uint8_t *)cbuff,1);
		sprintf(cbuff,"%c%c%c%c%c%c%c%c",0x80,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x80);
		LCD_custom((uint8_t *)cbuff,2);
		sprintf(cbuff,"%c%c%c%c%c%c%c%c",0x80,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x80);
		LCD_custom((uint8_t *)cbuff,3);
		sprintf(cbuff,"%c%c%c%c%c%c%c%c",0x80,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x80);
		LCD_custom((uint8_t *)cbuff,4);
		sprintf(cbuff,"%c%c%c%c%c%c%c%c",0x80,0x01,0x03,0x06,0x06,0x03,0x01,0x80);
		LCD_custom((uint8_t *)cbuff,5);
		sprintf(cbuff,"%c%c%c%c%c%c%c%c",0x80,0x10,0x18,0x0C,0x0C,0x18,0x10,0x80);
		LCD_custom((uint8_t *)cbuff,6);
		break;
	case 0x02: // main menu screen init
		LCD_clear();
		LCD_home();
		menu_pos = 0x02; // mark menu position now at 0x02

		// reset flags

		//Init screen with respective menu
		strcpy (sbuff[0], "adjust DMX");
		strcpy (sbuff[1], "setting");
		strcpy (sbuff[2], "about");
		LCD_setCursor(0,0);
		LCD_write_byte(0x00);
		LCD_setCursor(0,2);
		LCD_write(sbuff[gflag]);
		LCD_setCursor(1,2);
		LCD_write(sbuff[gflag+1]);

		sprintf(cbuff,"%c%c%c%c%c%c%c%c",0x80,0x80,0x04,0x02,0x1F,0x02,0x04,0x80);
		LCD_custom((uint8_t *)cbuff,0);
		break;

	case 0x03: // about screen init
		LCD_clear();
		LCD_home();
		menu_pos = 0x03; // mark menu possition now at 0x03

		// reset flags
		eflag = 0;
		gflag = 0;
		cursor_pos = 0;

		LCD_setCursor(0,0);
		LCD_write("SW 1.00");
		LCD_setCursor(0,8);
		LCD_write("HW 1.00");
		LCD_setCursor(1,0);
		LCD_write("git.io/JvAIZ");
		break;
	case 0x04:
		LCD_clear();
		LCD_home();
		menu_pos = 0x04;
		memset(sbuff,0,sizeof(sbuff));
		strcpy (sbuff[0], "contrast");
		strcpy (sbuff[1], "backlit");

		// reset flags
		eflag = 0;
		gflag = 0;
		cursor_pos = 0;

		LCD_setCursor(0,0);
		LCD_write_byte(0x00);
		LCD_setCursor(0,2);
		LCD_write("contrast");
		LCD_setCursor(1,2);
		LCD_write("backlit");

		sprintf(cbuff,"%c%c%c%c%c%c%c%c",0x80,0x80,0x04,0x02,0x1F,0x02,0x04,0x80);
		LCD_custom((uint8_t *)cbuff,0);
		break;
	case 0x05:
		LCD_clear();
		LCD_home();
		menu_pos = 0x05;

		// reset flags
		eflag = 0;
		gflag = 0;
		cursor_pos = 0;

		LCD_command(0x35);
		LCD_setCursor(0,0);
		LCD_write("Rotate Left-Dial");
		break;
	case 0x06: // backlit
		LCD_clear();
		LCD_home();
		menu_pos = 0x06;

		// reset flags
		eflag = 0;
		gflag = 0;
		cursor_pos = 0;

		LCD_command(0x35);
		LCD_setCursor(0,0);
		LCD_write("Rotate Left-Dial");
		break;
	default:
		break;
	}

}
void GUI_nav(uint8_t sw, uint8_t nav)
{
	switch (access_call)
	{
	case 0x01: // adjust 1 channel
		GUI_1ch(sw,nav);
		break;
	case 0x02: // main menu
		GUI_main(sw,nav);
		break;
	case 0x03: // about
		GUI_abt(sw,nav);
		break;
	case 0x04: //setting
		GUI_set(sw,nav);
		break;
	case 0x05: //ctrast
		GUI_set_ctrast(sw,nav);
		break;
	case 0x06: //constrast
		GUI_set_blit(sw,nav);
		break;
	default:
		break;
	}
}
void GUI_main(uint8_t sw, uint8_t nav)
{
	if(menu_pos != 0x02)
	{
		menu_init(0x02);
	}
	if (sw == SW2)
	{
		LCD_clear();
		LCD_setCursor(0,0);
		LCD_write_byte(0x00);
		if(nav == sw_cc)
		{
			gflag--;
			if (gflag > 3)
				gflag = 0;
			LCD_setCursor(0,2);
			LCD_write(sbuff[gflag]);
			LCD_setCursor(1,2);
			LCD_write(sbuff[gflag+1]);
			eflag = 0;

		}
		else
		{

			gflag++;
			if (gflag == 3)
				gflag = 2;
			LCD_setCursor(0,2);
			LCD_write(sbuff[gflag]);
			LCD_setCursor(1,2);
			LCD_write(sbuff[gflag+1]);
			eflag = 0;

		}
	}
	if(sw == SW1)
	{
		if(nav == sw_cc)
		{
			eflag++;
			if(eflag == 5)
			{
				eflag = 0;
				switch (gflag) // depend on where gflag is, select the right function to enter
				{
					case 0:
						// set flag for next function
						gflag = 1;
						eflag = 5;
						cursor_pos = 12;
						access_call = 0x01; // adjust DMX 1 channel
						GUI_nav(sw,nav);
						break;
					case 1:
						gflag = 1;
						eflag = 5;
						cursor_pos = 0;
						access_call = 0x04; // setting screen
						GUI_nav(sw,nav);
						break;
					case 2:
						gflag = 1;
						eflag = 5;
						cursor_pos = 0;
						access_call = 0x03; // about screen
						GUI_nav(sw,nav);
						break;
					default:
						break;
				}
			}
		}
		else
		{
			eflag = 0;
		}
	}

}
void GUI_abt(uint8_t sw, uint8_t nav)
{
	if(menu_pos != 0x03)
	{
		menu_init(0x03);
	}
	if(sw == SW1)
	{
		if(nav != sw_cc)
		{
			eflag++;
			if(eflag == 5)
			{
				eflag = 0;
				gflag = 2;
				cursor_pos = 0;
				access_call = 0x02;
				GUI_nav(sw,nav);// trigger main screen to write back correct position
			}
		}
		else
		{
			eflag = 0;
		}
	}
}
void GUI_set(uint8_t sw, uint8_t nav)
{
	if(menu_pos != 0x04)
	{
		menu_init(0x04);
	}
	if (sw == SW2)
	{
		LCD_clear();
		LCD_setCursor(0,0);
		LCD_write_byte(0x00);
		if(nav == sw_cc)
		{

			gflag--;
			if (gflag > 2)
				gflag = 0;
			LCD_setCursor(0,2);
			LCD_write(sbuff[gflag]);
			LCD_setCursor(1,2);
			LCD_write(sbuff[gflag+1]);
			eflag = 0;

		}
		else
		{

			gflag++;
			if (gflag == 2)
				gflag = 1;
			LCD_setCursor(0,2);
			LCD_write(sbuff[gflag]);
			LCD_setCursor(1,2);
			LCD_write(sbuff[gflag+1]);
			eflag = 0;

		}
	}
	if (sw == SW1)
	{
		if(nav != sw_cc)
		{
			gflagc = 0;
			eflag++;
			if(eflag == 5)
			{
				eflag = 0;
				gflag = 1;
				cursor_pos = 0;
				access_call = 0x02;
				GUI_nav(sw,nav);// trigger main screen to write back correct position
			}
		}
		else
		{
			gflagc++;
			eflag = 0;
			if (gflagc == 5)
			{
				switch (gflag)
				{
				case 0: // setting child menu - contrast enter
					eflag = 0;
					gflag = 1;
					cursor_pos = 0;
					access_call = 0x05;
					GUI_nav(sw,nav);// trigger main screen to write back correct position
					break;
				case 1:
					eflag = 0;
					gflag = 1;
					cursor_pos = 0;
					access_call = 0x06;
					GUI_nav(sw,nav);// trigger main screen to write back correct position
					break;
				default:
					break;
				}
			}
		}
	}
}
void GUI_set_ctrast(uint8_t sw, uint8_t nav)
{
	if(menu_pos != 0x05)
	{
		menu_init(0x05);
	}
	if (sw == SW2)
	{
		LCD_setCursor(0,0);
		if(nav == sw_cc)
		{
			if (contrast != 16)
			{
				contrast++;
				LCD_contrast(contrast);
			}
		}
		else
		{
			if (contrast != 0)
			{
				contrast--;
				LCD_contrast(contrast);
			}
		}
	}
	if (sw == SW1)
	{
		if(nav != sw_cc)
		{
			eflag++;
			if(eflag == 5)
			{
				eflag = 0;
				gflag = 0;
				cursor_pos = 0;
				access_call = 0x04;
				LCD_command(0x39);
				GUI_nav(sw,nav);// trigger main screen to write back correct position
			}
		}
		}
		else
		{
			eflag = 0;
		}

}
void GUI_set_blit(uint8_t sw, uint8_t nav)
{
	if(menu_pos != 0x06)
		{
			menu_init(0x06);
		}
		if (sw == SW2)
		{
			LCD_setCursor(0,0);
			if(nav == sw_cc)
			{
				if(backlit != 100)
				{
					backlit=backlit+10;
					LCD_backlight(backlit);
				}
			}
			else
			{
				if (backlit != 0)
				{
					backlit=backlit-10;
					LCD_backlight(backlit);
				}
			}
		}
		if (sw == SW1)
		{
			if(nav != sw_cc)
			{
				eflag++;
				if(eflag == 5)
				{
					eflag = 0;
					gflag = 1;
					cursor_pos = 0;
					access_call = 0x04;
					LCD_command(0x39);
					GUI_nav(sw,nav);// trigger main screen to write back correct position
				}
			}
			}
			else
			{
				eflag = 0;
			}
}
void GUI_startup(void)
{
	LCD_command(0x35);
	LCD_setCursor(0,4);
	LCD_write("DMX PILL");
	HAL_Delay(1500);
	LCD_command(0x39);
	GUI_nav(SW1,!sw_cc);
}



