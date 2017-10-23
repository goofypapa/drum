
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <string>

#include "ws_common.h"
#include "wifi.h"

#include "wpa_ctrl.h"


using std::cout;
using std::endl;


namespace ws_core
{

    const char config_device_path[] = "/var/run/wpa_supplicant/wlan0";
    const char config_start_wpa_supplicant_cmd[] = "wpa_supplicant -Dnl80211 -iwlan0 -c /tmp/wpa_supplicant.conf &";

    std::string ap_ssid = "haha";
    std::string ap_psk = "haha1234";

    bool is_creste_wpa_supplicant_conf = false;

    struct wpa_ctrl *ctrl;

    bool run_wpa_cmd( const char * p_cmd, char * p_buf, size_t * p_buf_len );


    bool start_router(void);
    bool close_router(void);
    bool start_ap(void);
    bool close_ap(void);

    bool start_router(void)
    {
        FILE* fp;
        int hostapd_pid;

        close_ap();
        // close_router();

        if(!is_creste_wpa_supplicant_conf)
        {
            fp = fopen("/tmp/wpa_supplicant.conf", "wt+");
            if (fp <= 0) 
            {
                cout << "error create /tmp/wpa_supplicant.conf failed" << endl;
                return false;
            }

            fputs("ctrl_interface=/var/run/wpa_supplicant\n", fp);
            fputs("ap_scan=1\n", fp);
            fclose(fp);
            is_creste_wpa_supplicant_conf = true;
        }

        while(get_pid((char *)"wpa_supplicant") <= 0)
        {
            console_run(config_start_wpa_supplicant_cmd);
        }

        while(!ctrl)
        {
            ctrl = wpa_ctrl_open( config_device_path );
            if( !ctrl )
            {
                cout << "wpa ctrl open fild" << endl;
                usleep(2000);
            }
        }
        
        return true;
    }
    bool close_router(void)
    {
        char cmd_buf[16] = {0};
        int wpa_supplicant_pid = get_pid((char *)"wpa_supplicant");
        if( wpa_supplicant_pid > 0)
        {
            snprintf(cmd_buf, sizeof(cmd_buf), "kill %d", wpa_supplicant_pid);
            console_run(cmd_buf);
        }

        if(ctrl)
        {
            wpa_ctrl_close(ctrl);
            ctrl = NULL;
        }

        return true;
    }

    bool start_ap(void)
    {
        FILE * fp;

        close_ap();
        close_router();

        fp = fopen("/tmp/hostapd.conf", "wt+");
        if (!fp) 
        {
            return false;
        }

        fputs("interface=wlan0\n", fp);
        fputs("driver=nl80211\n", fp);
        fputs("ssid=", fp);
        fputs(ap_ssid.c_str(), fp);
        fputs("\n", fp);
        fputs("channel=6\n", fp);
        fputs("hw_mode=g\n", fp);
        fputs("ieee80211n=1\n", fp);
        fputs("ht_capab=[SHORT-GI-20]\n", fp);
        fputs("ignore_broadcast_ssid=0\n", fp);
        fputs("auth_algs=1\n", fp);
        fputs("wpa=3\n", fp);
        fputs("wpa_passphrase=", fp);
        fputs(ap_psk.c_str(), fp);
        fputs("\n", fp);
        fputs("wpa_key_mgmt=WPA-PSK\n", fp);
        fputs("wpa_pairwise=TKIP\n", fp);
        fputs("rsn_pairwise=CCMP", fp);

        fclose(fp);

        console_run("ifconfig wlan0 up");
        console_run("ifconfig wlan0 192.168.100.1 netmask 255.255.255.0");
        console_run("echo 1 > /proc/sys/net/ipv4/ip_forward");
        console_run("iptables --flush");
        console_run("iptables --table nat --flush");
        console_run("iptables --delete-chain");
        console_run("iptables --table nat --delete-chain");
        console_run("iptables --table nat --append POSTROUTING --out-interface eth0 -j MASQUERADE");
        console_run("iptables --append FORWARD --in-interface wlan0 -j ACCEPT");
        console_run("/usr/sbin/hostapd /tmp/hostapd.conf -B");

        return true;
        
    }

