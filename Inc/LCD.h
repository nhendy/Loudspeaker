/*
 * LCD.h
 *
 *  Created on: Apr 1, 2018
 *      Author: noureldinhendy
 */

#ifndef LCD_H_
#define LCD_H_
void LCD_Clear();
void LCD_transmit(int x);
void LCD_turnoff();
void LCD_turnOn();
void LCD_transmitString( char * str);
void LCD_displayChar(char c);
void LCD_cursorHome();
void LCD_BlinkCursor();
void LCD_init();
void LCD_backspace();
void LCD_backlight(int x);
void LCD_contrast(int x);


#endif /* LCD_H_ */
