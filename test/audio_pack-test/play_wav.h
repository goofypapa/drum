#ifndef __PLAY_WAV_H__
#define __PLAY_WAV_H__

typedef struct _wav_riff_t{
    char id[5];               //ID:"RIFF"
    int  size;                //file_len - 8
    char type[5];             //type:"WAVE"
}wav_riff_t;


typedef struct _wav_format_t{
    char  id[5];              //ID:"fmt"
    int   size;
    short compression_code;
    short channels;
    int   samples_per_sec;
    int   avg_bytes_per_sec;
    short block_align;
    short bits_per_sample;
}wav_format_t;


typedef struct _wav_fact_t{
    char id[5];
    int  size;
}wav_fact_t;


typedef struct _wav_data_t{
    char id[5];
    int  size;
}wav_data_t;


typedef struct _wav_t{
    FILE         *fp;
    wav_riff_t   riff;
    wav_format_t format;
    wav_fact_t   fact;
    wav_data_t   data;
    int          file_size;
    int          data_offset;
    int          data_size;
}wav_t;

typedef struct _play_info{
    const char * file_path;
    float gain;
}play_info;


wav_t * openwav( const char * p_filePath );
void closewav( wav_t ** p_wav );

bool playWav( const char * p_filePath, float gain );



#endif //__PLAY_WAV_H__