#include "json.h"
#include <iostream>

int main(int argc, char ** argv)
{

    const char * json[] = {
        "{\"haha\": 123, haha2: 1.2 ,\
         haha3 : { \"haha4\": \"456\" \
         ,\
         xiaowu: { \"name\"  \
         :   \" big sb \" } } }",
        "{ha\"ha:{\"1haha2\": 123}}",
        "[12, -123, 12.3, 1.23e2]"
    };


    // int err = 0;
    // for( int i = 0; i < sizeof(json) / sizeof(char *); ++i )
    // {
    //     std::cout << "check: " << json[i] << std::endl;

    //     err = ws_core::checks( json[i] );
    //     if( err )
    //     {
    //         std::cout << "index: " << err << " char: " << json[i][err] << std::endl;
    //     }
    // }

    // for( int i = 0; i < sizeof(json) / sizeof(char *); ++i )
    // {
    //     ws_core::node * tmp = ws_core::parse( json[i] );
    //     if( !tmp )
    //     {
    //         std::cout << "parse faild" << std::endl;
    //     }
    //     ws_core::free_json_node( &tmp );
    // }

    ws_core::node * tmp = ws_core::parse( json[0] );

    std::cout << "haha: " << tmp->get_sub_node( "haha" )->get_int_val() << std::endl;
    std::cout << "haha3.xiaowu.name: " << tmp->get_sub_node( "haha3" )->get_sub_node("xiaowu")->get_string_val("name") << std::endl;


    std::cout << "tmp->to_string(): " << tmp->to_string() << std::endl;

    ws_core::free_json_node( &tmp );

    return 0;
}