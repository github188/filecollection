#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "memmanager.h"
#include "logprocess.h"
#include "schedule.h"
#include "iniprocess.h"
#include "dbprocess.h"
#include "thread.h"
#include "taskmanager.h"
#include "taskitem.h"


extern char *TASK_INI;
extern char g_exec_time[13];


/*Task Status*/
enum 
{
	STOP=0,
	RUN
};

/*Task Internal Struct*/
typedef struct _sch_internal_item _sch_internal_item;

struct _sch_internal_item{
	struct SCHEDULE_ITEM	item;
	int			status;
	pid_t			itpid;
	_sch_internal_item*	next;	
};

/*Local Global Var*/
static struct _sch_internal_item* _sch_internal_list=NULL;
static struct pool* _sch_internal_pool=NULL;
/*Static Funcion*/
static int _sch_internal_msg();
static void _sch_internal_update_item();
static void _sch_internal_son_exit( int __id, pid_t __pid );
static void _sch_internal_main_loop();
static void _sch_internal_start_item( struct _sch_internal_item* __p );
static int _sch_internal_if_exec( struct tm *__tm, struct _sch_internal_item* __p );
static int _if_time_equ( int __tm, char *__tm_str );
static int _sch_internal_add_item( struct SCHEDULE_ITEM* __p );
static int _sch_internal_del_item( int __id );
void _sch_internal_son_exit_msg( int __msg_id, int __id, pid_t __pid );
void setsignal();
void sigcatch(int sig);

/*===============================================================================
 *Follow Interface Called By User
 *===============================================================================*/ 

/*
 *Create A Schedule Process, Return Process Id
 */
pid_t schedule_start( int *msg_id )
{
	pid_t _pid;

	*msg_id=msgget(g_key, IPC_CREAT| IPC_EXCL | 0666 );
	if ( *msg_id==-1 )
	{
		*msg_id=msgget(g_key, 0);
		if ( *msg_id==-1 )
		{
			log_msg(g_log_file, LOG_BY_MONTH, "msgget key=%d Error:%s", g_key, strerror(errno));
			return -1;
		}
	}

	if ( (_pid=fork())<0 )
		log_msg( g_log_file, LOG_BY_MONTH, "Start Schedule Process Fail!" );
	else
	if ( _pid==0 )
	{
		sleep(2);		
		log_msg( g_log_file, LOG_BY_MONTH, "Start Schedule Succeed!" );
		//signal(SIGCHLD,SIG_IGN);
		//signal(SIGPIPE,SIG_IGN);
		setsignal();

		/*Set Communication MSG QUEUE*/
		log_msg( g_log_file, LOG_BY_MONTH, "Message Queue ID:%d.", *msg_id);
		_sch_internal_queue_id=*msg_id;
		/*Create Memory Pool*/
		_sch_internal_pool=make_sub_pool(permanent_pool);
		if (_sch_internal_pool==NULL)
		{
			log_msg( g_log_file, LOG_BY_MONTH, "Schedule Process's Memory Pool Create Error, Process Exit.");
			exit(1);
		}
		/*Main Loop*/
		_sch_internal_main_loop();
	}
	return _pid;
}

/*
 *Stop Schedule Process
 */
int schedule_stop( int __msg_id )
{	
	TMSG msg;
	memset(&msg,0,sizeof(msg));
	msg.mtype = MSG_CMD_SCHEDULE;
	msg._cmd_buf[0] = _SCH_MAIN_STOP;
	
	return msgsnd(__msg_id, &msg, sizeof(msg)-sizeof(long), 0);
}

/*
 *Start Someone Schedule Item
 */
int schedule_start_item( pid_t __pid, T *__p )
{
	return 1;
}

/*
 *Stop Someone Schedule Item
 */
int schedule_stop_item( pid_t __pid, T *__p )
{
	return 1;
}

/*
 *Add Someone Schedule Item
 */
int schedule_add_item( int __msg_id, T *__p )
{
	TMSG msg;
	long cmd = 0;
	memset(&msg,0,sizeof(msg));
	msg.mtype = MSG_CMD_SCHEDULE;
	msg._cmd_buf[0] = _SCH_MAIN_ADD;
	memcpy(msg._cmd_buf+1, __p, sizeof(*__p));
	cmd = msg._cmd_buf[0];
	
	return msgsnd(__msg_id, &msg, sizeof(msg)-sizeof(long), 0);	
}

