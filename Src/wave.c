

/**
 * Read and parse a wave file
 *
 **/
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stm32f0xx_hal.h"
#include "dac.h"
#include "tim.h"
#include "wave.h"
#include "port.h"
#include "fatfs.h"
#include "ff.h"


//function declarations




#define TRUE 1 
#define FALSE 0
typedef enum{READ_SUCCESS, READ_FAIL}Read;

//Global variables

//to help parsing
unsigned char buffer4[4];
unsigned char buffer2[2];
struct HEADER header;
FIL  fil;
UINT br, bw;

//used to transmit
char str[100];


//more info about the data
long num_samples;
long size_of_each_sample;
float duration_in_seconds;
long low_limit;
long high_limit;
long bytes_in_each_channel;

//data buffer containing one sample at a time
char  data_buffer[100];

//sample index
int SampleNumber = 1;







void ParseFile(char * file) {

// filename = (char*) malloc(sizeof(char) * 1024);
// if (filename == NULL) {
//   sprintf(str,"Error in malloc\n");
//   transmit(str);
//   exit(1);
// }

 // get file path
 //char cwd[1024];
// if (getcwd(cwd, sizeof(cwd)) != NULL) {
//
//	strcpy(filename, cwd);

	// get filename from command line
	// if (argc < 2) {
	//   printf("No wave file specified\n");
	//   return ;
	// }
	
//	strcat(filename, "/");
//	strcat(filename, file);
	FATFS FatFs;// = malloc(sizeof(FATFS));

	FRESULT fm;


	fm = f_mount(&FatFs, USERPath, 1);

	if(fm != FR_OK)
	{
		sprintf(str, "f_mount fail\n");
		transmit(str);
	}

	sprintf(str, "Opening  file..\n");
	 transmit(str);

	fm = f_open(&fil, file, FA_READ| FA_OPEN_EXISTING);

	if(fm != FR_OK)
	{
		sprintf(str, "f_open fail\n");
		transmit(str);
		exit(1);
	}






	sprintf(str,"%s\n", file);
	transmit(str);





 // open file

// ptr = fopen(file, "rb");
// if (ptr == NULL) {
//	sprintf(str,"Error opening file\n");
//	transmit(str);
//	exit(1);
// }
 
 FRESULT read = 0;
 
 // read header parts

 //read = f_read(&fil, header.riff, sizeof(header.riff), &br);


 f_read(&fil, (unsigned char *) header.riff, sizeof(header.riff),&br);
 sprintf(str,"(1-4): %s \n", header.riff);
 transmit(str);

  read = f_read(&fil,(unsigned char *) buffer4, sizeof(buffer4), &br);
  sprintf(str,"%u %u %u %u\n", buffer4[0], buffer4[1], buffer4[2], buffer4[3]);
  transmit(str);


 // convert little endian to big endian 4 byte int
 header.overall_size  = buffer4[0] | 
						(buffer4[1]<<8) | 
						(buffer4[2]<<16) | 
						(buffer4[3]<<24);

 sprintf(str,"(5-8) Overall size: bytes:%u, Kb:%u \n", header.overall_size, header.overall_size/1024);
 transmit(str);
 read = f_read(&fil, header.wave, sizeof(header.wave), &br);
 sprintf(str,"(9-12) Wave marker: %s\n", header.wave);
 transmit(str);
 read = f_read(&fil,header.fmt_chunk_marker, sizeof(header.fmt_chunk_marker), &br);
 sprintf(str,"(13-16) Fmt marker: %s\n", header.fmt_chunk_marker);
 transmit(str);
 read = f_read(&fil,buffer4, sizeof(buffer4), &br);
 sprintf(str,"%u %u %u %u\n", buffer4[0], buffer4[1], buffer4[2], buffer4[3]);
 transmit(str);
 // convert little endian to big endian 4 byte integer
 header.length_of_fmt = buffer4[0] |
							(buffer4[1] << 8) |
							(buffer4[2] << 16) |
							(buffer4[3] << 24);
 sprintf(str,"(17-20) Length of Fmt header: %u \n", header.length_of_fmt);
 transmit(str);
 read = f_read(&fil, buffer2, sizeof(buffer2), &br); sprintf(str,"%u %u \n", buffer2[0], buffer2[1]);transmit(str);
 
 header.format_type = buffer2[0] | (buffer2[1] << 8);
 char format_name[10] = "";
 if (header.format_type == 1)
   strcpy(format_name,"PCM"); 
 else if (header.format_type == 6)
  strcpy(format_name, "A-law");
 else if (header.format_type == 7)
  strcpy(format_name, "Mu-law");

 sprintf(str,"(21-22) Format type: %u %s \n", header.format_type, format_name);
 transmit(str);
 read = f_read(&fil, buffer2, sizeof(buffer2), &br);
 sprintf(str,"%u %u \n", buffer2[0], buffer2[1]);
 transmit(str);
 header.channels = buffer2[0] | (buffer2[1] << 8);
 sprintf(str,"(23-24) Channels: %u \n", header.channels);
 transmit(str);
 read = f_read(&fil, buffer4, sizeof(buffer4), &br);
 sprintf(str,"%u %u %u %u\n", buffer4[0], buffer4[1], buffer4[2], buffer4[3]);
 transmit(str);
 header.sample_rate = buffer4[0] |
						(buffer4[1] << 8) |
						(buffer4[2] << 16) |
						(buffer4[3] << 24);

 sprintf(str,"(25-28) Sample rate: %u\n", header.sample_rate);
 transmit(str);
 read = f_read(&fil, buffer4, sizeof(buffer4), &br);
 sprintf(str,"%u %u %u %u\n", buffer4[0], buffer4[1], buffer4[2], buffer4[3]);
 transmit(str);
 header.byterate  = buffer4[0] |
						(buffer4[1] << 8) |
						(buffer4[2] << 16) |
						(buffer4[3] << 24);
 sprintf(str,"(29-32) Byte Rate: %u , Bit Rate:%u\n", header.byterate, header.byterate*8);
 transmit(str);
 read = f_read(&fil, buffer2, sizeof(buffer2), &br);
 sprintf(str,"%u %u \n", buffer2[0], buffer2[1]);
 transmit(str);
 header.block_align = buffer2[0] |
					(buffer2[1] << 8);
 sprintf(str,"(33-34) Block Alignment: %u \n", header.block_align);
 transmit(str);
 read = f_read(&fil, buffer2, sizeof(buffer2), &br);
 sprintf(str,"%u %u \n", buffer2[0], buffer2[1]);
 transmit(str);
 header.bits_per_sample = buffer2[0] |
					(buffer2[1] << 8);
sprintf(str,"(35-36) Bits per sample: %u \n", header.bits_per_sample);
transmit(str);
 read = f_read(&fil, header.data_chunk_header, sizeof(header.data_chunk_header), &br);
sprintf(str,"(37-40) Data Marker: %s \n", header.data_chunk_header);
transmit(str);
 read = f_read(&fil, buffer4, sizeof(buffer4), &br);
sprintf(str,"%u %u %u %u\n", buffer4[0], buffer4[1], buffer4[2], buffer4[3]);
transmit(str);
 header.data_size = buffer4[0] |
				(buffer4[1] << 8) |
				(buffer4[2] << 16) | 
				(buffer4[3] << 24 );
sprintf(str,"(41-44) Size of data chunk: %u \n", header.data_size);
transmit(str);

 // calculate no.of samples
 num_samples = (8 * header.data_size) / (header.channels * header.bits_per_sample);
sprintf(str,"Number of samples:%lu \n", num_samples);
transmit(str);
size_of_each_sample = (header.channels * header.bits_per_sample) / 8;
sprintf(str,"Size of each sample:%ld bytes\n", size_of_each_sample);
transmit(str);
 // calculate duration of file
 duration_in_seconds = (float) header.overall_size / header.byterate;
 sprintf(str, "Approx.Duration in seconds=%d\n", duration_in_seconds);
 transmit(str);



 //sprintf(str,"Approx.Duration in h:m:s=%s\n", seconds_to_time(duration_in_seconds));


 // read each sample from data chunk if PCM
 if (header.format_type == 1) { // PCM
    sprintf(str,"Dump sample data");
    transmit(str);
	char c = 'n';
//	scanf("%c", &c);
	if (1)
	{ 
		long i =0;
		//int  * data_buffer = malloc(sizeof(char) * size_of_each_sample);
		int  size_is_correct = TRUE;

		// make sure that the bytes-per-sample is completely divisible by num.of channels
		bytes_in_each_channel = (size_of_each_sample / header.channels);
		sprintf(str, "bytes in each channel %ld\n", bytes_in_each_channel);
		transmit(str);
		if ((bytes_in_each_channel  * header.channels) != size_of_each_sample) {
			sprintf(str,"Error: %ld x %ud <> %ld\n", bytes_in_each_channel, header.channels, size_of_each_sample);
			transmit(str);
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
//				case 16:
//					low_limit = -32768;
//					high_limit = 32767;
//					break;
//				case 32:
//					low_limit = -2147483648;
//					high_limit = 2147483647;
//					break;
			}					

			sprintf(str,"\n\n.Valid range for data values : %ld to %ld \n", low_limit, high_limit);
			transmit(str);

			//LOOP
//			for (i =1; i <= num_samples; i++) {
//				sprintf(str,"==========Sample %ld / %ld=============\n", i, num_samples);
//				transmit(str);
//				read = f_read(&fil, (int * )data_buffer, sizeof(char) , &br);
//				if (read == FR_OK) {
//
//					// dump the data read
//					unsigned int  xchannels = 0;
//					int data_in_channel = 0;
//
//					for (xchannels = 0; xchannels < header.channels; xchannels ++ ) {
//						sprintf(str,"Channel#%d : ", (xchannels+1));
//						transmit(str);
//						// convert data from little endian to big endian based on bytes in each channel sample
//						if (bytes_in_each_channel == 4) {
//							data_in_channel =	data_buffer[0] |
//												(data_buffer[1]<<8) |
//												(data_buffer[2]<<16) |
//												(data_buffer[3]<<24);
//						}
//						else if (bytes_in_each_channel == 2) {
//							data_in_channel = data_buffer[0] |
//												(data_buffer[1] << 8);
//						}
//						else if (bytes_in_each_channel == 1) {
//
//							data_in_channel = data_buffer[0];
//						}
//
//						sprintf(str,"%d ", data_in_channel);
//						transmit(str);
//						// check if value was in range
//						if (data_in_channel < low_limit || data_in_channel > high_limit)
//							sprintf(str,"**value out of range\n");
//						transmit(str);
//						sprintf(str," | ");
//						transmit(str);
//					}
//
//					sprintf(str,"\n");
//					transmit(str);
//				}
//				else {
//					sprintf(str,"Error reading file. %d bytes\n", read);
//					transmit(str);
//					break;
//				}
//
//			} // 	for (i =1; i <= num_samples; i++) {



			for (i =1; i <= num_samples; i++) {
				readSample(SampleNumber);
				SampleNumber++;
			}

		} // 	if (size_is_correct) { 
		//free(data_buffer);
	 } // if (1) {
	
 } //  if (header.format_type == 1) { 


