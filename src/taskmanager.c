#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include "logprocess.h"
#include "taskmanager.h"
#include "memmanager.h"
#include "schedule.h"
#include "regexpress.h"
#include "fileprocess.h"
#include "ftpprocess.h"
#include "taskitem.h"
#include "dbprocess.h"
#include "iniprocess.h"
#include "g_string.h"
#include "frame_memory.h"
#include "frame_mutex.h"
#include "sftp.h"
/*
UPDATE-LOG
2005-03-17�����Ӷ��ļ�·��������ڱ�־��������
2005-03-18:���Ӷ��ļ���������ڱ�־��������
2005-03-24:���ӵ�����FTP���ع����У��ļ�Ϊ0�Ļ��ҿ�ɾ����־Ϊ1����ֱ��ɾ����
2005-03-25:���ӵ�����FTP�ϴ������У��ļ�Ϊ0�Ļ��ҿ�ɾ����־Ϊ1����ֱ��ɾ����
2005-03-28:����������ѡ��USE_TMP�����崫�͹������Ƿ�ʹ����ʱ�ļ�����
2005-04-28:���Ӷ��Ѿ����ػ��ϴ����ļ�����¼������
2005-06-20:�����жϴ�СΪ0�ʹ�С���ڱ仯��λ�ã��Ѵ�С���ڱ仯�ķ���ǰ�棬�ж�0���ں���
*/

/*
*1.Get Ini File Name
*2.Load Ini File Name And Create List
*3.Start Schedule
*4.Add Schedule Item
*/
extern char g_exec_time[13];
extern int g_file_count;
extern char *EXEC_PATH;
extern char *TASK_INI;
extern tFileInfoMap *g_pMapFileInfo;
extern tSystemVSemaphore *g_pFileSem;
extern tMemoryAllocator *g_pAlloc;
extern struct TASK_ITEM* _task_list;
extern pool* permanent_pool;
/*Macro*/
#define LOG_MSG(A, B...)  log_msg(g_log_file, LOG_BY_MONTH, A, ##B)
#define LOG_TASK_MSG(A, B...)  log_msg(g_log_file,  LOG_BY_MONTH, A, ##B)

/*2005-04-28 ADD BEGIN*/
extern char *EXEC_PATH;
static char FILE_LIST_LOG[]="FILE_LIST";
/*2005-04-28 ADD END*/

void 
creat_task_list()
{   
	//create_task_item_list_by_ini(TASK_INI);
	connect_oracle(TASK_INI);
	create_task_item_list_by_db(g_pid);
	disconnect_oracle();
}

int load_task()
{
	pid_t _sch_pid;
	int _sch_msg_id;
	TASK_ITEM *_cur;
	int _p_id;
	_p_id=read_ini_int(TASK_INI, TASK_CAPTION, "SCHEDULE_P_ID", 0);

	if ( _p_id!=0 )
	{
		printf("Scheduler Is exist, program need not run.\n");
		return 0; 
	}

	_cur=_task_list;

	/*Start Schedule*/
	
	_sch_pid=schedule_start(&_sch_msg_id); 
	if(_sch_pid == -1)
	{
		printf("start program error:%s\n",strerror(errno));
		exit(0);
	}
	write_ini_int(TASK_INI, TASK_CAPTION, "SCHEDULE_P_ID", _sch_pid);
	write_ini_int(TASK_INI, TASK_CAPTION, "SCHEDULE_MSG_ID", _sch_msg_id);

	start_db_process(_sch_msg_id);
	/*Add*/
	while(_cur)
	{
		if(schedule_add_item( _sch_msg_id, &(_cur->sch_item) )<0)
		{
			LOG_MSG("add schedult id %d fail:%s",_cur->sch_item.id,strerror(errno));
		}
		_cur=_cur->next;
	}
	return 0;
}

/*
�����ļ�����Ҫ����:
1.�õ���һ��FTP ���ļ��б�
2.10���õ��ڶ���FTP ���ļ��б�(��֤�ļ�����û����д)
3.ѭ����ȡ�ڶ����б�,�ѷ��������������ļ�����,�γɷ���Ҫ����ļ����б�
a:�������ļ���Ҫ���
b:���ҵ����ؼ�¼�ļ����Ѿ����ڵ�(˵���Ѿ����ع�,ͬʱƥ�����ƺʹ�С)
c:�ڱ����ļ��и��ļ����ʹ�С���ڵ�(˵���ļ��Ѿ�����,ͬʱƥ�����ƺʹ�С
d:�͵�һ���ļ��б��С��ͬ��(˵���ļ����ڽ���д������ʱ����)
f:�ļ���СΪ0���ļ�
4.���ط���Ҫ����ļ�.
5.ɨ�豾���ļ���,������ķ���Ҫ����ļ��б�Ƚ�,�����سɹ���д�����ؼ�¼�ļ���,ͬʱɾ���ļ�����ͬ,����С��ͬ���ļ�,�ȴ��´�����.
6.ִ�������ļ��еĽű�.


�����ļ�����Ҫ����
1.�õ���һ�α����ļ��б�
2. 10���õ��ڶ��α����ļ��б�(��֤�ļ�����û����д)
3.�õ�FTP�ļ����б�
4.ѭ����ȡ�ڶ��α����ļ��б�,ȷ��Ҫ�ϴ����ļ�,�ѷ��������������ļ��޳�
a:����ɨ���С��ͬ���ļ���(��������д���ļ�)
b:��FTP���Ѿ�����ͬ������С��ͬ���ļ���(�����Ѿ����ڵ��ļ�)
c:�Ѿ��ڳɹ��ϴ��ļ�¼�ļ��д��ڵ�(�����Ѿ����ڵ��ļ�)
d:�������ļ����淶��    (��������Ҫ���ļ�)
f:�ļ���СΪ0���ļ�
5.�ϴ��ļ�.
6.�õ�FTP�ļ��б�,�ѳɹ��ϴ����ļ�д�뵽�ɹ���¼��,û���ϴ��ɹ��´ο��Լ�������.
7.ִ�������ļ��еĽű�.
*/


