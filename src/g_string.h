#ifndef G_STRING_INCLUDED_HEADER
#define G_STRING_INCLUDED_HEADER
#include <string.h>

/*得到字符串中的单词个数*/
int get_string_wards_count( char* __str );

/*得到字符串中某个文字的单词*/
int get_string_wards_by_pos( char* __str, int __pos, char* __buf, int __buf_size );

/*在一个字符串__str中用__replace代替__find,并填充到__buf中*/
int string_replace( char* __str, char* __find, char* __replace, char* __buf, int __size );

/*取一个字符串的子字符串*/
int sub_string( const char* __src, int __direction, int __start, int __len, char *__buf, int __size );

/*得到字符串中某字母出现的个数*/
int get_string_letter_count(char *_str, char c);
#endif 
