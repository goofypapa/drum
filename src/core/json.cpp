#include "json.h"

#include <cstdlib>
#include <iostream>
#include <cstring>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include "ws_log.h"

namespace ws_core
{

    struct flags
    {
        char m_is_single_comments;
        char m_is_multiline_comments;
        char m_is_key;


        char m_is_diagonal;
        char m_is_backslash;
        char m_is_star;
        char m_is_single_quotes;
        char m_is_double_quotes;
    };

    int checks( const char * p_json, node * p_parent );
    int _checks( const char * p_json, int & p_offset, node * p_parent  );
    int parse_val( const char * p_json, int p_offset, int & p_val_start, int & p_val_end, node * p_node );
    void free_node( node * p_node );
    void free_sub_node( node * p_node );

    std::map< std::string, node * > * get_map( node * p_node );
    std::vector< node * > * get_vector( node * p_node );

    void stringfiy( node * p_node, std::stringstream & p_output );

    node * append_node_to( node * p_parent, const char * p_key );
    node * append_node_to( node * p_parent );

    bool append_node_to( node * p_node, node * p_parent );

    bool divisional( node * p_parent, node * p_node );

    int checks( const char * p_json )
    {
        return checks( p_json, NULL );
    }

    node * parse( const char * p_json )
    {
        node * t_result = create_json_node();

        t_result->m_key = NULL;

        int err = checks( p_json, t_result );
        if( err )
        {
            err() << "format err by " << err << " " << p_json[err - 2] << p_json[err - 1] << p_json[err] << p_json[err + 1] << p_json[err + 2] << std::endl;
            free_json_node( &t_result );
        }

        return t_result;
    }

    node * node::get_sub_node( const char * p_key )
    {
        if( !this || !m_value || !p_key )
        {
            return NULL;
        }

        if( m_node_type != OBJECT )
        {
            err() << "node type is not object" << std::endl;
            return NULL;
        }

        std::map< std::string, node * > * t_map = get_map( this );
        if( !t_map || !t_map->size() )
        {
            return NULL;
        }

        std::map< std::string, node * >::iterator it = t_map->find( std::string( p_key ) );
        if( it == t_map->end() )
        {
            return NULL;
        }

        return it->second;
    }

    node * node::get_sub_node( const int p_index )
    {
        if( !this || !m_value || p_index < 0 )
        {
            return NULL;
        }

        if( m_node_type != ARRAY )
        {
            err() << "node type is not array" << std::endl;
            return NULL;
        }

        std::vector< node * > * t_vector = get_vector( this );
        if( t_vector->size() <= p_index )
        {
            return NULL;
        }

        return t_vector->operator[]( p_index );
    }

    std::string node::get_string_val( const char * p_key )
    {
        return get_sub_node( p_key )->get_string_val();
    }
    std::string node::get_string_val( const int p_index )
    {
        return get_sub_node( p_index )->get_string_val();
    }

    int node::get_int_val( const char * p_key )
    {
        return get_sub_node( p_key )->get_int_val();
    }
    int node::get_int_val( const int p_index )
    {
        return get_sub_node( p_index )->get_int_val();
    }

    float node::get_float_val( const char * p_key )
    {
        return get_sub_node( p_key )->get_float_val();
    }
    float node::get_float_val( const int p_index )
    {
        return get_sub_node( p_index )->get_float_val();
    }

    bool node::get_boolean_val( const char * p_key )
    {
        return get_sub_node( p_key )->get_boolean_val();
    }
    bool node::get_boolean_val( const int p_index )
    {
        return get_sub_node( p_index )->get_boolean_val();
    }

    std::string node::get_string_val( void )
    {  
        if( !this || !m_value )
        {
            return NULL;
        }

        if( m_node_type == STRING )
        {
            return std::string( (char *)m_value );
        }

        std::stringstream t_ss;

        if( m_node_type == INT )
        {
            int t_val = *(int *)m_value;
            t_ss << t_val;
            return t_ss.str();
        }

        if( m_node_type == FLOAT )
        {
            float t_val = *(float *)m_value;
            t_ss << t_val;
            return t_ss.str();
        }

        if( m_node_type == BOOLEAN )
        {
            return (char)*(char *)m_value == '\1' ? "true" : "false";
        }

        return NULL;
    }

    int node::get_int_val( void )
    {
        if( !this || !m_value )
        {
            return 0;
        }

        if( m_node_type == STRING )
        {
            return atoi( (char *)m_value );
        }

        if( m_node_type == INT )
        {
            return *(int *)m_value;
        }

        if( m_node_type == FLOAT )
        {
            return (int)*(float *)m_value;
        }

        if( m_node_type == BOOLEAN )
        {
            return (char)*(char *)m_value == '\1' ? 1 : 0;
        }

        err() << "can't convert to int" << std::endl;
        return 0;
    }

