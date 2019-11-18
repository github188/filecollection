#ifndef TASK_ITEM_HEADER
#define TASK_ITEM_HEADER
#include <time.h>
#include <string.h>
#include "fileprocess.h"
#include "sftp.h"
#include "schedule.h"

/*
#���Ǹýṹ˵����ʵ���ĵ�
#ParmFile�������ļ���ʽ˵��:
#[����] + [Сʱ] + [����] + [�·�] + [��] + [FTP��������Ϣ] + [FTP·��] + [����·��] + [�ļ���(�������ʾʽ��ʾ)] + [���ͷ���] + [�Ƿ�ɾ��Դ�ļ�] + [��ɺ�ִ�еĽű�]
#���еļӺ��ÿո����.
#���ֶ�˵��:
#[����] + [Сʱ] + [����] + [�·�] + [��]:��crontabһ��,֧��(����, *, * / ����, "ʱ��,ʱ��")���ַ�ʽ.
#[FTP��������Ϣ]: user:pass:ip:port
#[FTP·��]+[����·��]:����·��
#[�ļ���(�������ʾʽ��ʾ)]:�ļ���������������ʽ��ʾ.
#[���ͷ���]:0��ʾ��FTP������,1��ʾ���ص�FTP.
#[�Ƿ�ɾ��Դ�ļ�]:0��ʾ��ɾ��,1��ʾɾ��.
#[��ɺ�ִ�еĽű�]:��ʾ���سɹ�����ִ�еĽű�
*/

/*
 *UPDATE-LOG:
 *	2005-03-28:	����һ���������ԣ�˵������������Ƿ�ʹ����ʱ�ļ���
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
	time_t			file_ctime_escape;/*�ļ�����ʱ������*/
	int				use_tmp;/*2005-03-28�ռ�*/
	int             copy_flag;/*2009-09-07����0������������1�����������*/
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
 ��Ҫ����ӵĹ���
 1.�����ļ����ļ�������������ڹ���
*/

/*�õ�ITEM��FTP��������Ϣ*/
struct FTP_SERVER_INFO*
get_ftp_info( struct TASK_ITEM* __item);

/**/
int 
cp_item_to_dir( const char* __format, const char *__file, const char *__file_path, int copy_flag );

/*�õ�������ĳ��ID��ITEM*/
struct TASK_ITEM*
get_task_item_by_id( int __id );

/*���������ļ������б�*/
void
create_task_item_list_by_ini( const char* __file );


/*
 *�õ�FTP���ļ����б�
 */
struct FTP_FILE_INFO* get_ftp_file_list( pool* __pool, TASK_ITEM* __item, const char* __remote);

/*
 *�õ�SFTP���ļ����б�
 */
struct FTP_FILE_INFO* get_sftp_file_list( pool* __pool, SFTP *sftp ,TASK_ITEM* __item, const char* __remote);


/*
 *�õ������ļ��б�
 */
struct FTP_FILE_INFO*
get_local_file_list(pool* __pool, const char* __local);

/*
 *�Ƿ����ļ��б��д���
 */
int
if_in_list_exist( FTP_FILE_INFO* __list, FTP_FILE_INFO* __item );

/*
 *�滻�����ļ�PATH��ʱ�����
 */
int
trans_path_str( const char *__src, char *__dest, int __size );

const char * GetTime(time_t t);

/*��ȡ�ļ��Ƿ��Ѿ��������*/
int read_ftped_file(const int sch_item_id,const char *file,const int nFileSize,const int direction);

/*д�Ѿ���������ļ���hash�����ڴ�*/
int write_ftped_file(const int sch_item_id,const char *file,const int nFileSize,const int direction);

/*����ÿ��ftp�����Ƿ�װ�����Ѿ���������ļ�havsֵ*/
bool ftpid_hash_file_loaded(const int sch_item_id);

/*װ��ÿ��FTP���̵��Ѿ���������ļ�����hash�ڴ�*/
int load_file_hash_value(const int sch_item_id,const int direction);

/*���ļ���ת��hashֵ�����뵽�����ڴ���*/
int shm_insert_file(const int sch_item_id,const char *file,const int size,const time_t ftptime);


#endif

