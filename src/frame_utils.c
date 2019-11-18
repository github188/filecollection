#include <frame_utils.h>
#include <stdlib.h>
#include <stdio.h>
#include <frame_utils.h>
#include <iomanip>
#include <definitions.h>
#include <logprocess.h>
#include <sys/types.h>
#include <unistd.h>
void debugprint(char relevance, string subject, string func) 
{
	LOG_MSG("[%s]: %s",func.c_str(),subject.c_str());
}

void debugprintunix(char relevance, string subject, string func) {
    debugprint(relevance, subject + ": errno(" + int2str(errno) + "): " + strerror(errno), func);
}


double str2double(double& t,const string & ss)                  
{                                              
    t = atof(ss.c_str());
    return t;                                    
}                                              

short str2short(short& t,const string & ss)    
{                                              
    t=(short)atoi(ss.c_str());
    return t;                                    
}                                              

int str2int(int& t,const string & ss)          
{                                                 
    t=atoi(ss.c_str());
    return t;                                    
}            

string time2str(time_t t, const char *fmt)
{
	struct tm m;
	char szbuf[32] = "";
	if(t<=0)
		return "";
	localtime_r(&t,&m);
	strftime(szbuf, sizeof(szbuf), fmt,&m);
	return szbuf;	
}

time_t str2time(const char *str)
{
	if(NULL == str || str[0] == 0 || str[0] == '0')
		return 0;
	struct tm m;
	strptime(str, "%Y%m%d%H%M%S", &m);
	return mktime(&m);
}

    string int2str(int value) 
    {
        char buf[64]="";
        snprintf(buf, sizeof(buf), "%d",value);
        return string(buf);
    }
    
    string dbl2str(double value) 
	{

        char buf[64]="";
        snprintf(buf, sizeof(buf), "%.0lf", value);
        return string(buf);
    }

    string ptr2str(void* pointer) 
    {
        char buf[64]="";
        snprintf(buf, sizeof(buf), "%p",pointer);
        return string(buf);
    }
/******************************************
 *函数名 rtrim
 *参数　char* str 输入(可能会发生改变)
 *输出参数　去除空格后的首地址
 *功能　去除字符串右边的空格及格式字符串
 *******************************************/
char* rtrim(char * str)
{
	char * pEnd = str + strlen(str) - 1;
	while(pEnd >= str &&
		(*pEnd == ' '  ||
		*pEnd == '\t' ||
		*pEnd == '\r' ||
		*pEnd == '\n'))
	{
		*pEnd = '\0';
		pEnd--;
	}
	return str;
}

/******************************************
 *函数名 ltrim
 *参数　char* str 输入(不会发生变化)
 *输出参数　去除空格后的首地址
 *功能　去除字符串左边的空格及格式字符串
 *******************************************/
char* ltrim(char *str)
{
	char *pEnd = str + strlen(str);
	char *pStart = str;
	while(pStart <= pEnd &&
		( *pStart == ' ' || 
		  *pStart == '\t' ||
		  *pStart == '\r' ||
		  *pStart == '\n') )
	{
		pStart ++;
	}
	return pStart;
}

/******************************************
 *函数名 trim
 *参数　char* str 输入(可能会发生变化)
 *输出参数　去除空格后的首地址
 *功能　去除字符串左、右边的空格及格式字符串
 *******************************************/
char* trim(char *str, char *def)
{
	rtrim(str);
	str = ltrim(str);
	if(strlen(str)==0)
		return def;
	else
		return str;
}    

int Alarm(int type, const char *module,const char * msg)	/*type的取值范围是10, 11, 12*/
{
	char cmd[1024] = "";
	char alarmMod[100];
	char folder[256];
	char cmdName[512];
	if(fork()) return 0;

	strcpy(folder, ALARMCMDDIR); //告警程序的路径
	strcpy(cmdName, ALARMCMDNAME);
	strcpy(alarmMod, "./%s %s %d %d \"%s\"");
	/*这个一般来说不需要设，但是有的时候使用双引号不行，需要使用单引号， 也就是说CmdMod要写成./%s %s %d %d '%s' */

	chdir(folder);
	sprintf(cmd, alarmMod, cmdName, module, ALARMCMDTYPE, type, msg);
	system(cmd);
	printf("%s",cmd);
	exit(0);
}

string GetValue(const string &content,const string &key,const string & split)
{
	string::size_type pos = content.find(key);
	if(pos == string::npos)
		return "";
	string stmp=content.substr(pos+key.length()+1);
	pos = stmp.find(split);
	if(pos == string::npos)
		return stmp;
	return stmp.substr(0,pos);
}