    float node::get_float_val( void )
    {
        if( !this || !m_value )
        {
            return 0.0f;
        }

        if( m_node_type == STRING )
        {
            return atof( (char *)m_value );
        }

        if( m_node_type == INT )
        {
            return (float)*(int *)m_value;
        }

        if( m_node_type == FLOAT )
        {
            return *(float *)m_value;
        }

        if( m_node_type == BOOLEAN )
        {
            return (char)*(char *)m_value == '\1' ? 1.0f : 0.0f;
        }

        err() << "can't convert to float" << std::endl;
        return 0.0f;
    }

    bool node::get_boolean_val( void )
    {
        if( !this || !m_value )
        {
            return false;
        }

        if( m_node_type == STRING )
        {
            return *(char *)m_value;
        }

        if( m_node_type == INT )
        {
            return (bool)*(int *)m_value;
        }

        if( m_node_type == FLOAT )
        {
            return (bool)*(float *)m_value;
        }

        if( m_node_type == BOOLEAN )
        {
            return (char)*(char *)m_value == '\1' ? true : false;
        }

        if( m_node_type == UNDEFINED )
        {
            return false;
        }

        err() << "can't convert to boolean" << std::endl;
        return false;
    }

    int node::get_length( void )
    {
        if( m_node_type != ARRAY )
        {
            return 0;
        }
        std::vector< node * > * t_vector = get_vector( this );
        if( !t_vector )
        {
            return 0;
        }
        return t_vector->size();
    }

    int node::get_index( node * p_node )
    {
        if( m_node_type != ARRAY || !p_node )
        {
            return -1;
        }

        std::vector< node * > * t_vector = get_vector( this );

        std::vector< node * >::iterator it = std::find( t_vector->begin(), t_vector->end(), p_node );
        if( it == t_vector->end() )
        {
            return -1;
        }

        return it - t_vector->begin();
    }

    NODE_TYPE node::get_node_type( void )
    {
        return m_node_type;
    }

    node * node::parent(void)
    {
        if( !this )
        {
            return NULL;
        }
        return m_parent;
    }

    node * node::append( const char * p_key, const char * p_value )
    {
        node * t_node = NULL;

        if( !p_key || !p_value )
        {
            err() << "key or value is null" << std::endl;
            return this;
        }
        
        t_node = append_node_to(this, p_key);
        if( !t_node )
        {
            return this;
        }

        t_node->set_val( p_value );

        return this;
    }
    node * node::append( const char * p_value )
    {
        node * t_node = NULL;
        
        t_node = append_node_to(this);
        if( !t_node )
        {
            return this;
        }

        t_node->set_val( p_value );

        return this;
    }

    node * node::append( const char * p_key, const int p_value )
    {
        node * t_node = NULL;

        if( !p_key )
        {
            err() << "key is null" << std::endl;
            return this;
        }
        
        t_node = append_node_to(this, p_key);
        if( !t_node )
        {
            return this;
        }

        t_node->set_val( p_value );

        return this;
    }
    node * node::append( const int p_value )
    {
        node * t_node = NULL;
        
        t_node = append_node_to(this);
        if( !t_node )
        {
            return this;
        }

        t_node->set_val( p_value );
        return this;
    }

    node * node::append( const char * p_key, const float p_value )
    {
        node * t_node = NULL;

        if( !p_key )
        {
            err() << "key is null" << std::endl;
            return this;
        }
        
        t_node = append_node_to(this, p_key);
        if( !t_node )
        {
            return this;
        }

        t_node->set_val( p_value );

        return this;
    }
    node * node::append( const float p_value )
    {
        node * t_node = NULL;
        
        t_node = append_node_to(this);
        if( !t_node )
        {
            return this;
        }

        t_node->set_val( p_value );

        return this;
    }

    node * node::append( const char * p_key, const bool p_value )
    {
        node * t_node = NULL;
        
        if( !p_key )
        {
            err() << "key is null" << std::endl;
            return this;
        }
        
        t_node = append_node_to(this, p_key);
        if( !t_node )
        {
            return this;
        }

        t_node->set_val( p_value );

        return this;
    }
    node * node::append( const bool p_value )
    {
        node * t_node = NULL;
        
        t_node = append_node_to(this);
        if( !t_node )
        {
            return this;
        }

        t_node->set_val( p_value );

        return this;
    }

