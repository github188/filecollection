#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include "memmanager.h"
#include "iniprocess.h"
#include "ftpprocess.h"
#include "logprocess.h"
#include "taskitem.h"
#include "g_string.h"
#include "frame_memory.h"
#include "definitions.h"
#include "frame_utils.h"



struct TASK_ITEM* _task_list_update=NULL;
struct TASK_ITEM* _task_list=NULL;
extern void start_task( int __id );
extern tFileInfoMap *g_pMapFileInfo;
extern tSystemVSemaphore *g_pFileSem;
extern tMemoryAllocator *g_pAlloc;
int IsNumber(const char *szstr)
{
	size_t i = 0;
	if(NULL == szstr)
		return 0;
	for(i = 0; i<strlen(szstr); ++i)
	{	
		if(szstr[i]<'0' ||szstr[i] >'9')
		{
			/*非数字*/
			return 0;
		}
	}
	return 1;
}
const char * GetTime(time_t t)
{
	static char szBuf[64];/*多进程返回它没有关系*/
	struct tm m;
	memset(szBuf, 0, sizeof(szBuf));
	localtime_r(&t, &m);
	strftime(szBuf, sizeof(szBuf), "%Y-%m-%d %H:%M:%S",&m);
	return szBuf;
}
	
time_t MakeTime(const char *szTime_year, const char *szMon, char *szDay)
{
	struct tm m;
	int i=0;
	char szChinaMon[12][6] = {"1月","2月","3月","4月","5月","6月","7月","8月","9月","10月","11月","12月"};
	char szEnglishMon[12][6] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
	const char *pch = NULL;
	char szhHour[4] = "";
	char szMin[4] = ""; 
	int bFound = 0;
    int ch = ':';
	time_t now = time(0);	
	localtime_r(&now, &m);
	
	if(szTime_year == NULL || szMon == NULL || szDay == NULL)
		return time(0);
	pch = strchr(szTime_year,ch); 
	if(pch != NULL)
	{
		/*有冒号证明是时间，则年份是今年*/
		strncpy(szhHour, szTime_year, pch - szTime_year);
		strncpy(szMin, pch +1, 2);
		m.tm_hour = atoi(szhHour); //时差+ 8
		m.tm_min = atoi(szMin);	
		m.tm_sec = 0;
	}
	else
	{
		m.tm_year = atoi(szTime_year) - 1900;
		m.tm_hour = 0;  //是去年的时间，时分取系统时间
		m.tm_min = 0;	
		m.tm_sec = 0;
	}

	//取月份
	for(i = 0; bFound == 0 && i<12; ++i)
	{
		if(strncmp(szMon, szChinaMon[i],strlen(szChinaMon[i])) == 0
			||strncmp(szMon, szEnglishMon[i],strlen(szEnglishMon[i])) == 0)
		{
			/*是出现的中文月份*/
			bFound = 1;
			m.tm_mon = i;
		}
	}
	if(0 == bFound && IsNumber(szMon))
	{
		m.tm_mon = atoi(szMon);
	}

	m.tm_mday = atoi(szDay);
	return mktime(&m);		
}

/*
 *得到FTP信息
 */
struct FTP_SERVER_INFO*
get_ftp_info( struct TASK_ITEM* __item)
/*
 *Format:user:pass:ip:port
 */
{
	char _user[32];
	char _pass[32];
	char _ip[32];
	char  _port[32];
	struct FTP_SERVER_INFO* _info;
	
	memset(_user, 0, sizeof(_user));
	memset(_pass, 0, sizeof(_pass));
	memset(_ip, 0, sizeof(_ip));
	memset(_port, 0, sizeof(_port));

	if(sscanf( __item->ftp_host,"%[^:]:%[^:]:%[^:]:%[^:]", _user, _pass, _ip, _port )!=4)
		LOG_MSG("error host:%s,please using format: user:pass:ip:port",__item->ftp_host);
	
	if ( (strlen(_user)==0) || (strlen(_pass)==0) || (strlen(_ip)==0) || (_port==0) )
		return NULL;
        LOG_MSG("userinfo : %s:%s:%s:%s,please using format: user:pass:ip:port",_user, _pass, _ip, _port);
	_info=(FTP_SERVER_INFO*)pcalloc(permanent_pool, sizeof(struct FTP_SERVER_INFO));
	
	strcpy(_info->user, _user);
	strcpy(_info->pass, _pass);
	strcpy(_info->ip, _ip);
	_info->port=atoi(_port);
	
	return _info;
}

/*
 *拷贝__file从__file_path到_item中规定的个格式
 *FORMAT: PREV:DIR:END
 *if file name is __file,
 *then DIR/PREV__fileEND is the copy file name.
 */
