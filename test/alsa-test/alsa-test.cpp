#include <alsa/asoundlib.h>
#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <string>
#include <cstring>
#include <sys/time.h>

#include "alsa-test.h"


#define nullptr NULL

using std::cout;
using std::endl;
using std::string;


wav_t * openwav( const char * p_filePath );
void closewav( wav_t ** p_wav );

void * randomPlay(void *);


int main(int argc, char ** argv)
{
    char cmdBuffer[100];
    pthread_t pthread_id;
    string filePath;
    int playSpace;
    while( true )
    {
        memset(cmdBuffer, 0, sizeof(cmdBuffer));
        cout << "input play file: " << endl;
        // std::cin >> cmdBuffer;

        std::cin.getline(cmdBuffer, sizeof(cmdBuffer));

        cout << "----" << cmdBuffer << endl;

        if( !strcmp( cmdBuffer, "quit" ) )
        {
            exit(2);
        }


        if( !strcmp( cmdBuffer, "random2" ) )
        {
            playSpace = 500000;
            if( !pthread_create( &pthread_id, nullptr, randomPlay, (void *) &playSpace) )
            {
                cout << "randomPlay fail" << endl;
            }
            continue;
        }

        if( !strcmp( cmdBuffer, "random5" ) )
        {
            playSpace = 200000;
            if( !pthread_create( &pthread_id, nullptr, randomPlay, (void *) &playSpace) )
            {
                cout << "randomPlay fail" << endl;
            }
            continue;
        }

        if( !strcmp( cmdBuffer, "random10" ) )
        {
            playSpace = 100000;
            if( !pthread_create( &pthread_id, nullptr, randomPlay, (void *) &playSpace) )
            {
                cout << "randomPlay fail" << endl;
            }
            continue;
        }

        if( !strcmp( cmdBuffer, "random20" ) )
        {
            playSpace = 50000;
            if( !pthread_create( &pthread_id, nullptr, randomPlay, (void *) &playSpace) )
            {
                cout << "randomPlay fail" << endl;
            }
            continue;
        }

        if( !strcmp( cmdBuffer, "random30" ) )
        {
            playSpace = 33333;
            if( !pthread_create( &pthread_id, nullptr, randomPlay, (void *) &playSpace) )
            {
                cout << "randomPlay fail" << endl;
            }
            continue;
        }

        cout << "strlen(cmdBuffer):" << strlen(cmdBuffer) << cmdBuffer << endl;

        if( !strlen(cmdBuffer) )
        {
            cmdBuffer[0] = '1';
            cmdBuffer[1] = '\0';
        }

        #ifdef __arm__
            filePath = string("/usr/bin/audio/") + string(cmdBuffer) + ".wav";
        #else
            filePath = cmdBuffer;
        #endif

        if( !pthread_create(&pthread_id, nullptr, playWav, (void *)filePath.c_str()) )
        {
            cout << "playing " << filePath << "..." << endl;
        }
        
    }

    return 0;
}

