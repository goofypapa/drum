#include <iostream>
#include <string.h>
#include <cstdlib>

#include "play_wav.h"

using std::cin;
using std::cout;
using std::endl;

int main(int argc, char ** argv)
{
    char cmd_buf[2048] = {0};

    while(true)
    {
        cin.getline(cmd_buf, sizeof(cmd_buf));
        
        if( !strcmp( cmd_buf, "q" ) || !strcmp( cmd_buf, "quit" ) || !strcmp( cmd_buf, "exit" ) )
        {
            exit(1);
        }

        ws_core::play_wav( cmd_buf, 0.8f );
    }
    return 0;
}