    node * node::append( const char * p_key, NODE_TYPE p_node_type )
    {
        node * t_node = NULL;

        if( !p_key )
        {
            err() << "key is null" << std::endl;
            return this;
        }
        
        t_node = append_node_to(this, p_key);
        if( !t_node )
        {
            return this;
        }
        t_node->m_node_type = p_node_type;

        return this;
    }
    node * node::append( node * p_node )
    {
        if( !p_node )
        {
            return this;
        }
        append_node_to( p_node, this );
        return this;
    }

    node * node::remove( const char * p_key )
    {
        if( !p_key || m_node_type != OBJECT || !m_value  )
        {
            return this;
        }
        
        std::map< std::string, node * > * t_map = get_map( this );
        std::map< std::string, node * >::iterator it = t_map->find( std::string( p_key ) );

        if( it == t_map->end() )
        {
            return this;
        }
        
        node * t_rm_node = it->second;
        t_map->erase( it );
        free_node( t_rm_node );
        return this;
    }
    node * node::remove( const int p_index )
    {
        if( m_node_type != ARRAY || p_index < 0  )
        {
            return this;
        }

        std::vector< node * > * t_vector = get_vector( this );
        if( t_vector->size() <= p_index )
        {
            return this;
        }

        node * t_rm_node = t_vector->operator[]( p_index );
        t_vector->erase( t_vector->begin() + p_index );
        free_node( t_rm_node );
        return this;
    }

    bool divisional( node * p_parent, node * p_node )
    {
        if( !p_parent || !p_node )
        {
            return false;
        }

        if( !p_parent->m_value || ( p_parent->m_node_type != OBJECT && p_parent->m_node_type != ARRAY ) )
        {
            return false;
        }

        if( p_parent->m_node_type == OBJECT )
        {
            if( !p_node->m_key )
            {
                return false;
            }
            std::map< std::string, node * > * t_map = get_map( p_parent );
            std::map< std::string, node * >::iterator it = t_map->find( std::string( p_node->m_key ) );

            if( it == t_map->end() )
            {
                return true;
            }

            t_map->erase( it );
            p_node->m_parent = NULL;
            return true;
        }

        if( p_parent->m_node_type == ARRAY )
        {
            int t_index = p_parent->get_index( p_node );
            if( t_index < 0 )
            {
                return false;
            }

            std::vector< node * > * t_vector = get_vector( p_parent );
            std::vector< node * >::iterator it = std::find( t_vector->begin(), t_vector->end(), p_node );

            if( it == t_vector->end() )
            {
                return true;
            }
            
            t_vector->erase( it );
            p_node->m_parent = NULL;
            return true;
        }

        return false;;
    }

    node * node::set_key( const char * p_key )
    {

        bool t_flag = false;
        if( m_key )
        {
            if( m_parent && !m_parent->get_sub_node(p_key) )
            {
                divisional( m_parent, this );
                t_flag = true;
            }

            free( m_key );
        }

        int p_key_len = strlen(p_key);
        if( !p_key || !p_key_len )
        {
            m_key = NULL;
            return this;
        }
        m_key = (char *)malloc( p_key_len + 1 );
        memcpy( m_key, p_key, p_key_len + 1 );
        
        if( t_flag )
        {
            m_parent->append( this );
        }
        
        return this;
    }

    node * node::set_val( const char * p_key, const char * p_value )
    {
        if( !p_key || !p_value )
        {
            err() << "key or value is null" << std::endl;
            return this;
        }
        node * t_node = get_sub_node( p_key );
        if( t_node )
        {
            t_node->set_val( p_value );
        }

        return this;
    }
    node * node::set_val( const char * p_key, const int p_value )
    {
        if( !p_key )
        {
            err() << "key is null" << std::endl;
            return this;
        }
        node * t_node = get_sub_node( p_key );
        if( t_node )
        {
            t_node->set_val( p_value );
        }

        return this;
    }
    node * node::set_val( const char * p_key, const float p_value )
    {
        if( !p_key )
        {
            err() << "key is null" << std::endl;
            return this;
        }
        node * t_node = get_sub_node( p_key );
        if( t_node )
        {
            t_node->set_val( p_value );
        }

        return this;
    }