wav_t * openwav( const char * p_filePath )
{
    wav_t * wav = NULL; 
    
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

    wav->fp = fopen(p_filePath, "rb");
    if(!wav->fp)
    {
        cout << "open " << p_filePath << "failed" << endl;
        return nullptr;
    }

    read_len = fread(buffer, 1, 12, wav->fp);
    if(read_len < 12)
    {
        cout << "error wav file" << endl;
        closewav( &wav );
        return nullptr;
    }

    if(strncasecmp("RIFF", buffer, 4))
    {
        cout << "file style is not wav" << endl;
        closewav( &wav );
        return nullptr;
    }

    memcpy(wav->riff.id, buffer, 4); 
    wav->riff.size = *(int *)(buffer + 4);
    if(strncasecmp("WAVE", buffer + 8, 4))
    {
        cout << "Error wav file" << endl;
        closewav( &wav );
        return nullptr;
    }

    memcpy(wav->riff.type, buffer + 8, 4);
    wav->file_size = wav->riff.size + 8;
    offset += 12;

    while(true)
    {
        char id_buffer[5] = {0};
        int  tmp_size = 0;

        read_len = fread(buffer, 1, 8, wav->fp);  
        if(read_len < 8)
        {
            cout << "Error wav file" << endl;
            closewav( &wav );
            return nullptr;
        }
        memcpy(id_buffer, buffer, 4);
        tmp_size = *(int *)(buffer + 4);

        if(!strncasecmp("FMT", id_buffer, 3))
        {
            memcpy(wav->format.id, id_buffer, 3);
            wav->format.size = tmp_size;
            read_len = fread(buffer, 1, tmp_size, wav->fp);
            if(read_len < tmp_size)
            {
                cout << "Error wav file" << endl;
                closewav( &wav );
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
            fseek(wav->fp, tmp_size, SEEK_CUR);
        }
        offset += 8 + tmp_size;
    }

    return wav;
}

void closewav( wav_t ** p_wav )
{
    cout << "closewav start" << endl;
    wav_t * wav = *p_wav;
    if( !wav )
    {
        return;
    }

    
    if(wav->fp)
    {
        fclose(wav->fp);
        wav->fp = nullptr;
    }
    free( wav );
    wav = nullptr;

    cout << "closewav end" << endl;
}

void * playWav( void * p_filePath )
{

    wav_t * wav = openwav( (char *) p_filePath );
    if( !wav )
    {
        return nullptr;
    }

    cout << "file size = " << wav->file_size << endl;

    cout << "RIFF WAVE Chunk" <<endl;
    cout << "id: " << wav->riff.id << endl;
    cout << "size: " << wav->riff.size << endl;
    cout << "type: " << wav->riff.type << endl;
    cout << endl;

    cout << "FORMAT Chunk" << endl;
    cout << "id: " << wav->format.id << endl;
    cout << "size: " << wav->format.size << endl;
    cout << "Channels = " << wav->format.channels << endl;
    cout << "SamplesPersec = " << wav->format.samples_per_sec << endl;
    cout << "avg_bytes_per_sec = " << wav->format.avg_bytes_per_sec << endl;
    cout << "block_align = " << wav->format.block_align << endl;
    cout << "BitsPerSample = " << wav->format.bits_per_sample << endl;
    cout << endl;

    cout << "DATA Chunk" << endl;
    cout << "id: " << wav->data.id << endl;
    cout << "data size: " << wav->data.size << endl;
    cout << "data offset: " << wav->data_offset << endl;

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
    data =(char*)malloc(dataSize);

    fseek(wav->fp, wav->data_offset, SEEK_SET);

    ret = fread( data, 1, dataSize, wav->fp );
    cout << "read size: " << ret << endl;
    if( ret != dataSize )
    {
        cout << "read wav data error" << endl;
        return nullptr;
    }

    while(true)
    {
        if( dataSize <= playDataSize )
        {
            cout << "play done" << endl;
            break;
        }

        memset( buffer, 0, size );
        memcpy( buffer, data + playDataSize, dataSize - playDataSize > size ? size : dataSize - playDataSize );
        playDataSize += dataSize - playDataSize > size ? size : dataSize - playDataSize;

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

    cout << "play Data size: " << playDataSize << endl;
    snd_pcm_drain( handle );
    snd_pcm_close( handle );
    free( data );
    free( buffer );
    closewav( &wav );

    return nullptr;
}

void * randomPlay(void * s)
{
    int i = 0;
    int index = 1;
    string wav_path;
    srand(time(NULL)); 
    char buffer[10];
    pthread_t pthread_id;
    int _sleep = *(int *)s;

    while( i++ < 100 )
    {
        index = rand() % 6 + 1;
        memset( buffer, 0, sizeof(buffer) );
        sprintf(buffer,"%d",index);
        wav_path = string("/usr/bin/audio/") + string(buffer) + ".wav";
        if( !pthread_create(&pthread_id, nullptr, playWav, (void *)wav_path.c_str()) )
        {
            cout << "playing " << wav_path << "..." << endl;
        }
        usleep( _sleep );
    }

    return nullptr;
}