    bool close_ap(void)
    {
        char cmd_buf[16] = {0};
        int hostapd_pid = get_pid((char *)"hostapd");
        if( hostapd_pid > 0 )
        {
            snprintf(cmd_buf, sizeof(cmd_buf), "kill %d", hostapd_pid);
            console_run(cmd_buf);
        }

        return true;
    }


    bool init( WIFI_MODULE p_wifi_module )
    {
        if( !switch_module(p_wifi_module) )
        {
            return false;
        }
        
        return true;
    }

    bool switch_module( WIFI_MODULE p_wifi_module )
    {
        switch(p_wifi_module)
        {
            case ROUTER:
                return start_router();
            case AP:
                return start_ap();
            default:
                cout << "error unknown module: " << p_wifi_module << endl;
                return false;
        }
    }

    bool scan( CALL_BACK p_call_back )
    {
        char buf[2048] = {0};
        size_t buf_len = sizeof(buf) - 1;

        if( !run_wpa_cmd("SCAN", buf, &buf_len) )
        {
            return false;
        }

        buf[buf_len] = '\0';

        cout << "scan: " << buf << endl;

        if( !strcmp( buf, "OK" ) )
        {
            return false;
        }

        buf_len = sizeof(buf) - 1;
        if( !run_wpa_cmd("SCAN_RESULTS", buf, &buf_len) )
        {
            return false;
        }

        buf[buf_len] = '\0';
        
        if( p_call_back )
        {
            p_call_back( buf );
        }
        
        return true;
    }


    bool connect( std::string p_ssid, std::string p_psk, CALL_BACK p_call_back )
    {
        char buf[2048] = {0};
        char cmd[128] = {0};
        int net_id = -1;
        size_t buf_len = sizeof(buf) - 1;

        //add network
        if( !run_wpa_cmd("ADD_NETWORK", buf, &buf_len) )
        {
            return false;
        }
        buf[buf_len] = '\0';

        cout << "add_network: " << buf << endl;

        net_id = atoi(buf);

        //set ssid
        buf_len = sizeof(buf) - 1;
        snprintf(cmd, sizeof(cmd), "SET_NETWORK %d %s \"%s\"", net_id, "ssid", p_ssid.c_str());
        if( !run_wpa_cmd(cmd, buf, &buf_len) )
        {
            return false;
        }
        buf[buf_len] = '\0';

        cout << cmd << ": " << buf << endl;

        //set psk
        buf_len = sizeof(buf) - 1;
        snprintf(cmd, sizeof(cmd), "SET_NETWORK %d %s \"%s\"", net_id, "psk", p_psk.c_str());
        if( !run_wpa_cmd(cmd, buf, &buf_len) )
        {
            return false;
        }
        buf[buf_len] = '\0';

        cout << cmd << ": " << buf << endl;


        //enable network
        buf_len = sizeof(buf) - 1;
        snprintf(cmd, sizeof(cmd), "ENABLE_NETWORK %d", net_id);
        if( !run_wpa_cmd(cmd, buf, &buf_len) )
        {
            return false;
        }
        buf[buf_len] = '\0';

        cout << cmd << ": " << buf << endl;

        //set address
        if( system("udhcpc -i wlan0") < 0 )
        {
            cout << "Error: udhcpc" << endl;
            return false;
        }

        int udhcpc_pid = ws_core::get_pid((char *)"udhcpc");
        if(udhcpc_pid <= 0)
        {
            return true;
        }

        cout << "udhcpc id:" << udhcpc_pid << endl;

        snprintf(cmd, sizeof(cmd), "kill %d", udhcpc_pid);

        if( system(cmd) < 0 )
        {
            cout << "Error: " << cmd << endl;
            return false;
        }

        return true;
    }

    bool set_ap_info( std::string p_ssid, std::string p_psk )
    {
        ap_ssid = p_ssid;
        ap_psk = p_psk;
        return true;
    }


