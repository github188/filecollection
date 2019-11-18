#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include "logprocess.h"
#include <errno.h>

void log_msg( const char* __file, int __log_type, const char *__fmt, ... )
{
	char _log_name[256];
	char _tm_str[32];
	FILE *_fd;
	va_list _ap;
	time_t _cur;
	struct tm *_cur_tm;
	
	/*Get Cur Time*/
	memset( _tm_str, 0, sizeof(_tm_str) );
	time(&_cur);
	_cur_tm=localtime(&_cur);
	strftime( _tm_str, 31, "[%Y-%m-%d %H:%M:%S]:", _cur_tm );
	
	memset( _log_name, 0, sizeof(_log_name) );
	switch(__log_type)
	{
		case LOG_BY_YEAR:
		{
			sprintf( _log_name, "%s%04d", __file, _cur_tm->tm_year+1900 ); 
			break;
		}
		case LOG_BY_MONTH:
		{
			sprintf( _log_name, "%s%04d%02d", __file, _cur_tm->tm_year+1900, _cur_tm->tm_mon+1 ); 
			break;
		}
		case LOG_BY_DAY:
		{
			sprintf( _log_name, "%s%04d%02d%02d", __file, _cur_tm->tm_year+1900, _cur_tm->tm_mon+1, _cur_tm->tm_mday ); 
			break;
		}
		case LOG_BY_HOUR:
		{
			sprintf( _log_name, "%s%04d%02d%02d%02d", __file, _cur_tm->tm_year+1900, _cur_tm->tm_mon+1, _cur_tm->tm_mday, _cur_tm->tm_hour); 
			break;
		}
		default:
		{
			fprintf(stderr, "Log Type [%d] Err!\n", __log_type ); 
			return;
		}
	}
	
	if ( (_fd=fopen(_log_name, "a+"))==NULL )
	{
		fprintf(stderr, "Open Log File:[%s] Fail :%s!\n", _log_name,strerror(errno) ); 
		return;
	}
	
	fputs(_tm_str, _fd);
	va_start(_ap, __fmt);
	vfprintf(_fd, __fmt, _ap);
	if (debug)
	{
		printf("%s", _tm_str);
		vprintf(__fmt, _ap);
		printf("\n");
	}
	va_end(_ap);
	if(strchr(__fmt,'\n') == NULL)
		fputs("\n", _fd);
	
	fclose(_fd);
	return;
}
