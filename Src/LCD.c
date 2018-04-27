/*
 * LCD.c
 *
 *  Created on: Apr 1, 2018
 *      Author: noureldinhendy
 */


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "stm32f0xx_hal.h"
#include "LCD.h"
#include "spi.h"

uint8_t  tx_packet[100];
//extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;

void LCD_turnoff()
{
	tx_packet[0] = 0xFE;
	tx_packet[1] = 0x42;
	LCD_transmit(2);
	HAL_Delay(100);
}

void LCD_transmitString( char * str)
{
	int length = strlen(str);

	int i;
	for(i = 0; i < length; i++)
	{
		LCD_displayChar(str[i]);
	}
}

void LCD_BlinkCursor()
{
	tx_packet[0] = 0xfe;
	tx_packet[1] = 0x4b;
	LCD_transmit(2);
	HAL_Delay(10);
}

void LCD_turnOn()
{
	tx_packet[0] = 0xFE;
	tx_packet[1] = 0x41;
	LCD_transmit(2);
	HAL_Delay(100);
}

void LCD_backlight(int x)
{
	tx_packet[0] = 0xfe;
	tx_packet[1] = 0x53;
	tx_packet[3] = x;
	LCD_transmit(3);
}

void LCD_contrast(int x)
{
	tx_packet[0] = 0xfe;
	tx_packet[1] = 0x52;
	tx_packet[3] = x;
	LCD_transmit(3);
}
void LCD_transmit(int x)
{
	HAL_SPI_Transmit(&hspi2, (uint8_t *)tx_packet, sizeof(uint8_t) * x, 0xFFFF);
}

void LCD_Clear()
{
	tx_packet[0] = 0xfe;
	tx_packet[1] = 0x51;
	LCD_transmit(2);
	HAL_Delay(10);
}

void LCD_displayChar(char c)
{
	tx_packet[0] = c;
	LCD_transmit(1);
	HAL_Delay(50);
}
void LCD_backspace()
{
	tx_packet[0] = 0xfe;
	tx_packet[1] = 0x4e;
	LCD_transmit(2);
	HAL_Delay(10);
}

void LCD_cursorHome()
{
	tx_packet[0]= 0xfe;
	tx_packet[1]= 0x46;
	LCD_transmit(2);
	HAL_Delay(10);
}

void LCD_init()
{
	LCD_Clear();
	LCD_turnOn();
	LCD_cursorHome();
	LCD_BlinkCursor();
}