static int ftp_put_task( struct TASK_ITEM* __item)
{
	struct FTP_SERVER_INFO *_srv_info;
	FTP_FILE_INFO *_first, *_second, *_ftp_list;
	char _tmp_remote[128];
	char _tmp_local_path[256];
	char _tmp_ftp_path[256];
	char _tmp_file_name[128];
	/*2005-04-28 ADD BEGIN*/
	char _tmp_file_list[128];
	char _dir[NAME_MAX+1]="";
	char _file_modify_time[32]="";
	int nFileRows = 0;	
	/*2005-04-28 ADD END*/
	int _ret=1;
	//printf("ftp_put_task pid=%d, and sleep 10\n", getpid());	
	//sleep(30);

	/*2005-04-28 ADD BEGIN*/
	memset(_tmp_file_list, 0, sizeof(_tmp_file_list));
	sprintf(_tmp_file_list, "%s/filelist/%s_%d_", EXEC_PATH, FILE_LIST_LOG, __item->sch_item.id);
	/*2005-04-28 ADD END*/

	/*Trans Path String*/
	trans_path_str( __item->local_path,  _tmp_local_path, sizeof( _tmp_local_path ) );
	trans_path_str( __item->ftp_path,  _tmp_ftp_path, sizeof( _tmp_ftp_path ) );
	/*Trans File Name String*/
	trans_path_str( __item->file,  _tmp_file_name, sizeof( _tmp_file_name )  );


	/*If Local Not Exist Exit*/
	if ( if_file_exist(_tmp_local_path) )
	{   
		/*Dir Or File Exist*/
		if ( if_isdir(_tmp_local_path)==0 )
		{
			LOG_MSG( "The local dir %s file, not is dir.", _tmp_local_path);
			return 0;
		}
	}
	else
		return 0;

	/*First Scan Local add by ganhh 2007_03_08*/
	_first=get_local_file_list(permanent_pool, _tmp_local_path);	

	if(_first == NULL)
	{
		LOG_MSG( "path:%s no file exist to ftp put.", _tmp_local_path);
		return 1;
	}
	while(sleep(10));
	/*Get Ftp Info*/
	_srv_info=get_ftp_info(__item);
	if(NULL == _srv_info)
	{
		LOG_MSG( "get_ftp_info fail.");
		return 0;
	}
	/*Connect*/
	if ( ftp_conn(_srv_info)==0 )
	{
		LOG_MSG( "Connect %s fail.", _srv_info->ip);
		return 0;
	}
	/*Login*/
	if ( ftp_login(_srv_info)==0 )
	{
		LOG_MSG( "Login %s fail.", _srv_info->ip);
		return 0;
	}   
	/*Change FTP Dir*/
	if ( ftp_cd(_tmp_ftp_path)==0 )
	{
		LOG_MSG( "ftp cd [%s] command fail.", _tmp_ftp_path );
               	if( ftp_mkdir(_tmp_ftp_path) == 0 ) 
		{
		    LOG_MSG( "ftp XMKD [%s] command fail", _tmp_ftp_path);
			return 0; 
		}
	}
       if ( ftp_cd(_tmp_ftp_path)==0 )
       {
           LOG_MSG( "reftp cd [%s] command fail.", _tmp_ftp_path );
	   return 0; 
       }
	/*Change Local Dir*/
	if ( ftp_lcd(_tmp_local_path)==0 )
	{
		LOG_MSG( "ftp lcd [%s] command fail.", _tmp_local_path);
		return 0;
	}
	
	/*Second Scan Local*/
	_second=get_local_file_list(permanent_pool, _tmp_local_path);
	/*Get Ftp List*/
	_ftp_list=get_ftp_file_list(permanent_pool, __item, _tmp_ftp_path);
	/*Loop Search List*/
	while( _second!=NULL)
	{
		/*Is Dir Not*/
		if ( _second->dir )
		{
			_second=_second->next;
			continue;
		}   
		
		/*Size Is Changeing Not*/
		if ( if_in_list_exist(_first, _second)==0 )
		{
			_second=_second->next;
			continue;
		}

		/*Put Ftp*/
		if ( if_in_list_exist(_ftp_list, _second))
		{
			_second=_second->next;
			continue;
		}	
		
		if(time(0) - _second->tModifyTime <= 60)
		{
			//�ļ�����1����ǰ������ϵģ���������֤�ļ����Ѿ�д��ȫ������
			LOG_DBG("file [%s] modifytime is [%d],ret=[%d]<=60 sec, delay to put",_second->file,_second->tModifyTime,time(0) - _second->tModifyTime);
			_second=_second->next;
			continue;
		}
		
		/*Name Equ*/
		if ( !cmp_reg_exp(_tmp_file_name, _second->file) )
		{
			_second=_second->next;
			continue;
		}
		if(read_ftped_file(__item->sch_item.id, _second->file, _second->size, FTP_PUT) != 0)
		{
			//�Ѿ�ftp���ˣ�����ftp
			_second=_second->next;
			continue;
		}

		/*Size=0 Not*/
		/*if(_second->size == 0)
		{
			LOG_MSG("file [%s] size is zero��delay to put!", _second->file);
			_second=_second->next;
			continue;
		}*/
		
		if (time(0) - _second->tModifyTime >= __item->file_ctime_escape)
		{			
			LOG_MSG("fle [%s] modify time [%s]��old than setting time [%d sec] and cann't put!", _second->file,GetTime(_second->tModifyTime), __item->file_ctime_escape);
			if ( __item->del_src== 1 )
			{ 
				char _dir[NAME_MAX+1];
				memset( _dir, 0, sizeof(_dir));
				sprintf(_dir, "%s/%s", _tmp_local_path, _second->file );
				remove(_dir);
			}
			_second=_second->next;
			continue;
		}
		
		if ( __item->del_src==2 )
		{
			//��ɾ��ԭʼ�ˣ������ɼ�
			memset( _dir, 0, sizeof(_dir));
			sprintf(_dir, "%s/%s", _tmp_local_path, _second->file );			
			remove(_dir);
			LOG_MSG("remove file [%s] error:%s",_dir);

			_second=_second->next;
			continue;
		}

			
		/*If Ini FILE'S USE_TMP=0,then not use temp file name*/
		memset(_tmp_remote, 0, sizeof(_tmp_remote));
		if ( __item->use_tmp == 1 )
		{
			sprintf( _tmp_remote, "tmp_%s", _second->file);
		}
		else
		{
			sprintf( _tmp_remote, "%s", _second->file);
		}
		if ( ftp_put(_second->file, _tmp_remote)==0 )
		{
			_ret=0;
		}
		else
		{
			struct tm m;
			if ( __item->use_tmp == 1 )
				/*rename the temp file*/
				ftp_rename(_tmp_remote, _second->file);
			
			localtime_r(&_second->tModifyTime,&m);
			strftime(_file_modify_time, sizeof(_file_modify_time),"%Y-%m-%d %H:%M:%S",&m);
			memset( _dir, 0, sizeof(_dir));
			sprintf(_dir, "%s/%s", _tmp_local_path, _second->file );
			nFileRows = GetFileRows(_dir);			
			/*If Del Src*/
			if ( __item->del_src==1)
			{                
				remove(_dir);
			}
			write_ftped_file(__item->sch_item.id,_second->file, _second->size,FTP_PUT);

			/*����ID|������IP|������·��|����·��|�ļ���|�ļ���С(�ֽ�)|�ļ�����|�ļ�����ʱ��|�ϴ����ر�� */
			log_msg( _tmp_file_list, LOG_BY_HOUR, "|%d|%s|%s|%s|%s|%d|%d|%s|1|", \
				__item->sch_item.id, _srv_info->ip, __item->ftp_path, __item->local_path,_second->file, _second->size,nFileRows,_file_modify_time);
			if(__item->generate_db_cdr)
			{
			        ++g_file_count;	
                                TMSG msg;
				memset(&msg,0,sizeof(msg));
				msg.mtype = MSG_CMD_SQL;
				sprintf(msg._cmd_buf, "insert into tb_log_filecollection (Program_id,FtpParm_id,File_name,File_size,File_rows,File_modify_time,Handle_time) values (%d,%d,'%s',%ld,%d,to_date('%s','yyyy-mm-dd hh24:mi:ss'),sysdate)"\
					,(int)g_pid,__item->sch_item.id,_second->file, _second->size,nFileRows,_file_modify_time);
				if(msgsnd(_sch_internal_queue_id,&msg,sizeof(msg)-sizeof(long),0)<0)
				{
					LOG_MSG("msgsnd [%s] error:%s",msg._cmd_buf,strerror(errno));
				}
			}

		}
		_second=_second->next;
	}
	/*Quit*/
	ftp_quit();
	/*Exec Sh*/
	if ( strcmp(__item->sh_file,"NO")!=0 )
	{
		if  ( system(__item->sh_file)==-1  )
		{
			LOG_MSG( "Exec Shell File Error:%s", strerror(errno));
			_ret=0;
		}
	}

	return _ret;
}

