#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "logprocess.h"
#include "fileprocess.h"



char *get_file_name(const char *__pathname, char *__buf, int __size)
{
	const char _path_separator = '/';
	const char *_s = strrchr(__pathname, _path_separator);
	memset(__buf, 0, __size);
	if ( _s==NULL )
		_s=__pathname;
	else
		_s++;

	if ( (int)strlen(_s) > __size-1 )
	{
		log_msg( g_log_file, LOG_BY_MONTH, "err!get_file_name buf is small.");
		return __buf;
	}
	
	return strncpy(__buf, _s,__size-1);
}

char *get_dir_name(const char *__pathname, char *__buf, int __size)
{
	const char _path_separator = '/';
	const char *_s = strrchr(__pathname, _path_separator);
	memset(__buf, 0, __size);
	if ( _s != NULL )
	{
		if ( (_s-__pathname)>__size-1 )
		{
			log_msg( g_log_file, LOG_BY_MONTH, "err!get_dir_name buf is small.");
			return __buf;
		}
		else
			return strncpy( __buf, __pathname, _s-__pathname);
	}	
	return __buf;
}

int if_file_read(const char *__pathname)
{
	return access(__pathname, R_OK)+1;
}
int if_file_write(const char *__pathname)
{
	return access(__pathname, W_OK)+1;
}
int if_file_exist(const char *__pathname)
{
	return access(__pathname, F_OK)+1;
}

/*
 *if_isfile, 1 file, 0 not file, -1 error
 */
int if_isfile(const char *__pathname)
{
	struct stat64 _stat;
	
	if ( stat64( __pathname, &_stat)<0 )
	{
		return -1;
	}
	return S_ISREG(_stat.st_mode) ? 1:0;                 
}

int if_isdir(const char *__pathname)
{
	struct stat64 _stat;
	
	if ( stat64( __pathname, &_stat)<0 )
	{
		return -1;
	}
	return S_ISDIR(_stat.st_mode) ? 1:0;  
}

int if_dir_read(const char *__pathname)
{
	struct stat64 _stat;
	
	if ( stat64( __pathname, &_stat)<0 )
	{
		return -1;
	}
	return (S_IRUSR&_stat.st_mode)==S_IRUSR ? 1:0; 
}
int if_dir_write(const char *__pathname)
{	
	struct stat64 _stat;
	
	if ( stat64( __pathname, &_stat)<0 )
	{
		return -1;
	}
	return (S_IWUSR&_stat.st_mode)==S_IWUSR  ? 1:0; 
	
}
long get_file_size(const char *__pathname)
{	
	struct stat64 _stat;
	
	if (  if_isdir(__pathname) )
		return 0;
	
	if ( stat64( __pathname, &_stat)<0 )
	{
		return -1;
	}
	return _stat.st_size; 
}

time_t get_file_last_modify(const char *__pathname)
{
	struct stat64 _stat;
	
	if (  if_isdir(__pathname) )
		return -1;
	
	if ( stat64( __pathname, &_stat)<0 )
	{
		return -1;
	}
	return _stat.st_mtime; 
}

int cp_file(const char *__src, const char *__dest, int __if_over)
{
	int from_fd,to_fd; 
	int bytes_read,bytes_write; 
	char buffer[BUFSIZ]; 
	char *ptr; 

	/*目标文件是否存在*/
	if ( if_file_exist(__dest) && (__if_over==0) )
	{
		log_msg(g_log_file, LOG_BY_MONTH, "Dest File %s Exits.", __dest );
		return 0;
	}
	
	/*打开源文件*/ 
	if((from_fd=open(__src,O_RDONLY))==-1) 
	{ 
		log_msg(g_log_file, LOG_BY_MONTH, "Open %s Error:%s\n", __src, strerror(errno)); 
		return 0;
	} 

	/* 创建目的文件 */ 
	if((to_fd=open(__dest,O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR|S_IROTH))==-1) 
	{ 
		log_msg(g_log_file, LOG_BY_MONTH, "Open %s Error:%s\n", __dest,strerror(errno)); 
		return 0;
	} 
	/* 以下代码是一个经典的拷贝文件的代码 */ 
	while((bytes_read=read(from_fd,buffer,BUFSIZ))!=0) 
	{ 
		/* 一个致命的错误发生了 */ 
		if((bytes_read==-1)&&(errno!=EINTR)) break; 
		else 
		if(bytes_read>0) 
		{ 
			ptr=buffer; 
			while((bytes_write=write(to_fd,ptr,bytes_read))!=0) 
			{ 
				/* 一个致命错误发生了 */ 
				if((bytes_write==-1)&&(errno!=EINTR))break; 
				/* 写完了所有读的字节 */ 
				else if(bytes_write==bytes_read) break; 
				/* 只写了一部分,继续写 */ 
				else if(bytes_write>0) 
				{ 
					ptr+=bytes_write; 
					bytes_read-=bytes_write; 
				} 
			} 
			/* 写的时候发生的致命错误 */ 
			if(bytes_write==-1)break; 
		} 
	}
	close(from_fd); 
	close(to_fd); 
	if ( bytes_write==-1 )
		return 0;
	return 1; 
} 

int GetFileRows(const char *__pathname)
{
	int nRows = 0;
	static char buf[8192] = "";
	FILE *pfile =  fopen64(__pathname, "a+");
	if(NULL == pfile)
	{
		log_msg(g_log_file, LOG_BY_MONTH, "Open %s Error:%s\n", __pathname, strerror(errno)); 
		return -1;
	}
	while(fgets(buf,sizeof(buf),pfile)) ++nRows;
	fclose(pfile);
	return nRows;
}


