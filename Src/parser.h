#define FILE_NAME_SIZE 20
#define FILE_LIST	   5     //n0. of files in SD card
#define FILETYPE_DIR   0
#define FILETYPE_FILE  1



FIL  fil;
//UINT br, bw;
//FRESULT fr;
//FATFS FatFs;

void ParseFile(char * file);
void ParseDirectory();
void InitializeHeader(void);
void Mount();