    bool run_wpa_cmd( const char * p_cmd, char * p_buf, size_t * p_buf_len )
    {
        int ret;
        if( !ctrl )
        {
            cout << "wpa ctrl not open" << endl;
            return false;
        }

        *p_buf_len = (*p_buf_len) - 1;

        ret = wpa_ctrl_request(ctrl, p_cmd, strlen(p_cmd), p_buf, p_buf_len, NULL);

        if( ret == -2 )
        {
            cout << "run_wpa_cmd: " << p_cmd << " is timeout" << endl;
            return false;
        }

        if( ret < 0 )
        {
            cout << "run_wpa_cmd: " << p_cmd << " failed" << endl;
            return false;
        }

        p_buf[*p_buf_len] = '\0';
        return true;
    }



    // char deviceFile[] = "/var/run/wpa_supplicant/wlan0";


    // int startHostApd( void );

    // void * listen_wpa_supplicant_message(void * p);

    // int main(int argc, char ** argv)
    // {

    //     char buf[2048];
    //     size_t len;
    //     int ret;
    //     int hostapd_pid;

    //     pthread_t pthread_id;

    //     FILE* fp;
    //     fp = fopen("/tmp/wpa_supplicant.conf", "wt+");

    //     if (fp != 0) {
    //         fputs("ctrl_interface=/var/run/wpa_supplicant\n", fp);
    //         fputs("ap_scan=1\n", fp);
    //         fclose(fp);
    //     }

    //     hostapd_pid = get_pid((char *)"hostapd");
    //     if( hostapd_pid > 0)
    //     {
    //         char cmd_buf[16] = {0};
    //         snprintf(cmd_buf, sizeof(cmd_buf), "kill %d", hostapd_pid);
    //         console_run(cmd_buf);
    //     }

    //     while(get_pid((char *)"wpa_supplicant") <= 0)
    //     {
    //         ret = system("/usr/sbin/wpa_supplicant -Dnl80211 -iwlan0 -c /tmp/wpa_supplicant.conf &");
    //         if( ret < 0 )
    //         {
    //             cout << "Error system /usr/sbin/wpa_supplicant -Dnl80211 -iwlan0 -c /tmp/wpa_supplicant.conf &" << endl;
    //         }
    //     }

    //     cout << "wpa_supplicant pid: " << get_pid((char *)"wpa_supplicant") << endl;

    //     while(!ctrl)
    //     {
    //         ctrl = wpa_ctrl_open( deviceFile );
    //         if( !ctrl )
    //         {
    //             cout << "wpa ctrl open fild" << endl;
    //             usleep(2000);
    //         }
    //     }
        
    //     cout << "hello wifi-test" << endl;

    //     while(true)
    //     {
    //         std::cin >> buf;
    //         len = sizeof(buf) - 1;
            
    //         if( !strcmp( buf, "quit" ) )
    //         {
    //             break;
    //         }

    //         if( !strcmp( buf, "scan" ) )
    //         {
    //             ret = wpa_ctrl_request(ctrl, "SCAN", 4, buf, &len, NULL);
    //             if( ret == -2 )
    //             {
    //                 cout << "time out" << endl;
    //                 return 1;
    //             }else if( ret < 0 )
    //             {
    //                 cout << "commond scan failed" << endl;
    //                 return 1;
    //             }
    //             buf[len] = '\0';
    //             cout  << "SCAN: " << buf << endl;
    //             continue;
    //         }

    //         if( !strcmp( buf, "scan_result" ) )
    //         {
    //             ret = wpa_ctrl_request(ctrl, "SCAN_RESULTS", 12, buf, &len, NULL);
    //             if( ret == -2 )
    //             {
    //                 cout << "time out" << endl;
    //                 return 1;
    //             }else if( ret < 0 )
    //             {
    //                 cout << "commond SCAN_RESULTS failed" << endl;
    //                 return 1;
    //             }
            
    //             buf[len] = '\0';
    //             cout  << "SCAN_RESULTS: " << buf << endl;
    //             continue;
    //         }

    //         if( !strcmp( buf, "bss" ) )
    //         {
    //             char cmd[10] = {0};

    //             int i = 0;

    //             while( true )
    //             {
    //                 len = sizeof(buf) - 1;
    //                 snprintf(cmd, sizeof(cmd), "BSS %d", i);
    //                 ret = wpa_ctrl_request(ctrl, cmd, strlen(cmd), buf, &len, NULL);
    //                 if( ret == -2 )
    //                 {
    //                     cout << "time out" << endl;
    //                     return 1;
    //                 }else if( ret < 0 )
    //                 {
    //                     cout << "commond " << cmd << " failed" << endl;
    //                     return 1;
    //                 }else if( len == 0 )
    //                 {
    //                     break;
    //                 }

