#ifndef __WIFI_H__
#define __WIFI_H__

#include <string>

namespace ws_core
{
 
    typedef void (*CALL_BACK)(std::string p_result);
    
    enum WIFI_MODULE{ ROUTER, AP };

    bool init( WIFI_MODULE p_wifi_module = ROUTER );

    bool switch_module( WIFI_MODULE p_wifi_module = ROUTER );

    bool scan( CALL_BACK p_call_back = NULL );

    bool connect( std::string p_ssid, std::string p_psk, CALL_BACK p_call_back = NULL );

    bool set_ap_info( std::string p_ssid, std::string p_psk );

}

#endif //__WIFI_H__