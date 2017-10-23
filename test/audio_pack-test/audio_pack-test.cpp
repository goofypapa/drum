#include <iostream>

#include "play_wav.h"

using std::cout;
using std::endl;



const char *audios[] = {
    "/media/cc/Extend/audio/1.wav",
    "/media/cc/Extend/audio/2.wav",
    "/media/cc/Extend/audio/3.wav",
    "/media/cc/Extend/audio/4.wav",
    "/media/cc/Extend/audio/5.wav"
};


int main(int argc, char ** argv)
{

    // int i = 0;
    // for(i = 0; i < sizeof(audios) / sizeof(char *); ++i)
    // {
    //     cout << i << ": " << audios[i] << endl;
    // }


    wav_t * wav_info = openwav("haha.wav");
    if( !wav_info )
    {
        cout << "open wav filed" << endl;
    }

    cout << "hello intensity test" << endl;


    char * buffer = 
    while(true)
    {

    }


    closewav(&wav_info);
    if( wav_info )
    {
        cout << "close wav filed" << endl;
    }


    return 0;
    
}