    node * node::set_val( const int p_index, const char * p_value )
    {
        node * t_node = get_sub_node( p_index );
        if( !t_node )
        {
            err() << "subscript out of range" << std::endl;
            return this;
        }
        t_node->set_val( p_value );

        return this;
    }
    node * node::set_val( const int p_index, const int p_value )
    {
        node * t_node = get_sub_node( p_index );
        if( !t_node )
        {
            err() << "subscript out of range" << std::endl;
            return this;
        }
        t_node->set_val( p_value );

        return this;
    }
    node * node::set_val( const int p_index, const float p_value )
    {
        node * t_node = get_sub_node( p_index );
        if( !t_node )
        {
            err() << "subscript out of range" << std::endl;
            return this;
        }
        t_node->set_val( p_value );

        return this;
    }

    node * append_node_to( node * p_node, const char * p_key )
    {
        node * result = NULL;
        std::map< std::string, node * > * t_map = NULL;
        int t_key_len = strlen( p_key );

        if( !p_node || !p_key || !t_key_len )
        {
            return NULL;
        }

        if( p_node->m_value && p_node->m_node_type != OBJECT )
        {
            err() << "node type is not object" << std::endl;
            free_sub_node(p_node);
        }

        t_map = get_map( p_node );

        std::map< std::string, node * >::iterator it = t_map->find( std::string(p_key) );
        if( it == t_map->end() )
        {
            result = create_json_node()->set_key( p_key );
            result->m_parent = p_node;
            t_map->operator[]( std::string( p_key ) ) = result;
        }

        return result;
    }

    node * append_node_to( node * p_parent )
    {
        node * result = NULL;
        std::vector< node * > * t_vector = NULL;

        if( !p_parent )
        {
            return NULL;
        }

        if( p_parent->m_value && p_parent->m_node_type != ARRAY )
        {
            err() << "node type is not array" << std::endl;
            return NULL;
        }

        
        t_vector = get_vector( p_parent );
        if( !t_vector )
        {
            return NULL;
        }
        result = create_json_node();
        result->m_parent = p_parent;
        t_vector->push_back( result );
        return result;
    }

    bool append_node_to( node * p_node, node * p_parent )
    {
        if( !p_node )
        {
            return false;
        }
        if( !p_parent )
        {
            return false;
        }

        if( p_node->m_parent )
        {
            divisional( p_node->m_parent, p_node );
        }

        if( ( p_parent->m_node_type == OBJECT || !p_parent->m_value ) && p_node->m_key )
        {
            std::map< std::string, node * > * t_map = get_map( p_parent );
            if( !t_map )
            {
                return false;
            }
            
            std::map< std::string, node * >::iterator it = t_map->find( std::string( p_node->m_key ) );
            if( it != t_map->end() )
            {
                divisional( p_parent, it->second );
                free_node( it->second );
            }

            t_map->operator[]( std::string( p_node->m_key ) ) = p_node;
            p_node->m_parent = p_parent;
            return true;
        }

        if( p_parent->m_node_type == ARRAY || !p_parent->m_value )
        {
            std::vector< node * > * t_vector = get_vector( p_parent );
            if( !t_vector )
            {
                return false;
            }
            t_vector->push_back( p_node );
            p_node->m_parent = p_parent;
            return true;
        }
        err() << "append faild" << std::endl;
        return false;  
    }

    node * node::set_val( const char * p_value )
    {
        free_sub_node(this);
        m_node_type = STRING;
        int t_str_len = strlen( p_value );
        char * t_val = (char *)malloc( t_str_len + 1 );
        memcpy( t_val, p_value, t_str_len + 1 );
        m_value = (void *)t_val;
        return this;
    }

    node * node::set_val( const int p_value  )
    {
        free_sub_node(this);
        m_node_type = INT;
        int * t_val = (int *)malloc( sizeof(int) );
        *t_val = p_value;
        m_value = (void *)t_val;
        return this;
    }

    node * node::set_val( const float p_value )
    {
        free_sub_node(this);
        m_node_type = FLOAT;
        float * t_val = (float *)malloc( sizeof(float) );
        *t_val = p_value;
        m_value = (void *)t_val;
        return this;
    }

    node * node::set_val( const bool p_value )
    {
        free_sub_node(this);
        m_node_type = BOOLEAN;
        char * t_val = (char *)malloc( sizeof(char) );
        *t_val = p_value ? '\1' : '\0';
        m_value = (void *)t_val;
        return this;
    }

    node * node::empty( void )
    {
        free_sub_node(this);
        return this;
    }
    
    std::string node::to_string( void )
    {
        std::stringstream ssresult;
        stringfiy( this, ssresult );
        return ssresult.str();
    }

    node * create_json_node( NODE_TYPE p_node_type )
    {
        node * result = (node *)malloc( sizeof(node) );
        result->m_key = NULL;
        result->m_value = NULL;
        result->m_node_type = p_node_type;
        result->m_parent = NULL;
        return result;
    }