    //                 buf[len] = '\0';
    //                 cout  << cmd << ": [" << len << "]" << buf << endl;

    //                 ++i;
    //             }

    //             continue;
    //         }

    //         if( !strcmp( buf, "add_network" ) )
    //         {
    //             ret = wpa_ctrl_request(ctrl, "ADD_NETWORK", 12, buf, &len, NULL);
    //             if( ret == -2 )
    //             {
    //                 cout << "time out" << endl;
    //                 return 1;
    //             }else if( ret < 0 )
    //             {
    //                 cout << "commond ADD_NETWORK failed" << endl;
    //                 return 1;
    //             }

    //             buf[len] = '\0';
    //             cout  << "ADD_NETWORK: " << buf << endl;
    //             continue;
    //         }

    //         if( !strcmp( buf, "set_network" ) )
    //         {
    //             char cmd[128] = {0};
    //             int net_id;
    //             char attribute[10] = {0};
    //             char value[128] = {0};

    //             cout << "net id: " << endl;
    //             std::cin >> net_id;

    //             cout << "attribute: " << endl;
    //             std::cin >> attribute;

    //             cout << "value: " << endl;
    //             std::cin >> value;

    //             snprintf(cmd, sizeof(cmd), "SET_NETWORK %d %s \"%s\"", net_id, attribute, value);

    //             ret = wpa_ctrl_request(ctrl, cmd, strlen(cmd), buf, &len, NULL);
    //             if( ret == -2 )
    //             {
    //                 cout << "time out" << endl;
    //                 return 1;
    //             }else if( ret < 0 )
    //             {
    //                 cout << "commond " << cmd << " failed" << endl;
    //                 return 1;
    //             }

    //             buf[len] = '\0';
    //             cout  << cmd << ": " << buf << endl;

    //             continue;
    //         }

    //         if( !strcmp( buf, "enable_network" ) )
    //         {
    //             int net_id;
    //             char cmd[20] = {0};

    //             cout << "net id: " << endl;
    //             std::cin >> net_id;

    //             snprintf(cmd, sizeof(cmd), "ENABLE_NETWORK %d", net_id);

    //             ret = wpa_ctrl_request(ctrl, cmd, strlen(cmd), buf, &len, NULL);
    //             if( ret == -2 )
    //             {
    //                 cout << "time out" << endl;
    //                 return 1;
    //             }else if( ret < 0 )
    //             {
    //                 cout << "commond " << cmd << " failed" << endl;
    //                 return 1;
    //             }

    //             buf[len] = '\0';
    //             cout  << cmd << ": " << buf << endl;

    //             continue;
    //         }

    //         if( !strcmp( buf, "select_network" ) )
    //         {
    //             int net_id;
    //             char cmd[20] = {0};

    //             cout << "net id: " << endl;
    //             std::cin >> net_id;

    //             snprintf(cmd, sizeof(cmd), "SELECT_NETWORK %d", net_id);

    //             ret = wpa_ctrl_request(ctrl, cmd, strlen(cmd), buf, &len, NULL);
    //             if( ret == -2 )
    //             {
    //                 cout << "time out" << endl;
    //                 return 1;
    //             }else if( ret < 0 )
    //             {
    //                 cout << "commond " << cmd << " failed" << endl;
    //                 return 1;
    //             }

    //             buf[len] = '\0';
    //             cout  << cmd << ": " << buf << endl;

    //             continue;
    //         }

    //         if( !strcmp( buf, "udhcpc" ) )
    //         {

    //             int id = -1;
    //             char cmd[20] = {0};
    //             ret = system("udhcpc -i wlan0");
    //             if( ret < 0 )
    //             {
    //                 cout << "Error: udhcpc" << endl;
    //                 return 1;
    //             }

    //             id = get_pid("udhcpc");
    //             if(id <= 0)
    //             {
    //                 continue;
    //             }

    //             cout << "udhcpc id:" << id << endl;

    //             snprintf(cmd, sizeof(cmd), "kill %d", id);