/*
 *Del Someoen Schedule Item
 */
int schedule_del_item( int __msg_id, int __id )
{

	TMSG msg;
	memset(&msg,0,sizeof(msg));
	msg.mtype = MSG_CMD_SCHEDULE;
	msg._cmd_buf[0] = _SCH_MAIN_DEL;
	*(msg._cmd_buf+1)=__id;

	return msgsnd(__msg_id, &msg, sizeof(msg)-sizeof(long), 0);		
}

/*===============================================================================
 *Follow Function Is Scheduler Internal implementation
 *===============================================================================*/ 
/*
 *子进程结束处理。
 */
static void _sch_internal_son_exit( int __id, pid_t __pid )
{
	struct _sch_internal_item* _tmp;
		
	/*Reset Internal List Status*/
	_tmp=_sch_internal_list;
	while(_tmp!=NULL)
	{
		if ( _tmp->item.id==__id)
		{
			_tmp->status=STOP;
			_tmp->itpid=0;
		}
		_tmp=_tmp->next;
	}
	log_msg(g_log_file, LOG_BY_MONTH, "TASK ID=%d, Son Process:%d End.", __id, __pid);
}
/*
 *处理通讯消息
 */
static int _sch_internal_msg()
{
	long _cmd;
	int _len;
	char* _cmd_arg;
	TMSG msg;
	memset(&msg,0,sizeof(msg));
	if ( (_len=msgrcv( _sch_internal_queue_id, &msg, sizeof(msg)-sizeof(long), MSG_CMD_SCHEDULE, IPC_NOWAIT ))<=0 )
	{	
		if ( errno!=ENOMSG )
		{
			log_msg(g_log_file, LOG_BY_MONTH, "接收消息队列错误:%s，退出。",strerror(errno) );
			msg._cmd_buf[0]=_SCH_MAIN_STOP;
		}
		else
			return _len;
	}
	_cmd = msg._cmd_buf[0];
	_cmd_arg=msg._cmd_buf+1;

	switch(_cmd)
	{
		case _SCH_MAIN_START:
			break;
		case _SCH_MAIN_STOP:
		{
			pid_t _pid;
			log_msg(g_log_file, LOG_BY_MONTH, "Start Stop Schedule,Waiting Son Process End.");
			while ( (_pid=wait(NULL))>0 )
			{
				log_msg(g_log_file, LOG_BY_MONTH, "Son Process:%d Exit.", _pid);
			}
			log_msg(g_log_file, LOG_BY_MONTH, "Final Stop Schedule Process Exit");
			msgctl(_sch_internal_queue_id,IPC_RMID, 0);
			exit(0);
			break;
		}
		case _SCH_MAIN_ADD:
		{
			struct SCHEDULE_ITEM item;
			memcpy(&item, _cmd_arg, sizeof(item));
			if ( _sch_internal_add_item( &item ) )
			{
				log_msg(g_log_file, LOG_BY_MONTH, "Add Schedule Task:%d Sucess.", item.id);
				printf("\nAdd Schedule Task:%d Sucess.", item.id);
				fflush(stdout); 
			}
			else
			{
				log_msg(g_log_file, LOG_BY_MONTH, "Add Schedule Task:%d Fail.", item.id);
				printf("\nAdd Schedule Task:%d Fail.", item.id);
			}
			break;
		}
		case _SCH_MAIN_DEL:
		{
			int id=0;
			memcpy(&id,_cmd_arg, sizeof(id));
			_sch_internal_del_item(id);
			break;
		}
		case _SCH_SON_START:
			break;
		case _SCH_SON_STOP:
		{
			int id,pid;
			memcpy(&id,_cmd_arg, sizeof(int));
			memcpy(&pid,_cmd_arg+sizeof(int),sizeof(int));
			_sch_internal_son_exit(id, pid);
			break;
		}
                case _SCH_DB_RELOAD:
		{
	 	   creat_task_list();
                   TASK_ITEM* iterr = _task_list,*iters = _task_list;
		   _sch_internal_item *_prev,*_cur;
	           _prev=_cur=_sch_internal_list;	
		   for(;iters!=NULL;iters=iters->next) //有则替换
		   {
		      _cur = _prev;
                      while(_cur)
                      {
                        if(_cur->item.id == iters->sch_item.id)
                        {
                           memcpy(&_cur->item,&iters->sch_item,sizeof(iters->sch_item));
                           log_msg(g_log_file, LOG_BY_MONTH,"TASK:%d  %s is update ",_cur->item.id,iters->ftp_path);
		       	   break;
                        }
                        _cur = _cur->next;
                      }
		  if(_cur == NULL)
		  {
	     	      if ( _sch_internal_add_item( &iters->sch_item ) )
		      {
		         log_msg(g_log_file, LOG_BY_MONTH, "Add Schedule Task:%d Sucess.", iters->sch_item.id);
			 printf("\nAdd Schedule Task:%d Sucess.", iters->sch_item.id);
			 fflush(stdout); 
		      }
		      else
		      {
		         log_msg(g_log_file, LOG_BY_MONTH, "Add Schedule Task:%d Fail.", iters->sch_item.id);
			 printf("\nAdd Schedule Task:%d Fail.", iters->sch_item.id);
		      }
		   }
			   
			}
			for(;_cur!=NULL;_cur=_cur->next)
			{
			    iters = iterr;
			    while(iters)
			    {
			       if(iters->sch_item.id == _cur->item.id)
                               {
                                  log_msg(g_log_file, LOG_BY_MONTH,"TASK:%d is exists ",_cur->item.id); 
                                  break;
                               }
				   iters = iters->next;
			    }

				if(!iters)
				{
				   _sch_internal_del_item(_cur->item.id);
                                   log_msg(g_log_file, LOG_BY_MONTH,"TASK:%d is del ",_cur->item.id);
				}
			}
		    break;
		}
		default:
			log_msg(g_log_file, LOG_BY_MONTH, "Schedule Not UnderStand Msg Type:%l.", _cmd );
	}
	return 0;
}

