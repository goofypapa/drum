#include "ws_log.h"
#include "json.h"


#include <iostream>
#include <fstream>

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

    ifs.seekg (0, ifs.end);
    int t_file_len = ifs.tellg();
    ifs.seekg (0, ifs.beg);

    char * t_file_buf = new char[ t_file_len + 1 ];
    ifs.read( t_file_buf, t_file_len );

    if( !ifs )
    {
        err() << "read file " << argv[1] << "faild" << std::endl;
        return 3;
    }
    ifs.close();
    t_file_buf[ t_file_len ] = '\0';

    ws_core::node * t_rhythm = ws_core::parse( t_file_buf );
    if( !t_rhythm )
    {
        err() << "parse rhythm faild" << std::endl;
        return 4;
    }

    float t_beatTime = 60.0f / (float)t_rhythm->get_int_val("speed");
    ws_core::node * t_rhythmData = t_rhythm->get_sub_node( "data" );
    float t_playTime = 0.0f;
    int t_currSubNodeIndex = 0;

    ws_core::node * t_outFile = ws_core::create_json_node();

    while(true)
    {
        ws_core::node * t_node = t_rhythmData->get_sub_node( t_currSubNodeIndex++ );
        
        if( !t_node )
        {
            break;
        }

        int t_note = t_node->get_int_val( "note" );
        std::string t_tone = t_node->get_string_val("tone");

        int t_tone_int = -1;
        if( t_tone == "B" )
        {
            t_tone_int = 0;
        }else if( t_tone == "T" )
        {
            t_tone_int = 1;
        }else if( t_tone == "S" )
        {
            t_tone_int = 2;
        }

        if( t_tone_int != -1 )
        {
            t_outFile->append( ws_core::create_json_node(ws_core::ARRAY)->append( t_playTime )->append( 6.0f )->append( t_tone_int ) );
        }
        
        t_playTime += t_beatTime / (float)t_note;
    }

    ofs << t_outFile->to_string();
    ofs.close();

    ws_core::free_json_node( &t_rhythm );
    ws_core::free_json_node( &t_outFile );

    return 0;
}