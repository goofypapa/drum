#include "ws_log.h"
#include <sstream>

namespace ws_core
{

    pthread_mutex_t _WS_LOG::m_mutex;
    std::ostream * _WS_LOG::m_stream = &std::cout;
    LOG_LEVEL limit_lavel = DEBUG;

    std::ostream & ws_log( LOG_LEVEL p_log_level, const char * p_file, const char * p_function, const int p_line )
    {
        std::stringstream t_sstr;

        t_sstr << "[";

        switch( p_log_level )
        {
            case DEBUG:
                t_sstr << "Debug";
            break;
            case INFO:
                t_sstr << "Info";
            break;
            case NOTICE:
                t_sstr << "Notice";
            break;
            case WARN:
                t_sstr << "Warning";
            break;
            case ERR:
                t_sstr << "Error";
            break;
            case CRIT:
                t_sstr << "Critical";
            break;
            case ALERT:
                t_sstr << "Alert";
            break;
            case EMERG:
                t_sstr << "Emergency";
            break;
        }

        t_sstr << "]";

        if( p_log_level != INFO )
        t_sstr << " " << p_line << " " << p_file << " " << p_function << " -" << std::endl;

        pthread_mutex_lock( &_WS_LOG::m_mutex );
        *_WS_LOG::m_stream << std::endl << t_sstr.str();
        pthread_mutex_unlock( &_WS_LOG::m_mutex );

        return *_WS_LOG::m_stream;
    }
}