/*
 *调度进程的主循环
 */
static void _sch_internal_main_loop()
{

	int nTimeInterVal = 0, nReloadInterval = 0;
	time_t  _last_tm = time(0) , _reload_time = time(0);
	struct tm _cur_st_tm;
	struct _sch_internal_item* _tmp;

	nTimeInterVal = read_ini_int(TASK_INI, TASK_CAPTION, "SCHEDULE_TIME_INTERVAL", 30);
        nReloadInterval = read_ini_int(TASK_INI, TASK_CAPTION, "RELOAD_TIME_INTERVAL", 30);
	while(1)
	{
		usleep_r(10);
                nReloadInterval = read_ini_int(TASK_INI, TASK_CAPTION, "RELOAD_TIME_INTERVAL", 30);
                if(time(0) - _reload_time >= nReloadInterval)
                {
                   connect_oracle(TASK_INI);
	           int _ncount = create_task_item_list_by_db_updated(g_pid);
	           disconnect_oracle();
                    if(_ncount>0) _sch_internal_update_item();
                   _reload_time = time(0);
                }

		_sch_internal_msg();
	        	
		/*Process Task*/
		if ( time(0) - _last_tm >=nTimeInterVal )
		{
			/*New Min Recv Check Task*/
			_last_tm = time(0);
			_tmp=_sch_internal_list;
			localtime_r(&_last_tm, &_cur_st_tm);
			while(_tmp!=NULL)
			{
				if ( _tmp->status!=RUN )
				{
			       	   if (  _sch_internal_if_exec(&_cur_st_tm, _tmp) )
                                   {
                                      sprintf(g_exec_time,"%04d%02d%02d%02d%02d",_cur_st_tm.tm_year+1900,_cur_st_tm.tm_mon+1,_cur_st_tm.tm_mday,_cur_st_tm.tm_hour,_cur_st_tm.tm_min);
				      g_exec_time[12]='\0';
				      _sch_internal_start_item( _tmp );
                                   }
				}
				_tmp=_tmp->next;
			}
			
		}
	}
}

/*
 *启动时间到达需执行的任务
 */
static void _sch_internal_start_item( struct _sch_internal_item* __p )
{
	pid_t _pid;
	
	if ( (_pid=fork())<0 )
	{
		log_msg( g_log_file, LOG_BY_MONTH, "Fork Process ID=%d's Task Error." );
		return;
	}
	else
	if ( _pid==0 )
	{
		setsignal();
		sleep(2);
		log_msg( g_log_file, LOG_BY_MONTH, "TASK ID=%d,Process ID=%d Start!", __p->item.id, getpid() );
		(__p->item.func) (__p->item.arg);
		_sch_internal_son_exit_msg( _sch_internal_queue_id, __p->item.id, getpid() );
		exit(0);
	}
	__p->itpid=_pid;
	__p->status=RUN;
}

