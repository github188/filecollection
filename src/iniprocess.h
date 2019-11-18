#ifndef INI_PROCESS_HEADER
#define INI_PROCESS_HEADER
#include "memmanager.h"

/*读取配置文件中的配置,和WIN配置文件的格式一样*/
char* read_ini_string( struct pool* __pool, const char* __file, const char* __section, const char* __key, const char* __default);
int read_ini_int( const char* __file, const char* __section, const char* __key, int  __default );

/*改写配置文件*/
void write_ini_string( const char* __file, const char* __section, const char* __key, const char* __value );
void write_ini_int( const char* __file, const char* __section, const char* __key, int __value );

#endif
