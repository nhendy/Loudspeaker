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
#include "parser.h"





//FILE_file FileList;
FATFS FatFs;







//void ParseDirectory()
//{
//
//
//
//	FRESULT result = FR_OK;
//	DIR dir;
//	FILINFO fno;
//
//
//	result = f_opendir(&dir, "");
//	//FileList.index = 0;
//	int i = 0;
//	while(result == FR_OK)
//	{
//
//		result = f_readdir(&dir, &fno);
//		if(fno.fname[0] == 0 || result != FR_OK) {break ;}
//
//		//filename = fno.fname;
//
//		if(FileList.index < FILE_LIST)
//		{
//			if((fno.fattrib & AM_DIR) == 0)
//			{
//				if((strstr(fno.fname, "wav")) || (strstr(fno.fname, "WAV")))
//				{
//					strncpy((char *) FileList.files[FileList.index].name, (char *) fno.fname, FILE_NAME_SIZE);
//					FileList.index ++;
//
//				}
//			}
//		}
//
//
//	}
//
//
//
//}




void ParseFile(char * file) {

	FRESULT fr;
	// open file

	fr = f_open(&fil, file, FA_READ| FA_OPEN_EXISTING);

	if(fr != FR_OK)
	{
		Error_Handler();
	}


	InitializeHeader();
	return ;

}




void InitializeHeader(void)
{
	unsigned char buffer4[4];
	unsigned char buffer2[2];
	long num_samples;
	long size_of_each_sample;
	float duration_in_seconds;
	long low_limit;
	long high_limit;
	long bytes_in_each_channel;
	struct HEADER header;
	UINT br;
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
