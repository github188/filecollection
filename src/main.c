#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/msg.h>
#include "ftpprocess.h"
#include "schedule.h"
#include "memmanager.h"
#include "taskmanager.h"
#include "dbprocess.h"
#include "iniprocess.h"
#include "frame_memory.h"
#include "definitions.h"
char *EXEC_PATH;
char *TASK_INI;
int  g_file_count;
char  g_exec_time[13];//保存执行时间
char g_db_time[17];   //保存数据库时间。
int debug=0;
pid_t g_pid,mem_id;
key_t g_key; 
int _sch_internal_queue_id = -1;
char *g_log_file;
tFileInfoMap *g_pMapFileInfo;
tSystemVSemaphore *g_pFileSem;
tMemoryAllocator *g_pAlloc;

void init(int argc, char *argv[])
{
	char shm_key_name[128];
	char Shm_Sem_Name[128];
	long shm_address=0;
	long shm_size = 0;
	key_t shmid = 0;
	bool master = false;

	//初始化内存
	init_alloc();
	EXEC_PATH=(char*)pcalloc(permanent_pool, NAME_MAX+1);
	TASK_INI=(char*)pcalloc(permanent_pool, NAME_MAX+1);
	g_log_file=(char*)pcalloc(permanent_pool, NAME_MAX+1);

	//读取相关配置信息
	getcwd(EXEC_PATH, NAME_MAX+1);
	sprintf(TASK_INI, "%s/ini/Task.ini", EXEC_PATH);
	sprintf(g_log_file, "%s/log/sys_log", EXEC_PATH);
	g_pid = mem_id = read_ini_int(TASK_INI,TASK_CAPTION,"PROGRAM_ID",-999);
	if(g_pid == -999)
	{
		printf("not set program id, and it must large than 0.\n");
		exit(0);
	}

	bool bStart = (argc == 2 && strncmp(argv[1],"start", 5) == 0);
	if (bStart)
	{
		//存在,则退出
		if(read_ini_int(TASK_INI, TASK_CAPTION, "SCHEDULE_P_ID", 0) != 0)
		{
			printf("Scheduler Is exist, program need not run.\n");
			exit(0);
		}
	}
	else
	{
		//非启动程序,则使用不同的共享内存ID值。
		mem_id = 10;	
	}
		
	//读取共享内存信息
	g_key = ftok(EXEC_PATH,0);  
               bool bReload = (argc==2 && strncmp(argv[1],"reload",6) == 0);
               if(bReload)
               {
                  int reload_queue_id = msgget(g_key, IPC_CREAT| IPC_EXCL | 0666 );
                  if(reload_queue_id == -1)
                     reload_queue_id = msgget(g_key,0);
                  printf("%d\n",reload_queue_id);
                  if(reload_queue_id>=0)
                  {
                       TMSG msg;
                       memset(&msg,0,sizeof(msg));
                       msg.mtype = MSG_CMD_SCHEDULE;
                       msg._cmd_buf[0] = _SCH_DB_RELOAD;
                       int retn = msgsnd(reload_queue_id, &msg, sizeof(msg)-sizeof(long), 0);
                       printf("%d\n",retn);
                       exit(0);
                  }
                  exit(0);
               }  
    strncpy(shm_key_name,read_ini_string(permanent_pool,TASK_INI,TASK_CAPTION,"SHM_KEY_NAME","shm_filecollection"),sizeof(shm_key_name));
	strncpy(Shm_Sem_Name,read_ini_string(permanent_pool,TASK_INI,TASK_CAPTION,"SHM_SEMAPHORE_NAME","shm_sem"),sizeof(Shm_Sem_Name));
	shm_address = strtol(read_ini_string(permanent_pool,TASK_INI,TASK_CAPTION,"SHM_ADDRESS","0x10000000"),NULL,16);
	shm_size = read_ini_int(TASK_INI,TASK_CAPTION,"SHM_SIZE",2147483647);

	typedef tSTLShMemAllocator<TFileInfo> tSTLAllocator;
	//初始化内存值
	unsigned long key = crc32(shm_key_name);
	shmid = shmget(key, shm_size, 0);
	if(shmid>= 0)
	{
		master=false;
	  	tMemoryAllocator *myalloc=new tMemoryAllocator(master, new tSharedMemoryPool(master, shm_key_name, shm_size, shm_address), new tSystemVSemaphore(master, Shm_Sem_Name));
	  	tSTLAllocator * stl_allocator = static_cast<tSTLAllocator*>(myalloc->getdata(CFG_STLShMemAllocatorKey));
		if(NULL == stl_allocator)
		{
			printf("Error:not enough memory to malloc.");
			exit(0);
		}
	  	stl_allocator->setmalloc(myalloc);
		g_pAlloc = myalloc;
	  	g_pMapFileInfo = static_cast<tFileInfoMap *>(myalloc->getdata(CFG_KeyExampleMap));
  	}
  	else
	{
		master=true;
		tMemoryAllocator *myalloc=new tMemoryAllocator(master, new tSharedMemoryPool(master, shm_key_name, shm_size,shm_address), new tSystemVSemaphore(master, Shm_Sem_Name));
	  	tMemoryAllocator::tBlock memblock = myalloc->malloc_key(sizeof(tSTLAllocator), CFG_STLShMemAllocatorKey);
	  	tSTLAllocator *stl_allocator = static_cast<tSTLAllocator*>(new((memblock.data))tSTLAllocator);
		if(NULL == stl_allocator)
		{
			printf("Error:not enough memory to malloc.");
			exit(0);
		}
	  	stl_allocator->setmalloc(myalloc);
	  	memblock = static_cast<tMemoryAllocator::tBlock>(myalloc->malloc_key(sizeof(tFileInfoMap), CFG_KeyExampleMap));
		g_pAlloc = myalloc;
	  	g_pMapFileInfo = static_cast<tFileInfoMap *>(new((memblock.data))tFileInfoMap(std::less<ULONG>(), (tSTLShMemAllocator<TFileInfo>)*stl_allocator));
	}	
	g_pFileSem= new tSystemVSemaphore(master, CHILDPID_OPERATE_FILE_SEMAPHORE); 
	g_pFileSem->create();
}