    void free_json_node( node ** p_node )
    {
        free_node( *p_node );
        *p_node = NULL;
    }

    int checks( const char * p_json, node * p_parent )
    {
        int t_offset = 0;
        
        return _checks( p_json, t_offset, p_parent );
    }

    int _checks( const char * p_json, int & p_offset, node * p_parent  )
    {
        node * t_node = NULL;

        int t_json_len = strlen(p_json);

        flags t_flags;
        t_flags.m_is_key = 0;
        t_flags.m_is_single_comments = 0;
        t_flags.m_is_multiline_comments = 0;
        t_flags.m_is_diagonal = 0;
        t_flags.m_is_backslash = 0;
        t_flags.m_is_single_quotes = 0;
        t_flags.m_is_double_quotes = 0;
        t_flags.m_is_star = 0;

        char t_is_brace = 0;
        char t_is_bracket = 0;
        char t_is_comma = 0;
        int t_key_start = -1;
        int t_key_end = -1;
        int t_val_start = -1;
        int t_val_end = -1;


        for( ; p_offset < t_json_len; p_offset++ )
        {
            char data = p_json[p_offset];
            //如果在多行注释内
            if( t_flags.m_is_multiline_comments )
            {
                switch( data )
                {
                    case '*':
                        t_flags.m_is_star = 1;
                    break;
                    case '/':
                        if( t_flags.m_is_star )
                        {
                            t_flags.m_is_multiline_comments = 0;
                        }
                    break;
                    default:
                        t_flags.m_is_star = 0;
                    break;
                }
                continue;
            }

            //如果在单行注释内
            if( t_flags.m_is_single_comments )
            {
                if( data == '\n' )
                {
                    t_flags.m_is_single_comments = 0; 
                }
                continue;
            }

            //如果上一个字符是反斜线
            if( t_flags.m_is_backslash )
            {
                t_flags.m_is_backslash = 0;
                continue;
            }

            //如果上一个字符是斜线
            if( t_flags.m_is_diagonal )
            {
                switch( data )
                {
                    case '/':
                        t_flags.m_is_single_comments = 1;
                    break;
                    case '*':
                        t_flags.m_is_multiline_comments = 1;
                    break;
                }
                t_flags.m_is_diagonal = 0;
            }

            //如果在单引号里面
            if( t_flags.m_is_single_quotes )
            {
                switch( data )
                {
                    case '\'':
                        t_flags.m_is_single_quotes = 0;
                        if( t_key_start )
                        {
                            t_key_end = p_offset - 1;
                        }

                        if( t_val_start >= 0 )
                        {
                            t_val_end = p_offset - 1;
                        }
                    break;
                    case '\\':
                        t_flags.m_is_backslash = 1;
                    break;
                }
                continue;
            }

            //如果在双引号里面
            if( t_flags.m_is_double_quotes )
            {
                switch( data )
                {
                    case '"':
                        t_flags.m_is_double_quotes = 0;
                        if( t_key_start >= 0 )
                        {
                            t_key_end = p_offset - 1;
                        }

                        if( t_val_start >= 0 )
                        {
                            t_val_end = p_offset - 1;
                        }
                    break;
                    case '\\':
                        t_flags.m_is_backslash = 1;
                    break;
                }
                continue;
            }

            //如果name结束后　不允许输入其他字符
            if( t_key_end >= 0 )
            {
                switch( data )
                {
                    case ' ':
                    case '\n':
                    case ':':
                    break;
    
                    default:
                    return p_offset;
                }
            }

            //如果在大括号里面
            if( t_is_brace )
            {
                switch( data )
                {
                    case ':':
                        if( t_flags.m_is_key && t_key_start >= 0 )
                        {
                            if( t_key_end < 0 )
                            {
                                for( t_key_end = p_offset - 1; t_key_end >= t_key_start; --t_key_end )
                                {
                                    if( p_json[t_key_end] == ' ' || p_json[t_key_end] == '\n' )
                                    {
                                        continue;
                                    }
                                    break;
                                }

                                if( t_key_end < t_key_start )
                                {
                                    return t_key_start;
                                }
                            }
                            
                            if( p_parent && !t_node )
                            {
                                //创建节点
                                t_node = create_json_node();
                            
                                //赋值name
                                int t_key_len = t_key_end - t_key_start + 1;
                                t_node->m_key = (char *)malloc( t_key_len + 1 );
                                memcpy( t_node->m_key, p_json + t_key_start, t_key_len );
                                t_node->m_key[t_key_len] = '\0';

                                //添加到容器
                                p_parent->append( t_node );
                            }

                            t_key_start = -1;
                            t_flags.m_is_key = 0;

                            t_key_end = -1;
                            continue;
                        }
                    return p_offset;
                    case ',':
                        if( !t_flags.m_is_key && t_val_start >= 0 || t_val_start < -1 )
                        {                            
                            if( t_val_start >= 0 && parse_val( p_json, p_offset, t_val_start, t_val_end, t_node ) )
                            {
                                return p_offset;
                            }

                            t_node = NULL;
                            t_val_start = -1;
                            t_flags.m_is_key = 1;
                            continue;
                        }

                    return p_offset;
                    case '{':
                    case '[':

                        if( t_flags.m_is_key )
                        {
                            return p_offset;
                        }

                        if( _checks( p_json, p_offset, t_node ) )
                        {
                            return p_offset;
                        }

                        t_node = NULL;
                        t_val_start = -2;
                    continue;
                    case '}':
                        if( t_val_start >= 0 && !t_flags.m_is_key && parse_val( p_json, p_offset, t_val_start, t_val_end, t_node ) )
                        {
                            return p_offset;
                        }
                        t_node = NULL;
                        t_flags.m_is_key = 0;
                    return 0;
                }
            }

            //如果在中括号里面
            if( t_is_bracket )
            {
                switch( data )
                {
                    case '{':
                        t_flags.m_is_key = 1;
                    case '[':
                        if( !t_node )
                        {
                            t_node = create_json_node();
                            if( data == '{' )
                            {
                                t_node->m_node_type = OBJECT;
                            }
                            if( data == '[' )
                            {
                                t_node->m_node_type = ARRAY;
                            }

                            p_parent->append( t_node );
                        }

                        if( _checks( p_json, p_offset, t_node ) )
                        {
                            return p_offset;
                        }

                        t_val_start = -2;
                        t_node = NULL;
                    continue;
                    case ']':
                        if( !t_flags.m_is_key && t_val_start >= 0 )
                        {    
                            if(  p_parent && !t_node )
                            {
                                t_node = create_json_node();

                                p_parent->append( t_node );
                            }                  
                            if( parse_val( p_json, p_offset, t_val_start, t_val_end, t_node ) )
                            {
                                return p_offset;
                            }
                        }
                        t_val_start = -1;
                        t_node = NULL;
                        t_flags.m_is_key = 0;
                    return 0;
                    case ',':
                        //next value
                        if( !t_flags.m_is_key && t_val_start >= 0 )
                        {
                            if( p_parent && !t_node )
                            {
                                t_node = create_json_node();
                                p_parent->append( t_node );
                            }
                            
                            if( t_val_start >= 0 && parse_val( p_json, p_offset, t_val_start, t_val_end, t_node ) )
                            {
                                return p_offset;
                            }
                            t_val_start = -1;
                            t_node = NULL;
                            continue;
                        }

                        if( t_val_start < -1 )
                        {
                            t_val_start = -1;
                            continue;
                        }
                    break;
                }
            }

            //name校验
            if( t_flags.m_is_key && t_key_start >= 0 )
            {
                if( (data >= 'a' && data <= 'z') || (data >= 'A' && data <= 'Z') || (data >= '0' && data <= '9') )
                {
                    continue;
                }

                switch( data )
                {
                    case '_':
                    continue;
                    case ' ':
                    case '\n':
                        if( t_key_end < 0 )
                        {
                            t_key_end = p_offset -1;
                        }
                    continue;
                }
                return p_offset;
            }

            //data校验
            if( !t_flags.m_is_key && t_val_start >= 0 )
            {

                if( (data >= '0' && data <= '9') || (data >= 'a' && data <= 'z') )
                {
                    continue;
                }
                switch( data )
                {
                    case '.':
                    case 'e':
                    case 'E':
                    case '-':
                    continue;

                    case ' ':
                    case '\n':
                        if( !t_val_end )
                        {
                            t_val_end = p_offset -1;
                        }  
                    continue;

                    case '{':
                    case '[':
                    break;
                }
                return p_offset;
            }


            if( data == '\'' || data == '"' )
            {
                if( t_key_start >= 0 )
                {
                    return p_offset;
                }
                if( !t_flags.m_is_key )
                {
                    if( !t_node )
                    {
                        t_node = create_json_node();
                    }
                    t_node->m_node_type = STRING;  
                }
            }

            switch( data )
            {
                case '\\':
                    t_flags.m_is_backslash = 1;
                break;
                case '/':
                    t_flags.m_is_diagonal = 1;
                break;
                case '\'':
                    t_flags.m_is_single_quotes = 1;
                break;
                case '"':
                    t_flags.m_is_double_quotes = 1;
                break;
                case '{':
                    t_is_brace = 1;
                    t_flags.m_is_key = 1;
                    if( p_parent )
                    {
                        p_parent->m_node_type = OBJECT;
                        get_map( p_parent );
                    }
                break;
                case '[':
                    t_is_bracket = 1;
                    t_flags.m_is_key = 0;
                    if( p_parent )
                    {
                        p_parent->m_node_type = ARRAY;
                        get_vector( p_parent );
                    }
                break;

                case ' ':
                case '\n':
                break;

                default:
                    if( t_flags.m_is_key && t_key_start < 0 )
                    {
                        //name首字母校验
                        if( (data < 'a' || data > 'z') && (data < 'A' || data > 'Z') && data != '_' )
                        {
                            return p_offset;
                        }
                        t_key_start = p_offset;
                    }else if( !t_flags.m_is_key && t_val_start < 0 )
                    {
                        //value首字母校验
                        if( (data < '0' || data > '9') && ( data < 'a' || data > 'z' )  && data != '-' )
                        {
                            return p_offset;
                        }
                        t_val_start = p_offset;

                    }else
                    {
                        return p_offset;
                    }

                break;
            }

            if( t_flags.m_is_key && t_key_start < 0 && ( t_flags.m_is_single_quotes || t_flags.m_is_double_quotes ) )
            {
                t_key_start = p_offset + 1;
            }

            if( !t_flags.m_is_key && t_val_start < 0 && ( t_flags.m_is_single_quotes || t_flags.m_is_double_quotes ) )
            {
                t_val_start = p_offset + 1;
            }
        }
        
    }

