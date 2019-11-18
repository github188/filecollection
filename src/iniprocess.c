#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "g_string.h"
#include "iniprocess.h"
#include "logprocess.h"
/***************************
 * 配置文件格式例子        
 *[SECTION]
 *KEY = VALUE
 *#开头的行为注释
 ***************************/

/*读取配置文件中的配置,和WIN配置文件的格式一样*/
char* read_ini_string( struct pool* __pool, const char* __file, const char* __section, const char* __key, const char* __default )
{
	FILE *_fd;
	char _buf[256];
	char _section[128];
	char _key[128];
	char _value[128];
	
	/*First Open File*/
	if ( (_fd=fopen(__file, "r")) == NULL )
	{
		LOG_MSG("read_ini_string Open Ini File:[%s] Fail!!!\n", __file );
		return pstrdup( __pool, __default);
	}

	memset( _buf, 0, sizeof(_buf) );
	memset( _section, 0, sizeof(_section) );
	memset( _key, 0, sizeof(_key) );
	memset( _value, 0, sizeof(_value) );
	
	while( fgets(_buf, 256, _fd)!=NULL )
	{
			/*Get Line First Word*/
			get_string_wards_by_pos( _buf, 1, _section, 128);
			if ( _section[0]=='[' && *(_section+strlen(__section)+1)==']' )
			{
				/*Find The Section*/
				if ( strncmp(_section+1, __section, strlen(__section))==0 )
				{
					/*Finding The Key?*/
					while ( fgets(_buf, 256, _fd)!=NULL )
					{
						get_string_wards_by_pos( _buf, 1, _key, 128);
						/*Find Next Section, Stop */
						if ( _key[0]== '[' )
							break;
						/*Find The Key ?*/
						if ( strcmp( _key, __key )!= 0 )
						{
							memset( _buf, 0, sizeof(_buf) );
							memset( _key, 0, sizeof(_key) );
							continue;
						}
						/*Find The Key*/
						get_string_wards_by_pos( _buf, 3, _value, 128);
						fclose(_fd);
						return pstrdup( __pool, _value);
					}
					/*Find Section, But Not Find Key*/
					fclose(_fd);
					LOG_MSG("read_ini_string Open Ini File:[%s], Find The Section:[%s], But Not Find The Key:[%s].\n", __file, __section, __key );
					return pstrdup( __pool, __default);
				}
			}	
			memset( _buf, 0, sizeof(_buf) );
			memset( _section, 0, sizeof(_section) );
			memset( _key, 0, sizeof(_key) );
	}
	fclose(_fd);
	LOG_MSG("read_ini_string Open Ini File:[%s], Not Find The Section:[%s].\n", __file, __section );
	return pstrdup( __pool, __default);
}

int read_ini_int( const char* __file, const char* __section, const char* __key ,int  __default )
{
	FILE *_fd;
	char _buf[256];
	char _section[128];
	char _key[128];
	char _value[128];
	
	/*First Open File*/
	if ( (_fd=fopen(__file, "r")) == NULL )
	{
		LOG_MSG("read_ini_int Open Ini File:[%s] Fail!!!\n", __file );
		return __default;
	}

	memset( _buf, 0, sizeof(_buf) );
	memset( _section, 0, sizeof(_section) );
	memset( _key, 0, sizeof(_key) );
	memset( _value, 0, sizeof(_value) );
	
	while( fgets(_buf, 256, _fd)!=NULL )
	{
			/*Get Line First Word*/
			get_string_wards_by_pos( _buf, 1, _section, 128);
			if ( _section[0]=='[' && *(_section+strlen(__section)+1)==']' )
			{
				/*Find The Section*/
				if ( strncmp(_section+1, __section, strlen(__section))==0 )
				{
					/*Finding The Key?*/
					while ( fgets(_buf, 256, _fd)!=NULL )
					{
						if(_buf[0] == '#' || _buf[0] == '\n')
							continue;
						get_string_wards_by_pos( _buf, 1, _key, 128);
						/*Find Next Section, Stop */
						if ( _key[0]== '[' )
							break;
						/*Find The Key ?*/
						if ( strcmp( _key, __key )!= 0 )
						{
							memset( _buf, 0, sizeof(_buf) );
							memset( _key, 0, sizeof(_key) );
							continue;
						}
						/*Find The Key*/
						get_string_wards_by_pos( _buf, 3, _value, 128);
						fclose(_fd);
						return atoi(_value);
					}
					/*Find Section, But Not Find Key*/
					fclose(_fd);
					LOG_MSG("read_ini_int Open Ini File:[%s], Find The Section:[%s], But Not Find The Key:[%s].\n", __file, __section, __key );
					return __default;
				}
			}	
			memset( _buf, 0, sizeof(_buf) );
			memset( _section, 0, sizeof(_section) );
			memset( _key, 0, sizeof(_key) );
	}
	fclose(_fd);
	LOG_MSG("read_ini_int Open Ini File:[%s], Not Find The Section:[%s].\n", __file, __section );
	return __default;
}

