#include <alsa/asoundlib.h>
#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <string>
#include <cstring>
#include <sys/time.h>

#include "play_wav.h"


#define nullptr NULL

using std::cout;
using std::endl;
using std::string;

namespace ws_core
{

    void * play_wav( void * p_play_info );

    bool play_wav( const char * p_filePath, float gain )
    {
        wav_t * wav_info = open_wav( p_filePath );
        if( !wav_info )
        {
            cout << "open file: " << p_filePath << " faild" << endl;
            return false;
        }

        return play_wav( wav_info, gain );
    }

    bool play_wav( wav_t * p_wav, float gain )
    {
        pthread_t pthread_id;

        if( !p_wav )
        {
            cout << "p_wav is nullptr " << endl;
            return false;
        }

        play_info * data_info = (play_info *) malloc(sizeof(play_info));
        data_info->wav_info = p_wav;
        data_info->gain = gain;


        if( !pthread_create(&pthread_id, nullptr, play_wav, (void *)data_info) )
        {
            cout << "playing " << "..." << endl;
        }

        return true;
    }

    void * play_wav(void * p_play_info)
    {

        play_info * p_data_info = (play_info *)p_play_info;
        play_info data_info = *p_data_info;
        free(p_data_info);

        wav_t * wav = data_info.wav_info;
        if( !wav )
        {
            return nullptr;
        }

        // cout << "file size = " << wav->file_size << endl;

        // cout << "RIFF WAVE Chunk" <<endl;
        // cout << "id: " << wav->riff.id << endl;
        // cout << "size: " << wav->riff.size << endl;
        // cout << "type: " << wav->riff.type << endl;
        // cout << endl;

        // cout << "FORMAT Chunk" << endl;
        // cout << "id: " << wav->format.id << endl;
        // cout << "size: " << wav->format.size << endl;
        // cout << "Channels = " << wav->format.channels << endl;
        // cout << "SamplesPersec = " << wav->format.samples_per_sec << endl;
        // cout << "avg_bytes_per_sec = " << wav->format.avg_bytes_per_sec << endl;
        // cout << "block_align = " << wav->format.block_align << endl;
        // cout << "BitsPerSample = " << wav->format.bits_per_sample << endl;
        // cout << endl;

        // cout << "DATA Chunk" << endl;
        // cout << "id: " << wav->data.id << endl;
        // cout << "data size: " << wav->data.size << endl;
        // cout << "data offset: " << wav->data_offset << endl;

        

        int rc;
        int ret;
        int size;
        unsigned int val;
        snd_pcm_t* handle; //PCI设备句柄
        snd_pcm_hw_params_t* params;//硬件信息和PCM流配置
        int dir = 0;
        char *buffer;
        char *data;
        snd_pcm_uframes_t frames;
        snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK;

        int channels = wav->format.channels;
        int frequency = wav->format.samples_per_sec;
        int bit = wav->format.bits_per_sample;
        int datablock = wav->format.block_align;
        int dataSize = wav->data.size;
        int playDataSize = 0;
        int monotonic = 0;

        rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
        if( rc < 0 )
        {
            cout << "Error open PCM device failed" << endl;
            return nullptr;
        }

        snd_pcm_hw_params_alloca(&params); //分配params结构体

        rc = snd_pcm_hw_params_any(handle, params);//初始化params
        if( rc < 0 )
        {
            cout << "Error snd_pcm_hw_params_any" << endl;
            return nullptr;
        }

        rc = snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED); //初始化访问权限
        if( rc < 0 )
        {
            cout << "Error snd_pcm_hw_params_set_access" << endl;
            return nullptr;
        }