static int ftp_get_task( struct TASK_ITEM* __item)
{
	struct FTP_SERVER_INFO *_srv_info;
	FTP_FILE_INFO *_first, *_second;
	char _local[256] = "";
	char _tmp_local[256] = "";
	char _renamed_file[256] = ""; //���غ��޸ĵ��ļ���
	char _tmp_local_path[256] = "";
	char _tmp_ftp_path[256] = "";
	char _tmp_file_name[256] = "";
	/*2005-04-28 ADD BEGIN*/
	char _tmp_file_list[256] = "";
	/*2005-04-28 ADD END*/   
	int _ret;
	//printf("ftp_get_task pid=%d, and sleep 30\n", getpid());
	//sleep(30);
	/*2005-04-28 ADD BEGIN*/
	memset(_tmp_file_list, 0, sizeof(_tmp_file_list));
	sprintf(_tmp_file_list,  "%s/filelist/%s_%d_", EXEC_PATH,  FILE_LIST_LOG, __item->sch_item.id);
	/*2005-04-28 ADD END*/

	/*Trans Path String*/
	trans_path_str( __item->local_path,  _tmp_local_path, sizeof( _tmp_local_path ) );
	trans_path_str( __item->ftp_path,  _tmp_ftp_path, sizeof( _tmp_ftp_path ) );
	/*Trans File Name String*/
	trans_path_str( __item->file,  _tmp_file_name, sizeof( _tmp_file_name )  );
	/*If Local Not Exist, Create it*/
	if ( if_file_exist(_tmp_local_path) )
	{
		/*Dir Or File Exist*/
		if ( if_isdir(_tmp_local_path)==0 )
		{
			LOG_MSG( "The local dir %s file, not is dir.", _tmp_local_path);
			return 0;
		}
	}
	else
	{
		/*Create Local Dir*/
		if ( mkdir( _tmp_local_path, 0755 )!=0 )
		{   
			LOG_MSG( "Create Son Dir: %s Fail, Error %s.", _tmp_local_path, strerror(errno) );
			return 0;
		} 
	}

	/*Get Ftp Info*/
	_srv_info=get_ftp_info(__item);
	if(NULL == _srv_info)
	{
		LOG_MSG( "get_ftp_info fail.");
		return 0;
	}
	/*Connect*/
	if ( ftp_conn(_srv_info)==0 )
	{
		LOG_MSG( "Connect %s fail.", _srv_info->ip);
		return 0;
	}
	/*Login*/
	if ( ftp_login(_srv_info)==0 )
	{
		LOG_MSG( "Login %s fail.", _srv_info->ip);
		return 0;
	}
	/*Change FTP Dir*/
	if ( ftp_cd(_tmp_ftp_path)==0 )
	{
		LOG_MSG( "ftp cd [%s] command fail.", _tmp_ftp_path );
		return 0; 
	}
	/*Change Local Dir*/
	if ( ftp_lcd(_tmp_local_path)==0 )
	{
		LOG_MSG( "ftp lcd [%s] command fail.", _tmp_local_path);
		return 0;
	}
	/*++++make sure changing safe+++*/ 
	/*First Scan*/
	_first=get_ftp_file_list(permanent_pool, __item, _tmp_ftp_path);
	/*Second Scan*/
	while(sleep(10));
	_second=get_ftp_file_list(permanent_pool,__item,  _tmp_ftp_path);   

	/*++++make sure file and down+++++*/    
	_ret=1;
	/*Loop Search List*/
	while(_second!=NULL)
	{
		if ( _second->dir )
		{
			_second=_second->next;
			continue;
		}

		LOG_DBG("begin to get file [%s],size [%d].", _second->file, _second->size);
		if(strncmp(_second->file,"HI",2) == 0 && _second->file[8] == '.') 
		{
			//BSS�޸��ļ�����Ļ�����ȥ��ǰ��BSS���ӵ���HIIBGP01.����
			strncpy(_renamed_file,_second->file + 9, sizeof(_renamed_file));
		}
		else
		{
			strncpy(_renamed_file,_second->file, sizeof(_renamed_file));
		}

		/*File Name Not Equ*/
		if ( cmp_reg_exp(_tmp_file_name, _second->file) == 0 && cmp_reg_exp(_tmp_file_name, _renamed_file) == 0)
		{
			LOG_DBG("file [%s] regmatch error,skip get", _second->file);
			_second=_second->next;
			continue;
		}

		/*file older,not get*/
		if (time(0) - _second->tModifyTime >= __item->file_ctime_escape)
		{
   		    LOG_MSG("fle [%s] modify time [%s],old than setting time [%d sec] and need not get.", _second->file,GetTime(_second->tModifyTime), __item->file_ctime_escape);
			if ( __item->del_src == 1 )
			{	
				ftp_del(_second->file);
			}
			_second=_second->next;
			continue;
		}
		
		if ( __item->del_src==2 )
		{
			//��ɾ��ԭʼ�ˣ������ɼ�
			ftp_del(_second->file);	
			_second=_second->next;
			continue;
		}

		//file already ftp,donn't ftp again.
		if(read_ftped_file(__item->sch_item.id, _renamed_file, _second->size, FTP_GET)!= 0)
		{			
			_second = _second->next;
			continue;
		}

		/*+++++++make sure file not download again++++++*/ 
		memset(_local, 0, sizeof(_local));
		sprintf(_local,"%s/%s", _tmp_local_path, _renamed_file);
		/*Not Get Is Exits*/
		if ( if_file_exist(_local) )
		{
			if ( _second->size==get_file_size(_local) )
			{
				LOG_MSG("file [%s] size[%d] already exists,need not ftp",_local, _second->size);
				_second=_second->next;
				continue;
			}
		}
		
		/*Not Get Size Is Changing*/
		if ( if_in_list_exist(_first, _second)==0 )
		{
			_second=_second->next;
			continue;
		}


		/*Get File*/
		if ( __item->use_tmp==1 )
		{
			sprintf(_tmp_local,"%s/temp__%s", _tmp_local_path, _renamed_file);
		}
		else
		{
			strncpy(_tmp_local, _local, sizeof(_tmp_local));
		}
		if ( ftp_get(_tmp_local, _second->file)==0 )
		{
			_ret=0;
		}
		else
		{
			char _dir[NAME_MAX+1];
			char _file_modify_time[32]="";
			int nFileRows = 0;
			struct tm m;
			LOG_DBG("end get file [%s].", _second->file);
			
			if ( __item->del_src==1 )
				ftp_del(_second->file);		
			/*Download Finnaly And Rename.*/
			if ( __item->use_tmp==1 )
				rename(_tmp_local, _local);
			
			/*If Move To Son Dir*/
			if ( strcmp(__item->son_dir_mask, "NULL")!=0 )
			{
				char _direction[8];
				char _start[8];
				char _len[8];
				char _file[NAME_MAX+1];

				memset(_direction, 0, sizeof(_direction));
				memset(_start, 0, sizeof(_start));
				memset(_len, 0, sizeof(_len));
				memset(_dir, 0, sizeof(_dir));
				if(sscanf(__item->son_dir_mask, "%[^-]-%[^-]-%[^-]", _direction, _start, _len ) !=3)
				{
					LOG_MSG( "son_dir_mask format error: %s ", __item->son_dir_mask);
					goto skipmvson;

				}
				sub_string(_renamed_file, atoi(_direction), atoi(_start), atoi(_len), _file, NAME_MAX+1);
				sprintf(_dir, "%s/%s", _tmp_local_path, _file );

				if ( strlen(_dir)>0 )
				{
					if ( if_file_exist(_dir) )
					{
						/*Dir Or File Exist*/
						if ( if_isdir(_dir)==0 )
						{
							LOG_MSG( "Son Dir Same File: %s Exists.", _dir );
							goto skipmvson;
						}
					}
					else
					{
						/*Create Dir*/
						if ( mkdir( _dir, 0755 )!=0 )
						{   
							LOG_MSG( "Create Son Dir: %s Fail, Error %s.", _dir, strerror(errno) );
							goto skipmvson;
						} 
					}
					/*Mv File To Son Dir*/
					memset(_file, 0, sizeof(_file));
					sprintf(_dir, "%s/%s", _dir, _renamed_file);
					if ( if_file_exist(_dir)!=0 )
					{
						LOG_MSG( "Rename to SubDir File:%s Already Exists.", _dir );
						goto skipmvson;
					}
					if ( rename(_local, _dir) != 0 )
					{
						LOG_MSG( "Move File:%s To %s Fail, Error %s.", _local, _dir, strerror(errno) );
					}
					else
					{
						strncpy(_local, _dir, sizeof(_local));
					}
				}
				
			}
skipmvson:		
			if(strcmp(_second->file, _renamed_file) != 0)
			{
				LOG_MSG("BSS remote file [%s] has getted and rename to local file [%s].", _second->file, _renamed_file);
			}

			cp_item_to_dir(__item->cp_dir_1, _renamed_file, _local, __item->copy_flag);
			cp_item_to_dir(__item->cp_dir_2, _renamed_file, _local, __item->copy_flag); 
			cp_item_to_dir(__item->cp_dir_3, _renamed_file, _local, __item->copy_flag);
			cp_item_to_dir(__item->cp_dir_4, _renamed_file, _local, __item->copy_flag);			
			
			nFileRows = GetFileRows(_local);
			write_ftped_file(__item->sch_item.id,_renamed_file, _second->size,FTP_GET);			
			/*����ID|������IP|������·��|����·��|�ļ���|�ļ���С(�ֽ�)|�ļ�����|�ļ�����ʱ��|�ϴ����ر�� */
			localtime_r(&_second->tModifyTime,&m);	
			strftime(_file_modify_time, sizeof(_file_modify_time),"%Y-%m-%d %H:%M:%S",&m);
			log_msg( _tmp_file_list, LOG_BY_HOUR, "|%d|%s|%s|%s|%s|%d|%d|%s|1|", \
				__item->sch_item.id, _srv_info->ip, __item->ftp_path, __item->local_path, _renamed_file, _second->size,nFileRows,_file_modify_time);
			if(__item->generate_db_cdr == 1)
			{
				/*����־д�����ݿ���*/
                                ++g_file_count; 
				TMSG msg;
				memset(&msg,0,sizeof(msg));
				msg.mtype = MSG_CMD_SQL;
				sprintf(msg._cmd_buf, "insert into tb_log_filecollection (Program_id,FtpParm_id,File_name,File_size,File_rows,File_modify_time,Handle_time) values (%d,%d,'%s',%ld,%d,to_date('%s','yyyy-mm-dd hh24:mi:ss'),sysdate)"\
					,(int)g_pid,__item->sch_item.id,_renamed_file, _second->size,nFileRows,_file_modify_time);
				if(msgsnd(_sch_internal_queue_id,&msg,sizeof(msg)-sizeof(long),0)<0)
				{
					LOG_MSG("msgsnd [%s] error:%s",msg._cmd_buf,strerror(errno));
				}
			}
		}
		_second=_second->next;
	}

	/*Quit*/
	ftp_quit();

	if ( strcmp(__item->sh_file,"NO")!=0 )
	{
		if  ( system(__item->sh_file)==-1  )
		{       
			LOG_MSG( "Exec Shell File Error:%s", strerror(errno));
			_ret=0; 
		}       
	}

	return _ret;
}