int 
cp_item_to_dir( const char* __format, const char *__file, const char *__file_path, int copy_flag )
{
	int ret = 0;
	char _prev[32]="";
	char _end[32]="";
	char _dir[NAME_MAX+1]="";
	if ( strcmp(__format, "NULL")==0 )
		return 0;
	
	memset(_prev, 0, sizeof(_prev));
	memset(_end, 0, sizeof(_end));
	memset(_dir, 0, sizeof(_dir));
	
	ret = sscanf(__format, "%[^:]:%[^:]:%[^:]", _prev, _dir, _end);
	if(ret != 3)
		return 0;
	_prev[sizeof(_prev)-1] = '\0';
	_dir[sizeof(_dir)-1] = '\0';
	_end[sizeof(_end)-1] = '\0';
	strcat(_dir, "/");
	if ( _prev[0] != '\0' && strcmp(_prev, "NO")!=0 )
		strncat( _dir, _prev,sizeof(_prev));
	strncat(_dir, __file, sizeof(_dir));
	if ( _end[0] != '\0' && strcmp(_end, "NO")!=0 )
		strncat(_dir, _end,sizeof(_prev));

#define COPY_NULL         0   /*不执行拷贝操作*/
#define COPY_REAL         1   /*物理拷贝*/
#define COPY_SYMBOL_LINK  2   /*符号链接*/
#define COPY_HARD_LINK    3   /*硬链接*/

	switch(copy_flag)
	{
		case COPY_REAL:	
			//物理拷贝
			ret = cp_file(__file_path, _dir, 0);
			LOG_MSG("copy file [%s] to [%s] [%s]",__file_path,_dir, ret==1? "ok":"fail");
			break;
	    case COPY_SYMBOL_LINK:	
			//符号链接
			ret = (symlink(__file_path, _dir) == 0) ? 1: 0;
			if(1 == ret)
				LOG_MSG("symbol link file [%s] to [%s] ok.",__file_path,_dir);
			else
				LOG_MSG("symbol link file [%s] to [%s] error [%s]",__file_path,_dir,strerror(errno));
			break;
		case COPY_HARD_LINK:
			ret = (link(__file_path, _dir) == 0) ? 1: 0;			
			if(1 == ret)
				LOG_MSG("hard link file [%s] to [%s] ok",__file_path,_dir);
			else
				LOG_MSG("hard link file [%s] to [%s] error [%s]",__file_path,_dir,strerror(errno));
			break;
		case COPY_NULL:
		default:
			LOG_MSG("file [%s] need not copy.",__file_path);
			break;
	}

	return ret;
}

/*
 *得到某个ID的ITEM
 */
struct TASK_ITEM*
get_task_item_by_id( int __id )
{
	TASK_ITEM* _cur;
	_cur=_task_list;
	while(_cur)
	{
		if ( _cur->sch_item.id==__id  )
		{
			return _cur;
		}
		_cur=_cur->next;
	}
	return NULL;
}

