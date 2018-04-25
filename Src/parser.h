#define FILE_NAME_SIZE 20
#define FILE_LIST	   5     //n0. of files in SD card
#define FILETYPE_DIR   0
#define FILETYPE_FILE  1

typedef struct{
	uint8_t name[FILE_NAME_SIZE];
}FILE_Line;

typedef struct{
	FILE_Line files[FILE_LIST];
	uint8_t  index;
}FILE_file;

FIL  fil;
//UINT br, bw;
//FRESULT fr;
//FATFS FatFs;

void ParseFile(char * file);
void ParseDirectory();
void InitializeHeader(void);
void Mount();