static int sftp_put_task( struct TASK_ITEM* __item)
{
    struct FTP_SERVER_INFO *_srv_info;
    FTP_FILE_INFO *_first, *_second, *_ftp_list;
    char _tmp_remote[128];
    char local_path[256] = {0};
    char _tmp_local_path[256];
    char _tmp_ftp_path[256];
    char _tmp_ftp_target[256] = {0};
    char _tmp_file_name[128];
    /*2005-04-28 ADD BEGIN*/
    char _tmp_file_list[128];
    char _dir[NAME_MAX+1]="";
    char _file_modify_time[32]="";
    int nFileRows = 0;
    /*2005-04-28 ADD END*/
    int _ret=1;
    //printf("ftp_put_task pid=%d, and sleep 10\n", getpid());
    //sleep(30);

    /*2005-04-28 ADD BEGIN*/
    memset(_tmp_file_list, 0, sizeof(_tmp_file_list));
    sprintf(_tmp_file_list, "%s/filelist/%s_%d_", EXEC_PATH, FILE_LIST_LOG, __item->sch_item.id);
    /*2005-04-28 ADD END*/

    /*Trans Path String*/
    trans_path_str( __item->local_path,  _tmp_local_path, sizeof( _tmp_local_path ) );
    trans_path_str( __item->ftp_path,  _tmp_ftp_path, sizeof( _tmp_ftp_path ) );
    /*Trans File Name String*/
    trans_path_str( __item->file,  _tmp_file_name, sizeof( _tmp_file_name )  );


    /*If Local Not Exist Exit*/
    if ( if_file_exist(_tmp_local_path) )
    {
        /*Dir Or File Exist*/
        if ( if_isdir(_tmp_local_path)==0 )
        {
            LOG_MSG( "The local dir %s file, not is dir.", _tmp_local_path);
            return 0;
        }
    }
    else
        return 0;

    /*First Scan Local add by ganhh 2007_03_08*/
    _first=get_local_file_list(permanent_pool, _tmp_local_path);

    if(_first == NULL)
    {
        LOG_MSG( "path:%s no file exist to ftp put.", _tmp_local_path);
        return 1;
    }
    while(sleep(10));
    /*Get Ftp Info*/
    _srv_info=get_ftp_info(__item);
    if(NULL == _srv_info)
    {
        LOG_MSG( "get_ftp_info fail.");
        return 0;
    }

    SFTP sftp(_srv_info->ip,_srv_info->port);
    //��¼
    if ( !sftp.openSession(_srv_info->user,_srv_info->pass) )
    {
        LOG_MSG( "Connect %s fail.", _srv_info->ip);
        return 0;
    }

    if ( !sftp.exists(_tmp_ftp_path) )
    {
        LOG_MSG( "sftp cd [%s] command fail.", _tmp_ftp_path );
        if( !sftp.mkdir(_tmp_ftp_path) )
        {
            LOG_MSG( "sftp mkdir [%s] command fail", _tmp_ftp_path);
            return 0;
        }
    }

    /*Second Scan Local*/
    _second=get_local_file_list(permanent_pool, _tmp_local_path);
    /*Get Ftp List*/
    _ftp_list=get_sftp_file_list(permanent_pool, &sftp, __item, _tmp_ftp_path);
    /*Loop Search List*/
    while( _second!=NULL)
    {
        /*Is Dir Not*/
        if ( _second->dir )
        {
            _second=_second->next;
            continue;
        }

        /*Size Is Changeing Not*/
        if ( if_in_list_exist(_first, _second)==0 )
        {
            _second=_second->next;
            continue;
        }

        /*Put Ftp*/
        if ( if_in_list_exist(_ftp_list, _second))
        {
            _second=_second->next;
            continue;
        }

        if(time(0) - _second->tModifyTime <= 60)
        {
            //�ļ�����1����ǰ������ϵģ���������֤�ļ����Ѿ�д��ȫ������
            LOG_DBG("file [%s] modifytime is [%d],ret=[%d]<=60 sec, delay to put",_second->file,_second->tModifyTime,time(0) - _second->tModifyTime);
            _second=_second->next;
            continue;
        }

        /*Name Equ*/
        if ( !cmp_reg_exp(_tmp_file_name, _second->file) )
        {
            _second=_second->next;
            continue;
        }
        if(read_ftped_file(__item->sch_item.id, _second->file, _second->size, FTP_PUT) != 0)
        {
            //�Ѿ�ftp���ˣ�����ftp
            _second=_second->next;
            continue;
        }

        /*Size=0 Not*/
        /*if(_second->size == 0)
        {
            LOG_MSG("file [%s] size is zero��delay to put!", _second->file);
            _second=_second->next;
            continue;
        }*/

        if (time(0) - _second->tModifyTime >= __item->file_ctime_escape)
        {
            LOG_MSG("fle [%s] modify time [%s]��old than setting time [%d sec] and cann't put!", _second->file,GetTime(_second->tModifyTime), __item->file_ctime_escape);
            if ( __item->del_src== 1 )
            {
                char _dir[NAME_MAX+1];
                memset( _dir, 0, sizeof(_dir));
                sprintf(_dir, "%s/%s", _tmp_local_path, _second->file );
                remove(_dir);
            }
            _second=_second->next;
            continue;
        }

        if ( __item->del_src==2 )
        {
            //��ɾ��ԭʼ�ˣ������ɼ�
            memset( _dir, 0, sizeof(_dir));
            sprintf(_dir, "%s/%s", _tmp_local_path, _second->file );
            remove(_dir);
            LOG_MSG("remove file [%s] error:%s",_dir);

            _second=_second->next;
            continue;
        }


        /*If Ini FILE'S USE_TMP=0,then not use temp file name*/
        memset(_tmp_remote, 0, sizeof(_tmp_remote));
        if ( __item->use_tmp == 1 )
        {
            sprintf( _tmp_remote, "%s/tmp_%s", _tmp_ftp_path, _second->file);
        }
        else
        {
            sprintf( _tmp_remote, "%s/%s", _tmp_ftp_path, _second->file);
        }
        memset(local_path,0, sizeof(local_path));
        sprintf(local_path, "%s/%s", _tmp_local_path, _second->file );
        if ( sftp.sftp_put(local_path, _tmp_remote)==0 )
        {
            _ret=0;
        }
        else
        {
            struct tm m;
            if ( __item->use_tmp == 1 )
            {
                memset(_tmp_ftp_target,0, sizeof(_tmp_ftp_target));
                sprintf( _tmp_ftp_target, "%s/%s", _tmp_ftp_path, _second->file);
                /*rename the temp file*/
                sftp.rename(_tmp_remote, _tmp_ftp_target);
            }

            localtime_r(&_second->tModifyTime,&m);
            strftime(_file_modify_time, sizeof(_file_modify_time),"%Y-%m-%d %H:%M:%S",&m);
            memset( _dir, 0, sizeof(_dir));
            sprintf(_dir, "%s/%s", _tmp_local_path, _second->file );
            nFileRows = GetFileRows(_dir);
            /*If Del Src*/
            if ( __item->del_src==1)
            {
                remove(_dir);
            }
            write_ftped_file(__item->sch_item.id,_second->file, _second->size,FTP_PUT);

            /*����ID|������IP|������·��|����·��|�ļ���|�ļ���С(�ֽ�)|�ļ�����|�ļ�����ʱ��|�ϴ����ر�� */
            log_msg( _tmp_file_list, LOG_BY_HOUR, "|%d|%s|%s|%s|%s|%d|%d|%s|1|", \
				__item->sch_item.id, _srv_info->ip, __item->ftp_path, __item->local_path,_second->file, _second->size,nFileRows,_file_modify_time);
            if(__item->generate_db_cdr)
            {
                ++g_file_count;
                TMSG msg;
                memset(&msg,0,sizeof(msg));
                msg.mtype = MSG_CMD_SQL;
                sprintf(msg._cmd_buf, "insert into tb_log_filecollection (Program_id,FtpParm_id,File_name,File_size,File_rows,File_modify_time,Handle_time) values (%d,%d,'%s',%ld,%d,to_date('%s','yyyy-mm-dd hh24:mi:ss'),sysdate)"\
					,(int)g_pid,__item->sch_item.id,_second->file, _second->size,nFileRows,_file_modify_time);
                if(msgsnd(_sch_internal_queue_id,&msg,sizeof(msg)-sizeof(long),0)<0)
                {
                    LOG_MSG("msgsnd [%s] error:%s",msg._cmd_buf,strerror(errno));
                }
            }

        }
        _second=_second->next;
    }

    /*Quit*/
    sftp.closeSession();

    /*Exec Sh*/
    if ( strcmp(__item->sh_file,"NO")!=0 )
    {
        if  ( system(__item->sh_file)==-1  )
        {
            LOG_MSG( "Exec Shell File Error:%s", strerror(errno));
            _ret=0;
        }
    }

    return _ret;
}

