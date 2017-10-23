#include "wifi.h"

#include <iostream>
#include <cstdlib>
#include <string.h>
#include <string>


using std::cin;
using std::cout;
using std::endl;


void scan_result( std::string p_result );

int main(int argc, char ** argv)
{
    char cmd_buf[128] = {0};

    ws_core::init();

    while(true)
    {
        cin.getline(cmd_buf, sizeof(cmd_buf));

        if( !strcmp( cmd_buf, "q" ) || !strcmp( cmd_buf, "quit" ) || !strcmp( cmd_buf, "exit" ) )
        {
            exit(1);
        }

        if( !strcmp( cmd_buf, "scan" ) )
        {
            if( ! ws_core::scan(scan_result) )
            {
                cout << "scan: filad" << endl;
            }
        }

        if( !strcmp( cmd_buf, "connect" ) )
        {
            char ssid[128] = {0};
            char psk[128] = {0};

            cout << "ssid: ";
            cin.getline(ssid, sizeof(ssid));

            cout << "psk: ";
            cin.getline(psk, sizeof(psk));

            if( ! ws_core::connect( ssid, psk ) )
            {
                cout << "connect:" << ssid << "," << psk << " filad" << endl;
            }
        }

        if( !strcmp( cmd_buf, "ap" ) )
        {
            char ssid[128] = {0};
            char psk[128] = {0};

            cout << "ssid: ";
            cin.getline(ssid, sizeof(ssid));

            cout << "psk: ";
            cin.getline(psk, sizeof(psk));
            
            ws_core::set_ap_info( ssid, psk );

            if( ws_core::init( ws_core::AP ) )
            {
                cout << "init: AP success" << endl;
            }
        }

        if( !strcmp( cmd_buf, "router" ) )
        {
            char ssid[128] = {0};
            char psk[128] = {0};

            if( ws_core::init( ws_core::ROUTER ) )
            {
                cout << "init: ROUTER success" << endl;
            }
        }

    }
    return 0;
}

void scan_result( std::string p_result )
{
    cout << "scan: " << p_result << endl;
}