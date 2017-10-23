#include "ws_common.h"

#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>


namespace ws_core
{

    int get_pid( char * p_name )
    {
        int pid = -1;
        FILE *pFile = NULL;
        std::string cmd = std::string("pidof ") + std::string(p_name);
    
        pFile = popen(cmd.c_str(), "r");
        char result[16] = {0};
        if(!pFile)
        {
            return pid;
        }
        while ( fgets(result, sizeof(result), pFile) ) 
        {
            pid = atoi(result);
            break;
        }
        pclose(pFile);
        return pid;
    }


    const bool console_run(const char *cmdline) 
    {
        int ret;
        ret = system(cmdline);
        if( ret < 0 )
        {
            std::cout << "Running cmdline failed: " << cmdline << std::endl;
            return false;
        }
        return true;
    }

}