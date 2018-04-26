#define FILE_NAME_SIZE 20
#define FILE_LIST	   5     //n0. of files in SD card
#define FILETYPE_DIR   0
#define FILETYPE_FILE  1

//typedef struct{
//	char name[FILE_NAME_SIZE];
//}FILE_Line;


//volatile char files[FILE_LIST][FILE_NAME_SIZE] = { "sharmoofers.wav", "AmrDiab.wav", "NancyAjram.wav", "hall.wav" } ;


FIL  fil;
//UINT br, bw;
//FRESULT fr;
//FATFS FatFs;

void ParseFile(char * file);
void ParseDirectory();
void InitializeHeader(void);
void Mount();
