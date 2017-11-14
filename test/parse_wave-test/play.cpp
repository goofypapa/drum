#include "play_wav.h"
#include "ws_log.h"
#include "json.h"

#include <unistd.h>
#include <sys/time.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

#ifdef __arm__
    #define AFRICAN_DRUM_HIGH "/usr/bin/wave/S1.wav"
    #define AFRICAN_DRUM_MEDIANT "/usr/bin/wave/T1.wav"
    #define AFRICAN_DRUM_BASS "/usr/bin/wave/B1.wav"
#else
    #define AFRICAN_DRUM_HIGH "/mnt/hgfs/Work/wave/S1.wav"
    #define AFRICAN_DRUM_MEDIANT "/mnt/hgfs/Work/wave/T1.wav"
    #define AFRICAN_DRUM_BASS "/mnt/hgfs/Work/wave/B1.wav"
#endif

int main( int argc, char ** argv )
{

    if( argc < 2 )
    {
        err() << "input play file" << std::endl;
        return 1;
    }

    std::ifstream ifs( argv[1], std::ifstream::in );
    if( !ifs.is_open() )
    {
        return 2;
    }

    struct timeval t_start_time, t_curr_time;
    gettimeofday( &t_start_time, NULL );

    // ws_core::wav_t * t_wav = ws_core::open_wav( "/mnt/hgfs/Work/wave/1.wav" );

    ifs.seekg (0, ifs.end);
    int length = ifs.tellg();
    ifs.seekg (0, ifs.beg);

    char * buffer = new char[ length + 1 ];
    ifs.read( buffer, length );

    if( !ifs )
    {
        err() << "read file " << argv[1] << "faild" << std::endl;
        return 3;
    }
    ifs.close();
    buffer[ length ] = '\0';

    ws_core::node * t_json = ws_core::parse( buffer );

    int t_json_len = t_json->get_length();

    if( t_json_len <= 0 )
    {
        info() << "json file is empty" << std::endl;
        return 4;
    }

    int t_curr_index = 0;
    ws_core::node * t_item = t_json->get_sub_node( t_curr_index );
    float t_time = t_item->get_float_val(0);
    float t_value = t_item->get_float_val(1);
    int t_voice_type = t_item->get_int_val(2);
    float t_time_offset = t_time > 0.2f ? t_time - 0.2f : 0.0f;

    info() << "t_json_len: " << t_json_len << " " << t_time << ", " << t_value << std::endl;    
    while(true)
    {
        gettimeofday( &t_curr_time, NULL );
        float t_tmp = (1000000 * (t_curr_time.tv_sec - t_start_time.tv_sec)+ t_curr_time.tv_usec - t_start_time.tv_usec) / 1000000.0f + t_time_offset;
        
        if( fabs( t_time - t_tmp ) < 0.01f )
        {

            float t_gain = t_value / 8.0f * 0.6f + 0.4f;
            //播放声音
            info() << "playing: " << t_time << ", " << t_gain << std::endl;

            switch( t_voice_type )
            {
                case 0:
                    ws_core::play_wav( AFRICAN_DRUM_BASS, t_gain );
                    info() << "playing: " << AFRICAN_DRUM_BASS << std::endl;
                break;
                case 1:
                    ws_core::play_wav( AFRICAN_DRUM_MEDIANT, t_gain );
                    info() << "playing: " << AFRICAN_DRUM_MEDIANT << std::endl;
                break;
                case 2:
                    ws_core::play_wav( AFRICAN_DRUM_HIGH, t_gain );
                    info() << "playing: " << AFRICAN_DRUM_HIGH << std::endl;
                break;
            }
            
            //下一个点
            if( ++t_curr_index >= t_json_len )
            {
                std::cout << "播放完成" << std::endl;
                break;
            }

            t_item = t_json->get_sub_node( t_curr_index );
            t_time = t_item->get_float_val(0);
            t_value = t_item->get_float_val(1);
            t_voice_type = t_item->get_int_val(2);
            // info() << t_time << ", " << t_value << std::endl;
        }
        

        // std::cout << "curr: " << t_tmp << std::endl;

        usleep( 2000 );
    }

    usleep( 1000000 );
    // ws_core::close_wav( &t_wav );
    delete[] buffer;
    return 0;
}