unsigned long hash(const char *string)
{
 	return crc32(string);
	//crc32函数比下面代码有较少的冲突
	/*const char *p = string;
	int n = strlen(string);
	unsigned int h = 0;
	unsigned int g;

	while (n--) 
	{
		h = (h << 4) + (*p++);
		if ((g = (h & 0xf0000000)) != 0)
			h ^= g >> 23;
		h &= ~g;
	}

	return h;*/
}    

    unsigned long crc_table_[] =
    {
        0x0,
            0x04c11db7, 0x09823b6e, 0x0d4326d9, 0x130476dc, 0x17c56b6b,
            0x1a864db2, 0x1e475005, 0x2608edb8, 0x22c9f00f, 0x2f8ad6d6,
            0x2b4bcb61, 0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
            0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9, 0x5f15adac,
            0x5bd4b01b, 0x569796c2, 0x52568b75, 0x6a1936c8, 0x6ed82b7f,
            0x639b0da6, 0x675a1011, 0x791d4014, 0x7ddc5da3, 0x709f7b7a,
            0x745e66cd, 0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
            0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5, 0xbe2b5b58,
            0xbaea46ef, 0xb7a96036, 0xb3687d81, 0xad2f2d84, 0xa9ee3033,
            0xa4ad16ea, 0xa06c0b5d, 0xd4326d90, 0xd0f37027, 0xddb056fe,
            0xd9714b49, 0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
            0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1, 0xe13ef6f4,
            0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d, 0x34867077, 0x30476dc0,
            0x3d044b19, 0x39c556ae, 0x278206ab, 0x23431b1c, 0x2e003dc5,
            0x2ac12072, 0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16,
            0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca, 0x7897ab07,
            0x7c56b6b0, 0x71159069, 0x75d48dde, 0x6b93dddb, 0x6f52c06c,
            0x6211e6b5, 0x66d0fb02, 0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1,
            0x53dc6066, 0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
            0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e, 0xbfa1b04b,
            0xbb60adfc, 0xb6238b25, 0xb2e29692, 0x8aad2b2f, 0x8e6c3698,
            0x832f1041, 0x87ee0df6, 0x99a95df3, 0x9d684044, 0x902b669d,
            0x94ea7b2a, 0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e,
            0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2, 0xc6bcf05f,
            0xc27dede8, 0xcf3ecb31, 0xcbffd686, 0xd5b88683, 0xd1799b34,
            0xdc3abded, 0xd8fba05a, 0x690ce0ee, 0x6dcdfd59, 0x608edb80,
            0x644fc637, 0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
            0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f, 0x5c007b8a,
            0x58c1663d, 0x558240e4, 0x51435d53, 0x251d3b9e, 0x21dc2629,
            0x2c9f00f0, 0x285e1d47, 0x36194d42, 0x32d850f5, 0x3f9b762c,
            0x3b5a6b9b, 0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
            0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623, 0xf12f560e,
            0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7, 0xe22b20d2, 0xe6ea3d65,
            0xeba91bbc, 0xef68060b, 0xd727bbb6, 0xd3e6a601, 0xdea580d8,
            0xda649d6f, 0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
            0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7, 0xae3afba2,
            0xaafbe615, 0xa7b8c0cc, 0xa379dd7b, 0x9b3660c6, 0x9ff77d71,
            0x92b45ba8, 0x9675461f, 0x8832161a, 0x8cf30bad, 0x81b02d74,
            0x857130c3, 0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640,
            0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c, 0x7b827d21,
            0x7f436096, 0x7200464f, 0x76c15bf8, 0x68860bfd, 0x6c47164a,
            0x61043093, 0x65c52d24, 0x119b4be9, 0x155a565e, 0x18197087,
            0x1cd86d30, 0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
            0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088, 0x2497d08d,
            0x2056cd3a, 0x2d15ebe3, 0x29d4f654, 0xc5a92679, 0xc1683bce,
            0xcc2b1d17, 0xc8ea00a0, 0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb,
            0xdbee767c, 0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18,
            0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4, 0x89b8fd09,
            0x8d79e0be, 0x803ac667, 0x84fbdbd0, 0x9abc8bd5, 0x9e7d9662,
            0x933eb0bb, 0x97ffad0c, 0xafb010b1, 0xab710d06, 0xa6322bdf,
            0xa2f33668, 0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4};