/*
 *判断是否定时的时刻和现在时刻相同，否则，执行。
 */
static int _sch_internal_if_exec( struct tm *__tm, struct _sch_internal_item* __p )
{
	if ( !_if_time_equ(__tm->tm_mon, __p->item.month) )
		return 0;
	else
	if ( !_if_time_equ(__tm->tm_mday, __p->item.day) )
		return 0;
	else
	if ( !_if_time_equ(__tm->tm_hour, __p->item.hour) )
		return 0;
	else
	if ( !_if_time_equ(__tm->tm_min, __p->item.min) )
		return 0;	
	return 1;
}

/*
 *规则通CRONTAB里的。
 */
static int _if_time_equ( int __tm, char *__tm_str )
{
	int _tmp;
	char *_prev,*_cur;
	_prev=_cur=__tm_str;
	
	/* * */
	if ( strcmp(__tm_str, "*")==0 )
		return 1;
	else
	/* * / num */
	if ( strncmp(__tm_str, "*/", 2)==0 )
	{
		if ( strlen(__tm_str)==2 )
			return 0;
		if ( (_tmp=atoi(__tm_str+2))==0 )
			return 0;
		if (  (__tm%_tmp)==0 )
			return 1;
		return 0;
	}
	else
	/*num*/
        if ( strchr(__tm_str, ',')== NULL )
        {
                if ( atoi(__tm_str)==__tm )
                        return 1;   
        }
	else
	{
		/* num,num,num */
		char _str[10];
	        _prev = __tm_str;
	        while ( (_cur=strchr(_prev, ','))!=NULL  )
		{       
			memset( _str, 0, sizeof(_str) );
			strncpy( _str, _prev, _cur-_prev );
			if ( atoi(_str)==__tm )
				return 1;
		        _prev=_cur+1;
	            	if ( _prev==NULL )
	                	break;
		}
		if ( _prev!=NULL )
		{
			if ( atoi(_prev)==__tm )
				return 1;
		}	
	}
	return 0;
}


static void _sch_internal_update_item()
{
    TASK_ITEM *iters = _task_list;
	_sch_internal_item *_cur;
	_cur=_sch_internal_list;	
	for(;iters!=NULL;iters=iters->next) //有则替换
	{
	    _cur = _sch_internal_list;
           int _nflag = 0;
        while(_cur)
        {
           if(_cur->item.id == iters->sch_item.id)
           {
              if( 0 == iters->ftp_flag)
              {
                 _sch_internal_del_item(_cur->item.id);
				 log_msg(g_log_file, LOG_BY_MONTH,"TASK:%d  %s is del ",_cur->item.id,iters->ftp_path);
				 break;
              }
	      else
	      {
                 memcpy(&_cur->item,&iters->sch_item,sizeof(iters->sch_item));
                 log_msg(g_log_file, LOG_BY_MONTH,"TASK:%d is update ",_cur->item.id);
                 break;
	      }
              _nflag = 1;
            }
                        _cur = _cur->next;
          }
		  if(_cur == NULL && _nflag ==0 && 1 == iters->ftp_flag)
		  {
	     	  if ( _sch_internal_add_item( &iters->sch_item ) )
		  {
		         log_msg(g_log_file, LOG_BY_MONTH, "Add Schedule Task:%d Sucess.", iters->sch_item.id);
			 printf("\nAdd Schedule Task:%d Sucess.", iters->sch_item.id);
			 fflush(stdout); 
		   }
		      else
		      {
		         log_msg(g_log_file, LOG_BY_MONTH, "Add Schedule Task:%d Fail.", iters->sch_item.id);
			     printf("\nAdd Schedule Task:%d Fail.", iters->sch_item.id);
		      }
		   }
			   
			}
}

/*
 *添加新任务列表的末尾
 */
