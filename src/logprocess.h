#ifndef LOG_PROCESS_HEADER
#define LOG_PROCESS_HEADER
#include <string.h>

/*Write log*/
enum {
LOG_BY_YEAR=1,
LOG_BY_MONTH,
LOG_BY_DAY,
LOG_BY_HOUR
};

extern char *g_log_file;
extern int debug;

void log_msg( const char* __file, int __log_type, const char *__fmt, ... );
void trace(const char *file, const char *line, const char *fmt, ...);

#ifdef _DEBUG
#define TRACE(A,B...) trace(__FILE__,__LINE__, A, ##B);
#define LOG_DBG(A, B...)  log_msg(g_log_file, LOG_BY_MONTH, A, ##B)
#else
#define TRACE(A,B...) ;
#define LOG_DBG(A, B...) ;
#endif

/*Macro*/
#define LOG_MSG(A, B...)  log_msg(g_log_file, LOG_BY_MONTH, A, ##B)

#endif