    //             ret = system(cmd);
    //             if( ret < 0 )
    //             {
    //                 cout << "Error: " << cmd << endl;
    //                 return 1;
    //             }

    //             continue;
    //         }


    //         if( !strcmp( buf, "hostapd" ) )
    //         {
    //             startHostApd();
    //             continue;
    //         }

    //     }

    //     // ret = wpa_ctrl_request(ctrl, "LIST_NETWORKS", 13, buf, &len, NULL);
    //     // if( ret == -2 )
    //     // {
    //     //     cout << "time out" << endl;
    //     //     return 1;
    //     // }else if( ret < 0 )
    //     // {
    //     //     cout << "commond LIST_NETWORKS failed" << endl;
    //     //     return 1;
    //     // }

    //     // buf[len] = '\0';
    //     // cout  << "LIST_NETWORKS: " << buf << endl;


    //     // if( wpa_ctrl_request(ctrl, "STATUS", 6, buf, &len, NULL) < 0 )
    //     // {
    //     //     cout << "commond STATUS failed" << endl;
    //     //     return 1;
    //     // }
    //     // buf[len] = '\0';
    //     // cout  << "STATUS: " << buf << endl;

    //     wpa_ctrl_close( ctrl );

    //     return 0;
    // }


    // int startHostApd( void )
    // {
    //     char cmd[16] = {0};
    //     FILE * fp;
    //     fp = fopen("/tmp/hostapd.conf", "wt+");
    //     if (!fp) 
    //     {
    //         return -1;
    //     }

    //     int wpa_supplicant_pid = get_pid((char *)"wpa_supplicant");
    //     if( wpa_supplicant_pid > 0)
    //     {
    //         snprintf(cmd, sizeof(cmd), "kill %d", wpa_supplicant_pid);
    //         console_run(cmd);
    //     }

    //     char * name = "haha";
    //     char * password = "haha1234";

    //     fputs("interface=wlan0\n", fp);
    //     fputs("driver=nl80211\n", fp);
    //     fputs("ssid=", fp);
    //     fputs(name, fp);
    //     fputs("\n", fp);
    //     fputs("channel=6\n", fp);
    //     fputs("hw_mode=g\n", fp);
    //     fputs("ieee80211n=1\n", fp);
    //     fputs("ht_capab=[SHORT-GI-20]\n", fp);
    //     fputs("ignore_broadcast_ssid=0\n", fp);
    //     fputs("auth_algs=1\n", fp);
    //     fputs("wpa=3\n", fp);
    //     fputs("wpa_passphrase=", fp);
    //     fputs(password, fp);
    //     fputs("\n", fp);
    //     fputs("wpa_key_mgmt=WPA-PSK\n", fp);
    //     fputs("wpa_pairwise=TKIP\n", fp);
    //     fputs("rsn_pairwise=CCMP", fp);

    //     fclose(fp);

    //     console_run("ifconfig wlan0 up");
    //     console_run("ifconfig wlan0 192.168.100.1 netmask 255.255.255.0");
    //     console_run("echo 1 > /proc/sys/net/ipv4/ip_forward");
    //     console_run("iptables --flush");
    //     console_run("iptables --table nat --flush");
    //     console_run("iptables --delete-chain");
    //     console_run("iptables --table nat --delete-chain");
    //     console_run("iptables --table nat --append POSTROUTING --out-interface eth0 -j MASQUERADE");
    //     console_run("iptables --append FORWARD --in-interface wlan0 -j ACCEPT");
    //     console_run("/usr/sbin/hostapd /tmp/hostapd.conf -B");


    //     cout << "start ap" << endl;

    //     return 0;
    // }


    // void * listen_wpa_supplicant_message(void * p)
    // {
    //     char buf[256];
    //     size_t len;
    //     while(true)
    //     {
    //         if( monitor_conn && wpa_ctrl_pending(monitor_conn) > 0 )
    //         {
    //             len = sizeof(buf) - 1;
    //             if (wpa_ctrl_recv(monitor_conn, buf, &len) == 0)
    //             {
    //                 buf[len] = '\0';
    //                 cout << "wpa_ctrl_recv: " << buf << endl;
    //             }

    //             continue;
    //         }

    //         usleep( 2000 );
    //     }
    // }
}