static int sftp_get_task( struct TASK_ITEM* __item)
{
    struct FTP_SERVER_INFO *_srv_info;
    FTP_FILE_INFO *_first, *_second;
    char _local[256] = "";
    char _tmp_local[256] = "";
    char _renamed_file[256] = ""; //���غ��޸ĵ��ļ���
    char _tmp_local_path[256] = "";
    char ftp_path[256] = {0};
    char _tmp_ftp_path[256] = "";
    char _tmp_file_name[256] = "";
    /*2005-04-28 ADD BEGIN*/
    char _tmp_file_list[256] = "";
    /*2005-04-28 ADD END*/
    int _ret;
    //printf("ftp_get_task pid=%d, and sleep 30\n", getpid());
    //sleep(30);
    /*2005-04-28 ADD BEGIN*/
    memset(_tmp_file_list, 0, sizeof(_tmp_file_list));
    sprintf(_tmp_file_list,  "%s/filelist/%s_%d_", EXEC_PATH,  FILE_LIST_LOG, __item->sch_item.id);
    /*2005-04-28 ADD END*/

    /*Trans Path String*/
    trans_path_str( __item->local_path,  _tmp_local_path, sizeof( _tmp_local_path ) );
    trans_path_str( __item->ftp_path,  _tmp_ftp_path, sizeof( _tmp_ftp_path ) );
    /*Trans File Name String*/
    trans_path_str( __item->file,  _tmp_file_name, sizeof( _tmp_file_name )  );
    /*If Local Not Exist, Create it*/
    if ( if_file_exist(_tmp_local_path) )
    {
        /*Dir Or File Exist*/
        if ( if_isdir(_tmp_local_path)==0 )
        {
            LOG_MSG( "The local dir %s file, not is dir.", _tmp_local_path);
            return 0;
        }
    }
    else
    {
        /*Create Local Dir*/
        if ( mkdir( _tmp_local_path, 0755 )!=0 )
        {
            LOG_MSG( "Create Son Dir: %s Fail, Error %s.", _tmp_local_path, strerror(errno) );
            return 0;
        }
    }

    /*Get Ftp Info*/
    _srv_info=get_ftp_info(__item);
    if(NULL == _srv_info)
    {
        LOG_MSG( "get_ftp_info fail.");
        return 0;
    }

    SFTP sftp(_srv_info->ip,_srv_info->port);
    //��¼
    if ( !sftp.openSession(_srv_info->user,_srv_info->pass) )
    {
        LOG_MSG( "Connect %s fail.", _srv_info->ip);
        return 0;
    }

    /*First Scan*/
    _first=get_sftp_file_list(permanent_pool, &sftp, __item, _tmp_ftp_path);
    /*Second Scan*/
    while(sleep(10));
    _second=get_sftp_file_list(permanent_pool, &sftp, __item, _tmp_ftp_path);

    /*++++make sure file and down+++++*/
    _ret=1;
    /*Loop Search List*/
    while(_second!=NULL)
    {
        if ( _second->dir )
        {
            _second=_second->next;
            continue;
        }

        LOG_DBG("begin to get file [%s],size [%d].", _second->file, _second->size);

        strncpy(_renamed_file,_second->file, sizeof(_renamed_file));

        /*File Name Not Equ*/
        if ( cmp_reg_exp(_tmp_file_name, _second->file) == 0 )
        {
            LOG_DBG("file [%s] regmatch error,skip get.tmp_file_name[%s]", _second->file,_tmp_file_name);
            _second=_second->next;
            continue;
        }

        /*file older,not get*/
        if (time(0) - _second->tModifyTime >= __item->file_ctime_escape)
        {
            LOG_MSG("file [%s] modify time [%s],old than setting time [%d sec] and need not get.", _second->file,GetTime(_second->tModifyTime), __item->file_ctime_escape);
            if ( __item->del_src == 1 )
            {
                sftp.rm(_second->file);
            }
            _second=_second->next;
            continue;
        }

        if ( __item->del_src == 2 )
        {
            //��ɾ��ԭʼ�ˣ������ɼ�
            sftp.rm(_second->file);
            _second=_second->next;
            continue;
        }

        //file already ftp,donn't ftp again.
        if(read_ftped_file(__item->sch_item.id, _renamed_file, _second->size, FTP_GET)!= 0)
        {
            _second = _second->next;
            continue;
        }

        /*+++++++make sure file not download again++++++*/
        memset(_local, 0, sizeof(_local));
        sprintf(_local,"%s/%s", _tmp_local_path, _renamed_file);
        /*Not Get Is Exits*/
        if ( if_file_exist(_local) )
        {
            if ( _second->size==get_file_size(_local) )
            {
                LOG_MSG("file [%s] size[%d] already exists,need not ftp",_local, _second->size);
                _second=_second->next;
                continue;
            }
        }

        /*Not Get Size Is Changing*/
        if ( if_in_list_exist(_first, _second)==0 )
        {
            _second=_second->next;
            continue;
        }


        /*Get File*/
        if ( __item->use_tmp==1 )
        {
            sprintf(_tmp_local,"%s/temp__%s", _tmp_local_path, _renamed_file);
        }
        else
        {
            strncpy(_tmp_local, _local, sizeof(_tmp_local));
        }
        memset(ftp_path, 0, sizeof(ftp_path));
        sprintf(ftp_path,"%s/%s", _tmp_ftp_path, _second->file);
        if ( sftp.sftp_get(_tmp_local, ftp_path) == 0 )
        {
            _ret=0;
        }
        else
        {
            char _dir[NAME_MAX+1];
            char _file_modify_time[32]="";
            int nFileRows = 0;
            struct tm m;
            LOG_DBG("end get file [%s].", _second->file);

            if ( __item->del_src==1 )
                sftp.rm(_second->file);
            /*Download Finnaly And Rename.*/
            if ( __item->use_tmp==1 )
                rename(_tmp_local, _local);

            /*If Move To Son Dir*/
            if ( strcmp(__item->son_dir_mask, "NULL")!=0 )
            {
                char _direction[8];
                char _start[8];
                char _len[8];
                char _file[NAME_MAX+1];

                memset(_direction, 0, sizeof(_direction));
                memset(_start, 0, sizeof(_start));
                memset(_len, 0, sizeof(_len));
                memset(_dir, 0, sizeof(_dir));
                if(sscanf(__item->son_dir_mask, "%[^-]-%[^-]-%[^-]", _direction, _start, _len ) !=3)
                {
                    LOG_MSG( "son_dir_mask format error: %s ", __item->son_dir_mask);
                    goto skipmvson;

                }
                sub_string(_renamed_file, atoi(_direction), atoi(_start), atoi(_len), _file, NAME_MAX+1);
                sprintf(_dir, "%s/%s", _tmp_local_path, _file );

                if ( strlen(_dir)>0 )
                {
                    if ( if_file_exist(_dir) )
                    {
                        /*Dir Or File Exist*/
                        if ( if_isdir(_dir)==0 )
                        {
                            LOG_MSG( "Son Dir Same File: %s Exists.", _dir );
                            goto skipmvson;
                        }
                    }
                    else
                    {
                        /*Create Dir*/
                        if ( mkdir( _dir, 0755 )!=0 )
                        {
                            LOG_MSG( "Create Son Dir: %s Fail, Error %s.", _dir, strerror(errno) );
                            goto skipmvson;
                        }
                    }
                    /*Mv File To Son Dir*/
                    memset(_file, 0, sizeof(_file));
                    sprintf(_dir, "%s/%s", _dir, _renamed_file);
                    if ( if_file_exist(_dir)!=0 )
                    {
                        LOG_MSG( "Rename to SubDir File:%s Already Exists.", _dir );
                        goto skipmvson;
                    }
                    if ( rename(_local, _dir) != 0 )
                    {
                        LOG_MSG( "Move File:%s To %s Fail, Error %s.", _local, _dir, strerror(errno) );
                    }
                    else
                    {
                        strncpy(_local, _dir, sizeof(_local));
                    }
                }

            }
            skipmvson:
            if(strcmp(_second->file, _renamed_file) != 0)
            {
                LOG_MSG("BSS remote file [%s] has getted and rename to local file [%s].", _second->file, _renamed_file);
            }

            cp_item_to_dir(__item->cp_dir_1, _renamed_file, _local, __item->copy_flag);
            cp_item_to_dir(__item->cp_dir_2, _renamed_file, _local, __item->copy_flag);
            cp_item_to_dir(__item->cp_dir_3, _renamed_file, _local, __item->copy_flag);
            cp_item_to_dir(__item->cp_dir_4, _renamed_file, _local, __item->copy_flag);

            nFileRows = GetFileRows(_local);
            write_ftped_file(__item->sch_item.id,_renamed_file, _second->size,FTP_GET);
            /*����ID|������IP|������·��|����·��|�ļ���|�ļ���С(�ֽ�)|�ļ�����|�ļ�����ʱ��|�ϴ����ر�� */
            localtime_r(&_second->tModifyTime,&m);
            strftime(_file_modify_time, sizeof(_file_modify_time),"%Y-%m-%d %H:%M:%S",&m);
            log_msg( _tmp_file_list, LOG_BY_HOUR, "|%d|%s|%s|%s|%s|%d|%d|%s|1|", \
				__item->sch_item.id, _srv_info->ip, __item->ftp_path, __item->local_path, _renamed_file, _second->size,nFileRows,_file_modify_time);
            if(__item->generate_db_cdr == 1)
            {
                /*����־д�����ݿ���*/
                ++g_file_count;
                TMSG msg;
                memset(&msg,0,sizeof(msg));
                msg.mtype = MSG_CMD_SQL;
                sprintf(msg._cmd_buf, "insert into tb_log_filecollection (Program_id,FtpParm_id,File_name,File_size,File_rows,File_modify_time,Handle_time) values (%d,%d,'%s',%ld,%d,to_date('%s','yyyy-mm-dd hh24:mi:ss'),sysdate)"\
					,(int)g_pid,__item->sch_item.id,_renamed_file, _second->size,nFileRows,_file_modify_time);
                if(msgsnd(_sch_internal_queue_id,&msg,sizeof(msg)-sizeof(long),0)<0)
                {
                    LOG_MSG("msgsnd [%s] error:%s",msg._cmd_buf,strerror(errno));
                }
            }
        }
        _second=_second->next;
    }

    /*Quit*/
    sftp.closeSession();

    if ( strcmp(__item->sh_file,"NO")!=0 )
    {
        if  ( system(__item->sh_file)==-1  )
        {
            LOG_MSG( "Exec Shell File Error:%s", strerror(errno));
            _ret=0;
        }
    }

    return _ret;
}


