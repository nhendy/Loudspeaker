/*
 * port.h
 *
 *  Created on: Apr 21, 2018
 *      Author: drijhwan
 */
//#include "usart.h"
//#ifndef PORT_H_
//#define PORT_H_
char Rdata[20];
char Rbuffer[2];
int Rindex;



void transmit(char * str);
void transmitString(UART_HandleTypeDef * UART, char * str);

//#endif /* PORT_H_ */
