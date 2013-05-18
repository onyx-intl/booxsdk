
/***********************************************
*   COPYRIGHT (c) 2010 Acapela Group
***********************************************/
#include <stdio.h>


extern int global_audio_dsp;
#define AUDIO_BUFFER 5000

#ifdef __cplusplus
extern "C"
{
#endif

// Service function: write a .wav header
void write_riff_header(FILE* wave_file, unsigned char nb_channel, unsigned short sampling_freq, long nb_samples);

FILE* audio_open(const char* out_name, int sampling_rate);

int audio_write(FILE* file, char *buffer, unsigned int size);

int audio_close(FILE* file, int sampling_rate, int samples);
#ifdef __cplusplus
}
#endif