/*根据配置文件创建列表*/
void
create_task_item_list_by_ini( const char* __file )
{
    int _file_num;
	int _i;
	struct TASK_ITEM *_cur=_task_list;
	/*确定配置文件数*/
	_file_num=read_ini_int( __file, TASK_CAPTION, "TaskNum", 0 );
	
	/*循环读取配置文件中的内容*/
	for( _i=1; _i<=_file_num; _i++ )
	{
		char _key[32], *_file;
		memset( _key, 0, sizeof(_key) );
		sprintf(_key,"ID%d", _i);
		_file=read_ini_string(permanent_pool, __file, TASK_CAPTION, _key, "NULL" );
		if ( strcmp(_file,"NULL")==0 )
			continue;
		else
		{
			/*循环读取每个配置章节中的配置数据*/
			int _list_num, _n;
			_list_num=read_ini_int(_file, "ParmList", "ListNum", 0 );
			for( _n=1; _n<=_list_num; ++_n )
			{
				char _section[32];
				struct TASK_ITEM *_task;
				memset(_section,0,sizeof(_section));
				sprintf(_section, "FtpParm%d", _n);
				_task=(TASK_ITEM *)pcalloc(permanent_pool, sizeof(*_task));
				_task->sch_item.id=read_ini_int( _file, _section, "ID", 0 );
				if ( _task->sch_item.id==0 )
					continue;
				
				strcpy( _task->sch_item.month, read_ini_string(permanent_pool, _file, _section, "MONTH", "*"));  	
				strcpy( _task->sch_item.day, read_ini_string(permanent_pool, _file, _section, "DAY", "*"));
				strcpy( _task->sch_item.hour, read_ini_string(permanent_pool, _file, _section, "HOUR", "*"));
				strcpy( _task->sch_item.min, read_ini_string(permanent_pool, _file, _section, "MIN", "*"));
				
				_task->sch_item.func=start_task;
				_task->sch_item.arg=_task->sch_item.id;
				strcpy( _task->ftp_host, read_ini_string(permanent_pool, _file, _section, "FTP_HOST", "NULL"));
				strcpy( _task->ftp_path, read_ini_string(permanent_pool, _file, _section, "FTP_PATH", "NULL"));
				strcpy( _task->local_path, read_ini_string(permanent_pool, _file, _section, "LOCAL_PATH", "NULL"));
				strcpy( _task->file, read_ini_string(permanent_pool, _file, _section, "FILE", "NULL"));	
				
				_task->direction=read_ini_int( _file, _section, "DIRECTION", 0 );		
				_task->del_src=read_ini_int( _file, _section, "DEL_SRC", 0 );
				_task->file_ctime_escape = read_ini_int( _file, _section, "FILE_MODIFY_TIME_ESCAPE", 365*24*3600); //默认为一年
				_task->use_tmp=read_ini_int( _file, _section, "USE_TMP",  1);/*2005-03-28 ADD*/
				strcpy( _task->sh_file, read_ini_string(permanent_pool, _file, _section, "SH_FILE", "NULL"));	 
				strcpy( _task->son_dir_mask, read_ini_string(permanent_pool, _file, _section, "SON_DIR_MASK","NULL"));
				strcpy( _task->cp_dir_1, read_ini_string(permanent_pool, _file, _section, "CP_DIR_1", "NULL"));
				strcpy( _task->cp_dir_2, read_ini_string(permanent_pool, _file, _section, "CP_DIR_2", "NULL"));
				strcpy( _task->cp_dir_3, read_ini_string(permanent_pool, _file, _section, "CP_DIR_3", "NULL"));
				strcpy( _task->cp_dir_4, read_ini_string(permanent_pool, _file, _section, "CP_DIR_4", "NULL"));
				if (_cur!=NULL)
					_cur->next=_task;	
				else
					_task_list=_task;
				_cur=_task;
				_task->next=NULL;
			}
		}		
	} 
}    


/*
 *得到FTP中文件的列表
 */
struct FTP_FILE_INFO* get_ftp_file_list( pool* __pool, TASK_ITEM* __item, const char* __remote)
{
	char _tmp_file[128];
	int _i=0;
	FILE *_tmp_fd;
	char _buf[256];
	char szMon[16];
	char szDay[16];
	char szTime_year[16];
	FTP_FILE_INFO *_list=NULL,*_prev=NULL, *_cur;
	
	memset(_tmp_file, 0, sizeof(_tmp_file));/*Sample,./TASK_ID_I_LIST_NUM*/
	/*Produce Temp File */
	sprintf(_tmp_file, "%s/log/TASK_ID_%d_LIST_TMP_%d", EXEC_PATH, __item->sch_item.id, _i);
	while( if_file_exist(_tmp_file) )
	{
		_i++;
		sprintf(_tmp_file, "%s/log/TASK_ID_%d_LIST_TMP_%d", EXEC_PATH, __item->sch_item.id, _i);
	}
	
	/*First Get List To Temp File*/
	ftp_list(__remote, _tmp_file);	
	
	/*Second Scan Temp File And Create FTP_FILE_LIST*/
	_tmp_fd=fopen(_tmp_file, "r");
	if ( _tmp_fd==NULL )
		return NULL;

	while( fgets(_buf, sizeof(_buf), _tmp_fd)!=NULL )
	{
		if ( get_string_wards_count(_buf)==9 )
		{
			char _tmp_str[32];
			_cur=(FTP_FILE_INFO *)pcalloc(__pool, sizeof(*_cur));	
			if (_buf[0]=='d')
			{
				_cur->dir=1;
			}
			get_string_wards_by_pos( _buf, 1, _cur->access, sizeof(_cur->access) );
			memset(_tmp_str, 0, sizeof(_tmp_str));
			get_string_wards_by_pos( _buf, 5, _tmp_str, sizeof(_tmp_str) );
			_cur->size=atoi(_tmp_str);
			/*======add by ganhh 20070823======*/
			get_string_wards_by_pos(_buf, 6, szMon, sizeof(szMon));
			get_string_wards_by_pos(_buf, 7, szDay, sizeof(szDay));
			get_string_wards_by_pos(_buf, 8, szTime_year, sizeof(szTime_year));
			if(IsNumber(szMon) && strlen(szMon)==4)
			{
				//年月日格式:-rw-rw-rw-   1 oracle   oinstall       3 2007   9月 19 JFK0720070918.txt
				_cur->tModifyTime = MakeTime(szMon, szDay,szTime_year);
			}
			else
			{
				//月日年格式:-rw-r--r--   1 oracle   oinstall       0  9月 19 2007 JFK0720070918.txt
				_cur->tModifyTime = MakeTime(szTime_year, szMon, szDay);
			}
			/*=======end=====*/
			get_string_wards_by_pos( _buf, 9, _cur->file, sizeof(_cur->file) );
			if ( strlen(_cur->file)>1 )
			{
				if ( _cur->file[strlen(_cur->file)-1]==0x0A ||  _cur->file[strlen(_cur->file)-1]==0x0D )
					_cur->file[strlen(_cur->file)-1]=0;
				if ( _cur->file[strlen(_cur->file)-2]==0x0D )
					_cur->file[strlen(_cur->file)-2]=0;					
			}		
			if ( _list==NULL )
				_list=_cur;
			_cur->next=NULL;
			if ( _prev!=NULL )
				_prev->next=_cur;
			_prev=_cur;
			
		}
		memset(_buf, 0, sizeof(_buf));
	}
	fclose(_tmp_fd);
	remove(_tmp_file);