    void free_node( node * p_node )
    {
        if( !p_node )
        {
            return;
        }

        if( p_node->m_key )
        {
            free( (void *)p_node->m_key );
        }

        free_sub_node( p_node );

        free( p_node );
    }

    void free_sub_node( node * p_node )
    {
        if( !p_node || !p_node->m_value  )
        {
            return;
        }

        if( p_node->m_node_type == OBJECT )
        {
            std::map< std::string, node * > * t_map = get_map(p_node);

            for( std::map< std::string, node * >::iterator it = t_map->begin(); it != t_map->end(); ++it )
            {
                if( it->second )
                {
                    free_node( it->second );
                }
            }
            delete t_map;
        }else if( p_node->m_node_type == ARRAY )
        {
            std::vector< node * > * t_vector = get_vector( p_node );
            for( std::vector< node * >::iterator it = t_vector->begin(); it != t_vector->end(); ++it )
            {
                if( *it )
                {
                    free_node( *it );
                }
            }
            delete t_vector;
        }else
        {
            free( p_node->m_value );
        }
        p_node->m_value = NULL;
    }

    std::map< std::string, node * > * get_map( node * p_node )
    {
        std::map< std::string, node * > * result = NULL;
        if( !p_node )
        {
            return result;
        }

        if( !p_node->m_value )
        {
            result = new std::map< std::string, node * >();
            p_node->m_node_type = OBJECT;
            p_node->m_value = (void *)result;
        }else if( p_node->m_node_type != OBJECT )
        {
            return result;
        }
        else
        {
            result = (std::map< std::string, node * > *)p_node->m_value;
        }

        return result;
    }
    std::vector< node * > * get_vector( node * p_node )
    {
        std::vector< node * > * result = NULL;
        if( !p_node )
        {
            return result;
        }

        if( !p_node->m_value )
        {
            result = new std::vector< node * >();
            p_node->m_node_type = ARRAY;
            p_node->m_value = (void *)result;
        }else if( p_node->m_node_type != ARRAY )
        {
            return result;
        }
        else
        {
            result = (std::vector< node * > *)p_node->m_value;
        }

        return result;
    }