int  main(int argc, char *argv[])
{		
	init(argc,argv);
	if ( argc==1 )
	{
		creat_task_list();
		list_task();
	}
	else if ( argc==2 )
	{				
		if (strcmp(argv[1],"start")==0)
		{
			creat_task_list(); 
			load_task();
		}
		else if (strcmp(argv[1],"stop")==0)
			stop_task();
		else if(strcmp(argv[1],"clean")==0)
		{
			stop_task();
			char cmd[256] = "";
			char shm_key_name[64]="";
			int msg_id=msgget( g_key, IPC_EXCL | 0666 );
			if(msgctl(msg_id,IPC_RMID, 0) >=0)
				printf("program clean ipc msgid [%d] ok.\n", msg_id);
			else
				printf("program clean ipc msgid [%d] fail [%s].\n", msg_id,strerror(errno));

			//clean hash share memory
			strncpy(shm_key_name,read_ini_string(permanent_pool,TASK_INI,TASK_CAPTION,"SHM_KEY_NAME","shm_filecollection"),sizeof(shm_key_name));			
			int	shm_size = read_ini_int(TASK_INI,TASK_CAPTION,"SHM_SIZE",2147483647);
			unsigned long shmkey = crc32(shm_key_name);
			int shmid = shmget(shmkey, shm_size, 0);
			sprintf(cmd,"ipcrm -m %d",shmid);
			system(cmd);
			printf("program clean ipc shmid [%d] ok.\n", shmid);
		}
		else
			printf("USEAGE: %s start | stop | clean\n", argv[0]);
	}
	else if ( argc==3 )
	{
		if ( strcmp(argv[1],"start")!=0 )
		{
			printf("USEAGE: %s start id\n", argv[0]);
			return 0;
		}		
		_sch_internal_queue_id = msgget(g_key, IPC_CREAT| IPC_EXCL | 0666 );
		if(_sch_internal_queue_id == -1)
			_sch_internal_queue_id = msgget(g_key,0);
		creat_task_list(); 
		printf("start ftpid:%s, and only can start one schedule\n", argv[2]);
		start_task(atoi(argv[2]));
		printf("stop ftpid:%s\n", argv[2]);
		
	}
	else
		printf("USEAGE: %s start | stop | clean\n", argv[0]);

	return 0;
}