void start_task( int __id )
{
	TASK_ITEM* _cur;
	int _ret=0;

	/*
	*������־�ļ�Ϊ��������ID����־
	*/
	memset(g_log_file, 0, NAME_MAX+1);
	sprintf(g_log_file, "%s/log/task_%d_log", EXEC_PATH, __id);
	//g_pFileSem= new tSystemVSemaphore(false, CHILDPID_OPERATE_FILE_SEMAPHORE); 
	//g_pFileSem->create();

	_cur=get_task_item_by_id(__id);

	if (_cur==NULL)
	{
		LOG_MSG( "Not Find ID:%d's Task, Please Restart Program.", __id );
		return;
	}
        LOG_MSG("%s ",_cur->ftp_path);
	/*
	*���ݴ��ͷ�����ȷ�����õķ���
	*/
       	g_file_count =0;
	switch(_cur->direction)
	{
	case FTP_GET:
		LOG_MSG( "==PID:%d Start FTP GET Task ID:%d=======================================",(int)getpid(), __id);
		_ret=ftp_get_task(_cur);
		LOG_MSG( "==PID:%d Stop FTP GET Task ID:%d++++++++++++++++++++++++++++++++++++++++", (int)getpid(),__id);
		break;
	case FTP_PUT:
		LOG_MSG( "==PID:%d Start FTP PUT Task ID:%d=======================================", (int)getpid(),__id);
		_ret=ftp_put_task(_cur);
		LOG_MSG( "==PID:%d Stop FTP PUT Task ID:%d++++++++++++++++++++++++++++++++++++++++", (int)getpid(),__id);
		break;
    case SFTP_GET:
        LOG_MSG( "==PID:%d Start SFTP GET Task ID:%d=======================================",(int)getpid(), __id);
        _ret=sftp_get_task(_cur);
        LOG_MSG( "==PID:%d Stop SFTP GET Task ID:%d++++++++++++++++++++++++++++++++++++++++", (int)getpid(),__id);
        break;
    case SFTP_PUT:
        LOG_MSG( "==PID:%d Start SFTP PUT Task ID:%d=======================================", (int)getpid(),__id);
        _ret=sftp_put_task(_cur);
        LOG_MSG( "==PID:%d Stop SFTP PUT Task ID:%d++++++++++++++++++++++++++++++++++++++++", (int)getpid(),__id);
        break;
	default:
		LOG_MSG( "ID:%d's Task's Direction Can't Recognize., Please Charge Ini File And Restart Program.", __id );
	}
	//delete g_pFileSem;
	/*
	*ȷ���Ƿ񱨾�
	*/
	if ( _ret==0 )
	{
		LOG_MSG( "Alarm!!!!!!, Alarm, Task ID:%d Fail\n", __id );
	}
/* ԭ����ɴ洢��������JOB���ɼ�����ɼ��������JOB״̬���Ȳ�Ʒ�����ó��򲻹ܲɼ�ģ�飬��ע�͵�
    TMSG msg;
	memset(&msg,0,sizeof(msg));
	msg.mtype = MSG_CMD_SQL;
	sprintf(msg._cmd_buf, "UPDATE RASDEV.TB_UC_TASK_SCHED SET STATUS = 'COMPLETE', RESULT = %d, ERROR_CODE = '%s',
            RESULT_DESC = '%s' WHERE OUTER_ID = '%d' AND TO_CHAR(NORMAL_START_TIME,'YYYYMMDDHH24MI') = '%s'",
                    _ret==0?-1:0,"","",_cur->sch_item.id,g_exec_time);
	if(msgsnd(_sch_internal_queue_id,&msg,sizeof(msg)-sizeof(long),0)<0)
	{
		LOG_MSG("msgsnd [%s] error:%s",msg._cmd_buf,strerror(errno));
	}
         LOG_MSG("msgsnd %d ",_sch_internal_queue_id);
*/
	return; 
}