	return _list;
}


/*
 *得到SFTP中文件的列表
 */
struct FTP_FILE_INFO* get_sftp_file_list(pool* __pool, SFTP *sftp ,TASK_ITEM* __item, const char* __remote)
{
    char _tmp_file[128];
    int _i=0;
    FILE *_tmp_fd;
    char _buf[256];
    char szMon[16];
    char szDay[16];
    char szTime_year[16];
    FTP_FILE_INFO *_list=NULL,*_prev=NULL, *_cur;

    memset(_tmp_file, 0, sizeof(_tmp_file));/*Sample,./TASK_ID_I_LIST_NUM*/
    /*Produce Temp File */
    sprintf(_tmp_file, "%s/log/TASK_ID_%d_LIST_TMP_%d", EXEC_PATH, __item->sch_item.id, _i);
    while( if_file_exist(_tmp_file) )
    {
        _i++;
        sprintf(_tmp_file, "%s/log/TASK_ID_%d_LIST_TMP_%d", EXEC_PATH, __item->sch_item.id, _i);
    }

    /*First Get List To Temp File*/
    sftp->sftp_list(_tmp_file,__remote);

    /*Second Scan Temp File And Create FTP_FILE_LIST*/
    _tmp_fd=fopen(_tmp_file, "r");
    if ( _tmp_fd==NULL )
        return NULL;

    while( fgets(_buf, sizeof(_buf), _tmp_fd)!=NULL )
    {
        if ( get_string_wards_count(_buf)==9 )
        {
            char _tmp_str[32];
            _cur=(FTP_FILE_INFO *)pcalloc(__pool, sizeof(*_cur));
            if (_buf[0]=='d')
            {
                _cur->dir=1;
            }
            get_string_wards_by_pos( _buf, 1, _cur->access, sizeof(_cur->access) );
            memset(_tmp_str, 0, sizeof(_tmp_str));
            get_string_wards_by_pos( _buf, 5, _tmp_str, sizeof(_tmp_str) );
            _cur->size=atoi(_tmp_str);
            /*======add by ganhh 20070823======*/
            get_string_wards_by_pos(_buf, 6, szMon, sizeof(szMon));
            get_string_wards_by_pos(_buf, 7, szDay, sizeof(szDay));
            get_string_wards_by_pos(_buf, 8, szTime_year, sizeof(szTime_year));
            if(IsNumber(szMon) && strlen(szMon)==4)
            {
                //年月日格式:-rw-rw-rw-   1 oracle   oinstall       3 2007   9月 19 JFK0720070918.txt
                _cur->tModifyTime = MakeTime(szMon, szDay,szTime_year);
            }
            else
            {
                //月日年格式:-rw-r--r--   1 oracle   oinstall       0  9月 19 2007 JFK0720070918.txt
                _cur->tModifyTime = MakeTime(szTime_year, szMon, szDay);
            }
            /*=======end=====*/
            get_string_wards_by_pos( _buf, 9, _cur->file, sizeof(_cur->file) );
            if ( strlen(_cur->file)>1 )
            {
                if ( _cur->file[strlen(_cur->file)-1]==0x0A ||  _cur->file[strlen(_cur->file)-1]==0x0D )
                    _cur->file[strlen(_cur->file)-1]=0;
                if ( _cur->file[strlen(_cur->file)-2]==0x0D )
                    _cur->file[strlen(_cur->file)-2]=0;
            }
            if ( _list==NULL )
                _list=_cur;
            _cur->next=NULL;
            if ( _prev!=NULL )
                _prev->next=_cur;
            _prev=_cur;

        }
        memset(_buf, 0, sizeof(_buf));
    }
    fclose(_tmp_fd);
    remove(_tmp_file);

    return _list;
}


/*
 *得到本地文件列表
 */
