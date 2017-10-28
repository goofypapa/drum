#include "ws_log.h"


int main( int argc, char ** argv )
{

    ws_core::limit_lavel = ws_core::ERR;

    debug() << "debug" << std::endl;
    info() << "info" << std::endl;
    warn() << "warning" << std::endl;
    err() << "error" <<std::endl;
    return 0;
}