static int _sch_internal_add_item( struct SCHEDULE_ITEM* __p )
{
	_sch_internal_item *_tmp;
	_sch_internal_item *_new;
	/*在POOL中创建一个对象*/
	_new=(_sch_internal_item *)palloc(_sch_internal_pool, sizeof(struct _sch_internal_item));
	assert(_new!=NULL);
	
	_new->item=*__p;
	_new->status=STOP;
	_new->itpid=0;
	_new->next = NULL;
	
	if ( _sch_internal_list==NULL)
	{
		_sch_internal_list=_new;
		return 1;
	}
	
	/*判断任务ID是否存在*/
	_tmp=_sch_internal_list;
	while (_tmp->next)
	{
		if ( _tmp->item.id==__p->id )
			return 0;		
		_tmp=_tmp->next;
	}
	/*_tmp指向最后一个元素了*/
	if ( _tmp->item.id==__p->id )
		return 0;
	_tmp->next=_new;
	
	return 1;	
} 

/*
 *删除任务BY ID,0没有发现，1成功，－1，正在运行。
 */
static int _sch_internal_del_item( int __id )
{	
	_sch_internal_item *_prev,*_cur;
	
	_prev=_cur=_sch_internal_list;
	
	while(_cur)
	{
		if ( _cur->item.id==__id )
		{
			//if ( _cur->status==RUN )
			if(_prev == _cur)
			{
				//log_msg( g_log_file, LOG_BY_MONTH, "The Task ID:%d Is Running, Not Been Del.", __id );
				//return -1;
				_sch_internal_list = _sch_internal_list->next;
			}
			else
			{
			    _prev->next=_cur->next;
			    _cur->next=NULL;
			}
			return 1;
		}
		_prev=_cur;
		_cur=_cur->next;
	}
	return 0;
}

/*
 *Post Son Process Exit Msg
 */
void _sch_internal_son_exit_msg( int __msg_id, int __id, pid_t __pid )
{
/*	char _buf[256];
	memset(_buf, 0, sizeof(_buf));
	*((int*)_buf)=_SCH_SON_STOP;
	memcpy(_buf+sizeof(long), &__id, sizeof(int));
	memcpy(_buf+sizeof(long)+sizeof(int), &__pid, sizeof(int));
	msgsnd(__msg_id, _buf, sizeof(long)+sizeof(int)+sizeof(int), 0);
*/
	TMSG msg;
	memset(&msg,0,sizeof(msg));
	msg.mtype = MSG_CMD_SCHEDULE;
	msg._cmd_buf[0] = _SCH_SON_STOP;
	memcpy(msg._cmd_buf+1, &__id, sizeof(int));
	memcpy(msg._cmd_buf+1+sizeof(int), &__pid, sizeof(int));

	msgsnd(__msg_id, &msg, sizeof(msg)-sizeof(long), 0);	
}

void sigcatch(int sig)
{
  char signame[30] = "";
  char dir[256] = "";
  switch(sig)
  {
  case SIGINT:
    strcpy(signame, "SIGINT");
    break;
  case SIGILL:
    strcpy(signame, "SIGILL");
    break;
  case SIGFPE:
    strcpy(signame, "SIGFPE");
    break;
  case SIGSEGV:
    strcpy(signame, "SIGSEGV");
    break;
  case SIGTERM:
    strcpy(signame,"SIGTERM");
    break;
  case SIGALRM:
    strcpy(signame,"SIGALRM");
    break;
  case SIGPIPE:
    strcpy(signame,"SIGPIPE");
    break;
  case SIGABRT:
    strcpy(signame, "SIGABRT");
    break;
default:
  	sprintf(signame, "%d", sig);
	break;
  	}
  if (!getcwd(dir, sizeof(dir))) {
		if (errno==ERANGE) strcpy(dir, "<too long>");
			else strcpy(dir, "???");
	}
  LOG_MSG("pid:%d catch signal:%s(%d),child dir:%s process exit.",int(getpid()),signame,sig, dir);
  //exit(0);
  signal(sig, SIG_IGN);
}

void setsignal()
{
/* interrupt */
  signal(SIGINT, sigcatch);
/* SIGCHLD */
  signal(SIGCHLD, SIG_IGN);
/* illegal instruction - invalid function image */
  signal(SIGILL, sigcatch);
/* floating point exception */
  signal(SIGFPE, sigcatch);
/* segment violation */
  signal(SIGSEGV, sigcatch);
/* Software termination signal from kill */
  signal(SIGTERM, sigcatch);
/* Alarm Clock */
  signal(SIGALRM, sigcatch);
/* Broken Pipe */
  signal(SIGPIPE, sigcatch);
/* abnormal termination triggered by abort call */
  signal(SIGABRT, sigcatch);
}