// free(data_buffer);
 f_sync(&fil);
 fm = f_close(&fil);

 if(fm == FR_OK)
 {
	 sprintf(str,"Closed file..\n");
	 transmit(str);
 }


  // cleanup before quitting
 //free(filename);
 return ;

}


uint8_t convert(int8_t x)
{
	if(x == 0)
	{
		return 127;
	}

	return x + 128;


}


extern void HAL_TIM_PeriodElapsedCallback (TIM_HandleTypeDef *
		htim)
{
	if(htim -> Instance == TIM6)
	{
		HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
		readSample(SampleNumber);
		SampleNumber++;
		HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1,DAC_ALIGN_12B_R, data_buffer[0]);
		HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);


	}
}


int readSample(int counter)
{

	int read;
	int i = counter;


	sprintf(str,"==========Sample %ld / %ld=============\n", i, num_samples);
	transmit(str);
	read = f_read(&fil, data_buffer, sizeof(char) * size_of_each_sample, &br);
	if (read == FR_OK) {

		// dump the data read
		unsigned int  xchannels = 0;
		int data_in_channel = 0;

		for (xchannels = 0; xchannels < header.channels; xchannels ++ ) {
			sprintf(str,"Channel#%d : ", (xchannels+1));
			transmit(str);
			// convert data from little endian to big endian based on bytes in each channel sample
			if (bytes_in_each_channel == 4) {
				data_in_channel =	data_buffer[0] |
						(data_buffer[1]<<8) |
						(data_buffer[2]<<16) |
						(data_buffer[3]<<24);
			}
			else if (bytes_in_each_channel == 2) {
				data_in_channel = data_buffer[0] |
						(data_buffer[1] << 8);
			}
			else if (bytes_in_each_channel == 1) {
				data_in_channel = data_buffer[0];
			}

			sprintf(str,"%d ", data_in_channel);
			transmit(str);
			// check if value was in range
			if (data_in_channel < low_limit || data_in_channel > high_limit){
				sprintf(str,"**value out of range\n");
			transmit(str);}
			sprintf(str," | ");
			transmit(str);
		}

		sprintf(str,"\n");
		transmit(str);
	}
	else {
		sprintf(str,"Error reading file. %d bytes\n", read);
		transmit(str);
		return READ_FAIL;

	}
	return READ_SUCCESS;

}




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