struct FTP_FILE_INFO*
get_local_file_list(pool* __pool, const char* __local)
{
	struct FTP_FILE_INFO *_list=NULL, *_cur=NULL, *_prev=NULL;
	DIR *_dp;
	struct dirent *_item;

	/*Open Dir*/
	if ((_dp=opendir(__local))==NULL)
	{
		log_msg(g_log_file, LOG_BY_MONTH, "Can's Open Dir:%s,error %s",strerror(errno));
		return NULL;
	}
	/*Read Dir*/
	while((_item=readdir(_dp))!=NULL)
	{
		if ( strcmp(_item->d_name, ".")==0 || strcmp(_item->d_name, "..")==0 )
			continue;
		_cur=(FTP_FILE_INFO *)pcalloc(__pool, sizeof(*_cur));		
		if(__local[strlen(__local)-1] == '/')
			sprintf(_cur->file, "%s%s", __local, _item->d_name);
		else
			sprintf(_cur->file, "%s/%s", __local, _item->d_name);			
		/*Dir*/
		if ( if_isdir(_cur->file) )
			_cur->dir=1;
		/*Size*/
		_cur->size=get_file_size(_cur->file);
		_cur->tModifyTime = get_file_last_modify(_cur->file);
		get_file_name(_item->d_name, _cur->file, sizeof(_cur->file));
		if ( _list==NULL )
			_list=_cur;
		_cur->next=NULL;
		if ( _prev!=NULL)
			_prev->next=_cur;
		_prev=_cur;
	}
	/*Close Dir*/
	closedir(_dp);
	return _list;
}

/*
 *判断LIST存在文件名和大小相同
 *原和目标相同，返回1，不同则返回0
 */
int
if_in_list_exist( FTP_FILE_INFO* __list, FTP_FILE_INFO* __item )
{
    FTP_FILE_INFO* _tmp;
    
    _tmp=__list;
    while(_tmp)
	{
		if ( (_tmp->size==__item->size) && (strcmp(_tmp->file, __item->file)==0))
				break;
			_tmp=_tmp->next;
	}
	if ( _tmp==NULL )
         return 0;
     return 1;
}

