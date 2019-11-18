#ifndef DEFINTIONS_H
#define DEFINTIONS_H

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sstream>
using namespace std;
#define ALARM_TYPE_SERIOUES    10 /*业务模块一级告警, 最高级别的告警，建议在需要手工处理故障的时候使用*/
#define ALARM_TYPE_WARNING     11 /*业务模块二级告警, 告警模块的会有一个灯闪烁*/
#define ALARM_TYPE_MSG         12 /*业务模块三级告警，最低级别的告警，告警系统只是记录这个告警信息*/
#define ALARMCMDDIR     "/home/oracle/program/alarm/command" /*每一个机器都不一样*/
#define ALARMCMDNAME    "demo" /*统一使用cmdalarm*/
#define ALARMCMDTYPE     2

#define CFG_MemPoolStdSize      512*1024
#define CFG_STLShMemIdxAlloc    5000
#define CFG_STLShMemAllocatorKey 5001
#define CFG_KeyExampleMap		99
#define CHILDPID_OPERATE_FILE_SEMAPHORE "childpid_operate_file_semaphore"

#ifndef NAME_MAX
#define NAME_MAX 256
#endif

//!Fehlercodes
enum tERROR {
  ERR_OK,             
  ERR_Mutex,
  ERR_NotImplemented
};  

typedef struct TFileInfo_
{
	char FileName[48];		//真正的文件名
	int  nFileSize;			//文件大小
	int  nConflictCount;	//冲突的个数
	time_t ftptime;			//传输的时间
        int   nftpid;
}TFileInfo;


string errcode2str(tERROR err);

#endif
