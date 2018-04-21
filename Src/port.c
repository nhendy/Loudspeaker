
#include "main.h"
#include "stm32f0xx_hal.h"
#include "dac.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "port.h"

char Rdata[20];
char Rbuffer[2];
int Rindex;
volatile int _rcplt = 0;




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
    //transmitString(huart,Rbuffer);
    HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
    if(Rbuffer[0] == '\n')
    {
        _rcplt = 1;
        Rdata[Rindex] = '\0';
        Rindex = 0;
        //transmitString(huart, "complete reception");
    }
    else if(Rbuffer[0] != '\r' && Rbuffer[0] != '\n')
    {
        Rdata[Rindex ++] = Rbuffer[0];
        transmitString(huart, Rbuffer);
    }
    HAL_UART_Receive_IT(huart, (uint8_t *)Rbuffer, 1);
}
