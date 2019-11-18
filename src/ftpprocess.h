#ifndef FTP_PROCESS_HEADER
#define FTP_PROCESS_HEADER

#define FTP_GET     0
#define FTP_PUT     1
#define SFTP_GET    3
#define SFTP_PUT    4

typedef struct FTP_SERVER_INFO
{
	char	ip[32];
	short port;
	char	user[20];
	char	pass[20];
}FTP_SERVER_INFO;

enum
{
	NO_OVER_WRITE=0,
	DO_OVER_WRITE
};

/*下面的函数都是返回0失败，1成功*/
int ftp_conn( FTP_SERVER_INFO* __srv );
int ftp_login( FTP_SERVER_INFO* __srv );
int ftp_list( const char*  __path, const char* __local );
int ftp_lcd( const char* __path );
int ftp_cd( const char* __path );
int ftp_mkdir(char* __path);
int ftp_get( const char* __local, const char *__remote);
int ftp_put( const char* __local, const char *__remote);
int ftp_del( const char* __remote );
int ftp_rename( const char* __src, const char* __dest);
int ftp_quit();
#undef M
#endif
