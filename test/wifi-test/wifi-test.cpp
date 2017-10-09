#include "wifi-test.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <pthread.h>
#include <unistd.h>

#include "wpa_ctrl.h"

#include <iostream>
#include <string>

using std::cout;
using std::endl;


char deviceFile[] = "/var/run/wpa_supplicant/wlan0";
struct wpa_ctrl *ctrl;
struct wpa_ctrl *monitor_conn;

int get_pid( char * p_name );

int startHostApd( void );

void * listen_wpa_supplicant_message(void * p);

const bool console_run(const char *cmdline);

int main(int argc, char ** argv)
{

    char buf[2048];
    size_t len;
    int ret;
    int hostapd_pid;

    pthread_t pthread_id;

    FILE* fp;
    fp = fopen("/tmp/wpa_supplicant.conf", "wt+");

    if (fp != 0) {
        fputs("ctrl_interface=/var/run/wpa_supplicant\n", fp);
        fputs("ap_scan=1\n", fp);
        fclose(fp);
    }

    hostapd_pid = get_pid((char *)"hostapd");
    if( hostapd_pid > 0)
    {
        char cmd_buf[16] = {0};
        snprintf(cmd_buf, sizeof(cmd_buf), "kill %d", hostapd_pid);
        console_run(cmd_buf);
    }

    while(get_pid((char *)"wpa_supplicant") <= 0)
    {
        ret = system("/usr/sbin/wpa_supplicant -Dnl80211 -iwlan0 -c /tmp/wpa_supplicant.conf &");
        if( ret < 0 )
        {
            cout << "Error system /usr/sbin/wpa_supplicant -Dnl80211 -iwlan0 -c /tmp/wpa_supplicant.conf &" << endl;
        }
    }

    cout << "wpa_supplicant pid: " << get_pid((char *)"wpa_supplicant") << endl;

    while(!ctrl)
    {
        ctrl = wpa_ctrl_open( deviceFile );
        if( !ctrl )
        {
            cout << "wpa ctrl open fild" << endl;
            usleep(2000);
        }
    }
    
    // monitor_conn = wpa_ctrl_open(deviceFile);

    // if (monitor_conn == NULL) 
    // {
	// 	wpa_ctrl_close(ctrl);
	// 	return -1;
    // }
    
    // if (wpa_ctrl_attach(monitor_conn)) 
    // {
	// 	wpa_ctrl_close(monitor_conn);
	// 	monitor_conn = NULL;
	// 	wpa_ctrl_close(ctrl);
	// 	ctrl = NULL;
	// 	return -1;
    // }
    
    // if( !pthread_create(&pthread_id, NULL, listen_wpa_supplicant_message,NULL) )
    // {
    //     cout << "listen_wpa_supplicant_message..." << endl;
    // }

    // len = sizeof(buf) - 1;
    // cout << "len: " << len << endl;

    // if (wpa_ctrl_request(ctrl, "INTERFACES", 10, buf, &len, NULL) >= 0) 
    // {
    //     buf[len] = '\0';
    //     cout << "INTERFACES: [" << len << "]" << buf << endl;
    // }

    
    // if (wpa_ctrl_request(ctrl, "GET_CAPABILITY eap", 18, buf, &len, NULL) >= 0)
    // {
    //     buf[len] = '\0';
    //     cout << "GET_CAPABILITY eap: [" << len << "] " << buf << endl;
    // }

    cout << "hello wifi-test" << endl;

    while(true)
    {
        std::cin >> buf;
        len = sizeof(buf) - 1;
        
        if( !strcmp( buf, "quit" ) )
        {
            break;
        }

        if( !strcmp( buf, "scan" ) )
        {
            ret = wpa_ctrl_request(ctrl, "SCAN", 4, buf, &len, NULL);
            if( ret == -2 )
            {
                cout << "time out" << endl;
                return 1;
            }else if( ret < 0 )
            {
                cout << "commond scan failed" << endl;
                return 1;
            }
            buf[len] = '\0';
            cout  << "SCAN: " << buf << endl;
            continue;
        }

        if( !strcmp( buf, "scan_result" ) )
        {
            ret = wpa_ctrl_request(ctrl, "SCAN_RESULTS", 12, buf, &len, NULL);
            if( ret == -2 )
            {
                cout << "time out" << endl;
                return 1;
            }else if( ret < 0 )
            {
                cout << "commond SCAN_RESULTS failed" << endl;
                return 1;
            }
        
            buf[len] = '\0';
            cout  << "SCAN_RESULTS: " << buf << endl;
            continue;
        }

        if( !strcmp( buf, "bss" ) )
        {
            char cmd[10] = {0};

            int i = 0;

            while( true )
            {
                len = sizeof(buf) - 1;
                snprintf(cmd, sizeof(cmd), "BSS %d", i);
                ret = wpa_ctrl_request(ctrl, cmd, strlen(cmd), buf, &len, NULL);
                if( ret == -2 )
                {
                    cout << "time out" << endl;
                    return 1;
                }else if( ret < 0 )
                {
                    cout << "commond " << cmd << " failed" << endl;
                    return 1;
                }else if( len == 0 )
                {
                    break;
                }

                buf[len] = '\0';
                cout  << cmd << ": [" << len << "]" << buf << endl;

                ++i;
            }

            continue;
        }

        if( !strcmp( buf, "add_network" ) )
        {
            ret = wpa_ctrl_request(ctrl, "ADD_NETWORK", 12, buf, &len, NULL);
            if( ret == -2 )
            {
                cout << "time out" << endl;
                return 1;
            }else if( ret < 0 )
            {
                cout << "commond ADD_NETWORK failed" << endl;
                return 1;
            }

            buf[len] = '\0';
            cout  << "ADD_NETWORK: " << buf << endl;
            continue;
        }

        if( !strcmp( buf, "set_network" ) )
        {
            char cmd[128] = {0};
            int net_id;
            char attribute[10] = {0};
            char value[128] = {0};

            cout << "net id: " << endl;
            std::cin >> net_id;

            cout << "attribute: " << endl;
            std::cin >> attribute;

            cout << "value: " << endl;
            std::cin >> value;

            snprintf(cmd, sizeof(cmd), "SET_NETWORK %d %s \"%s\"", net_id, attribute, value);

            ret = wpa_ctrl_request(ctrl, cmd, strlen(cmd), buf, &len, NULL);
            if( ret == -2 )
            {
                cout << "time out" << endl;
                return 1;
            }else if( ret < 0 )
            {
                cout << "commond " << cmd << " failed" << endl;
                return 1;
            }

            buf[len] = '\0';
            cout  << cmd << ": " << buf << endl;

            continue;
        }

        if( !strcmp( buf, "enable_network" ) )
        {
            int net_id;
            char cmd[20] = {0};

            cout << "net id: " << endl;
            std::cin >> net_id;

            snprintf(cmd, sizeof(cmd), "ENABLE_NETWORK %d", net_id);

            ret = wpa_ctrl_request(ctrl, cmd, strlen(cmd), buf, &len, NULL);
            if( ret == -2 )
            {
                cout << "time out" << endl;
                return 1;
            }else if( ret < 0 )
            {
                cout << "commond " << cmd << " failed" << endl;
                return 1;
            }

            buf[len] = '\0';
            cout  << cmd << ": " << buf << endl;

            continue;
        }

        if( !strcmp( buf, "select_network" ) )
        {
            int net_id;
            char cmd[20] = {0};

            cout << "net id: " << endl;
            std::cin >> net_id;

            snprintf(cmd, sizeof(cmd), "SELECT_NETWORK %d", net_id);

            ret = wpa_ctrl_request(ctrl, cmd, strlen(cmd), buf, &len, NULL);
            if( ret == -2 )
            {
                cout << "time out" << endl;
                return 1;
            }else if( ret < 0 )
            {
                cout << "commond " << cmd << " failed" << endl;
                return 1;
            }

            buf[len] = '\0';
            cout  << cmd << ": " << buf << endl;

            continue;
        }

        if( !strcmp( buf, "udhcpc" ) )
        {

            int id = -1;
            char cmd[20] = {0};
            ret = system("udhcpc -i wlan0");
            if( ret < 0 )
            {
                cout << "Error: udhcpc" << endl;
                return 1;
            }

            id = get_pid("udhcpc");
            if(id <= 0)
            {
                continue;
            }

            cout << "udhcpc id:" << id << endl;

            snprintf(cmd, sizeof(cmd), "kill %d", id);

            ret = system(cmd);
            if( ret < 0 )
            {
                cout << "Error: " << cmd << endl;
                return 1;
            }

            continue;
        }


        if( !strcmp( buf, "hostapd" ) )
        {
            startHostApd();
            continue;
        }

    }

    // ret = wpa_ctrl_request(ctrl, "LIST_NETWORKS", 13, buf, &len, NULL);
    // if( ret == -2 )
    // {
    //     cout << "time out" << endl;
    //     return 1;
    // }else if( ret < 0 )
    // {
    //     cout << "commond LIST_NETWORKS failed" << endl;
    //     return 1;
    // }

    // buf[len] = '\0';
    // cout  << "LIST_NETWORKS: " << buf << endl;


    // if( wpa_ctrl_request(ctrl, "STATUS", 6, buf, &len, NULL) < 0 )
    // {
    //     cout << "commond STATUS failed" << endl;
    //     return 1;
    // }
    // buf[len] = '\0';
    // cout  << "STATUS: " << buf << endl;

    wpa_ctrl_close( ctrl );

    return 0;
}


