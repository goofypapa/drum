#include "play_wav.h"
#include "ws_log.h"
#include "json.h"


#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/time.h>

#ifdef __arm__
    #define AFRICAN_DRUM_HIGH "/usr/bin/wave/S1.wav"
    #define AFRICAN_DRUM_MEDIANT "/usr/bin/wave/T1.wav"
    #define AFRICAN_DRUM_BASS "/usr/bin/wave/B1.wav"
#else
    #define AFRICAN_DRUM_HIGH "/mnt/hgfs/Work/wave/S1.wav"
    #define AFRICAN_DRUM_MEDIANT "/mnt/hgfs/Work/wave/T1.wav"
    #define AFRICAN_DRUM_BASS "/mnt/hgfs/Work/wave/B1.wav"
#endif

#define RHYTHM_FILE "rhythm.json"

int main( int argc, char ** argv )
{   

    int m_file_len = 0;
    char * m_file_buf = NULL;
    ws_core::node * m_rhythm = NULL;
    struct timeval m_startPlayingTime, m_currPlayingTime;

    //每拍时间　单位毫秒
    int m_beatTime = 0;

    std::ifstream ifs( RHYTHM_FILE, std::ifstream::in );
    if( !ifs.is_open() )
    {
        err() << "open file faild: " << RHYTHM_FILE << std::endl;
        return 2;
    }

     ifs.seekg (0, ifs.end);
    m_file_len = ifs.tellg();
    ifs.seekg (0, ifs.beg);

    m_file_buf = new char[ m_file_len + 1 ];
    ifs.read( m_file_buf, m_file_len );

    if( !ifs )
    {
        err() << "read file " << RHYTHM_FILE << "faild" << std::endl;
        return 3;
    }
    ifs.close();
    m_file_buf[ m_file_len ] = '\0';

    m_rhythm = ws_core::parse( m_file_buf );
    if( !m_rhythm )
    {
        err() << "parse rhythm faild" << std::endl;
        return 4;
    }

    info() << "playing " << m_rhythm->get_string_val("name") << " by " << m_rhythm->get_int_val("speed") << "/min" << std::endl;
    m_beatTime = 60 * 1000 / m_rhythm->get_int_val("speed");
    info() << "beatTime: " << m_beatTime << std::endl;

    gettimeofday( &m_startPlayingTime, NULL );


    int t_toneStartTime = 0, t_toneEndTime = 0;
    int t_currNodeTime = 0;

    ws_core::node * t_rhythmData = m_rhythm->get_sub_node( "data" );

    int t_currNodeIndex = 0;
    ws_core::node * t_currNode = t_rhythmData->get_sub_node( t_currNodeIndex );

    if( !t_currNode )
    {
        info() << "rhythm is empty" << std::endl;
        return 0;
    }

    debug() << "---------------" << std::endl;

    t_currNodeTime = m_beatTime / t_currNode->get_int_val( "note" );

    debug() << "---------------" << std::endl;
    t_toneStartTime = t_toneEndTime + t_currNodeTime / 2;
    t_toneEndTime += t_currNodeTime;
    bool t_isPlayd = false;

    debug() << "---------------" << std::endl;
    while(true)
    {
        gettimeofday( &m_currPlayingTime, NULL );
        int t_curr_time = (1000000 * (m_currPlayingTime.tv_sec - m_startPlayingTime.tv_sec)+ m_currPlayingTime.tv_usec - m_startPlayingTime.tv_usec) / 1000;

        if( t_isPlayd )
        {
            if( t_curr_time >= t_toneEndTime )
            {
                t_currNode = t_rhythmData->get_sub_node( ++t_currNodeIndex );
                if( !t_currNode )
                {
                    break;
                }
                t_currNodeTime = m_beatTime / t_currNode->get_int_val( "note" );
                t_toneStartTime = t_toneEndTime + t_currNodeTime / 2;
                t_toneEndTime += t_currNodeTime;
                t_isPlayd = false;
            }
        }else
        {
            if( t_curr_time >= t_toneStartTime )
            {

                std::string t_tone = t_currNode->get_string_val( "tone" );
                info() << "play " << t_tone;

                if( t_tone == "B" )
                {
                    ws_core::play_wav( AFRICAN_DRUM_BASS, 1.0f );
                }
                if( t_tone == "T" )
                {
                    ws_core::play_wav( AFRICAN_DRUM_MEDIANT, 1.0f );
                }
                if( t_tone == "S" )
                {
                    ws_core::play_wav( AFRICAN_DRUM_HIGH, 1.0f );
                }

                t_isPlayd = true;
            }
        }

        usleep( 1000 );
    }

    info() << "play end" << std::endl;
    usleep(1000000);

    ws_core::free_json_node( &m_rhythm );
    delete[] m_file_buf;
    return 0;
}