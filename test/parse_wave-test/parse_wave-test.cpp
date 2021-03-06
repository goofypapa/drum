#include <iostream>
#include <cstdio>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <cmath>
#include <unistd.h>
#include "ws_log.h"
#include "json.h"

#define INVALID_DATA -6.8f
#define BUFFER_TIME 0.10f
#define STEP 31
#define MIN 0.4f

int main( int argc, char ** argv )
{
    if( argc < 3 )
    {
        info() << "please designated input file and output file " << std::endl;
        return 1;
    }

    std::ifstream ifs( argv[1], std::ifstream::in );
    if( !ifs.is_open() )
    {
        err() << "open file faild: " << argv[1] << std::endl;
        return 2;
    }

    std::ofstream ofs( argv[2], std::ofstream::out );
    if( !ofs.is_open() )
    {
        err() << "open file faild: " << argv[2] << std::endl;
        return 2;
    }

    char buf[1024] = {0};
    std::vector< std::pair< float, float > > t_vector;
    std::string t_line, t_item;
    size_t i = 0;
    size_t t_prev_index = 0, t_curr_index = 0;
    float t_time, t_value;
    float t_start_time;
    bool find_start_time = true;
    float t_tunk_time = 0.0f, t_prev_tunk_time = 0.0f;
    float t_tunk_value = 0.0f, t_prev_tunk_value = 0.0f;
    ws_core::node * t_json = ws_core::create_json_node( );

    while( ifs.good() )
    {
        ifs.getline( buf, sizeof( buf ) );
        if( strlen( buf ) >= sizeof( buf ) )
        {
            err() << "readline overflow" << std::endl;
            return 3;
        }

        t_line = buf;
        
        t_prev_index = 0;
        t_curr_index = 0;
        for( i = 0; true; ++i )
        {
            if( i > 1 )
            {
                break;
            }

            t_curr_index = t_line.find( ",", t_curr_index + 1);
            if( t_curr_index == std::string::npos )
            {
                if( i == 0 )
                {
                    break;
                }
                t_item = t_line.substr( t_prev_index );
            }
            else
            {
                t_item = t_line.substr( t_prev_index, t_curr_index - t_prev_index );
            }
            t_prev_index = t_curr_index + 1;

            // std::cout << "i: " << i << ", " << t_item << std::endl;
            float t_tmp = atof( t_item.c_str() );
            if( i == 0 )
            {
                if( t_tmp == 0.0f )
                {
                    break;
                }
                t_time = t_tmp;
            }

            if( i == 1 )
            {
                t_value = t_tmp;
            }

            if( t_curr_index == std::string::npos )
            {
                break;
            }
        }

        if( i != 1 )
        {
            continue;
        }

        if( t_value == INVALID_DATA )
        {
            continue;
        }else if( find_start_time )
        {
            t_start_time = t_time;
            find_start_time = false;
        }

        // std::cout << "t_time: " << t_time << ", t_value: " << t_value << std::endl;

        t_vector.push_back( std::pair< float, float >( t_time - t_start_time, fabs( t_value ) ) );

        if( t_vector.size() > STEP )
        {
            t_vector.erase( t_vector.begin() );
        }else
        {
            continue;
        }

        bool t_is_valley = true;
        int t_mid = (int)( t_vector.size() / 2 );
        for( i = 0; i < t_vector.size(); ++i )
        {
            if( i == t_mid )
            {
                continue;
            }
            if( t_vector[i].second > t_vector[t_mid].second )
            {
                t_is_valley = false;
            }
        }

        if( !t_is_valley )
        {
            continue;
        }

        t_tunk_time = t_vector[t_mid].first;
        t_tunk_value = t_vector[t_mid].second;

        if( t_tunk_value < MIN )
        {
            t_prev_tunk_time = t_tunk_time;
            t_prev_tunk_value = t_tunk_value;
            continue;
        }

        if( t_tunk_time - t_prev_tunk_time < BUFFER_TIME )
        {
            if( t_prev_tunk_value >= t_tunk_value || t_tunk_time == t_prev_tunk_time )
            {
                t_prev_tunk_time = t_tunk_time;
                t_prev_tunk_value = t_tunk_value;
                continue;
            }
        }

        t_prev_tunk_time = t_tunk_time;
        t_prev_tunk_value = t_tunk_value;

        std::cout << "t_tunk_time: " << t_tunk_time << ", t_tunk_value: " << t_tunk_value << std::endl;

        t_json->append( ws_core::create_json_node( ws_core::ARRAY )->append( t_tunk_time )->append( t_tunk_value )->append( 2 ) );

        // std::cout << "-------------------" << t_vector[t_vector.size() - 1 - t_mid].first << ":" << t_vector[t_vector.size() - 1 - t_mid].second << std::endl;
    
        // usleep( 20000 );
    }

    ifs.close();

    ofs << t_json->to_string();

    debug() << "parse wave seccuss" << std::endl;

    return 0;
}