void list_task()
{
	int _i;
	struct TASK_ITEM *_cur;

	_cur=_task_list;

	_i=0;
	printf("--------------------------------------------------------------------------------------\n");
	printf("|TASK ID--MONTH--DAY--HOUR--MIN--FTP HOST--FTP PATH--DIRECTION--DEL SRC--FILE--SH FILE--SON DIR MASK|\n");
	printf("--------------------------------------------------------------------------------------\n");

	while ( _cur )
	{
		printf("%d--%s--%s--%s--%s--%s--%s--%s--%d--%d--%s--%s--%s\n",
		        _cur->sch_item.id,
		        _cur->sch_item.month,
		        _cur->sch_item.day,
		        _cur->sch_item.hour,
		        _cur->sch_item.min,
		        _cur->ftp_host,
		        _cur->ftp_path,
		        _cur->local_path,
		        _cur->direction,
		        _cur->del_src,
		        _cur->file,
		        _cur->sh_file,
		        _cur->son_dir_mask );
		_cur=_cur->next;
		_i++;
	}
	printf("--------------------------------TOTAL [%d] TASK----------------------------------------\n", _i);
	printf("Scheduler Infor: MSG_ID %d, P_ID %d\n",
		read_ini_int(TASK_INI, TASK_CAPTION, "SCHEDULE_MSG_ID", 0),
		read_ini_int(TASK_INI, TASK_CAPTION, "SCHEDULE_P_ID", 0) );

	printf("share memory used:\n%s, map size:%d\n",g_pAlloc->getmeminfo().c_str(), g_pMapFileInfo->size());
	
}

