#ifndef __WS_LOG_H__
#define __WS_LOG_H__

#include <iostream>
#include <pthread.h>

namespace ws_core
{

    #define debug() ws_core::limit_lavel <= ws_core::DEBUG && ws_log( ws_core::DEBUG, __FILE__, __FUNCTION__, __LINE__ )
    #define info() ws_core::limit_lavel <= ws_core::INFO && ws_log( ws_core::INFO, __FILE__, __FUNCTION__, __LINE__ )
    #define notice() ws_core::limit_lavel <= ws_core::NOTICE && ws_log( ws_core::NOTICE, __FILE__, __FUNCTION__, __LINE__ )
    #define warn() ws_core::limit_lavel <= ws_core::WARN && ws_log( ws_core::WARN, __FILE__, __FUNCTION__, __LINE__ )
    #define err() ws_core::limit_lavel <= ws_core::ERR && ws_log( ws_core::ERR, __FILE__, __FUNCTION__, __LINE__ )
    #define crit() ws_core::limit_lavel <= ws_core::CRIT && ws_log( ws_core::CRIT, __FILE__, __FUNCTION__, __LINE__ )
    #define alert() ws_core::limit_lavel <= ws_core::ALERT && ws_log( ws_core::ALERT, __FILE__, __FUNCTION__, __LINE__ )
    #define emerg() ws_core::limit_lavel <= ws_core::EMERG && ws_log( ws_core::EMERG, __FILE__, __FUNCTION__, __LINE__ )

    enum LOG_LEVEL{ DEBUG = 0, INFO, NOTICE, WARN, ERR, CRIT, ALERT, EMERG };
    struct _WS_LOG
    {
    private:
        friend std::ostream & ws_log( LOG_LEVEL p_log_level, const char *, const char *, const int );
        static pthread_mutex_t m_mutex;
        static std::ostream * m_stream;
    };

    extern LOG_LEVEL limit_lavel;

    std::ostream & ws_log( LOG_LEVEL p_log_level, const char * p_file, const char * p_function, const int p_line );

}
#endif //__WS_LOG_H__