/*
 *功能：把配置文件中含有日期标志符的地方替换成实际的日期
 *      主要有以下几种可替换的字符：
 *      当天[_CUR_DATE_L]如20050301,[_CUR_DATE_S]如050301
 *      前一天[_PREV_DATE_L]如20050228,[_PREV_DATE_S]如050228, 前一天的月份[_PREV_DATE_MONTH_L],[_PREV_DATE_MONTH_S]
 *      当月[_CUR_MONTH_L]如200503,[_CUR_MONTH_S]如0503
 *      前一月[_PREV_MONTH_L]如200502,[_PREV_MONTH_S]如0502
 *输入参数：__src,源字符串
 *          __dest,目标字符串
 *          __size,目标字符串长度
 *返回：    新字符串长度.
            -1表示失败
 */
 int
 trans_path_str( const char *__src, char *__dest, int __size )
 {
    char *_tmp_str;
    
    char _cur_day_s_str[10];
    char _cur_month_s_str[10];
    char _prev_day_s_str[10];
    char _prev_month_s_str[10];
    char _cur_day_l_str[10];
    char _cur_month_l_str[10];
    char _prev_day_l_str[10];
    char _prev_month_l_str[10];    
    /*Add Prev Day's Month*/
    char _prev_day_month_l_str[10];
    char _prev_day_month_s_str[10];
    
    time_t _cur_day;
    time_t _prev_day;
    time_t _prev_month;
    struct tm *_cur_day_st;
    struct tm *_prev_day_st;
    struct tm *_prev_month_st;
    
    /*得到当天*/
    time(&_cur_day);
    _cur_day_st=localtime(&_cur_day);
    memset(_cur_day_s_str, 0, sizeof(_cur_day_s_str));
    memset(_cur_day_l_str, 0, sizeof(_cur_day_l_str));
    sprintf(_cur_day_s_str, "%02d%02d%02d", _cur_day_st->tm_year+1900-2000, _cur_day_st->tm_mon+1, _cur_day_st->tm_mday);
    sprintf(_cur_day_l_str, "%04d%02d%02d", _cur_day_st->tm_year+1900, _cur_day_st->tm_mon+1, _cur_day_st->tm_mday); 
    
    /*得到当月*/
    memset(_cur_month_s_str, 0, sizeof(_cur_month_s_str));
    memset(_cur_month_l_str, 0, sizeof(_cur_month_l_str));
    sprintf(_cur_month_s_str, "%02d%02d", _cur_day_st->tm_year+1900-2000, _cur_day_st->tm_mon+1);
    sprintf(_cur_month_l_str, "%04d%02d", _cur_day_st->tm_year+1900, _cur_day_st->tm_mon+1); 
        
    
    /*得到前一天*/
    _cur_day_st->tm_mday--;
    _prev_day=mktime(_cur_day_st);
    _prev_day_st=localtime(&_prev_day);
    memset(_prev_day_s_str, 0, sizeof(_prev_day_s_str));
    memset(_prev_day_l_str, 0, sizeof(_prev_day_l_str));
    sprintf(_prev_day_s_str, "%02d%02d%02d", _prev_day_st->tm_year+1900-2000, _prev_day_st->tm_mon+1, _prev_day_st->tm_mday);
    sprintf(_prev_day_l_str, "%04d%02d%02d", _prev_day_st->tm_year+1900, _prev_day_st->tm_mon+1, _prev_day_st->tm_mday);   
    memset( _prev_day_month_l_str, 0, sizeof(_prev_day_month_l_str));
    memset( _prev_day_month_s_str, 0, sizeof(_prev_day_month_s_str));    
    sprintf( _prev_day_month_l_str, "%04d%02d", _prev_day_st->tm_year+1900, _prev_day_st->tm_mon+1);
    sprintf( _prev_day_month_s_str, "%02d%02d", _prev_day_st->tm_year+1900-2000, _prev_day_st->tm_mon+1);
    
        
    /*得到前一个月*/
    time(&_cur_day);
    _prev_month_st=localtime(&_cur_day);
    _prev_month_st->tm_mon--;
    _prev_month=mktime(_prev_month_st);
    _prev_month_st=localtime(&_prev_month);
    memset(_prev_month_s_str, 0, sizeof(_prev_month_s_str));
    memset(_prev_month_l_str, 0, sizeof(_prev_month_l_str));
    sprintf(_prev_month_s_str, "%02d%02d", _prev_month_st->tm_year+1900-2000, _prev_month_st->tm_mon+1);
    sprintf(_prev_month_l_str, "%04d%02d", _prev_month_st->tm_year+1900, _prev_month_st->tm_mon+1);     
    
    /*测试*/
    /*
    printf("CUR_DAY_s:%s\n", _cur_day_s_str);
    printf("CUR_MONTH_s:%s\n", _cur_month_s_str);
    printf("PREV_DAY_s:%s\n", _prev_day_s_str);
    printf("PREV_MONTH_s:%s\n", _prev_month_s_str);
    printf("CUR_DAY_l:%s\n",_cur_day_l_str);
    printf("CUR_MONTH_l:%s\n", _cur_month_l_str);
    printf("PREV_DAY_l:%s\n", _prev_day_l_str);
    printf("PREV_MONTH_l:%s\n", _prev_month_l_str);
    printf("PREV_DAY_MONTH_l:%s\n", _prev_day_month_l_str);
    printf("PREV_DAY_MONTH_s:%s\n", _prev_day_month_s_str);
    */
    
    
    /*分配和__dest同样大小的buff*/
    if ( (_tmp_str=(char*)malloc(__size)) == NULL )
    {
        return -1;
    }
    
    /*替换[_CUR_DATE_L]*/
    memset(__dest,0 , __size);
    memset(_tmp_str, 0, __size);
    strncpy(_tmp_str, __src,__size-1);                                     
    string_replace( _tmp_str, "[_CUR_DATE_L]", _cur_day_l_str, __dest, __size );
      
    /*替换[_CUR_DATE_S]*/
    memset(_tmp_str, 0, __size);
    strncpy(_tmp_str, __dest,__size-1); 
    memset(__dest, 0, __size);                                   
    string_replace( _tmp_str, "[_CUR_DATE_S]", _cur_day_s_str, __dest, __size ); 
      
    /*替换[_PREV_DATE_L]*/                                       
    memset(_tmp_str, 0, __size);
    strncpy(_tmp_str, __dest,__size-1); 
    memset(__dest, 0, __size);         
    string_replace( _tmp_str, "[_PREV_DATE_L]", _prev_day_l_str, __dest, __size );  
    
    /*替换[_PREV_DATE_S]*/  
    memset(_tmp_str, 0, __size);
    strncpy(_tmp_str, __dest,__size-1); 
    memset(__dest, 0, __size);                                             
    string_replace( _tmp_str, "[_PREV_DATE_S]", _prev_day_s_str, __dest, __size );  
    
    /*替换[_PREV_DATE_MONTH_S]*/  
    memset(_tmp_str, 0, __size);
    strncpy(_tmp_str, __dest,__size-1); 
    memset(__dest, 0, __size);                                             
    string_replace( _tmp_str, "[_PREV_DATE_MONTH_S]", _prev_day_month_s_str, __dest, __size );  
    
    /*替换[_PREV_DATE_MONTH_L]*/  
    memset(_tmp_str, 0, __size);
    strncpy(_tmp_str, __dest,__size-1); 
    memset(__dest, 0, __size);                                             
    string_replace( _tmp_str, "[_PREV_DATE_MONTH_L]", _prev_day_month_l_str, __dest, __size );  
    
    
    /*替换[_CUR_MONTH_L]*/ 
    memset(_tmp_str, 0, __size);
    strncpy(_tmp_str, __dest,__size-1); 
    memset(__dest, 0, __size);                                        
    string_replace( _tmp_str, "[_CUR_MONTH_L]", _cur_month_l_str, __dest, __size ); 
     
    /*替换[_CUR_MONTH_S]*/
    memset(_tmp_str, 0, __size);
    strncpy(_tmp_str, __dest,__size-1); 
    memset(__dest, 0, __size);                                            
    string_replace( _tmp_str, "[_CUR_MONTH_S]", _cur_month_s_str, __dest, __size );
       
    /*替换[_PREV_MONTH_L]*/ 
    memset(_tmp_str, 0, __size);
    strncpy(_tmp_str, __dest,__size-1); 
    memset(__dest, 0, __size);                                            
    string_replace( _tmp_str, "[_PREV_MONTH_L]", _prev_month_l_str, __dest, __size );  
    
    /*替换[_PREV_MONTJ_S]*/     
    memset(_tmp_str, 0, __size);
    strncpy(_tmp_str, __dest,__size-1); 
    memset(__dest, 0, __size); 

    return string_replace( _tmp_str, "[_PREV_MONTH_S]", _prev_month_s_str, __dest, __size );                                                                
   /*printf("%s\n", __dest);*/
 }                   