        switch( bit / 8 )
        {
            case 1:
                snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_U8);
                break;
            case 2:
                snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
                break;
            case 3:
                snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S24_3LE);
                break;
            case 4:
                snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_FLOAT);
                break;
        }

        rc = snd_pcm_hw_params_set_channels(handle, params, channels); //设置声道,1表示单声>道，2表示立体声
        if( rc < 0 )
        {
            cout << "Error snd_pcm_hw_params_set_channels" << endl;
            return nullptr;
        }

        val = frequency;
        rc = snd_pcm_hw_params_set_rate_near(handle, params, &val, &dir);
        if( rc < 0 )
        {
            cout << "Error snd_pcm_hw_params_set_rate_near" << endl;
            return nullptr;
        }

        monotonic = snd_pcm_hw_params_is_monotonic(params);

        rc = snd_pcm_hw_params(handle, params);
        if( rc < 0 )
        {
            cout << "Error snd_pcm_hw_params" << endl;
            return nullptr;
        }

        rc = snd_pcm_hw_params_get_period_size(params, &frames, &dir); /*获取周期长度*/
        if( rc < 0 )
        {
            cout << "Error snd_pcm_hw_params_get_period_size" << endl;
            return nullptr;
        }

        size = frames * datablock; /*4 代表数据快长度*/

        buffer = (char *)malloc(size);

        while(true)
        {

            int snd_size;

            if( dataSize <= playDataSize )
            {
                cout << "play done" << endl;
                break;
            }

            snd_size = dataSize - playDataSize > size ? size : dataSize - playDataSize;

            memset( buffer, 0, size );
            memcpy( buffer, wav->data_buf + playDataSize, snd_size );
            playDataSize += snd_size;


            switch(bit / 8)
            {
                case 1:
                    for( char * iterator = buffer; iterator < buffer + snd_size; iterator++ )
                    {
                        *iterator = (char)(*iterator * data_info.gain);
                    }
                break;
                case 2:
                    for( short * iterator = (short *)buffer; (char *)iterator < buffer + snd_size; iterator++ )
                    {
                        *iterator = (short)(*iterator * data_info.gain);
                    }
                break;
                case 3:
                    for( char * iterator = buffer; iterator < buffer + snd_size; iterator += 3 )
                    {            
                        short * p_data = (short *)(iterator + 1);
                        short tmp = (short)( *p_data * data_info.gain );
                        char * tmp_bit = (char * )&tmp;
            
                        iterator[1] = tmp_bit[0];
                        iterator[2] = tmp_bit[1];
                    }
                break;
                case 4:
                    for( float * iterator = (float *)buffer; (char *)iterator < buffer + snd_size; iterator++ )
                    {
                        *iterator = (float)(*iterator * data_info.gain);
                    }
                break;
            }



            while( (ret = snd_pcm_writei(handle, buffer, frames)) < 0 )
            {
                //usleep(200);
                if( ret == -EPIPE )
                {
                    cout << "underrun occurred" << endl;
                    //完成硬件参数设置，使设备准备好
                    snd_pcm_prepare(handle);
                    // xrun( handle, monotonic );
                }
                else if( ret < 0 )
                {
                    cout << "error from writei: " << snd_strerror(ret) <<endl;
                }
            }
        }

        // cout << "play Data size: " << playDataSize << endl;
        snd_pcm_drain( handle );
        snd_pcm_close( handle );
        free( buffer );
        close_wav( &wav );

        return nullptr;
    }

    wav_t * open_wav( const char * p_filePath )
    {
        wav_t * wav = NULL; 


        FILE * fp;
        
        char buffer[256];
        int  read_len = 0;
        int  offset = 0;

        wav = (wav_t *)malloc(sizeof(wav_t));
        if(!wav)
        {
            cout << "Error malloc wav failedly" << endl;
            return nullptr;
        }
        bzero(wav, sizeof(wav_t));

        fp = fopen(p_filePath, "rb");
        if(!fp)
        {
            cout << "open " << p_filePath << "failed" << endl;
            return nullptr;
        }

        read_len = fread(buffer, 1, 12, fp);
        if(read_len < 12)
        {
            cout << "error wav file" << endl;
            close_wav( &wav );
            return nullptr;
        }

        if(strncasecmp("RIFF", buffer, 4))
        {
            cout << "file style is not wav" << endl;
            close_wav( &wav );
            return nullptr;
        }

        memcpy(wav->riff.id, buffer, 4); 
        wav->riff.size = *(int *)(buffer + 4);
        if(strncasecmp("WAVE", buffer + 8, 4))
        {
            cout << "Error wav file" << endl;
            close_wav( &wav );
            return nullptr;
        }

        memcpy(wav->riff.type, buffer + 8, 4);
        wav->file_size = wav->riff.size + 8;
        offset += 12;

        while(true)
        {
            char id_buffer[5] = {0};
            int  tmp_size = 0;

            read_len = fread(buffer, 1, 8, fp);  
            if(read_len < 8)
            {
                cout << "Error wav file" << endl;
                close_wav( &wav );
                return nullptr;
            }
            memcpy(id_buffer, buffer, 4);
            tmp_size = *(int *)(buffer + 4);

            if(!strncasecmp("FMT", id_buffer, 3))
            {
                memcpy(wav->format.id, id_buffer, 3);
                wav->format.size = tmp_size;
                read_len = fread(buffer, 1, tmp_size, fp);
                if(read_len < tmp_size)
                {
                    cout << "Error wav file" << endl;
                    close_wav( &wav );
                    return nullptr;
                }
                wav->format.compression_code  = *(short *)buffer;
                wav->format.channels          = *(short *)(buffer + 2);
                wav->format.samples_per_sec   = *(int *)(buffer + 4);
                wav->format.avg_bytes_per_sec = *(int *)(buffer + 8);
                wav->format.block_align       = *(short *)(buffer + 12);
                wav->format.bits_per_sample   = *(short *)(buffer + 14);
            }
            else if(!strncasecmp("DATA", id_buffer, 3))
            {
                memcpy(wav->data.id, id_buffer, 4); 
                wav->data.size = tmp_size;
                offset += 8;
                wav->data_offset = offset;
                wav->data_size = wav->data.size; 
                break;
            }else
            {
                cout << "unhandled chunk: " << id_buffer << ", size: " << tmp_size << endl;
                fseek(fp, tmp_size, SEEK_CUR);
            }
            offset += 8 + tmp_size;
        }

        fseek(fp, wav->data_offset, SEEK_SET);
        wav->data_buf = (char *)malloc( wav->data.size );
        if( fread(wav->data_buf, 1, wav->data.size, fp) != wav->data.size )
        {
            cout << "read wav data faild" << endl;
        }
        fclose( fp );

        return wav;
    }

    void close_wav( wav_t ** p_wav )
    {
        wav_t * wav = *p_wav;
        if( !wav )
        {
            return;
        }

        if( wav->data_buf )
        {
            free( wav->data_buf );
        }

        free( wav );
        *p_wav = nullptr;
    }

}

