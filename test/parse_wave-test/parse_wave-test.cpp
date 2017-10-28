#include <iostream>
#include <cstdio>

int main( int argc, char ** argv )
{
    if( argc < 3 )
    {
        std::cout << "please designated input file and output file " << std::endl;
        return 1;
    }

    FILE * t_wave = fopen( argv[1], "r" );
    if( !t_wave )
    {
        std::cout << "open file faild: " << argv[1] << std::endl;
        return 2;
    }


    fclose( t_wave );
    return 0;
}