int startHostApd( void )
{
    char cmd[16] = {0};
    FILE * fp;
    fp = fopen("/tmp/hostapd.conf", "wt+");
    if (!fp) 
    {
        return -1;
    }

    int wpa_supplicant_pid = get_pid((char *)"wpa_supplicant");
    if( wpa_supplicant_pid > 0)
    {
        snprintf(cmd, sizeof(cmd), "kill %d", wpa_supplicant_pid);
        console_run(cmd);
    }

    char * name = "haha";
    char * password = "haha1234";

    fputs("interface=wlan0\n", fp);
    fputs("driver=nl80211\n", fp);
    fputs("ssid=", fp);
    fputs(name, fp);
    fputs("\n", fp);
    fputs("channel=6\n", fp);
    fputs("hw_mode=g\n", fp);
    fputs("ieee80211n=1\n", fp);
    fputs("ht_capab=[SHORT-GI-20]\n", fp);
    fputs("ignore_broadcast_ssid=0\n", fp);
    fputs("auth_algs=1\n", fp);
    fputs("wpa=3\n", fp);
    fputs("wpa_passphrase=", fp);
    fputs(password, fp);
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


    cout << "start ap" << endl;

    return 0;
}


void * listen_wpa_supplicant_message(void * p)
{
    char buf[256];
    size_t len;
    while(true)
    {
        if( monitor_conn && wpa_ctrl_pending(monitor_conn) > 0 )
        {
            len = sizeof(buf) - 1;
            if (wpa_ctrl_recv(monitor_conn, buf, &len) == 0)
            {
                buf[len] = '\0';
                cout << "wpa_ctrl_recv: " << buf << endl;
            }

            continue;
        }

        usleep( 2000 );
    }
}


const bool console_run(const char *cmdline) 
{
    int ret;
    ret = system(cmdline);
    if( ret < 0 )
    {
        cout << "Running cmdline failed: " << cmdline << endl;
        return false;
    }
    return true;
}


int get_pid( char * p_name )
{
    int pid = -1;
    FILE *pFile = NULL;
    std::string cmd = std::string("pidof ") + std::string(p_name);

    pFile = popen(cmd.c_str(), "r");
    char result[1024] = {0};
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