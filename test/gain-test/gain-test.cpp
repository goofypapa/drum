#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>

#include "play_wav.h"


using std::cin;
using std::cout;
using std::endl;
using std::string;

#define haha(a) (int)(a / 2.0f )

int main(int argc, char ** argv)
{
    char cmd_buf[128] = {0};

    string workPath = "/usr/bin/audio/";
    string filePath = workPath + "1.wav";
    
    while(true)
    {
        cin.getline(cmd_buf, sizeof(cmd_buf));

        if( !strcmp( cmd_buf, "quit" ) )
        {
            exit(1);
        }

        if( !strcmp( cmd_buf, "q" ) )
        {
            filePath = workPath + "1.wav";
            playWav(filePath.c_str(), 1.0f);
            continue;
        }

        if( !strcmp( cmd_buf, "w" ) )
        {
            filePath = workPath + "1.wav";
            playWav(filePath.c_str(), 0.8f);
            continue;
        }

        if( !strcmp( cmd_buf, "e" ) )
        {
            filePath = workPath + "1.wav";
            playWav(filePath.c_str(), 0.6f);
            continue;
        }

        if( !strcmp( cmd_buf, "r" ) )
        {
            filePath = workPath + "1.wav";
            playWav(filePath.c_str(), 0.4f);
            continue;
        }

        if( !strcmp( cmd_buf, "t" ) )
        {
            filePath = workPath + "1.wav";
            playWav(filePath.c_str(), 0.2f);
            continue;
        }

        if( !strcmp( cmd_buf, "y" ) )
        {
            filePath = workPath + "1.wav";
            playWav(filePath.c_str(), 0.1f);
            continue;
        }

        if( !strcmp( cmd_buf, "u" ) )
        {
            filePath = workPath + "1.wav";
            playWav(filePath.c_str(), 0.05f);
            continue;
        }

        if( !strcmp( cmd_buf, "a" ) )
        {
            filePath = workPath + "1.wav";
            playWav(filePath.c_str(), 1.0f);
            usleep(haha(1000000));
            filePath = workPath + "3.wav";
            playWav(filePath.c_str(), 1.0f);
            usleep(haha(500000));
            filePath = workPath + "4.wav";
            playWav(filePath.c_str(), 1.0f);
            usleep(haha(500000));
            filePath = workPath + "2.wav";
            playWav(filePath.c_str(), 1.0f);
            usleep(haha(500000));
            filePath = workPath + "3.wav";
            playWav(filePath.c_str(), 1.0f);
            usleep(haha(500000));
            filePath = workPath + "5.wav";
            playWav(filePath.c_str(), 1.0f);
            usleep(haha(1000000));

            filePath = workPath + "1.wav";
            playWav(filePath.c_str(), 1.0f);
            usleep(haha(1000000));
            filePath = workPath + "3.wav";
            playWav(filePath.c_str(), 1.0f);
            usleep(haha(500000));
            filePath = workPath + "4.wav";
            playWav(filePath.c_str(), 1.0f);
            usleep(haha(500000));
            filePath = workPath + "2.wav";
            playWav(filePath.c_str(), 1.0f);
            usleep(haha(500000));
            filePath = workPath + "3.wav";
            playWav(filePath.c_str(), 1.0f);
            usleep(haha(500000));
            filePath = workPath + "5.wav";
            playWav(filePath.c_str(), 1.0f);
            usleep(haha(1000000));

            filePath = workPath + "1.wav";
            playWav(filePath.c_str(), 1.0f);
            usleep(haha(1000000));
            filePath = workPath + "3.wav";
            playWav(filePath.c_str(), 1.0f);
            usleep(haha(500000));
            filePath = workPath + "4.wav";
            playWav(filePath.c_str(), 1.0f);
            usleep(haha(500000));
            filePath = workPath + "2.wav";
            playWav(filePath.c_str(), 1.0f);
            usleep(haha(500000));
            filePath = workPath + "1.wav";
            playWav(filePath.c_str(), 1.0f);
            usleep(haha(250000));
            filePath = workPath + "3.wav";
            playWav(filePath.c_str(), 1.0f);
            usleep(haha(250000));
            filePath = workPath + "5.wav";
            playWav(filePath.c_str(), 1.0f);
            usleep(haha(1000000));

            continue;

        }

        if( !strcmp( cmd_buf, "s" ) )
        {
            filePath = workPath + "1.wav";

            playWav(filePath.c_str(), 1.0f);
            usleep(haha(1000000));
            playWav(filePath.c_str(), 0.6f);
            usleep(haha(500000));
            playWav(filePath.c_str(), 0.4f);
            usleep(haha(500000));
            playWav(filePath.c_str(), 0.8f);
            usleep(haha(500000));
            playWav(filePath.c_str(), 0.6f);
            usleep(haha(500000));
            playWav(filePath.c_str(), 0.2f);
            usleep(haha(1000000));

            playWav(filePath.c_str(), 1.0f);
            usleep(haha(1000000));
            playWav(filePath.c_str(), 0.6f);
            usleep(haha(500000));
            playWav(filePath.c_str(), 0.4f);
            usleep(haha(500000));
            playWav(filePath.c_str(), 0.8f);
            usleep(haha(500000));
            playWav(filePath.c_str(), 0.6f);
            usleep(haha(500000));
            playWav(filePath.c_str(), 0.2f);
            usleep(haha(1000000));

            playWav(filePath.c_str(), 1.0f);
            usleep(haha(1000000));
            playWav(filePath.c_str(), 0.6f);
            usleep(haha(500000));
            playWav(filePath.c_str(), 0.4f);
            usleep(haha(500000));
            playWav(filePath.c_str(), 0.8f);
            usleep(haha(500000));
            playWav(filePath.c_str(), 0.6f);
            usleep(haha(250000));
            playWav(filePath.c_str(), 0.6f);
            usleep(haha(250000));
            playWav(filePath.c_str(), 0.2f);
            usleep(haha(1000000));

            continue;

        }

        filePath = workPath + string(cmd_buf) + ".wav";

        playWav(filePath.c_str(), 1.0f);
    }
}