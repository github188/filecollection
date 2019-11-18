#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include "libssh2.h"
#include "libssh2_sftp.h"
#include "sftp.h"

const unsigned int SFTP::READ(0x00000001);
const unsigned int SFTP::WRITE(0x00000002);
const unsigned int SFTP::CREATE(0x00000008);
const unsigned int SFTP::TRUNC(0x00000010);

const unsigned int SFTP::FILTER_FILE(0x00000001);
const unsigned int SFTP::FILTER_PATH(0x00000002);
const unsigned int SFTP::FILTER_LONG(0x00000004);

SFTP::SFTP(string ipAddress, unsigned short tcpPort) : SSH(ipAddress, tcpPort) 
{

}

SFTP::~SFTP(void) 
{
   this->closeSession();
}

bool SFTP::openSession(const string& userName, const string& password) 
{
    if (this->connected)
       return true;
    if (!this->connectTo(userName, password))
       return false;
   this->sftpSession = libssh2_sftp_init((LIBSSH2_SESSION*)this->session);
    if (!this->sftpSession) 
    {
       this->disconnect();
       return false;
    }
    return true;
}

void SFTP::closeSession(void) 
{
    if (this->connected) 
    {
       libssh2_sftp_shutdown((LIBSSH2_SFTP*)this->sftpSession);
       this->disconnect();
    }
}

bool SFTP::exists(const string& fullName) const 
{
    if (this->connected) 
    {
       LIBSSH2_SFTP_HANDLE *sftpHandle = libssh2_sftp_open((LIBSSH2_SFTP*)this->sftpSession, fullName.c_str(), LIBSSH2_FXF_READ, LIBSSH2_SFTP_OPENFILE);
       if (!sftpHandle)
           return false;
       libssh2_sftp_close(sftpHandle);
       return true;
    }
    return false;
}

unsigned long SFTP::size(const string& fullName) const 
{
    if (this->connected) 
    {
       LIBSSH2_SFTP_HANDLE* sftpHandle = libssh2_sftp_open((LIBSSH2_SFTP*)this->sftpSession, fullName.c_str(), LIBSSH2_FXF_READ, LIBSSH2_SFTP_OPENFILE);
       unsigned long r = this->sizeEx(sftpHandle);
       if (sftpHandle)
           libssh2_sftp_close(sftpHandle);
       return r;
    }
    return 0;
}

unsigned long SFTP::sizeEx(const void* handle) const 
{
    if (!handle)
       return 0;
   LIBSSH2_SFTP_ATTRIBUTES attr;
    if (!libssh2_sftp_fstat((LIBSSH2_SFTP_HANDLE*)handle, &attr))
       return attr.filesize;
    return 0;
}

void* SFTP::open(const string& fullName, const unsigned int flag) const 
{
    return libssh2_sftp_open((LIBSSH2_SFTP*)this->sftpSession, fullName.c_str(), flag, LIBSSH2_SFTP_OPENFILE);
}

void SFTP::seek(const void* handle, const unsigned long offset) const 
{
   libssh2_sftp_seek64((LIBSSH2_SFTP_HANDLE*)handle, offset);
}

long SFTP::read(const void* handle, char* buff, const unsigned int size) const 
{
    return libssh2_sftp_read((LIBSSH2_SFTP_HANDLE*)handle, buff, size);
}

long SFTP::write(const void* handle, const char* buff, const unsigned int size) const 
{
    return libssh2_sftp_write((LIBSSH2_SFTP_HANDLE*)handle, buff, size);
}

int SFTP::close(const void* handle) const 
{
    return libssh2_sftp_close((LIBSSH2_SFTP_HANDLE*)handle);
}

bool SFTP::mkdir(const string& pathName) const 
{
    if (this->connected)
       return libssh2_sftp_mkdir((LIBSSH2_SFTP*)this->sftpSession,
                                 pathName.c_str(),
                                 LIBSSH2_SFTP_S_IRWXU |
                                 LIBSSH2_SFTP_S_IRGRP |
                                 LIBSSH2_SFTP_S_IXGRP |
                                 LIBSSH2_SFTP_S_IROTH |
                                 LIBSSH2_SFTP_S_IXOTH) == 0;
    return false;
}

void SFTP::rmdir(const string& pathName) const 
{
    if (this->connected)
       libssh2_sftp_rmdir((LIBSSH2_SFTP*)this->sftpSession, pathName.c_str());
}

