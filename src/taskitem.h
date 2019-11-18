#ifndef TASK_ITEM_HEADER
#define TASK_ITEM_HEADER
#include <time.h>
#include <string.h>
#include "fileprocess.h"
#include "sftp.h"
#include "schedule.h"

/*
#这是该结构说明加实例文档
#ParmFile的配置文件格式说明:
#[分钟] + [小时] + [日期] + [月份] + [年] + [FTP服务器信息] + [FTP路径] + [本地路径] + [文件名(用正则表示式表示)] + [传送方向] + [是否删除源文件] + [完成后执行的脚本]
#其中的加号用空格代替.
#各字段说明:
#[分钟] + [小时] + [日期] + [月份] + [年]:跟crontab一样,支持(数字, *, * / 数字, "时间,时间")四种方式.
#[FTP服务器信息]: user:pass:ip:port
#[FTP路径]+[本地路径]:绝对路径
#[文件名(用正则表示式表示)]:文件名规则用正则表达式表示.
#[传送方向]:0表示从FTP到本地,1表示本地到FTP.
#[是否删除源文件]:0表示不删除,1表示删除.
#[完成后执行的脚本]:表示下载成功后所执行的脚本
*/

/*
 *UPDATE-LOG:
 *	2005-03-28:	增加一个配置属性，说明传输过程中是否使用临时文件名
 */
typedef struct TASK_ITEM TASK_ITEM;
extern struct TASK_ITEM *_task_list_update;
struct TASK_ITEM
{
	struct SCHEDULE_ITEM	sch_item;
	char			ftp_host[256];
	char			ftp_path[256];
	char			local_path[256];
	char			file[128];
	int			    direction;
	int			    del_src;
	int				generate_db_cdr;
	time_t			file_ctime_escape;/*文件创建时间消逝*/
	int				use_tmp;/*2005-03-28日加*/
	int             copy_flag;/*2009-09-07新增0代表物理拷贝，1代表符号链接*/
	char			sh_file[128];
	char			son_dir_mask[32];
	char			cp_dir_1[128];
	char 			cp_dir_2[128];
	char			cp_dir_3[128];
	char			cp_dir_4[128];
        int                     ftp_flag; 
	TASK_ITEM		*next;
};

typedef struct FTP_FILE_INFO FTP_FILE_INFO;

struct FTP_FILE_INFO
{
	int		dir;
	char	access[32];
	long		size;
	time_t	tModifyTime;
	char	file[256];
	FTP_FILE_INFO *next;	
};



extern struct TASK_ITEM* _task_list;
extern pool* permanent_pool;


/*
 *Task.ini Sample
 *[TaskList]
 *TaskNum = 12
 *ID1 = FileName
 *ID2 = FileName
 *....
 *ID12 = FileName
 *
 *FileName Sample
 *[ParmList]
 *ListNum = ""
 *[FtpParm1]
 *ID =
 *MONTH =
 *DAY =
 *HOUR =
 *MIN =
 *FTP_HOST =
 *FTP_PATH =
 *LOCAL_PATH =
 *FILE =
 *DIRECTION =
 *DEL_SRC =
 *SH_FILE =
 *SON_DIR_MASK =
 *[FtpParm2]
 *...
 */
 
/*
 主要需添加的功能
 1.对于文件夹文件名可以添加日期功能
*/

/*得到ITEM中FTP服务器信息*/
struct FTP_SERVER_INFO*
get_ftp_info( struct TASK_ITEM* __item);

/**/
int 
cp_item_to_dir( const char* __format, const char *__file, const char *__file_path, int copy_flag );

/*得到链表中某个ID的ITEM*/
struct TASK_ITEM*
get_task_item_by_id( int __id );

/*根据配置文件创建列表*/
void
create_task_item_list_by_ini( const char* __file );


/*
 *得到FTP中文件的列表
 */
struct FTP_FILE_INFO* get_ftp_file_list( pool* __pool, TASK_ITEM* __item, const char* __remote);

/*
 *得到SFTP中文件的列表
 */
struct FTP_FILE_INFO* get_sftp_file_list( pool* __pool, SFTP *sftp ,TASK_ITEM* __item, const char* __remote);


/*
 *得到本地文件列表
 */
struct FTP_FILE_INFO*
get_local_file_list(pool* __pool, const char* __local);

/*
 *是否在文件列表中存在
 */
int
if_in_list_exist( FTP_FILE_INFO* __list, FTP_FILE_INFO* __item );

/*
 *替换配置文件PATH中时间符号
 */
int
trans_path_str( const char *__src, char *__dest, int __size );

const char * GetTime(time_t t);

/*读取文件是否已经传输过的*/
int read_ftped_file(const int sch_item_id,const char *file,const int nFileSize,const int direction);

/*写已经传输完的文件和hash共享内存*/
int write_ftped_file(const int sch_item_id,const char *file,const int nFileSize,const int direction);

/*返回每个ftp进程是否装载完已经传输完的文件havs值*/
bool ftpid_hash_file_loaded(const int sch_item_id);

/*装载每个FTP进程的已经传输过的文件进入hash内存*/
int load_file_hash_value(const int sch_item_id,const int direction);

/*把文件名转成hash值，插入到共享内存中*/
int shm_insert_file(const int sch_item_id,const char *file,const int size,const time_t ftptime);


#endif

