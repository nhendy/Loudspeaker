
/**
 * Read and parse a wave file
 *
 **/
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "stm32f0xx_hal.h"
#include "dac.h"
#include "tim.h"
#include "wave.h"
#include "port.h"
#include "fatfs.h"
#include "ff.h"
#include "dma.h"

extern FIL  fil;;
FRESULT fr;

//data buffer containing one sample at a time
//char  data_buffer[100];
extern int XferCpltFlag;
extern int pauseFlag;
int buffer_num = 1;
uint8_t buttonPress;
//sample indexv
//int SampleNumber = 1;

extern DMA_HandleTypeDef hdma_dac1_ch1;


int isButtonPress()
{
	if(buttonPress == 1)
	{
		buttonPress = 0;
		return TRUE;
	}
	return FALSE;
}

void Play(void)
{
	UINT br;
	while(XferCpltFlag)
	{
		if(buffer_num == 1)
		{

			if(HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, (uint32_t *)rbuffer1, BUFF_SIZE, DAC_ALIGN_8B_R) != HAL_OK)
			{
				Error_Handler();
			}
			XferCpltFlag = 0;
			fr = f_read(&fil, rbuffer2, sizeof(uint8_t) * BUFF_SIZE, &br);
			buffer_num = 2;


		}
		else if(buffer_num ==2)
		{
			if(HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, (uint32_t *) rbuffer2, BUFF_SIZE, DAC_ALIGN_8B_R) != HAL_OK)
			{
				Error_Handler();
			}
			XferCpltFlag = 0;
			fr = f_read(&fil, rbuffer1, sizeof(uint8_t) * BUFF_SIZE, &br);
			buffer_num = 1;
		}


		if(f_eof(&fil) != 0 || isButtonPress() == TRUE)
		{
			XferCpltFlag = 0;
			CleanUp();

		}
	}



	//}
}



void CleanUp()
{
		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
		HAL_DAC_Stop_DMA(&hdac1, DAC_CHANNEL_1);
		HAL_DMA_Abort_IT(&hdma_dac1_ch1);

		int i;
		for(i = 0; i < BUFF_SIZE; i++)
		{
			rbuffer1[i] = 0;
			rbuffer2[i] = 0;
		}
		f_sync(&fil);
		f_close(&fil);
		return;
}


//uint8_t convert(int8_t x)
//{
//	if(x == 0)
//	{
//		return 127;
//	}
//
//	return x + 128;
//
//
//}



//int readSample(int counter)
//{
//
//	int read;
//	int i = counter;
//
//
//	sprintf(str,"==========Sample %ld / %ld=============\n", i, num_samples);
//	//transmit(str);
//	read = f_read(&fil, data_buffer, sizeof(char) * size_of_each_sample, &br);
//	if (read == FR_OK) {
//
//		// dump the data read
//		unsigned int  xchannels = 0;
//		int data_in_channel = 0;
//
//		for (xchannels = 0; xchannels < header.channels; xchannels ++ ) {
//			sprintf(str,"Channel#%d : ", (xchannels+1));
//			//transmit(str);
//			// convert data from little endian to big endian based on bytes in each channel sample
//			if (bytes_in_each_channel == 4) {
//				data_in_channel =	data_buffer[0] |
//						(data_buffer[1]<<8) |
//						(data_buffer[2]<<16) |
//						(data_buffer[3]<<24);
//			}
//			else if (bytes_in_each_channel == 2) {
//				data_in_channel = data_buffer[0] |
//						(data_buffer[1] << 8);
//			}
//			else if (bytes_in_each_channel == 1) {
//				data_in_channel = data_buffer[0];
//			}
//
//			sprintf(str,"%d ", data_in_channel);
//			//transmit(str);
//			// check if value was in range
//			if (data_in_channel < low_limit || data_in_channel > high_limit){
//				sprintf(str,"**value out of range\n");
//				//transmit(str);
//			}
//			sprintf(str," | ");
//			//transmit(str);
//		}
//
//		sprintf(str,"\n");
//		//transmit(str);
//	}
//	else {
//		sprintf(str,"Error reading file. %d bytes\n", read);
//		//transmit(str);
//		return READ_FAIL;
//
//	}
//	return READ_SUCCESS;
//
//}




/**
 * Convert seconds into hh:mm:ss format
 * Params:
 *	seconds - seconds value
 * Returns: hms - formatted string
 **/
// char* seconds_to_time(float raw_seconds) {
//  char *hms;
//  int hours, hours_residue, minutes, seconds, milliseconds;
//  hms = (char*) malloc(100);
//
// // sprintf(hms, "%f", raw_seconds);
//
//  hours = (int) raw_seconds/3600;
//  hours_residue = (int) raw_seconds % 3600;
//  minutes = hours_residue/60;
//  seconds = hours_residue % 60;
//  milliseconds = 0;
//
//  // get the decimal part of raw_seconds to get milliseconds
//  char *pos;
//  pos = strchr(hms, '.');
//  int ipos = (int) (pos - hms);
//  char decimalpart[15];
//  memset(decimalpart, ' ', sizeof(decimalpart));
//  strncpy(decimalpart, &hms[ipos+1], 3);
//  milliseconds = atoi(decimalpart);
//
//
//  sprintf(hms, "%d:%d:%d.%d", hours, minutes, seconds, milliseconds);
//  return hms;
//}