    void stringfiy( node * p_node, std::stringstream & p_output )
    {
        if( !p_node )
        {
            return;
        }

        if( p_node->m_key )
        {
            p_output << "\"" << p_node->m_key << "\"" << ":";
        }

        if( p_node->m_node_type == OBJECT )
        {
            p_output << "{";
            if( p_node->m_value )
            {
                std::map< std::string, node * > * t_map = get_map( p_node );
                for( std::map< std::string, node * >::iterator it = t_map->begin(); it != t_map->end(); ++it )
                {

                    if( it != t_map->begin() )
                    {
                        p_output << ",";
                    }

                    if( it->second )
                    {
                        stringfiy( it->second, p_output );
                    }
                }
            }
            p_output << "}";
        }else if( p_node->m_node_type == ARRAY )
        {
            p_output << "[";
            
            if( p_node->m_value )
            {
                std::vector< node * > * t_vector = get_vector( p_node );
                for( std::vector< node * >::iterator it = t_vector->begin(); it != t_vector->end(); ++it )
                {
                    if( it != t_vector->begin() )
                    {
                        p_output << ",";
                    }

                    if( *it )
                    {
                        stringfiy( *it, p_output );
                    }
                }
            }
            p_output << "]";
        }else if( p_node->m_node_type == STRING )
        {
            p_output << "\"";
            if( p_node->m_value )
            {
                p_output << ( char * )p_node->m_value;
            }
            p_output << "\"";
        }else if( p_node->m_value && p_node->m_node_type == INT )
        {
            p_output << *( int * )p_node->m_value;
        }else if( p_node->m_value && p_node->m_node_type == FLOAT )
        {
            p_output << *( float * )p_node->m_value;
        }else if( p_node->m_value && p_node->m_node_type == BOOLEAN )
        {
            p_output << ( *(char *)p_node->m_value == '\1' ? "true" : "false" );
        }else if( p_node->m_node_type == UNDEFINED )
        {
            p_output << "undefined";
        }
    }