list<string> SFTP::ls(const string& path, const unsigned int filter) const 
{
   list<string> result;
    if (this->connected) 
    {
       LIBSSH2_SFTP_HANDLE* sftpHandle = libssh2_sftp_opendir((LIBSSH2_SFTP*)this->sftpSession, path.c_str());
       if (sftpHandle) 
       {
           char buff[512];
           char longentry[512];
           LIBSSH2_SFTP_ATTRIBUTES attrs;
           if (filter == (FILTER_FILE | FILTER_PATH | FILTER_LONG)) 
           {
               while(libssh2_sftp_readdir_ex(sftpHandle, buff, sizeof(buff), longentry, sizeof(longentry), &attrs) > 0) 
               {
                   //过滤掉本级目录和上一级目录
                   if( strcmp(buff,".") == 0 || strcmp(buff,"..") == 0 ) continue;
                   if (longentry[0] != '\0')
                       result.push_back(longentry);
                   else
                       result.push_back(buff);
               }
           }
           else if (filter == (FILTER_FILE | FILTER_PATH )) 
           {
               while(libssh2_sftp_readdir_ex(sftpHandle, buff, sizeof(buff), longentry, sizeof(longentry), &attrs) > 0)
               {
                   //过滤掉本级目录和上一级目录
                   if( strcmp(buff,".") == 0 || strcmp(buff,"..") == 0 ) continue;
                   result.push_back(buff);
               }
           }
           else if (filter == (FILTER_FILE | FILTER_LONG)) 
           {
               while(libssh2_sftp_readdir_ex(sftpHandle, buff, sizeof(buff), longentry, sizeof(longentry), &attrs) > 0)
               {
                   //过滤掉本级目录和上一级目录
                   if( strcmp(buff,".") == 0 || strcmp(buff,"..") == 0 ) continue;
                   if (longentry[0] != '\0' && longentry[0] != 'd') result.push_back(longentry);
               }
           }
           else if (filter == (FILTER_PATH | FILTER_LONG)) 
           {
               while(libssh2_sftp_readdir_ex(sftpHandle, buff, sizeof(buff), longentry, sizeof(longentry), &attrs) > 0)
               {
                   //过滤掉本级目录和上一级目录
                   if( strcmp(buff,".") == 0 || strcmp(buff,"..") == 0 ) continue;
                   if (longentry[0] != '\0' && longentry[0] == 'd') result.push_back(longentry);
               }
           }
           else if (filter == FILTER_FILE) 
           {
               while(libssh2_sftp_readdir_ex(sftpHandle, buff, sizeof(buff), longentry, sizeof(longentry), &attrs) > 0)
               {
                   //过滤掉本级目录和上一级目录
                   if( strcmp(buff,".") == 0 || strcmp(buff,"..") == 0 ) continue;
                   if (longentry[0] != '\0' && longentry[0] != 'd') result.push_back(buff);
               }
           }
           else if (filter == FILTER_PATH) 
           {
               while(libssh2_sftp_readdir_ex(sftpHandle, buff, sizeof(buff), longentry, sizeof(longentry), &attrs) > 0)
               {
                   if( strcmp(buff,".") == 0 || strcmp(buff,"..") == 0 ) continue;
                   if (longentry[0] != '\0' && longentry[0] == 'd') result.push_back(buff);
               }
           }
           libssh2_sftp_closedir(sftpHandle);
       }
    }
    return result;
}

void SFTP::rm(const string& fullName) const 
{
    if (this->connected)
       libssh2_sftp_unlink((LIBSSH2_SFTP*)this->sftpSession, fullName.c_str());
}

bool SFTP::rename(const string& src, const string& obj) const 
{
    if (this->connected)
       return libssh2_sftp_rename((LIBSSH2_SFTP*)this->sftpSession, src.c_str(), obj.c_str()) == 0;
    return false;
}

int SFTP::sftp_list( const char* local_path, const char* remote_path )
{
    if (!this->connected) return 0;
    FILE *pfile = fopen(local_path,"w+");
    if( pfile == NULL )
    {
        LOG_MSG("sftp_list open file [%s] error[%s]\n", local_path,strerror(errno));
        return 0;
    }
    list<string> fileList = ls(remote_path);
    for(list<string>::iterator iter = fileList.begin(); iter != fileList.end(); iter++)
    {
        fprintf(pfile,"%s\n",(*iter).c_str());
    }
    fclose(pfile);
    return 1;
}

int SFTP::sftp_get( const char* local_path, const char* remote_path )
{
    char buff[1024] = {0};
    long size = 0L;
    if (!this->connected) return 0;
    FILE *pfile = fopen(local_path,"wb");
    if(pfile == NULL)
    {
        LOG_MSG("sftp_get open file [%s] error[%s]\n", local_path,strerror(errno));
        return 0;
    }
    void *sftp_handle = open(remote_path,LIBSSH2_FXF_READ);
    if(!sftp_handle)
    {
        LOG_MSG("unable to open file[%s],return code: %ld\n",remote_path,
                libssh2_sftp_last_error((LIBSSH2_SFTP *)sftpSession));
        return 0;
    }
    do
    {
        size = read(sftp_handle, buff, sizeof(buff));
        if(size > 0)
        {
            fwrite(buff,sizeof(char),size,pfile);
        }
        else
        {
            break;
        }
    } while(1);
    fclose(pfile);
    return 1;
}

int SFTP::sftp_put( const char* local_path, const char* remote_path )
{
    char buff[1024*100] = {0};
    size_t nRead = 0;
    long size = 0L;
    char *ptr = NULL;
    if (!this->connected) return 0;
    FILE *pfile = fopen(local_path,"rb");
    if(pfile == NULL)
    {
        LOG_MSG("sftp_get open file [%s] error[%s]\n", local_path,strerror(errno));
        return 0;
    }
    void *sftp_handle = open(remote_path,LIBSSH2_FXF_WRITE|LIBSSH2_FXF_CREAT|LIBSSH2_FXF_TRUNC);
    if(!sftp_handle)
    {
        LOG_MSG("unable to open file[%s],return code: %ld\n",remote_path,
                libssh2_sftp_last_error((LIBSSH2_SFTP *)sftpSession));
        return 0;
    }
    do
    {
        nRead = fread(buff, 1, sizeof(buff), pfile);
        if(nRead <= 0)
        {
            break;
        }
        ptr = buff;
        do
        {
            size = write(sftp_handle, ptr, nRead);
            if(size < 0)
                break;
            ptr += size;
            nRead -= size;
        } while(nRead);
    } while(size > 0);
    fclose(pfile);
    return 1;
}
