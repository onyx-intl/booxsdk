
/***********************************************
 *   COPYRIGHT (c) 2010 Acapela Group
 ***********************************************/
#include "audio.h"

#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/soundcard.h>
#define O_BINARY 0
#include <sys/stat.h>
#include <fcntl.h>

/*****************************************************************************/
/* Out on audio device or audio file */
int global_audio_dsp=0;
#define AUDIO_BUFFER 5000

// Service function: write a .wav header
void write_riff_header(FILE* wave_file, unsigned char nb_channel, unsigned short sampling_freq, long nb_samples)
{
	if (wave_file == NULL)
		return;

	fseek(wave_file, 0, SEEK_SET);		// write header

	int byte_per_sample= 2;
	unsigned long audio_size= nb_samples * byte_per_sample * nb_channel;      // 16 or 8 bits / samples
	unsigned long file_size= audio_size + 0x24;                  // RIFF header
	
	long int32_chunk_size= 0x10;
	short int16_format= (short) 1; 	        // WAVE_FORMAT_PCM
	short int16_nb_channel= (short) nb_channel; 	// Mono or stereo :-)
	long int32_sampling_freq = (long) sampling_freq;
	long int32_avg_flow = sampling_freq * byte_per_sample * int16_nb_channel; // average byte / second
	short int16_alignment= (short) (byte_per_sample * int16_nb_channel);
	short int16_bit_per_sample= (short) (8 * byte_per_sample);
	fwrite("RIFF", 1, 4, wave_file);
	fwrite(&file_size, 4, 1, wave_file);
	fwrite("WAVEfmt ", 1, 8, wave_file);;
	fwrite(&int32_chunk_size, 1, sizeof(int32_chunk_size), wave_file);
	fwrite(&int16_format, 1, sizeof(int16_format), wave_file);
	fwrite(&int16_nb_channel, 1, sizeof(int16_nb_channel), wave_file);
	fwrite(&int32_sampling_freq, 1, sizeof(int32_sampling_freq), wave_file);
	fwrite(&int32_avg_flow, 1, sizeof(int32_avg_flow), wave_file);
	fwrite(&int16_alignment, 1, sizeof(int16_alignment), wave_file);
	fwrite(&int16_bit_per_sample, 1, sizeof(int16_bit_per_sample), wave_file);
	fwrite("data", 1, 4, wave_file);
	fwrite(&audio_size, 4, 1, wave_file);

}

FILE* audio_open(const char* out_name, int sampling_rate)
{

	FILE* out=0;
	int out_des, format, nb_channel, tmp;
	if (strcmp(out_name,"/dev/dsp")!=0) {
	  	if (strcmp(out_name,"/dev/null")==0) 
	  		return 0;

		out = fopen(out_name,"wb");
		if (!out)
			printf("Can't open %s\n",out_name);
		else
			write_riff_header(out, 1, (unsigned short) sampling_rate, 0);


		return out;
	}

	// Open linux audio
	global_audio_dsp=1;
	out_des = open(out_name, O_WRONLY); // | O_NDELAY);
	if (out_des<0) {
		printf("Cant open %s\n", out_name); 
		return 0;
	}

	format= AFMT_S16_LE;
	ioctl(out_des,SNDCTL_DSP_SETFMT,&format);
	nb_channel=1;
	if ((ioctl(out_des, SNDCTL_DSP_STEREO, &nb_channel)<0) ){
		printf("Cant open /dev/dsp mono, channel % d\n",nb_channel); 
		return 0;
	}

	tmp= sampling_rate;
	if (ioctl(out_des, SNDCTL_DSP_SPEED, &tmp)<0)
	{
		printf("Cant open /dev/dsp Sampling rate\n"); 
		return 0;
	}

	return fdopen(out_des,"w");

}

int audio_write(FILE* file, char *buffer, unsigned int size)
{
	short* out_buffer= (short*) buffer;
	int out_size=size;
	short stereo_buffer[2*AUDIO_BUFFER];

	/* If output on dsp, then stereo signal */
	if (global_audio_dsp)
	{
		int i;
		for (i=0; i<(signed)size/2;i++) { 
			stereo_buffer[2*i]= out_buffer[i]; stereo_buffer[(2*i)+1]= out_buffer[i];
		}
		out_buffer= stereo_buffer;
		out_size*=2;
	}
	
	return fwrite((char*)out_buffer, 1, out_size, file);
}

int audio_close(FILE* file, int sampling_rate, int samples)
{

	write_riff_header(file, 1, (unsigned short)sampling_rate, samples);
	return fclose(file);
}


