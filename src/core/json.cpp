#include "json.h"

#include <cstdlib>
#include <iostream>
#include <cstring>
#include <vector>
#include <map>
#include <sstream>

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

    void stringfiy( node * p_node, std::stringstream & p_output );

    node * append_node( node * p_node, const char * p_key );
    node * append_node( node * p_node );

    int checks( const char * p_json )
    {
        return checks( p_json, NULL );
    }

    node * parse( const char * p_json )
    {
        node * t_result = (node *)malloc(sizeof(node));

        std::cout << "new: " << (void *)t_result << std::endl;

        t_result->m_key = NULL;

        int err = checks( p_json, t_result );
        if( err )
        {
            std::cout << "pase json error for: " << err << " [" << p_json[err - 1] << p_json[err] << p_json[err + 1] << "]" << std::endl;
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
            std::cout << "get_sub_node( const char * ): m_node_type is not OBJECT" << std::endl;
            return NULL;
        }

        std::map< std::string, node * > * t_map = (std::map< std::string, node *> *)m_value;
        if( !t_map->size() )
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
            std::cout << "get_sub_node( const char * ): m_node_type is not ARRAY" << std::endl;
            return NULL;
        }

        std::vector< node * > * t_vector = (std::vector< node * > *)m_value;
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

        std::cout << "get_int_val error: Unknown node_type" << std::endl;
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

        std::cout << "get_float_val error: Unknown node_type" << std::endl;
        return 0.0f;
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
            std::cout << "node::append( const char *, const char * ) error: parameter is null" << std::endl;
            return this;
        }
        
        t_node = append_node(this, p_key);
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
        
        t_node = append_node(this);
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
            std::cout << "node::append( const char *, const int ) error: p_key is null" << std::endl;
            return this;
        }
        
        t_node = append_node(this, p_key);
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
        
        t_node = append_node(this);
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
            std::cout << "node::append( const char *, const float ) error: p_key is null" << std::endl;
            return this;
        }
        
        t_node = append_node(this, p_key);
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
        
        t_node = append_node(this);
        if( !t_node )
        {
            return this;
        }

        t_node->set_val( p_value );

        return this;
    }

    node * node::set_val( const char * p_key, const char * p_value )
    {
        if( !p_key || !p_value )
        {
            std::cout << "set_val( const char *, const char * ) error: parameter is null" << std::endl;
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
            std::cout << "set_val( const char *, const int ) error: p_key is null" << std::endl;
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
            std::cout << "set_val( const char *, const int ) error: p_key is null" << std::endl;
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
            std::cout << "set_val( const int, const char *) error: subscript out of range" << std::endl;
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
            std::cout << "set_val( const int, const int ) error: subscript out of range" << std::endl;
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
            std::cout << "set_val( const int, const float ) error: subscript out of range" << std::endl;
            return this;
        }
        t_node->set_val( p_value );

        return this;
    }

    node * append_node( node * p_node, const char * p_key )
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
            std::cout << "append_node( node *, const char * ) warning: node_type is not object" << std::endl;
            free_sub_node(p_node);
        }
        p_node->m_node_type = OBJECT;

        if( !p_node->m_value )
        {
            t_map = new std::map< std::string, node * >();
            p_node->m_value = (void *)t_map;
        }else
        {
            t_map = (std::map< std::string, node * > *)p_node->m_value;
        }

        std::map< std::string, node * >::iterator it = t_map->find( std::string(p_key) );
        if( it == t_map->end() )
        {
            result = create_json_node( p_node );
            
            result->m_key = (char *)malloc( t_key_len + 1 );
            memcpy( result->m_key, p_key, t_key_len + 1 );
            t_map->operator[]( std::string( p_key ) ) = result;
        }else
        {
            result = ( node* )it->second;
            free_sub_node( result );
        }

        return result;
    }

    node * append_node( node * p_node )
    {
        node * result = NULL;
        std::vector< node * > * t_vector = NULL;

        if( !p_node )
        {
            return NULL;
        }

        if( p_node->m_node_type != ARRAY )
        {
            std::cout << "append_node( node * ) warning: node_type is not array" << std::endl;
            free_sub_node(p_node);
        }

        p_node->m_node_type = ARRAY;

        if( !p_node->m_value )
        {
            t_vector = new std::vector< node * >();
            p_node->m_value = (void *)t_vector;
        }else
        {
            t_vector = (std::vector< node * > *)p_node->m_value;
        }

        result = create_json_node( p_node );
        t_vector->push_back( result );

        return result;
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
        m_node_type = INT;
        float * t_val = (float *)malloc( sizeof(float) );
        *t_val = p_value;
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

    node * create_json_node( node * p_parent_node )
    {
        node * result = (node *)malloc( sizeof(node) );
        std::cout << "new: " << (void *)result << std::endl;
        if( p_parent_node )
        {
            result->m_parent = p_parent_node;
        }
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

        std::cout << "checks in :" << p_offset << std::endl;
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
        int t_key_start = 0;
        int t_key_end = 0;
        int t_val_start = 0;
        int t_val_end = 0;


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

                        if( t_val_start )
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
                        if( t_key_start )
                        {
                            t_key_end = p_offset - 1;
                        }

                        if( t_val_start )
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
            if( t_key_end )
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
                        if( t_flags.m_is_key && t_key_start )
                        {
                            if( !t_key_end )
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
                                t_node = create_json_node( p_parent );
                            
                                //赋值name
                                int t_key_len = t_key_end - t_key_start + 1;
                                t_node->m_key = (char *)malloc( t_key_len + 1 );

                                std::cout << "new: " << (void *)t_node->m_key << std::endl;

                                memcpy( t_node->m_key, p_json + t_key_start, t_key_len );
                                t_node->m_key[t_key_len] = '\0';

                                //添加到容器
                                if( p_parent->m_node_type == OBJECT )
                                {
                                    std::map< std::string, node * > * t_map = (std::map< std::string, node * > *)p_parent->m_value;
                                    t_map->operator[]( t_node->m_key ) = t_node;
                                }else if( p_parent->m_node_type == ARRAY )
                                {
                                    std::vector< node * > * t_vector = (std::vector< node * > *)p_parent->m_value;
                                    t_vector->push_back( t_node );
                                }else
                                {
                                    p_parent->m_value = (void *)t_node;
                                }
                            }

                            t_key_start = 0;
                            t_flags.m_is_key = 0;

                            t_key_end = 0;
                            continue;
                        }
                    return p_offset;
                    case ',':

                        if( !t_flags.m_is_key && t_val_start )
                        {                            
                            if( parse_val( p_json, p_offset, t_val_start, t_val_end, t_node ) )
                            {
                                return p_offset;
                            }

                            t_node = NULL;
                            t_flags.m_is_key = 1;
                            continue;
                        }
                        if( !t_flags.m_is_key && t_val_start )
                        {
                            t_val_start = 0;
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
                    continue;
                    case '}':
                        if( t_val_start && !t_flags.m_is_key && parse_val( p_json, p_offset, t_val_start, t_val_end, t_node ) )
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
                        if( _checks( p_json, p_offset, t_node ) )
                        {
                            return p_offset;
                        }
                    continue;
                    case ']':
                        if( !t_flags.m_is_key && t_val_start )
                        {    
                            if(  p_parent && !t_node )
                            {
                                t_node = create_json_node( p_parent );
                            }                  
                            if( parse_val( p_json, p_offset, t_val_start, t_val_end, t_node ) )
                            {
                                return p_offset;
                            }
                        }
                        t_node = NULL;
                        t_flags.m_is_key = 0;
                    return 0;
                    case ',':
                        //next value
                        if( !t_flags.m_is_key && t_val_start )
                        {
                            if( p_parent && !t_node )
                            {
                                t_node = create_json_node( p_parent );
                            }
                            
                            if( parse_val( p_json, p_offset, t_val_start, t_val_end, t_node ) )
                            {
                                return p_offset;
                            }
                            t_node = NULL;
                            continue;
                        }
                    break;
                }
            }

            //name校验
            if( t_flags.m_is_key && t_key_start )
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
                        if( !t_key_end )
                        {
                            t_key_end = p_offset -1;
                        }
                    continue;
                }
                return p_offset;
            }

            //data校验
            if( !t_flags.m_is_key && t_val_start )
            {

                if( (data >= '0' && data <= '9') )
                {
                    continue;
                }
                switch( data )
                {
                    case '.':
                    case 'e':
                    case 'E':
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
                if( t_key_start )
                {
                    return p_offset;
                }
                if( !t_flags.m_is_key )
                {
                    if( !t_node )
                    {
                        t_node = create_json_node( p_parent );
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
                        p_parent->m_value = (void *)new std::map< std::string, node * >();
                        std::cout << "new: " << (void *)p_parent->m_value << std::endl;
                    }
                break;
                case '[':
                    t_is_bracket = 1;
                    t_flags.m_is_key = 0;
                    if( p_parent )
                    {
                        p_parent->m_node_type = ARRAY;
                        p_parent->m_value = (void *)new std::vector< node * >();
                        std::cout << "new: " << (void *)p_parent->m_value << std::endl;
                    }
                break;

                case ' ':
                case '\n':
                break;

                default:
                    if( t_flags.m_is_key && !t_key_start )
                    {
                        //name首字母校验
                        if( (data < 'a' || data > 'z') && (data < 'A' || data > 'Z') && data != '_' )
                        {
                            return p_offset;
                        }
                        t_key_start = p_offset;
                    }else if( !t_flags.m_is_key && !t_val_start )
                    {
                        //value首字母校验
                        if( (data < '0' || data > '9') && data != '-' )
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

            if( t_flags.m_is_key && !t_key_start && ( t_flags.m_is_single_quotes || t_flags.m_is_double_quotes ) )
            {
                t_key_start = p_offset + 1;
            }

            if( !t_flags.m_is_key && !t_val_start && ( t_flags.m_is_single_quotes || t_flags.m_is_double_quotes ) )
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
            std::cout << "delete: " << (void *)p_node->m_key << std::endl;
            free( p_node->m_key );
        }

        free_sub_node( p_node );

        std::cout << "delete: " << (void *)p_node << std::endl;
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
            std::map< std::string, node * > * t_map = (std::map< std::string, node * > *)p_node->m_value;

            for( std::map< std::string, node * >::iterator it = t_map->begin(); it != t_map->end(); ++it )
            {
                if( it->second )
                {
                    free_node( it->second );
                }
            }
            std::cout << "delete: " << (void *)t_map << std::endl;
            delete t_map;
        }else if( p_node->m_node_type == ARRAY )
        {
            std::vector< node * > * t_map = (std::vector< node * > *)p_node->m_value;
            for( std::vector< node * >::iterator it = t_map->begin(); it != t_map->end(); ++it )
            {
                if( *it )
                {
                    free_node( *it );
                }
            }
            std::cout << "delete: " << (void *)t_map << std::endl;
            delete t_map;
        }else
        {
            std::cout << "delete: " << (void *)p_node->m_value << std::endl;
            free( p_node->m_value );
        }
        p_node->m_value = NULL;
    }

    void stringfiy( node * p_node, std::stringstream & p_output )
    {
        if( !p_node )
        {
            return;
        }

        if( p_node->m_key )
        {
            p_output << p_node->m_key << ":";
        }

        if( p_node->m_node_type == OBJECT )
        {
            p_output << "{";
            std::map< std::string, node * > * t_map = (std::map< std::string, node * > *)p_node->m_value;
            
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
            p_output << "}";
        }else if( p_node->m_node_type == ARRAY )
        {
            p_output << "[";
            std::vector< node * > * t_map = (std::vector< node * > *)p_node->m_value;
            for( std::vector< node * >::iterator it = t_map->begin(); it != t_map->end(); ++it )
            {
                if( it != t_map->begin() )
                {
                    p_output << ",";
                }

                if( *it )
                {
                    stringfiy( *it, p_output );
                }
                
            }
            p_output << "]";
        }else if( p_node->m_value && p_node->m_node_type == STRING )
        {
            p_output << "\"" << ( char * )p_node->m_value << "\"";
        }else if( p_node->m_value && p_node->m_node_type == INT )
        {
            p_output << *( int * )p_node->m_value;
        }else if( p_node->m_value && p_node->m_node_type == FLOAT )
        {
            p_output << *( float * )p_node->m_value << "f";
        }
    }

    int parse_val( const char * p_json, int p_offset, int & p_val_start, int & p_val_end, node * p_node )
    {
        if( !p_val_end )
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

        if( p_val_end < p_val_start )
        {
            return p_val_start;
        }

        int t_value_len = p_val_end - p_val_start + 1;
        char * t_str_val = (char *)malloc( t_value_len + 1 );

        std::cout << "new: " << (void *)t_str_val << std::endl;

        memcpy( t_str_val, p_json + p_val_start, t_value_len );
        t_str_val[t_value_len] = '\0';

        if( !p_node )
        {
            free( t_str_val );
        }

        if( p_node->m_node_type == STRING )
        {
            p_node->m_value = (void *)t_str_val;
        }else
        {
            int t_dot_index = -1, t_e_index = -1;
            for( int i = 0; i < t_value_len; ++i )
            {
                if( t_str_val[i] >= '0' && t_str_val[i] <= '9' )
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
        
        p_val_start = 0;
        p_val_end = 0;

        return 0;
    }
}