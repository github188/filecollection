#ifndef DEFINTIONS_H
#define DEFINTIONS_H

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sstream>
using namespace std;
#define ALARM_TYPE_SERIOUES    10 /*ҵ��ģ��һ���澯, ��߼���ĸ澯����������Ҫ�ֹ�������ϵ�ʱ��ʹ��*/
#define ALARM_TYPE_WARNING     11 /*ҵ��ģ������澯, �澯ģ��Ļ���һ������˸*/
#define ALARM_TYPE_MSG         12 /*ҵ��ģ�������澯����ͼ���ĸ澯���澯ϵͳֻ�Ǽ�¼����澯��Ϣ*/
#define ALARMCMDDIR     "/home/oracle/program/alarm/command" /*ÿһ����������һ��*/
#define ALARMCMDNAME    "demo" /*ͳһʹ��cmdalarm*/
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
	char FileName[48];		//�������ļ���
	int  nFileSize;			//�ļ���С
	int  nConflictCount;	//��ͻ�ĸ���
	time_t ftptime;			//�����ʱ��
        int   nftpid;
}TFileInfo;


string errcode2str(tERROR err);

#endif