int shm_insert_file(const int sch_item_id,const char *file,const int size, const time_t ftptime)
{
	TFileInfo FileInfo;
	char szSchFile[256]="";
	TASK_ITEM *piterm = NULL;
	int nConflictCount = 0;
	pair<map<ULONG,TFileInfo>::iterator, bool> pair;
	if(NULL==file || '\n' == file[0])
		return -1;
	memset(&FileInfo,0,sizeof(FileInfo));
	sprintf(szSchFile,"%d%s",sch_item_id,trim((char*)file));
	ULONG nKey = hash(szSchFile);
	memset(&FileInfo, 0, sizeof(FileInfo));
	strncpy(FileInfo.FileName,szSchFile,sizeof(FileInfo.FileName)-1);
	FileInfo.nFileSize = size;		
	FileInfo.nConflictCount = 0;
	FileInfo.ftptime = ftptime;
        FileInfo.nftpid  = sch_item_id; 
	//插入到共享内存中
	CSemLock Lock(g_pFileSem);
	if(!Lock.TryLock(-1))
	{		
		LOG_MSG("cann't insert key [%s] hash [%u] to map because semaphone been lock ",file,nKey);            
		return -1;  
	}
	piterm=get_task_item_by_id(sch_item_id);
	if(NULL == piterm || time(0)-ftptime > piterm->file_ctime_escape)
		return -1;
	
	while(1)
	{		
		pair = g_pMapFileInfo->insert(make_pair(nKey+nConflictCount,FileInfo));
		if(pair.second)
		{		
			//设置相同key值的冲突个数，以方便查找
			if(nConflictCount >= 1)
			{
				(*g_pMapFileInfo)[nKey].nConflictCount = nConflictCount;			
				LOG_MSG("ftp_id [%d] key [%s] find empty hash key to insert, conflict count is [%d].",\
					sch_item_id,szSchFile, nConflictCount);				
			}
			break;
		}
		else if(pair.first->second.nftpid == sch_item_id  && size == pair.first->second.nFileSize && strcmp(pair.first->second.FileName,szSchFile) == 0)
		{
			LOG_MSG("ftp_id [%d] key[%s] size[%d] already exists,old key [%s] hash value is [%u], conflict count is [%d],cann't insert to shm", \
				sch_item_id,szSchFile, size, pair.first->second.FileName,nKey, nConflictCount);	
			break;
		}
		else
		{
			nConflictCount += 1;
		}
	}

	return 0;
}

