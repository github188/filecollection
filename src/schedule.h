#ifndef SCHEDULE_HEADER
#define SCHEDULE_HEADER

#include <sys/types.h>
#ifdef T
#undef T
#endif

#define MSG_CMD_SCHEDULE 100
#define MSG_CMD_SQL		 102
#define MSG_CMD_SCHEDULE_ITEM 104
#define SCHEDULE_ITEM    T
#define TASK_CAPTION      "TaskList"
#define TASK_ITEM_CAPTION "FtpParm"

#define _SCH_MAIN_START 0	/*启动调度器*/
#define _SCH_MAIN_STOP	1   /*停止调度器,无参数*/ 
#define _SCH_MAIN_ADD	2   /*增加调度任务，参数为一个SCHEDULE_ITEM结构*/
#define _SCH_MAIN_DEL	3	/*删除调度任务，参数为SCHEDULE_ITEM结构的ID*/
#define _SCH_SON_START	4	/*任务进程开始*/
#define _SCH_SON_STOP	5	/*任务进程停止，参数为SON的ID,以及PID*/
#define _SCH_DB_RELOAD    6   /*重载数据库*/

#define COPY_NULL         0   /*不执行拷贝操作*/
#define COPY_REAL         1   /*物理拷贝*/
#define COPY_SYMBOL_LINK  2   /*符号链接*/
#define COPY_HARD_LINK    3   /*硬链接*/

typedef struct T
{
	int	id;
	char	month[16];
	char	day[16];
	char	hour[16];
	char	min[16];
	void	(*func)(int);
	int 	arg;
} T;

typedef struct TMSG
{
	long    mtype;
	char	_cmd_buf[256];
}TMSG;

pid_t schedule_start( int *__msg_id );
int schedule_stop( int __msg_id );

int schedule_start_item( pid_t __pid, T *__p );
int schedule_stop_item( pid_t __pid, T *__p );

int schedule_add_item( int __msg_id, T *__p );
int schedule_del_item( int __msg_id, int __id);

extern pid_t  g_pid;
extern int 	_sch_internal_queue_id;
extern key_t g_key; 
extern char *EXEC_PATH;

#endif
