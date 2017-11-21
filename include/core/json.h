#ifndef __JSON_H__
#define __JSON_H__

#include <cstdlib>
#include <string>
#include <map>
#include <vector>

namespace ws_core
{

    enum NODE_TYPE{ OBJECT, ARRAY, STRING, INT, FLOAT, BOOLEAN, UNDEFINED };
    struct node
    {
    private:
        char * m_key;
        void * m_value;
        NODE_TYPE m_node_type;
        node * m_parent;

        friend int _checks( const char * p_json, int & p_offset, node * p_parent  );
        friend int parse_val( const char * p_json, int p_offset, int & p_val_start, int & p_val_end, node * p_node );
        friend void free_node( node * p_node );
        friend void free_sub_node( node * p_node );

        friend void stringfiy( node * p_node, std::stringstream & p_output );

        friend node * append_node_to( node * p_parent, const char * p_key );
        friend node * append_node_to( node * p_parent );

        friend bool append_node_to( node * p_node, node * p_parent );

        friend bool divisional( node * p_parent, node * p_node );

        friend int checks( const char * p_json );
        friend node * parse( const char * p_json );

        friend std::map< std::string, node * > * get_map( node * p_node );
        friend std::vector< node * > * get_vector( node * p_node );

        friend node * create_json_node( NODE_TYPE p_node_type );
        friend void free_json_node( node ** p_node );

    public:
        node * get_sub_node( const char * p_key );
        node * get_sub_node( const int p_index );

        std::string get_string_val( const char * p_key );
        std::string get_string_val( const int p_index );

        int get_int_val( const char * p_key );
        int get_int_val( const int p_index );

        float get_float_val( const char * p_key );
        float get_float_val( const int p_index );

        bool get_boolean_val( const char * p_key );
        bool get_boolean_val( const int p_index );

        std::string get_string_val( void );
        int get_int_val( void );
        float get_float_val( void );
        bool get_boolean_val( void );

        int get_length( void );
        int get_index( node * p_node );
        NODE_TYPE get_node_type( void );

        node * parent(void);

        node * append( const char * p_key, const char * p_value );
        node * append( const char * p_value );

        node * append( const char * p_key, const int p_value );
        node * append( const int p_value );

        node * append( const char * p_key, const float p_value );
        node * append( const float p_value );

        node * append( const char * p_key, const bool p_value );
        node * append( const bool p_value );

        node * append( const char * p_key, NODE_TYPE p_node_type );
        node * append( node * p_node );

        node * remove( const char * p_key );
        node * remove( const int p_index );
        node * remove( node * p_node );

        node * set_key( const char * p_key );

        node * set_val( const char * p_key, const char * p_value );
        node * set_val( const char * p_key, const int p_value );
        node * set_val( const char * p_key, const float p_value );
        node * set_val( const char * p_key, const bool p_value );

        node * set_val( const int p_index, const char * p_value );
        node * set_val( const int p_index, const int p_value );
        node * set_val( const int p_index, const float p_value );
        node * set_val( const int p_index, const bool p_value );

        node * set_val( const char * p_value );
        node * set_val( const int p_value  );
        node * set_val( const float p_value );
        node * set_val( const bool p_value );

        node * empty( void );

        std::string to_string( void );
    };

    int checks( const char * p_json );
    node * parse( const char * p_json );

    node * create_json_node( NODE_TYPE p_node_type = UNDEFINED );
    void free_json_node( node ** p_node );

}
#endif //__JSON_H__