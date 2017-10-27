#include "json.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <sstream>
#include <sys/time.h>

int main(int argc, char ** argv)
{

    struct timeval t_start, t_end;

    const char * json[] = {
        "{\"haha\": 123, haha2: [1.1, [1.2, 1.3], 1.4, true, false],\
         haha3 : { \"haha4\": \"456\" \
         ,\
         xiaowu: { \"name\"  \
         :   \" big sb \", abc: true } } }",
        "{ha\"ha:{\"1haha2\": 123}}",
        "[12, -123, 12.3, 1.23e2]"
    };

    ws_core::node * t_root;

    gettimeofday(&t_start,NULL);
    ws_core::node * tmp = ws_core::parse( json[0] );
    gettimeofday(&t_end,NULL);

    std::cout << "parse time: " << 1000000 * (t_end.tv_sec - t_start.tv_sec)+ t_end.tv_usec - t_start.tv_usec << std::endl;


    gettimeofday(&t_start,NULL);
    std::cout << tmp->to_string() << std::endl;
    gettimeofday(&t_end,NULL);

    std::cout << "tostring time: " << 1000000 * (t_end.tv_sec - t_start.tv_sec)+ t_end.tv_usec - t_start.tv_usec << std::endl;


    gettimeofday(&t_start,NULL);
    free_json_node( &tmp );
    gettimeofday(&t_end,NULL);
    std::cout << "free time: " << 1000000 * (t_end.tv_sec - t_start.tv_sec)+ t_end.tv_usec - t_start.tv_usec << std::endl;

    //测试生成JSON
    t_root = ws_core::parse( "{ root: 12 }" );

    t_root->append("haha", ws_core::ARRAY)->get_sub_node("haha")
                    ->append("1")->append("2")->parent()
                    ->append("xiaowu", ws_core::STRING)->get_sub_node("xiaowu")->set_val("厉害了")->parent()
                    ->append( ws_core::create_json_node()->set_key( "abc" )->set_val(false) );

    std::cout << "myJson: " << t_root->to_string() << std::endl;

    ws_core::free_json_node( &t_root );

    //测试各api

    ws_core::node * t_test_root = ws_core::create_json_node();
    t_test_root->append( "string", "string---" )->append( "int", 12 )->append( "float", 12.34f )->append( "boolean", true )
    ->append( ws_core::create_json_node( )->set_key( "string1" )->set_val( "string1---" ) )
    ->append( ws_core::create_json_node( )->set_val( 13 )->set_key( "int1" ) )
    ->append( ws_core::create_json_node( )->set_key( "float1" )->set_val( 13.56f ) )
    ->append( ws_core::create_json_node( )->set_key( "boolean" )->set_val( false ) )
    ->append( ws_core::create_json_node( )->set_key( "object" )->append( "h", "haha" ) )
    ->append( ws_core::create_json_node( )->set_key( "array" )->append( "haha" )->append("xiaowu") )
    ->append( "string2", ws_core::STRING )->get_sub_node( "string2" )->set_val( "string2---" )->parent()
    ->append( "int2", ws_core::INT )->get_sub_node( "int2" )->set_val( 234 )->parent()
    ->append( "float2", ws_core::FLOAT )->get_sub_node( "float2" )->set_val( 56.27f )->parent()
    ->append( "boolean2", ws_core::BOOLEAN )->get_sub_node( "boolean2" )->set_val( false )->parent()
    ->append( "object2", ws_core::OBJECT )->get_sub_node( "object2" )->append("object2.2", ws_core::OBJECT)->parent()
    ->append( "array2", ws_core::ARRAY )
    ->append( ws_core::create_json_node( ws_core::OBJECT )->set_key( "object3" ) )
    ->append( ws_core::create_json_node( ws_core::OBJECT )->set_key(  "array3") )
    ->append( ws_core::create_json_node( ws_core::STRING )->set_key( "empty string" ) );



    std::cout << "test: " << t_test_root->to_string() << std::endl;

    char cmd_buf[2048] = {0};

    FILE * fp;
    
    while(true)
    {
        std::cout << "json file: ";
        std::cin.getline(cmd_buf, sizeof(cmd_buf));
        std::stringstream t_ss;
        
        if( !strcmp( cmd_buf, "q" ) || !strcmp( cmd_buf, "quit" ) || !strcmp( cmd_buf, "exit" ) )
        {
            exit(1);
        }

        fp = fopen( cmd_buf, "r" );
        if( !fp )
        {
            continue;
        }

        int t_read_len = 0;
        while(true)
        {
            t_read_len = fread( cmd_buf, 1, sizeof( cmd_buf ), fp );
            if( !t_read_len ) break;

            cmd_buf[t_read_len] = '\0';
            t_ss << cmd_buf;
        }

        std::string t_file_data = t_ss.str();

        gettimeofday(&t_start,NULL);
        ws_core::node * t_json = ws_core::parse( t_file_data.c_str() );
        gettimeofday(&t_end,NULL);
        std::cout << "parse time: " << 1000000 * (t_end.tv_sec - t_start.tv_sec)+ t_end.tv_usec - t_start.tv_usec << std::endl;


        gettimeofday(&t_start,NULL);
        std::string t_string = t_json->to_string();
        std::cout << "t_string.length: " << t_string.size() << std::endl;
        gettimeofday(&t_end,NULL);
        std::cout << "tostring time: " << 1000000 * (t_end.tv_sec - t_start.tv_sec)+ t_end.tv_usec - t_start.tv_usec << std::endl;

        std::cout << t_string << std::endl;

        gettimeofday(&t_start,NULL);
        ws_core::free_json_node( &t_json );
        gettimeofday(&t_end,NULL);
        std::cout << "free time: " << 1000000 * (t_end.tv_sec - t_start.tv_sec)+ t_end.tv_usec - t_start.tv_usec << std::endl;

    }

    return 0;
}