/*改写配置文件*/
void write_ini_string( const char* __file, const char* __section, const char* __key, const char* __value )
{	
	FILE *_fd;
	FILE *_fd_new;
	char _buf[256];
	char _section[128];
	char _key[128];
	char _value[128];
	char _file_new_name[128];
	int  _if_add=0;
	/*First Open File*/
	if ( (_fd=fopen(__file, "r")) == NULL )
	{
		if ( (_fd=fopen(__file, "w"))==NULL )
		{	
			log_msg(g_log_file, LOG_BY_MONTH, "Create File %s Fail, Error %s.", __file, strerror(errno));
			return;
		}
	}
	memset(_file_new_name, 0, sizeof(_file_new_name) );
	strcpy( _file_new_name, __file );
	strcat( _file_new_name, "_tmp" );
	if ( (_fd_new=fopen(_file_new_name, "w")) == NULL )
	{
		fclose(_fd);
		fprintf(stderr, "write_ini_string Open Ini File:[%s_tmp] For Update Fail!!!", __file );
		return;
	}
	
	memset( _buf, 0, sizeof(_buf) );
	memset( _section, 0, sizeof(_section) );
	memset( _key, 0, sizeof(_key) );
	memset( _value, 0, sizeof(_value) );
	
	while( (fgets(_buf, 256, _fd)!=NULL)&&(_if_add==0) )
	{
			/*Get Line First Word*/
			get_string_wards_by_pos( _buf, 1, _section, 128);
			if ( _section[0]=='[' && *(_section+strlen(__section)+1)==']' )
			{
				fputs( _buf, _fd_new);
				/*Find The Section*/
				if ( strncmp(_section+1, __section, strlen(__section))==0 )
				{
					/*Finding The Key?*/
					while ( fgets(_buf, 256, _fd)!=NULL )
					{
						get_string_wards_by_pos( _buf, 1, _key, 128);
						/*Find Next Section, Stop */
						if ( _key[0]== '[' )
						{
							break;
						}
						/*Find The Key ?*/
						if ( strcmp( _key, __key )!= 0 )
						{
							fputs( _buf, _fd_new);
							memset( _buf, 0, sizeof(_buf) );
							memset( _key, 0, sizeof(_key) );
							continue;
						}
						/*Find The Key*/
						fputs(__key, _fd_new);
						fputs(" = ", _fd_new);
						fputs(__value, _fd_new);
						fputs("\n", _fd_new);
						_if_add=1;
						memset( _buf, 0, sizeof(_buf) );
						memset( _key, 0, sizeof(_key) );
					}
					if ( _if_add==0 )
					{
						fputs(__key, _fd_new);
						fputs(" = ", _fd_new);  
						fputs(__value, _fd_new);
						fputs("\n", _fd_new);   
						fputs(_buf, _fd_new);
						_if_add=1;
					}
				}
			}
			else
				fputs(_buf, _fd_new);	
			memset( _buf, 0, sizeof(_buf) );
			memset( _section, 0, sizeof(_section) );
			memset( _key, 0, sizeof(_key) );
	}
	
	if ( _if_add==0 )
	{
		fprintf(_fd_new, "[%s]\n", __section);
		fprintf(_fd_new, "%s = %s\n", __key, __value);
	}
	fclose(_fd);
	fclose(_fd_new);
	remove( __file );
	rename( _file_new_name, __file );
	return;
	
}

void write_ini_int( const char* __file, const char* __section, const char* __key, int __value )
{
	char _buf[64];
	memset( _buf, 0, sizeof(_buf) );
	
	sprintf(_buf, "%d", __value );
	write_ini_string( __file, __section, __key, _buf );
}
