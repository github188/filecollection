#ifndef _SFTP_H_
#define _SFTP_H_

#include <list>
#include "ssh.h"

class SFTP : public SSH 
{
   private:
       void* sftpSession;

       SFTP(const SFTP&);
       SFTP& operator=(const SFTP&);

   public:
       SFTP(string, unsigned short = 22);
       virtual ~SFTP(void);

       virtual bool openSession(const string&, const string&);
       virtual void closeSession(void);

       bool exists(const string&) const;
       unsigned long size(const string&) const;
       unsigned long sizeEx(const void*) const;
       void* open(const string&, const unsigned int) const;
       void seek(const void*, const unsigned long) const;
       long read(const void*, char*, const unsigned int) const;
       long write(const void*, const char*, const unsigned int) const;
       int close(const void*) const;
       bool mkdir(const string&) const;
       void rmdir(const string&) const;
       list<string> ls(const string& = "./", const unsigned int = FILTER_FILE | FILTER_PATH | FILTER_LONG) const;
       void rm(const string&) const;
       bool rename(const string&, const string&) const;

       static const unsigned int READ;
       static const unsigned int WRITE;
       static const unsigned int CREATE;
       static const unsigned int TRUNC;

       static const unsigned int FILTER_FILE;  //文件名
       static const unsigned int FILTER_PATH;  //文件夹
       static const unsigned int FILTER_LONG;  //文件信息
    public:
        int sftp_list( const char* local_path, const char* remote_path );
        int sftp_get( const char* local_path, const char * remote_path );
        int sftp_put( const char* local_path, const char * remote_path );
};
#endif // _SFTP_H_