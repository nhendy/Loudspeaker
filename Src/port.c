
#include "main.h"
#include "stm32f0xx_hal.h"
#include "dac.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "port.h"


volatile int _rcplt = 0;
extern int ffFlag;
extern int rewindFlag;
extern int nextFlag;
extern int prevFlag;
extern uint8_t pauseFlag;



void transmit(char * str)
{
	transmitString(&huart1,  str);
}


void transmitString(UART_HandleTypeDef * UART, char * str)
{
	//HAL_UART_Transmit_IT(UART, (uint8_t *)str, strlen(str));
	HAL_UART_Transmit(UART, (uint8_t *) str, strlen(str), 0xFFFF);
}
extern void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	//UNUSED(huart);
	 transmit(Rbuffer);
	HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
	if(Rbuffer[0] == '\n' || Rbuffer[0] == '\r')
	{
		_rcplt = 1;
		Rdata[Rindex] = '\0';
		Rindex = 0;
		if(_rcplt == 1) {
//			_rcplt = 0;
			if(strcmp(Rdata, "PL") == 0 ) {
				//HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_9);
				pauseFlag = 0;
			}
			if(strcmp(Rdata, "PA") == 0 ) {
				//HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_9);
				pauseFlag = 1;
			}
			if(strcmp(Rdata, "FF") == 0 ) {
				//HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_9);
				ffFlag = 1;
			}
			if(strcmp(Rdata, "RW") == 0 ) {
				HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_9);
				rewindFlag = 1;
			}
			if(strcmp(Rdata, "SK") == 0 ) {
				HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_9);
				nextFlag = 1;
			}
			if(strcmp(Rdata, "PR") == 0 ) {
				HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_9);
				prevFlag = 1;
			}
		}
		//transmitString(huart, "complete reception");
	}
	else if(Rbuffer[0] != '\r' && Rbuffer[0] != '\n')
	{
		Rdata[Rindex ++] = Rbuffer[0];
		//transmitString(huart, Rbuffer);
	}
	HAL_UART_Receive_IT(huart, (uint8_t *)Rbuffer, 1);
}
