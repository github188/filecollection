#ifndef G_STRING_INCLUDED_HEADER
#define G_STRING_INCLUDED_HEADER
#include <string.h>

/*�õ��ַ����еĵ��ʸ���*/
int get_string_wards_count( char* __str );

/*�õ��ַ�����ĳ�����ֵĵ���*/
int get_string_wards_by_pos( char* __str, int __pos, char* __buf, int __buf_size );

/*��һ���ַ���__str����__replace����__find,����䵽__buf��*/
int string_replace( char* __str, char* __find, char* __replace, char* __buf, int __size );

/*ȡһ���ַ��������ַ���*/
int sub_string( const char* __src, int __direction, int __start, int __len, char *__buf, int __size );

/*�õ��ַ�����ĳ��ĸ���ֵĸ���*/
int get_string_letter_count(char *_str, char c);
#endif 
