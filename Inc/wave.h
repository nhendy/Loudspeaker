// WAVE file header format
struct HEADER {
	unsigned char riff[4];						// RIFF string
	unsigned int overall_size	;				// overall size of file in bytes
	unsigned char wave[4];						// WAVE string
	unsigned char fmt_chunk_marker[4];			// fmt string with trailing null char
	unsigned int length_of_fmt;					// length of the format data
	unsigned int format_type;					// format type. 1-PCM, 3- IEEE float, 6 - 8bit A law, 7 - 8bit mu law
	unsigned int channels;						// no.of channels
	unsigned int sample_rate;					// sampling rate (blocks per second)
	unsigned int byterate;						// SampleRate * NumChannels * BitsPerSample/8
	unsigned int block_align;					// NumChannels * BitsPerSample/8
	unsigned int bits_per_sample;				// bits per sample, 8- 8bits, 16- 16 bits etc
	unsigned char data_chunk_header [4];		// DATA string or FLLR string
	unsigned int data_size;						// NumSamples * NumChannels * BitsPerSample/8 - size of the next chunk that will be read
};

//function declarations
void ParseFile(char * file);
int readSample(int counter);
void Play(void);
void InitializeHeader(void);




#define BUFF_SIZE 512
#define TRUE 1
#define FALSE 0
typedef enum{READ_SUCCESS, READ_FAIL}Read;

//Global variables

//to help parsing
unsigned char buffer4[4];
unsigned char buffer2[2];
struct HEADER header;


//more info about the data
long num_samples;
long size_of_each_sample;
float duration_in_seconds;
long low_limit;
long high_limit;
long bytes_in_each_channel;
uint32_t rbuffer1[BUFF_SIZE];
uint32_t rbuffer2[BUFF_SIZE];

