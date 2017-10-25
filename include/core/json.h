#ifndef __JSON_H__
#define __JSON_H__

#include <cstdlib>
#include <string>

namespace ws_core
{

    enum NODE_TYPE{ OBJECT, ARRAY, STRING, INT, FLOAT };
    struct node
    {
        char * m_key;
        void * m_value;
        NODE_TYPE m_node_type;
        node * m_parent;

        node * get_sub_node( const char * p_key );
        node * get_sub_node( const int p_index );
    
        std::string get_string_val( const char * p_key );
        std::string get_string_val( const int p_index );
    
        int get_int_val( const char * p_key );
        int get_int_val( const int p_index );
    
        float get_float_val( const char * p_key );
        float get_float_val( const int p_index );
    
        std::string get_string_val( void );
        int get_int_val( void );
        float get_float_val( void );

        node * parent(void);

        node * append( const char * p_key, const char * p_value );
        node * append( const char * p_value );

        node * append( const char * p_key, const int p_value );
        node * append( const int p_value );

        node * append( const char * p_key, const float p_value );
        node * append( const float p_value );

        node * set_val( const char * p_key, const char * p_value );
        node * set_val( const char * p_key, const int p_value );
        node * set_val( const char * p_key, const float p_value );

        node * set_val( const int p_index, const char * p_value );
        node * set_val( const int p_index, const int p_value );
        node * set_val( const int p_index, const float p_value );

        node * set_val( const char * p_value ); 
        node * set_val( const int p_value  );
        node * set_val( const float p_value );

        node * empty( void );

        std::string to_string( void );
    };

    int checks( const char * p_json );
    node * parse( const char * p_json );

    node * create_json_node( node * p_parent_node = NULL );
    void free_json_node( node ** p_node );

}
#endif //__JSON_H__