    int parse_val( const char * p_json, int p_offset, int & p_val_start, int & p_val_end, node * p_node )
    {
        if( p_val_end < 0 )
        {
            for( p_val_end = p_offset; p_val_end >= p_val_start; --p_val_end )
            {
                if( p_json[p_val_end] == ' ' || p_json[p_val_end] == '\n' || p_json[p_val_end] == ',' || p_json[p_val_end] == '}' || p_json[p_val_end] == ']' )
                {
                    continue;
                }
                break;
            }
        }

        if( p_val_end < p_val_start && p_node->m_node_type != STRING || p_node->m_node_type == STRING && p_val_end - p_val_start < -1 )
        {
            return p_val_start;
        }

        int t_value_len = p_val_end - p_val_start + 1;
        char * t_str_val = (char *)malloc( t_value_len + 1 );


        memcpy( t_str_val, p_json + p_val_start, t_value_len );
        t_str_val[t_value_len] = '\0';

        if( p_node )
        {

            if( p_node->m_value )
            {
                free_sub_node(p_node);
            }

            if( p_node->m_node_type == STRING )
            {
                p_node->m_value = (void *)t_str_val;
            }else if( !strcmp( t_str_val, "true"  ) || !strcmp( t_str_val, "false"  ) )
            {
                p_node->m_node_type = BOOLEAN;
                char * t_val = (char *)malloc( sizeof( char ) );
                *t_val = !strcmp( t_str_val, "true"  ) ? '\1' : '\0';
                p_node->m_value = (void *)t_val;
            }else
            {
                int t_dot_index = -1, t_e_index = -1;
                for( int i = 0; i < t_value_len; ++i )
                {
                    if( t_str_val[i] >= '0' && t_str_val[i] <= '9' )
                    {
                        continue;
                    }
    
                    if( t_e_index && i == t_e_index + 1 && ( t_str_val[i] == '-' || t_str_val[i] == '+' ) )
                    {
                        continue;
                    }
    
                    switch( t_str_val[i] )
                    {
                        case '-':
                        case '+':
                            if( i )
                            {
                                return p_val_start + i;
                            }
                        break;
                        case '.':
                            if( t_dot_index >= 0 )
                            {
                                return p_val_start + i;
                            }
                            t_dot_index = i;
                        break;
                        case 'e':
                        case 'E':
                            if( t_e_index >= 0 )
                            {
                                return p_val_start + i;
                            }
                            t_e_index = i;
                        break;
                        default:
                        return p_val_start + i;
                    }
                }
    
                if( t_dot_index >= 0 )
                {
                    p_node->m_node_type = FLOAT;
                    float * t_val = (float *)malloc( sizeof( float ) );
                    *t_val = atof( t_str_val );
                    p_node->m_value = ( void * )t_val;
                }else
                {
                    p_node->m_node_type = INT;
                    int * t_val = (int *)malloc( sizeof( int ) );
                    *t_val = atoi( t_str_val );
                    p_node->m_value = ( void * )t_val;
                }
            }
        }
        
        if( !p_node || p_node->m_node_type != STRING )
        {
            free( t_str_val );
        }
        
        p_val_start = -1;
        p_val_end = -1;

        return 0;
    }
}