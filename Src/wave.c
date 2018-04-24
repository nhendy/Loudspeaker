
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

FIL  fil;
UINT br, bw;
FRESULT fr;
FATFS FatFs;


//data buffer containing one sample at a time
//char  data_buffer[100];
extern int completeFLAG;
int buffer_num = 1;
//sample index
//int SampleNumber = 1;

void ParseFile(char * file) {

	fr = f_mount(&FatFs, USERPath, 1);
	if(fr != FR_OK)
	{
		Error_Handler();
	}

	// open file
	fr = f_open(&fil, file, FA_READ| FA_OPEN_EXISTING);

	if(fr != FR_OK)
	{
		Error_Handler();
	}


	InitializeHeader();
	return ;

}




void Play(void)
{
	int play = 1;

	while(play)
	{
		while(completeFLAG)
		{
			if(buffer_num == 1)
			{
				if(HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, (uint32_t *)rbuffer1, BUFF_SIZE, DAC_ALIGN_8B_R) != HAL_OK)
				{
					Error_Handler();
				}
				fr = f_read(&fil, rbuffer2, sizeof(uint8_t) * BUFF_SIZE, &br);
				completeFLAG = 0;
				buffer_num = 2;


			}
			else if(buffer_num ==2)
			{
				if(HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, (uint32_t *) rbuffer2, BUFF_SIZE, DAC_ALIGN_8B_R) != HAL_OK)
				{
					Error_Handler();
				}
				fr = f_read(&fil, rbuffer1, sizeof(uint8_t) * BUFF_SIZE, &br);
				while(completeFLAG == 0);
				completeFLAG = 0;
				buffer_num = 1;
			}

		}

		if(f_eof(&fil) != 0)
		{
			completeFLAG = 1;
			play = 0;
			f_sync(&fil);
			f_close(&fil);

		}
	}
}


void InitializeHeader(void)
{
	f_read(&fil, header.riff, sizeof(header.riff), &br);
	f_read(&fil, (unsigned char *) header.riff, sizeof(header.riff),&br);
	f_read(&fil,(unsigned char *) buffer4, sizeof(buffer4), &br);
	// convert little endian to big endian 4 byte int
	header.overall_size  = buffer4[0] |
			(buffer4[1]<<8) |
			(buffer4[2]<<16) |
			(buffer4[3]<<24);

	f_read(&fil, header.wave, sizeof(header.wave), &br);
	f_read(&fil,header.fmt_chunk_marker, sizeof(header.fmt_chunk_marker), &br);
	f_read(&fil,buffer4, sizeof(buffer4), &br);
	// convert little endian to big endian 4 byte integer
	header.length_of_fmt = buffer4[0] |
			(buffer4[1] << 8) |
			(buffer4[2] << 16) |
			(buffer4[3] << 24);
	f_read(&fil, buffer2, sizeof(buffer2), &br);
	header.format_type = buffer2[0] | (buffer2[1] << 8);
	char format_name[10] = "";
	if (header.format_type == 1)
		strcpy(format_name,"PCM");
	else if (header.format_type == 6)
		strcpy(format_name, "A-law");
	else if (header.format_type == 7)
		strcpy(format_name, "Mu-law");


	f_read(&fil, buffer2, sizeof(buffer2), &br);
	header.channels = buffer2[0] | (buffer2[1] << 8);
	f_read(&fil, buffer4, sizeof(buffer4), &br);
	header.sample_rate = buffer4[0] |
			(buffer4[1] << 8) |
			(buffer4[2] << 16) |
			(buffer4[3] << 24);
	f_read(&fil, buffer4, sizeof(buffer4), &br);
	header.byterate  = buffer4[0] |
			(buffer4[1] << 8) |
			(buffer4[2] << 16) |
			(buffer4[3] << 24);
	f_read(&fil, buffer2, sizeof(buffer2), &br);
	header.block_align = buffer2[0] |
			(buffer2[1] << 8);

	f_read(&fil, buffer2, sizeof(buffer2), &br);

	header.bits_per_sample = buffer2[0] |
			(buffer2[1] << 8);

	f_read(&fil, header.data_chunk_header, sizeof(header.data_chunk_header), &br);

	f_read(&fil, buffer4, sizeof(buffer4), &br);
	header.data_size = buffer4[0] |
			(buffer4[1] << 8) |
			(buffer4[2] << 16) |
			(buffer4[3] << 24 );

	// calculate no.of samples
	num_samples = (8 * header.data_size) / (header.channels * header.bits_per_sample);
	size_of_each_sample = (header.channels * header.bits_per_sample) / 8;
	// calculate duration of file
	duration_in_seconds = (float) header.overall_size / header.byterate;
	// read each sample from data chunk if PCM
	if (header.format_type == 1) { // PCM

		int  size_is_correct = TRUE;

		// make sure that the bytes-per-sample is completely divisible by num.of channels
		bytes_in_each_channel = (size_of_each_sample / header.channels);

		if ((bytes_in_each_channel  * header.channels) != size_of_each_sample) {
			size_is_correct = FALSE;
		}

		if (size_is_correct) {
			// the valid amplitude range for values based on the bits per sample
			low_limit = 0l;
			high_limit = 0l;

			switch (header.bits_per_sample) {
			case 8:
				low_limit = -128;
				high_limit = 127;
				break;
			case 16:
				low_limit = -32768;
				high_limit = 32767;
				break;
			}

		} // 	if (size_is_correct) {

	} //  if (header.format_type == 1) {
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