void stop_task()
{
	int _p_id;
	int _msg_id;
	TMSG msg;
	long _cmd;
	char *_cmd_arg;
	
	
	_msg_id=read_ini_int(TASK_INI, TASK_CAPTION, "SCHEDULE_MSG_ID", 0 );
	_p_id=read_ini_int(TASK_INI, TASK_CAPTION, "SCHEDULE_P_ID", 0);

	if ( _p_id<=0 )
	{
		printf("Scheduler Is Already Stop!\n");
		return;
	}

	printf("program [%d] begin stop,please wait...\n", _p_id);
	schedule_stop(_msg_id);	
	//�������̹�ʱ����յ��˳���Ϣ��
	while(sleep(5));
	while(1)
	{
		memset(&msg,0,sizeof(msg));	
		if(msgrcv(_msg_id, &msg, sizeof(msg)-sizeof(long), MSG_CMD_SCHEDULE, IPC_NOWAIT)<=0)
		{
			if ( errno!=ENOMSG )
			{
				break;
			}
			else
			{
				usleep_r(100);
				continue;
			}
		}
		
		_cmd = msg._cmd_buf[0];
		_cmd_arg = msg._cmd_buf+1;
		if(_cmd == _SCH_SON_STOP)
		{
			int id,pid;
			memcpy(&id,_cmd_arg, sizeof(int));
			memcpy(&pid,_cmd_arg+sizeof(int),sizeof(int));
			printf("program [%d] ftp id [%d] success exit.\n",pid,id);
		}
		else 
		{
			printf("msgrcv other cmd [%d].\n",(int)_cmd);
			if(_cmd == _SCH_MAIN_STOP)
				break;
		}
	}

	printf("program [%d] stop.\n",_p_id);
	write_ini_int(TASK_INI, TASK_CAPTION, "SCHEDULE_MSG_ID", 0 );
	write_ini_int(TASK_INI, TASK_CAPTION, "SCHEDULE_P_ID", 0 ); 	
}

void start_db_process(int msg_id)
{
	if(fork()==0)
	{
		connect_oracle(TASK_INI);
		int index = 0;
		while(1)
		{
			TMSG msg;
			memset(&msg,0,sizeof(msg));
			if (msgrcv( msg_id, &msg, sizeof(msg)-sizeof(long), MSG_CMD_SQL, 0)<=0 )
			{	
				LOG_MSG("msgrcv MSG_CMD_SQL error:%s of msgid:%d, write db course exit",strerror(errno),msg_id);
				break;
			}
			bool bCommit = ++index%20 == 0;
			ExecuteSql(msg._cmd_buf, bCommit);
		}
		Commit();
		disconnect_oracle();
		exit(0);
	}
}