#define SIZEOF_LONG  4
#if defined(_UNICOS) || (SIZEOF_LONG == 8)
#  define COMPUTE(var, ch) (var) = ( 0x00000000ffffffff & ((var) << 8)) ^ crc_table_[(((var) >> 24) ^ (ch))&0xff]
#else /* _UNICOS */
#  define COMPUTE(var, ch) (var) = ((var) << 8) ^ crc_table_[(((var) >> 24) ^ (ch))&0xff]
#endif /* _UNICOS */
    
        /**Berechnet die CRC32-Pr黤summe eines nullterminierten C-Strings
        */
        unsigned long crc32(const char *string) {
            unsigned long crc = 0;
            unsigned long len = 0;
            for (const char *p = string; *p != 0; ++p) {
                COMPUTE(crc, *p);
                ++len;
            }
            // Include the length of the string.
            for (; len != 0; len >>= 8)
                COMPUTE(crc, len & 0xff);
            return crc;
        }
    

        unsigned long crc32(const char *buffer, unsigned int len) {
            unsigned long crc = 0;
            for (const char *p = buffer; p != buffer + len; ++p)
                COMPUTE(crc, *p);
            // Include the length of the string.
            for (; len != 0; len >>= 8)
                COMPUTE(crc, len & 0xff);
            return crc;
        }
    

        unsigned long crc32(string str) {
            return crc32(str.c_str());
        }
    

        sigfunc* setsignalhandler(int signo, sigfunc* newfunc) {
            struct sigaction act, oact;
            act.sa_handler = newfunc;
            sigemptyset(&act.sa_mask); //No other signals blocked while our signal handler is running
            if (sigaction(signo, &act, &oact) < 0) {
                debugprintunix('E', "error setting signal-handler", "setsignalhandler");
                return NULL;
            } else {
                return oact.sa_handler;
            }
        }
    
        /** Erstellt eine System-V-Semaphore und liefert deren ID zur點k
        */
        int semaphore_create(bool master, string name, int mode) {
            int semid;
            if (master) {
                //Wenn Semaphore schon existiert: l鰏chen
                if ((semid = semget(crc32(name), 1, 0)) != -1)
                    semctl(semid, 0, IPC_RMID);
                //Semaphore anlegen
                semid = semget(crc32(name), 1, mode | IPC_CREAT); //Arg 2: Anzahl Semaphoren; Semphore hat nach Erstellung den Wert "null"
            } else {
                semid = semget(crc32(name), 1, 0);
            }
            if (semid==-1)
                debugprintunix('E',"semaphore creation failed", "semaphore_create");
            return semid;
        }
    
        /**Entfernt eine Sempahore bzw. l鰏t die Verbindung zu einer Semaphore mit angegebener ID
        */
        void semaphore_remove(int semid) {
            if (semid==0)
                debugprint('W',"invalid semaphore ID; ID is zero", "semaphore_remove");
            if (semctl(semid, 0, IPC_RMID) == -1)
                debugprintunix('E',"destroying semaphore failed", "semaphore_remove");
        }
    
        /**Wartet auf ein Signal f黵 Semaphore semid
        */
        void semaphore_wait(int semid, int change) {
            if (semid==0)
                debugprint('W',"invalid semaphore ID; ID is zero", "semaphore_wait");
            struct sembuf sb = {0, change, 0}; //1. Parameter: Sem-Nr.; 2. Parameter: gew黱schte Wert鋘derung; 3. Parameter: Flags (SEM_UNDO etc.)
            int result;                        //3. Parameter: Anzahl zu bearbeitender Semaphoren
            do {
                result = semop(semid, &sb, 1);
                if ((result == -1) && (errno != EINTR))
                    debugprintunix('E',"waiting for semaphore failed", "semaphore_wait");
            } while ((result == -1) && (errno == EINTR));
        }
    
        /**Signalisiert Semaphore semid
        */
        void semaphore_signal(int semid) {
            if (semid==0)
                debugprint('W',"invalid semaphore ID; ID is zero", "semaphore_signal");
            //signal only once
            if (semaphore_getvalue(semid) < 1) {
                //do signalling
                struct sembuf sb = {0, 1, 0};
                if (semop(semid, &sb, 1) == -1)
                    debugprintunix('E',"error signaling semaphore", "semaphore_signal");
            }
        }
    
        /**Liefert den aktuellen Wert der Semaphore semid zur點k
        */
        int semaphore_getvalue(int semid) {
            if (semid==0)
                debugprint('W',"invalid semaphore ID; ID is zero", "semaphore_getvalue");
            return semctl(semid, 0, GETVAL, NULL);
        }