int load_file_hash_value(const int sch_item_id,const int direction)
{
	char szFileName[256]="";
	char buf[1024]="";
	int  nSize = 0;
	int  nFtpTime = 0;
	int  nHashRecord = 0;	
	if(direction == FTP_GET)
		sprintf(szFileName, "%s/log/TASK_ID_%d_HAVE_GET_LIST", EXEC_PATH, sch_item_id);
	else
		sprintf(szFileName, "%s/log/TASK_ID_%d_HAVE_PUT_LIST", EXEC_PATH, sch_item_id);

	FILE *pfile=fopen(szFileName,"r");
	if(NULL == pfile)
	{
		LOG_MSG("load_file_hash_value open file [%s] error[%s]", szFileName,strerror(errno));
		return -1;
	}
        static int n=0;
        LOG_MSG(" n = %d ",n); 
        n++;
	while(fgets(buf,sizeof(buf),pfile))
	{ 
		if(buf[0] == '\n' || buf[0] == '[')
			continue;
		char *pstr = strchr(buf,'=');
                LOG_MSG("found = on file [%s] line [%s]",szFileName, buf); 
		if(NULL == pstr)
		{
			LOG_MSG("not found = on file [%s] line [%s]",szFileName, buf);
			continue;
		}
		buf[pstr-buf] = '\0';	
		nSize = atoi(pstr+1);
		pstr = strchr(pstr+1,',');
		if(NULL == pstr)
			nFtpTime = time(0);
		else
			nFtpTime = atoi(pstr+1);
		if(shm_insert_file(sch_item_id, buf, nSize, nFtpTime) == 0)
			nHashRecord += 1;

	}
	LOG_MSG("file [%s] load hash record count [%d]", szFileName, nHashRecord);
	fclose(pfile);
	return 0;
}

bool ftpid_hash_file_loaded(const int sch_item_id)
{
	CSemLock Lock(g_pFileSem);
	if(!Lock.TryLock(-1))
	{		
		LOG_MSG("ftpid_hash_file_loaded[%d] to map because semaphone been lock ",sch_item_id);            
		return false;  
	}
	return g_pMapFileInfo->find(sch_item_id) != g_pMapFileInfo->end();
}

 int read_ftped_file(const int sch_item_id,const char *file,const int nFileSize, const int direction)
{
    ULONG nKey = 0;
	int nConflictCount = 0;
	TFileInfo FileInfo;
	char szFileName[256]="";
	map<ULONG,TFileInfo>::iterator iter;	
	if(NULL==file || '\n' == file[0] )
		return -1;
	memset(&FileInfo,0,sizeof(FileInfo));
	if(!ftpid_hash_file_loaded(sch_item_id) && load_file_hash_value(sch_item_id, direction) == 0)
	{
		CSemLock Lock(g_pFileSem);
		if(!Lock.TryLock(-1))
		{		
			LOG_MSG("item [%d] read_ftped_file [%s] fail because semaphone been lock. ",sch_item_id,file);            
			return -2;  
		}
		g_pMapFileInfo->insert(make_pair(sch_item_id,FileInfo));
	}
	sprintf(szFileName,"%d%s",sch_item_id,file);
	nKey = hash(szFileName);

	//在共享内存中查找此文件名的hash值
	CSemLock Lock(g_pFileSem);
	if(!Lock.TryLock(-1))
	{		
		LOG_MSG("item [%d] find hash [%u] fail because semaphone been lock. ",sch_item_id,nKey);            
		return -2;  
	}
	iter = g_pMapFileInfo->find(nKey);
	if(iter == g_pMapFileInfo->end())
	{
		//file not ftped
		return 0;
	}

	//hash值存在，看是否是自己的文件名，避免冲突.
	nConflictCount = iter->second.nConflictCount;
	for(int i=0; i<=nConflictCount;++i)
	{
		if(iter != g_pMapFileInfo->end() \
			&& strncmp(iter->second.FileName,szFileName,sizeof(FileInfo.FileName)-1) == 0 \
			&& iter->second.nFileSize == nFileSize \
                        && iter->second.nftpid == sch_item_id)

		{
			LOG_DBG("file [%s] hash value [%u] conflict count is [%d],at time [%s] have ftped, so need not ftp again", szFileName,nKey, nConflictCount, GetTime(iter->second.ftptime));
			return 1;
		}
		iter = g_pMapFileInfo->find(nKey+i+1);
	}
	return 0;	
}

int write_ftped_file(const int sch_item_id,const char *file,const int nFileSize,const int direction)
{
	char szFileName[256]="";
	time_t now=time(0);
	shm_insert_file(sch_item_id,file,nFileSize,now);
	if(direction == FTP_GET)
		sprintf(szFileName, "%s/log/TASK_ID_%d_HAVE_GET_LIST", EXEC_PATH, sch_item_id);
	else
		sprintf(szFileName, "%s/log/TASK_ID_%d_HAVE_PUT_LIST", EXEC_PATH, sch_item_id);
	FILE *pfile=fopen(szFileName,"a+");
	if(NULL == pfile)
	{
		LOG_MSG("write_ftped_file open file [%s] error[%s]\n", szFileName,strerror(errno));
		return -1;
	}
	fprintf(pfile,"%s = %d,%d\n",file, nFileSize, (int)now);
	fclose(pfile);
	return 0;
}

