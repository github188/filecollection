
/* Result Sets Interface */
#ifndef SQL_CRSR
#  define SQL_CRSR
  struct sql_cursor
  {
    unsigned int curocn;
    void *ptr1;
    void *ptr2;
    unsigned int magic;
  };
  typedef struct sql_cursor sql_cursor;
  typedef struct sql_cursor SQL_CURSOR;
#endif /* SQL_CRSR */

/* Thread Safety */
typedef void * sql_context;
typedef void * SQL_CONTEXT;

/* Object support */
struct sqltvn
{
  unsigned char *tvnvsn; 
  unsigned short tvnvsnl; 
  unsigned char *tvnnm;
  unsigned short tvnnml; 
  unsigned char *tvnsnm;
  unsigned short tvnsnml;
};
typedef struct sqltvn sqltvn;

struct sqladts
{
  unsigned int adtvsn; 
  unsigned short adtmode; 
  unsigned short adtnum;  
  sqltvn adttvn[1];       
};
typedef struct sqladts sqladts;

static struct sqladts sqladt = {
  1,1,0,
};

/* Binding to PL/SQL Records */
struct sqltdss
{
  unsigned int tdsvsn; 
  unsigned short tdsnum; 
  unsigned char *tdsval[1]; 
};
typedef struct sqltdss sqltdss;
static struct sqltdss sqltds =
{
  1,
  0,
};

/* File name & Package Name */
struct sqlcxp
{
  unsigned short fillen;
           char  filnam[13];
};
static const struct sqlcxp sqlfpn =
{
    12,
    "dbprocess.pc"
};


static unsigned int sqlctx = 287811;


static struct sqlexd {
   unsigned long  sqlvsn;
   unsigned int   arrsiz;
   unsigned int   iters;
   unsigned int   offset;
   unsigned short selerr;
   unsigned short sqlety;
   unsigned int   occurs;
      const short *cud;
   unsigned char  *sqlest;
      const char  *stmt;
   sqladts *sqladtp;
   sqltdss *sqltdsp;
   unsigned char  **sqphsv;
   unsigned long  *sqphsl;
            int   *sqphss;
            short **sqpind;
            int   *sqpins;
   unsigned long  *sqparm;
   unsigned long  **sqparc;
   unsigned short  *sqpadto;
   unsigned short  *sqptdso;
   unsigned int   sqlcmax;
   unsigned int   sqlcmin;
   unsigned int   sqlcincr;
   unsigned int   sqlctimeout;
   unsigned int   sqlcnowait;
            int   sqfoff;
   unsigned int   sqcmod;
   unsigned int   sqfmod;
   unsigned char  *sqhstv[23];
   unsigned long  sqhstl[23];
            int   sqhsts[23];
            short *sqindv[23];
            int   sqinds[23];
   unsigned long  sqharm[23];
   unsigned long  *sqharc[23];
   unsigned short  sqadto[23];
   unsigned short  sqtdso[23];
} sqlstm = {12,23};

// Prototypes
extern "C" {
  void sqlcxt (void **, unsigned int *,
               struct sqlexd *, const struct sqlcxp *);
  void sqlcx2t(void **, unsigned int *,
               struct sqlexd *, const struct sqlcxp *);
  void sqlbuft(void **, char *);
  void sqlgs2t(void **, char *);
  void sqlorat(void **, unsigned int *, void *);
}

// Forms Interface
static const int IAPSUCC = 0;
static const int IAPFAIL = 1403;
static const int IAPFTL  = 535;
extern "C" { void sqliem(char *, int *); }

 static const char *sq0006 = 
"select ftpparm_id ,ftp_host ,ftp_path ,local_path ,Nvl(file_pattern,'^.*') \
,direction ,Nvl(month,'*') ,Nvl(day,'*') ,Nvl(hour,'*') ,Nvl(min,'*') ,del_sr\
c ,Nvl(sh_file,'NO') ,Nvl(son_dir_mask,'NULL') ,Nvl(cp_dir_1,'NULL') ,Nvl(cp_\
dir_2,'NULL') ,Nvl(cp_dir_3,'NULL') ,Nvl(cp_dir_4,'NULL') ,file_modify_time_e\
scape ,generate_db_cdr ,use_tmp ,nvl(copy_flag,0) ,ftp_flag ,to_char(sysdate,\
'YYYYMMDDHH24MISS')  from tb_cfg_filecollection where (Program_id=:b0 and opt\
_time>=to_date(:b1,'YYYYMMDDHH24MISS'))           ";

 static const char *sq0007 = 
"select ftpparm_id ,ftp_host ,ftp_path ,local_path ,Nvl(file_pattern,'^.*') \
,direction ,Nvl(month,'*') ,Nvl(day,'*') ,Nvl(hour,'*') ,Nvl(min,'*') ,del_sr\
c ,Nvl(sh_file,'NO') ,Nvl(son_dir_mask,'NULL') ,Nvl(cp_dir_1,'NULL') ,Nvl(cp_\
dir_2,'NULL') ,Nvl(cp_dir_3,'NULL') ,Nvl(cp_dir_4,'NULL') ,file_modify_time_e\
scape ,generate_db_cdr ,use_tmp ,nvl(copy_flag,0) ,ftp_flag ,to_char(sysdate,\
'YYYYMMDDHH24MISS')  from tb_cfg_filecollection where (Program_id=:b0 and Ftp\
_Flag=1)           ";

typedef struct { unsigned short len; unsigned char arr[1]; } VARCHAR;
typedef struct { unsigned short len; unsigned char arr[1]; } varchar;

/* cud (compilation unit data) array */
static const short sqlcud0[] =
{12,4130,852,0,0,
5,0,0,1,0,0,27,26,0,0,4,4,0,2,5,68,66,50,51,54,1,97,0,0,1,97,0,0,1,97,0,0,1,10,
0,0,
41,0,0,2,0,0,30,36,0,0,0,0,0,2,5,68,66,50,51,54,
61,0,0,3,0,0,17,71,0,0,1,1,0,2,5,68,66,50,51,54,1,97,0,0,
85,0,0,3,0,0,21,72,0,0,0,0,0,2,5,68,66,50,51,54,
105,0,0,4,0,0,29,93,0,0,0,0,0,2,5,68,66,50,51,54,
125,0,0,5,0,0,31,99,0,0,0,0,0,2,5,68,66,50,51,54,
145,0,0,6,510,0,9,143,0,0,2,2,0,2,5,68,66,50,51,54,1,3,0,0,1,97,0,0,
173,0,0,6,0,0,13,146,0,0,23,0,0,2,5,68,66,50,51,54,2,3,0,0,2,97,0,0,2,97,0,0,2,
97,0,0,2,97,0,0,2,3,0,0,2,97,0,0,2,97,0,0,2,97,0,0,2,97,0,0,2,3,0,0,2,97,0,0,2,
97,0,0,2,97,0,0,2,97,0,0,2,97,0,0,2,97,0,0,2,3,0,0,2,3,0,0,2,3,0,0,2,3,0,0,2,3,
0,0,2,97,0,0,
285,0,0,6,0,0,15,223,0,0,0,0,0,2,5,68,66,50,51,54,
305,0,0,7,479,0,9,264,0,0,1,1,0,2,5,68,66,50,51,54,1,3,0,0,
329,0,0,7,0,0,13,267,0,0,23,0,0,2,5,68,66,50,51,54,2,3,0,0,2,97,0,0,2,97,0,0,2,
97,0,0,2,97,0,0,2,3,0,0,2,97,0,0,2,97,0,0,2,97,0,0,2,97,0,0,2,3,0,0,2,97,0,0,2,
97,0,0,2,97,0,0,2,97,0,0,2,97,0,0,2,97,0,0,2,3,0,0,2,3,0,0,2,3,0,0,2,3,0,0,2,3,
0,0,2,9,0,0,
441,0,0,7,0,0,15,316,0,0,0,0,0,2,5,68,66,50,51,54,
};


#line 1 "dbprocess.pc"
#include <stdio.h>
#include "dbprocess.h"
#include "frame_utils.h"
#include "logprocess.h"
#include "iniprocess.h"
#include "taskitem.h"
#include "taskmanager.h"

extern char *TASK_INI;
extern char g_db_time[17]; 
/* EXEC SQL DECLARE db236 DATABASE; */ 
#line 11 "dbprocess.pc"

/* EXEC SQL INCLUDE SQLCA;
 */ 
#line 1 "/oracle/10.2.0/db/precomp/public/SQLCA.H"
/*
 * $Header: sqlca.h 24-apr-2003.12:50:58 mkandarp Exp $ sqlca.h 
 */

/* Copyright (c) 1985, 2003, Oracle Corporation.  All rights reserved.  */
 
/*
NAME
  SQLCA : SQL Communications Area.
FUNCTION
  Contains no code. Oracle fills in the SQLCA with status info
  during the execution of a SQL stmt.
NOTES
  **************************************************************
  ***                                                        ***
  *** This file is SOSD.  Porters must change the data types ***
  *** appropriately on their platform.  See notes/pcport.doc ***
  *** for more information.                                  ***
  ***                                                        ***
  **************************************************************

  If the symbol SQLCA_STORAGE_CLASS is defined, then the SQLCA
  will be defined to have this storage class. For example:
 
    #define SQLCA_STORAGE_CLASS extern
 
  will define the SQLCA as an extern.
 
  If the symbol SQLCA_INIT is defined, then the SQLCA will be
  statically initialized. Although this is not necessary in order
  to use the SQLCA, it is a good pgming practice not to have
  unitialized variables. However, some C compilers/OS's don't
  allow automatic variables to be init'd in this manner. Therefore,
  if you are INCLUDE'ing the SQLCA in a place where it would be
  an automatic AND your C compiler/OS doesn't allow this style
  of initialization, then SQLCA_INIT should be left undefined --
  all others can define SQLCA_INIT if they wish.

  If the symbol SQLCA_NONE is defined, then the SQLCA variable will
  not be defined at all.  The symbol SQLCA_NONE should not be defined
  in source modules that have embedded SQL.  However, source modules
  that have no embedded SQL, but need to manipulate a sqlca struct
  passed in as a parameter, can set the SQLCA_NONE symbol to avoid
  creation of an extraneous sqlca variable.
 
MODIFIED
    lvbcheng   07/31/98 -  long to int
    jbasu      12/12/94 -  Bug 217878: note this is an SOSD file
    losborne   08/11/92 -  No sqlca var if SQLCA_NONE macro set 
  Clare      12/06/84 - Ch SQLCA to not be an extern.
  Clare      10/21/85 - Add initialization.
  Bradbury   01/05/86 - Only initialize when SQLCA_INIT set
  Clare      06/12/86 - Add SQLCA_STORAGE_CLASS option.
*/
 
#ifndef SQLCA
#define SQLCA 1
 
struct   sqlca
         {
         /* ub1 */ char    sqlcaid[8];
         /* b4  */ int     sqlabc;
         /* b4  */ int     sqlcode;
         struct
           {
           /* ub2 */ unsigned short sqlerrml;
           /* ub1 */ char           sqlerrmc[70];
           } sqlerrm;
         /* ub1 */ char    sqlerrp[8];
         /* b4  */ int     sqlerrd[6];
         /* ub1 */ char    sqlwarn[8];
         /* ub1 */ char    sqlext[8];
         };

#ifndef SQLCA_NONE 
#ifdef   SQLCA_STORAGE_CLASS
SQLCA_STORAGE_CLASS struct sqlca sqlca
#else
         struct sqlca sqlca
#endif
 
#ifdef  SQLCA_INIT
         = {
         {'S', 'Q', 'L', 'C', 'A', ' ', ' ', ' '},
         sizeof(struct sqlca),
         0,
         { 0, {0}},
         {'N', 'O', 'T', ' ', 'S', 'E', 'T', ' '},
         {0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0}
         }
#endif
         ;
#endif
 
#endif
 
/* end SQLCA */
/* EXEC SQL INCLUDE SQLDA;
 */ 
#line 1 "/oracle/10.2.0/db/precomp/public/SQLDA.H"
/*
 * $Header: sqlda.h 08-may-2002.12:13:42 apopat Exp $ sqlda.h 
 */

/***************************************************************
*      The SQLDA descriptor definition                         *
*--------------------------------------------------------------*
*      VAX/3B Version                                          *
*                                                              *
* Copyright (c) 1987, 2002, Oracle Corporation.  All rights reserved.  *
***************************************************************/


/* NOTES
  **************************************************************
  ***                                                        ***
  *** This file is SOSD.  Porters must change the data types ***
  *** appropriately on their platform.  See notes/pcport.doc ***
  *** for more information.                                  ***
  ***                                                        ***
  **************************************************************
*/

/*  MODIFIED
    apopat     05/08/02  - [2362423] MVS PE to make lines shorter than 79
    apopat     07/31/99 -  [707588] TAB to blanks for OCCS
    lvbcheng   10/27/98 -  change long to int for sqlda
    lvbcheng   08/15/97 -  Move sqlda protos to sqlcpr.h
    lvbcheng   06/25/97 -  Move sqlda protos to this file
    jbasu      01/29/95 -  correct typo
    jbasu      01/27/95 -  correct comment - ub2->sb2
    jbasu      12/12/94 - Bug 217878: note this is an SOSD file
    Morse      12/01/87 - undef L and S for v6 include files
    Richey     07/13/87 - change int defs to long 
    Clare      09/13/84 - Port: Ch types to match SQLLIB structs
    Clare      10/02/86 - Add ifndef SQLDA
*/

#ifndef SQLDA_
#define SQLDA_ 1
 
#ifdef T
# undef T
#endif
#ifdef F
# undef F
#endif

#ifdef S
# undef S
#endif
#ifdef L
# undef L
#endif
 
struct SQLDA {
  /* ub4    */ int        N; /* Descriptor size in number of entries        */
  /* text** */ char     **V; /* Ptr to Arr of addresses of main variables   */
  /* ub4*   */ int       *L; /* Ptr to Arr of lengths of buffers            */
  /* sb2*   */ short     *T; /* Ptr to Arr of types of buffers              */
  /* sb2**  */ short    **I; /* Ptr to Arr of addresses of indicator vars   */
  /* sb4    */ int        F; /* Number of variables found by DESCRIBE       */
  /* text** */ char     **S; /* Ptr to Arr of variable name pointers        */
  /* ub2*   */ short     *M; /* Ptr to Arr of max lengths of var. names     */
  /* ub2*   */ short     *C; /* Ptr to Arr of current lengths of var. names */
  /* text** */ char     **X; /* Ptr to Arr of ind. var. name pointers       */
  /* ub2*   */ short     *Y; /* Ptr to Arr of max lengths of ind. var. names*/
  /* ub2*   */ short     *Z; /* Ptr to Arr of cur lengths of ind. var. names*/
  };
 
typedef struct SQLDA SQLDA;
 
#endif

/* ----------------- */
/* defines for sqlda */
/* ----------------- */

#define SQLSQLDAAlloc(arg1, arg2, arg3, arg4) sqlaldt(arg1, arg2, arg3, arg4) 

#define SQLSQLDAFree(arg1, arg2) sqlclut(arg1, arg2) 




#line 14 "dbprocess.pc"
int connect_oracle(const char *file)
{
	/* EXEC SQL BEGIN DECLARE SECTION; */ 
#line 16 "dbprocess.pc"

		char username[20];
		char password[20];
		char oraclename[20];
	/* EXEC SQL END DECLARE SECTION; */ 
#line 20 "dbprocess.pc"

	
	strcpy(username,read_ini_string(permanent_pool,file,TASK_CAPTION,"DB_USER",""));
	strcpy(password,read_ini_string(permanent_pool,file,TASK_CAPTION,"DB_PASS",""));
	strcpy(oraclename,read_ini_string(permanent_pool,file,TASK_CAPTION,"DB_NAME",""));

	/* EXEC SQL CONNECT:username IDENTIFIED BY:password AT db236 using :oraclename; */ 
#line 26 "dbprocess.pc"

{
#line 26 "dbprocess.pc"
 struct sqlexd sqlstm;
#line 26 "dbprocess.pc"
 sqlstm.sqlvsn = 12;
#line 26 "dbprocess.pc"
 sqlstm.arrsiz = 4;
#line 26 "dbprocess.pc"
 sqlstm.sqladtp = &sqladt;
#line 26 "dbprocess.pc"
 sqlstm.sqltdsp = &sqltds;
#line 26 "dbprocess.pc"
 sqlstm.iters = (unsigned int  )10;
#line 26 "dbprocess.pc"
 sqlstm.offset = (unsigned int  )5;
#line 26 "dbprocess.pc"
 sqlstm.cud = sqlcud0;
#line 26 "dbprocess.pc"
 sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 26 "dbprocess.pc"
 sqlstm.sqlety = (unsigned short)256;
#line 26 "dbprocess.pc"
 sqlstm.occurs = (unsigned int  )0;
#line 26 "dbprocess.pc"
 sqlstm.sqhstv[0] = (unsigned char  *)username;
#line 26 "dbprocess.pc"
 sqlstm.sqhstl[0] = (unsigned long )20;
#line 26 "dbprocess.pc"
 sqlstm.sqhsts[0] = (         int  )20;
#line 26 "dbprocess.pc"
 sqlstm.sqindv[0] = (         short *)0;
#line 26 "dbprocess.pc"
 sqlstm.sqinds[0] = (         int  )0;
#line 26 "dbprocess.pc"
 sqlstm.sqharm[0] = (unsigned long )0;
#line 26 "dbprocess.pc"
 sqlstm.sqadto[0] = (unsigned short )0;
#line 26 "dbprocess.pc"
 sqlstm.sqtdso[0] = (unsigned short )0;
#line 26 "dbprocess.pc"
 sqlstm.sqhstv[1] = (unsigned char  *)password;
#line 26 "dbprocess.pc"
 sqlstm.sqhstl[1] = (unsigned long )20;
#line 26 "dbprocess.pc"
 sqlstm.sqhsts[1] = (         int  )20;
#line 26 "dbprocess.pc"
 sqlstm.sqindv[1] = (         short *)0;
#line 26 "dbprocess.pc"
 sqlstm.sqinds[1] = (         int  )0;
#line 26 "dbprocess.pc"
 sqlstm.sqharm[1] = (unsigned long )0;
#line 26 "dbprocess.pc"
 sqlstm.sqadto[1] = (unsigned short )0;
#line 26 "dbprocess.pc"
 sqlstm.sqtdso[1] = (unsigned short )0;
#line 26 "dbprocess.pc"
 sqlstm.sqhstv[2] = (unsigned char  *)oraclename;
#line 26 "dbprocess.pc"
 sqlstm.sqhstl[2] = (unsigned long )20;
#line 26 "dbprocess.pc"
 sqlstm.sqhsts[2] = (         int  )20;
#line 26 "dbprocess.pc"
 sqlstm.sqindv[2] = (         short *)0;
#line 26 "dbprocess.pc"
 sqlstm.sqinds[2] = (         int  )0;
#line 26 "dbprocess.pc"
 sqlstm.sqharm[2] = (unsigned long )0;
#line 26 "dbprocess.pc"
 sqlstm.sqadto[2] = (unsigned short )0;
#line 26 "dbprocess.pc"
 sqlstm.sqtdso[2] = (unsigned short )0;
#line 26 "dbprocess.pc"
 sqlstm.sqphsv = sqlstm.sqhstv;
#line 26 "dbprocess.pc"
 sqlstm.sqphsl = sqlstm.sqhstl;
#line 26 "dbprocess.pc"
 sqlstm.sqphss = sqlstm.sqhsts;
#line 26 "dbprocess.pc"
 sqlstm.sqpind = sqlstm.sqindv;
#line 26 "dbprocess.pc"
 sqlstm.sqpins = sqlstm.sqinds;
#line 26 "dbprocess.pc"
 sqlstm.sqparm = sqlstm.sqharm;
#line 26 "dbprocess.pc"
 sqlstm.sqparc = sqlstm.sqharc;
#line 26 "dbprocess.pc"
 sqlstm.sqpadto = sqlstm.sqadto;
#line 26 "dbprocess.pc"
 sqlstm.sqptdso = sqlstm.sqtdso;
#line 26 "dbprocess.pc"
 sqlstm.sqlcmax = (unsigned int )100;
#line 26 "dbprocess.pc"
 sqlstm.sqlcmin = (unsigned int )2;
#line 26 "dbprocess.pc"
 sqlstm.sqlcincr = (unsigned int )1;
#line 26 "dbprocess.pc"
 sqlstm.sqlctimeout = (unsigned int )0;
#line 26 "dbprocess.pc"
 sqlstm.sqlcnowait = (unsigned int )0;
#line 26 "dbprocess.pc"
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 26 "dbprocess.pc"
}

#line 26 "dbprocess.pc"

	if(sqlca.sqlcode == 0)
		LOG_MSG(" connect oracle [%s/%s@%s] success.", username,password,oraclename);
	else
		LOG_MSG(" connect oracle [%s/%s@%s] error  [%d:%s].", username,password,oraclename, sqlca.sqlcode,sqlca.sqlerrm.sqlerrmc);
	return sqlca.sqlcode;
}

int disconnect_oracle()
{
	/* EXEC SQL AT db236 COMMIT WORK RELEASE ; */ 
#line 36 "dbprocess.pc"

{
#line 36 "dbprocess.pc"
 struct sqlexd sqlstm;
#line 36 "dbprocess.pc"
 sqlstm.sqlvsn = 12;
#line 36 "dbprocess.pc"
 sqlstm.arrsiz = 4;
#line 36 "dbprocess.pc"
 sqlstm.sqladtp = &sqladt;
#line 36 "dbprocess.pc"
 sqlstm.sqltdsp = &sqltds;
#line 36 "dbprocess.pc"
 sqlstm.iters = (unsigned int  )1;
#line 36 "dbprocess.pc"
 sqlstm.offset = (unsigned int  )41;
#line 36 "dbprocess.pc"
 sqlstm.cud = sqlcud0;
#line 36 "dbprocess.pc"
 sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 36 "dbprocess.pc"
 sqlstm.sqlety = (unsigned short)256;
#line 36 "dbprocess.pc"
 sqlstm.occurs = (unsigned int  )0;
#line 36 "dbprocess.pc"
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 36 "dbprocess.pc"
}

#line 36 "dbprocess.pc"

	return 0;
}	

int CheckSqlResult()
{
    if(sqlca.sqlcode == 0)
    {
        return db_ok;
    }
    else
    {
        if(sqlca.sqlcode == 1403)
        {
            return db_nofound;
        }

        if(sqlca.sqlcode == -12571 || sqlca.sqlcode == -1089 || sqlca.sqlcode == -1090 || sqlca.sqlcode == -3114 || sqlca.sqlcode == -3113 || sqlca.sqlcode == -1092 || sqlca.sqlcode == -12326 || sqlca.sqlcode == -12329 || sqlca.sqlcode == -1012)
        {          
            return db_breaken;
        }
        else
        {
            return db_error;
        }
    }
}

int ExecuteSql(const char *sql,bool bCommit)
{
	/* EXEC SQL BEGIN DECLARE SECTION; */ 
#line 66 "dbprocess.pc"

		char sqlCmd[512];
	/* EXEC SQL END DECLARE SECTION; */ 
#line 68 "dbprocess.pc"

	strncpy(sqlCmd, sql, sizeof(sqlCmd));

	/* EXEC SQL AT db236 PREPARE sql_stmt FROM :sqlCmd; */ 
#line 71 "dbprocess.pc"

{
#line 71 "dbprocess.pc"
 struct sqlexd sqlstm;
#line 71 "dbprocess.pc"
 sqlstm.sqlvsn = 12;
#line 71 "dbprocess.pc"
 sqlstm.arrsiz = 4;
#line 71 "dbprocess.pc"
 sqlstm.sqladtp = &sqladt;
#line 71 "dbprocess.pc"
 sqlstm.sqltdsp = &sqltds;
#line 71 "dbprocess.pc"
 sqlstm.stmt = "";
#line 71 "dbprocess.pc"
 sqlstm.iters = (unsigned int  )1;
#line 71 "dbprocess.pc"
 sqlstm.offset = (unsigned int  )61;
#line 71 "dbprocess.pc"
 sqlstm.cud = sqlcud0;
#line 71 "dbprocess.pc"
 sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 71 "dbprocess.pc"
 sqlstm.sqlety = (unsigned short)256;
#line 71 "dbprocess.pc"
 sqlstm.occurs = (unsigned int  )0;
#line 71 "dbprocess.pc"
 sqlstm.sqhstv[0] = (unsigned char  *)sqlCmd;
#line 71 "dbprocess.pc"
 sqlstm.sqhstl[0] = (unsigned long )512;
#line 71 "dbprocess.pc"
 sqlstm.sqhsts[0] = (         int  )0;
#line 71 "dbprocess.pc"
 sqlstm.sqindv[0] = (         short *)0;
#line 71 "dbprocess.pc"
 sqlstm.sqinds[0] = (         int  )0;
#line 71 "dbprocess.pc"
 sqlstm.sqharm[0] = (unsigned long )0;
#line 71 "dbprocess.pc"
 sqlstm.sqadto[0] = (unsigned short )0;
#line 71 "dbprocess.pc"
 sqlstm.sqtdso[0] = (unsigned short )0;
#line 71 "dbprocess.pc"
 sqlstm.sqphsv = sqlstm.sqhstv;
#line 71 "dbprocess.pc"
 sqlstm.sqphsl = sqlstm.sqhstl;
#line 71 "dbprocess.pc"
 sqlstm.sqphss = sqlstm.sqhsts;
#line 71 "dbprocess.pc"
 sqlstm.sqpind = sqlstm.sqindv;
#line 71 "dbprocess.pc"
 sqlstm.sqpins = sqlstm.sqinds;
#line 71 "dbprocess.pc"
 sqlstm.sqparm = sqlstm.sqharm;
#line 71 "dbprocess.pc"
 sqlstm.sqparc = sqlstm.sqharc;
#line 71 "dbprocess.pc"
 sqlstm.sqpadto = sqlstm.sqadto;
#line 71 "dbprocess.pc"
 sqlstm.sqptdso = sqlstm.sqtdso;
#line 71 "dbprocess.pc"
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 71 "dbprocess.pc"
}

#line 71 "dbprocess.pc"

    /* EXEC SQL AT db236 EXECUTE sql_stmt; */ 
#line 72 "dbprocess.pc"

{
#line 72 "dbprocess.pc"
    struct sqlexd sqlstm;
#line 72 "dbprocess.pc"
    sqlstm.sqlvsn = 12;
#line 72 "dbprocess.pc"
    sqlstm.arrsiz = 4;
#line 72 "dbprocess.pc"
    sqlstm.sqladtp = &sqladt;
#line 72 "dbprocess.pc"
    sqlstm.sqltdsp = &sqltds;
#line 72 "dbprocess.pc"
    sqlstm.stmt = "";
#line 72 "dbprocess.pc"
    sqlstm.iters = (unsigned int  )1;
#line 72 "dbprocess.pc"
    sqlstm.offset = (unsigned int  )85;
#line 72 "dbprocess.pc"
    sqlstm.cud = sqlcud0;
#line 72 "dbprocess.pc"
    sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 72 "dbprocess.pc"
    sqlstm.sqlety = (unsigned short)256;
#line 72 "dbprocess.pc"
    sqlstm.occurs = (unsigned int  )0;
#line 72 "dbprocess.pc"
    sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 72 "dbprocess.pc"
}

#line 72 "dbprocess.pc"

	if(sqlca.sqlcode != 0)
	{
		LOG_MSG("sql [%s] error [%s]", sql,sqlca.sqlerrm.sqlerrmc);
		Rollback();
	}	
	else if(bCommit)
	{
		Commit();
	}
	int ret = CheckSqlResult();
	if(ret == db_breaken)
	{
		disconnect_oracle();
		connect_oracle(TASK_INI);
	}
	return ret;
}

int Commit()             
{
    /* EXEC SQL AT db236 COMMIT; */ 
#line 93 "dbprocess.pc"

{
#line 93 "dbprocess.pc"
    struct sqlexd sqlstm;
#line 93 "dbprocess.pc"
    sqlstm.sqlvsn = 12;
#line 93 "dbprocess.pc"
    sqlstm.arrsiz = 4;
#line 93 "dbprocess.pc"
    sqlstm.sqladtp = &sqladt;
#line 93 "dbprocess.pc"
    sqlstm.sqltdsp = &sqltds;
#line 93 "dbprocess.pc"
    sqlstm.iters = (unsigned int  )1;
#line 93 "dbprocess.pc"
    sqlstm.offset = (unsigned int  )105;
#line 93 "dbprocess.pc"
    sqlstm.cud = sqlcud0;
#line 93 "dbprocess.pc"
    sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 93 "dbprocess.pc"
    sqlstm.sqlety = (unsigned short)256;
#line 93 "dbprocess.pc"
    sqlstm.occurs = (unsigned int  )0;
#line 93 "dbprocess.pc"
    sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 93 "dbprocess.pc"
}

#line 93 "dbprocess.pc"

    return sqlca.sqlcode >= 0;
}

int Rollback()           
{
    /* EXEC SQL AT db236  ROLLBACK; */ 
#line 99 "dbprocess.pc"

{
#line 99 "dbprocess.pc"
    struct sqlexd sqlstm;
#line 99 "dbprocess.pc"
    sqlstm.sqlvsn = 12;
#line 99 "dbprocess.pc"
    sqlstm.arrsiz = 4;
#line 99 "dbprocess.pc"
    sqlstm.sqladtp = &sqladt;
#line 99 "dbprocess.pc"
    sqlstm.sqltdsp = &sqltds;
#line 99 "dbprocess.pc"
    sqlstm.iters = (unsigned int  )1;
#line 99 "dbprocess.pc"
    sqlstm.offset = (unsigned int  )125;
#line 99 "dbprocess.pc"
    sqlstm.cud = sqlcud0;
#line 99 "dbprocess.pc"
    sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 99 "dbprocess.pc"
    sqlstm.sqlety = (unsigned short)256;
#line 99 "dbprocess.pc"
    sqlstm.occurs = (unsigned int  )0;
#line 99 "dbprocess.pc"
    sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 99 "dbprocess.pc"
}

#line 99 "dbprocess.pc"

    return sqlca.sqlcode >= 0;
}


int create_task_item_list_by_db_updated(int program_id)
{
         /* EXEC SQL BEGIN DECLARE SECTION; */ 
#line 106 "dbprocess.pc"

                int nCount = 0;
                int prg_id;
                int ftpparm_id;
                int del_src;
                int copy_flag;
                int direction;
                int file_modify_time_escape;
                int generate_db_cdr;
                int use_tmp;
                char ftp_host[256];
                char ftp_path[256];
                char local_path[256];
                char file_pattern[128];
                char month[16];
                char day[16];
                char hour[16];
                char min[16];
                char sh_file[256];
                char son_dir_mask[256];
                char cp_dir_1[256];
                char cp_dir_2[256];
                char cp_dir_3[256];
                char cp_dir_4[256];
                int  ftp_flag; 
                char db_time[17];
        /* EXEC SQL END DECLARE SECTION; */ 
#line 132 "dbprocess.pc"

        prg_id = program_id;
        struct TASK_ITEM  *_cur,*_tail;
		_cur = _task_list;
		while(_cur)
		{
		  _tail = _cur;
		   _cur = _cur->next;		   
		}
        strcpy(db_time,g_db_time); 
        /* EXEC SQL at db236 DECLARE cfg_cursor_updated CURSOR for select ftpparm_id,ftp_host,ftp_path,local_path,Nvl(file_pattern,'^.*'),direction,Nvl(month,'*'),Nvl(day,'*'),Nvl(hour,'*'),Nvl(min,'*'),del_src,Nvl(sh_file,'NO'),Nvl(son_dir_mask,'NULL'),Nvl(cp_dir_1,'NULL'),Nvl(cp_dir_2,'NULL'),Nvl(cp_dir_3,'NULL'),Nvl(cp_dir_4,'NULL'), file_modify_time_escape,generate_db_cdr,use_tmp,nvl(copy_flag,0),ftp_flag,to_char(sysdate,'YYYYMMDDHH24MISS') from tb_cfg_filecollection where Program_id =:prg_id  and opt_time >= to_date(:db_time,'YYYYMMDDHH24MISS'); */ 
#line 142 "dbprocess.pc"

        /* EXEC SQL at db236 OPEN cfg_cursor_updated; */ 
#line 143 "dbprocess.pc"

{
#line 143 "dbprocess.pc"
        struct sqlexd sqlstm;
#line 143 "dbprocess.pc"
        sqlstm.sqlvsn = 12;
#line 143 "dbprocess.pc"
        sqlstm.arrsiz = 4;
#line 143 "dbprocess.pc"
        sqlstm.sqladtp = &sqladt;
#line 143 "dbprocess.pc"
        sqlstm.sqltdsp = &sqltds;
#line 143 "dbprocess.pc"
        sqlstm.stmt = sq0006;
#line 143 "dbprocess.pc"
        sqlstm.iters = (unsigned int  )1;
#line 143 "dbprocess.pc"
        sqlstm.offset = (unsigned int  )145;
#line 143 "dbprocess.pc"
        sqlstm.selerr = (unsigned short)1;
#line 143 "dbprocess.pc"
        sqlstm.cud = sqlcud0;
#line 143 "dbprocess.pc"
        sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 143 "dbprocess.pc"
        sqlstm.sqlety = (unsigned short)256;
#line 143 "dbprocess.pc"
        sqlstm.occurs = (unsigned int  )0;
#line 143 "dbprocess.pc"
        sqlstm.sqcmod = (unsigned int )0;
#line 143 "dbprocess.pc"
        sqlstm.sqhstv[0] = (unsigned char  *)&prg_id;
#line 143 "dbprocess.pc"
        sqlstm.sqhstl[0] = (unsigned long )sizeof(int);
#line 143 "dbprocess.pc"
        sqlstm.sqhsts[0] = (         int  )0;
#line 143 "dbprocess.pc"
        sqlstm.sqindv[0] = (         short *)0;
#line 143 "dbprocess.pc"
        sqlstm.sqinds[0] = (         int  )0;
#line 143 "dbprocess.pc"
        sqlstm.sqharm[0] = (unsigned long )0;
#line 143 "dbprocess.pc"
        sqlstm.sqadto[0] = (unsigned short )0;
#line 143 "dbprocess.pc"
        sqlstm.sqtdso[0] = (unsigned short )0;
#line 143 "dbprocess.pc"
        sqlstm.sqhstv[1] = (unsigned char  *)db_time;
#line 143 "dbprocess.pc"
        sqlstm.sqhstl[1] = (unsigned long )17;
#line 143 "dbprocess.pc"
        sqlstm.sqhsts[1] = (         int  )0;
#line 143 "dbprocess.pc"
        sqlstm.sqindv[1] = (         short *)0;
#line 143 "dbprocess.pc"
        sqlstm.sqinds[1] = (         int  )0;
#line 143 "dbprocess.pc"
        sqlstm.sqharm[1] = (unsigned long )0;
#line 143 "dbprocess.pc"
        sqlstm.sqadto[1] = (unsigned short )0;
#line 143 "dbprocess.pc"
        sqlstm.sqtdso[1] = (unsigned short )0;
#line 143 "dbprocess.pc"
        sqlstm.sqphsv = sqlstm.sqhstv;
#line 143 "dbprocess.pc"
        sqlstm.sqphsl = sqlstm.sqhstl;
#line 143 "dbprocess.pc"
        sqlstm.sqphss = sqlstm.sqhsts;
#line 143 "dbprocess.pc"
        sqlstm.sqpind = sqlstm.sqindv;
#line 143 "dbprocess.pc"
        sqlstm.sqpins = sqlstm.sqinds;
#line 143 "dbprocess.pc"
        sqlstm.sqparm = sqlstm.sqharm;
#line 143 "dbprocess.pc"
        sqlstm.sqparc = sqlstm.sqharc;
#line 143 "dbprocess.pc"
        sqlstm.sqpadto = sqlstm.sqadto;
#line 143 "dbprocess.pc"
        sqlstm.sqptdso = sqlstm.sqtdso;
#line 143 "dbprocess.pc"
        sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 143 "dbprocess.pc"
}

#line 143 "dbprocess.pc"

        while(1)
        {
                /* EXEC SQL at db236 FETCH cfg_cursor_updated INTO :ftpparm_id,:ftp_host,:ftp_path,:local_path,:file_pattern,:direction,:month,:day,:hour,:min,:del_src,:sh_file,:son_dir_mask,:cp_dir_1,:cp_dir_2,:cp_dir_3,:cp_dir_4,:file_modify_time_escape,:generate_db_cdr,:use_tmp,:copy_flag,:ftp_flag,:db_time; */ 
#line 146 "dbprocess.pc"

{
#line 146 "dbprocess.pc"
                struct sqlexd sqlstm;
#line 146 "dbprocess.pc"
                sqlstm.sqlvsn = 12;
#line 146 "dbprocess.pc"
                sqlstm.arrsiz = 23;
#line 146 "dbprocess.pc"
                sqlstm.sqladtp = &sqladt;
#line 146 "dbprocess.pc"
                sqlstm.sqltdsp = &sqltds;
#line 146 "dbprocess.pc"
                sqlstm.iters = (unsigned int  )1;
#line 146 "dbprocess.pc"
                sqlstm.offset = (unsigned int  )173;
#line 146 "dbprocess.pc"
                sqlstm.selerr = (unsigned short)1;
#line 146 "dbprocess.pc"
                sqlstm.cud = sqlcud0;
#line 146 "dbprocess.pc"
                sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 146 "dbprocess.pc"
                sqlstm.sqlety = (unsigned short)256;
#line 146 "dbprocess.pc"
                sqlstm.occurs = (unsigned int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqfoff = (         int )0;
#line 146 "dbprocess.pc"
                sqlstm.sqfmod = (unsigned int )2;
#line 146 "dbprocess.pc"
                sqlstm.sqhstv[0] = (unsigned char  *)&ftpparm_id;
#line 146 "dbprocess.pc"
                sqlstm.sqhstl[0] = (unsigned long )sizeof(int);
#line 146 "dbprocess.pc"
                sqlstm.sqhsts[0] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqindv[0] = (         short *)0;
#line 146 "dbprocess.pc"
                sqlstm.sqinds[0] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqharm[0] = (unsigned long )0;
#line 146 "dbprocess.pc"
                sqlstm.sqadto[0] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqtdso[0] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqhstv[1] = (unsigned char  *)ftp_host;
#line 146 "dbprocess.pc"
                sqlstm.sqhstl[1] = (unsigned long )256;
#line 146 "dbprocess.pc"
                sqlstm.sqhsts[1] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqindv[1] = (         short *)0;
#line 146 "dbprocess.pc"
                sqlstm.sqinds[1] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqharm[1] = (unsigned long )0;
#line 146 "dbprocess.pc"
                sqlstm.sqadto[1] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqtdso[1] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqhstv[2] = (unsigned char  *)ftp_path;
#line 146 "dbprocess.pc"
                sqlstm.sqhstl[2] = (unsigned long )256;
#line 146 "dbprocess.pc"
                sqlstm.sqhsts[2] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqindv[2] = (         short *)0;
#line 146 "dbprocess.pc"
                sqlstm.sqinds[2] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqharm[2] = (unsigned long )0;
#line 146 "dbprocess.pc"
                sqlstm.sqadto[2] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqtdso[2] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqhstv[3] = (unsigned char  *)local_path;
#line 146 "dbprocess.pc"
                sqlstm.sqhstl[3] = (unsigned long )256;
#line 146 "dbprocess.pc"
                sqlstm.sqhsts[3] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqindv[3] = (         short *)0;
#line 146 "dbprocess.pc"
                sqlstm.sqinds[3] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqharm[3] = (unsigned long )0;
#line 146 "dbprocess.pc"
                sqlstm.sqadto[3] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqtdso[3] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqhstv[4] = (unsigned char  *)file_pattern;
#line 146 "dbprocess.pc"
                sqlstm.sqhstl[4] = (unsigned long )128;
#line 146 "dbprocess.pc"
                sqlstm.sqhsts[4] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqindv[4] = (         short *)0;
#line 146 "dbprocess.pc"
                sqlstm.sqinds[4] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqharm[4] = (unsigned long )0;
#line 146 "dbprocess.pc"
                sqlstm.sqadto[4] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqtdso[4] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqhstv[5] = (unsigned char  *)&direction;
#line 146 "dbprocess.pc"
                sqlstm.sqhstl[5] = (unsigned long )sizeof(int);
#line 146 "dbprocess.pc"
                sqlstm.sqhsts[5] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqindv[5] = (         short *)0;
#line 146 "dbprocess.pc"
                sqlstm.sqinds[5] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqharm[5] = (unsigned long )0;
#line 146 "dbprocess.pc"
                sqlstm.sqadto[5] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqtdso[5] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqhstv[6] = (unsigned char  *)month;
#line 146 "dbprocess.pc"
                sqlstm.sqhstl[6] = (unsigned long )16;
#line 146 "dbprocess.pc"
                sqlstm.sqhsts[6] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqindv[6] = (         short *)0;
#line 146 "dbprocess.pc"
                sqlstm.sqinds[6] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqharm[6] = (unsigned long )0;
#line 146 "dbprocess.pc"
                sqlstm.sqadto[6] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqtdso[6] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqhstv[7] = (unsigned char  *)day;
#line 146 "dbprocess.pc"
                sqlstm.sqhstl[7] = (unsigned long )16;
#line 146 "dbprocess.pc"
                sqlstm.sqhsts[7] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqindv[7] = (         short *)0;
#line 146 "dbprocess.pc"
                sqlstm.sqinds[7] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqharm[7] = (unsigned long )0;
#line 146 "dbprocess.pc"
                sqlstm.sqadto[7] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqtdso[7] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqhstv[8] = (unsigned char  *)hour;
#line 146 "dbprocess.pc"
                sqlstm.sqhstl[8] = (unsigned long )16;
#line 146 "dbprocess.pc"
                sqlstm.sqhsts[8] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqindv[8] = (         short *)0;
#line 146 "dbprocess.pc"
                sqlstm.sqinds[8] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqharm[8] = (unsigned long )0;
#line 146 "dbprocess.pc"
                sqlstm.sqadto[8] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqtdso[8] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqhstv[9] = (unsigned char  *)min;
#line 146 "dbprocess.pc"
                sqlstm.sqhstl[9] = (unsigned long )16;
#line 146 "dbprocess.pc"
                sqlstm.sqhsts[9] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqindv[9] = (         short *)0;
#line 146 "dbprocess.pc"
                sqlstm.sqinds[9] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqharm[9] = (unsigned long )0;
#line 146 "dbprocess.pc"
                sqlstm.sqadto[9] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqtdso[9] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqhstv[10] = (unsigned char  *)&del_src;
#line 146 "dbprocess.pc"
                sqlstm.sqhstl[10] = (unsigned long )sizeof(int);
#line 146 "dbprocess.pc"
                sqlstm.sqhsts[10] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqindv[10] = (         short *)0;
#line 146 "dbprocess.pc"
                sqlstm.sqinds[10] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqharm[10] = (unsigned long )0;
#line 146 "dbprocess.pc"
                sqlstm.sqadto[10] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqtdso[10] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqhstv[11] = (unsigned char  *)sh_file;
#line 146 "dbprocess.pc"
                sqlstm.sqhstl[11] = (unsigned long )256;
#line 146 "dbprocess.pc"
                sqlstm.sqhsts[11] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqindv[11] = (         short *)0;
#line 146 "dbprocess.pc"
                sqlstm.sqinds[11] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqharm[11] = (unsigned long )0;
#line 146 "dbprocess.pc"
                sqlstm.sqadto[11] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqtdso[11] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqhstv[12] = (unsigned char  *)son_dir_mask;
#line 146 "dbprocess.pc"
                sqlstm.sqhstl[12] = (unsigned long )256;
#line 146 "dbprocess.pc"
                sqlstm.sqhsts[12] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqindv[12] = (         short *)0;
#line 146 "dbprocess.pc"
                sqlstm.sqinds[12] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqharm[12] = (unsigned long )0;
#line 146 "dbprocess.pc"
                sqlstm.sqadto[12] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqtdso[12] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqhstv[13] = (unsigned char  *)cp_dir_1;
#line 146 "dbprocess.pc"
                sqlstm.sqhstl[13] = (unsigned long )256;
#line 146 "dbprocess.pc"
                sqlstm.sqhsts[13] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqindv[13] = (         short *)0;
#line 146 "dbprocess.pc"
                sqlstm.sqinds[13] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqharm[13] = (unsigned long )0;
#line 146 "dbprocess.pc"
                sqlstm.sqadto[13] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqtdso[13] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqhstv[14] = (unsigned char  *)cp_dir_2;
#line 146 "dbprocess.pc"
                sqlstm.sqhstl[14] = (unsigned long )256;
#line 146 "dbprocess.pc"
                sqlstm.sqhsts[14] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqindv[14] = (         short *)0;
#line 146 "dbprocess.pc"
                sqlstm.sqinds[14] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqharm[14] = (unsigned long )0;
#line 146 "dbprocess.pc"
                sqlstm.sqadto[14] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqtdso[14] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqhstv[15] = (unsigned char  *)cp_dir_3;
#line 146 "dbprocess.pc"
                sqlstm.sqhstl[15] = (unsigned long )256;
#line 146 "dbprocess.pc"
                sqlstm.sqhsts[15] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqindv[15] = (         short *)0;
#line 146 "dbprocess.pc"
                sqlstm.sqinds[15] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqharm[15] = (unsigned long )0;
#line 146 "dbprocess.pc"
                sqlstm.sqadto[15] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqtdso[15] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqhstv[16] = (unsigned char  *)cp_dir_4;
#line 146 "dbprocess.pc"
                sqlstm.sqhstl[16] = (unsigned long )256;
#line 146 "dbprocess.pc"
                sqlstm.sqhsts[16] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqindv[16] = (         short *)0;
#line 146 "dbprocess.pc"
                sqlstm.sqinds[16] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqharm[16] = (unsigned long )0;
#line 146 "dbprocess.pc"
                sqlstm.sqadto[16] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqtdso[16] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqhstv[17] = (unsigned char  *)&file_modify_time_escape;
#line 146 "dbprocess.pc"
                sqlstm.sqhstl[17] = (unsigned long )sizeof(int);
#line 146 "dbprocess.pc"
                sqlstm.sqhsts[17] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqindv[17] = (         short *)0;
#line 146 "dbprocess.pc"
                sqlstm.sqinds[17] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqharm[17] = (unsigned long )0;
#line 146 "dbprocess.pc"
                sqlstm.sqadto[17] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqtdso[17] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqhstv[18] = (unsigned char  *)&generate_db_cdr;
#line 146 "dbprocess.pc"
                sqlstm.sqhstl[18] = (unsigned long )sizeof(int);
#line 146 "dbprocess.pc"
                sqlstm.sqhsts[18] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqindv[18] = (         short *)0;
#line 146 "dbprocess.pc"
                sqlstm.sqinds[18] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqharm[18] = (unsigned long )0;
#line 146 "dbprocess.pc"
                sqlstm.sqadto[18] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqtdso[18] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqhstv[19] = (unsigned char  *)&use_tmp;
#line 146 "dbprocess.pc"
                sqlstm.sqhstl[19] = (unsigned long )sizeof(int);
#line 146 "dbprocess.pc"
                sqlstm.sqhsts[19] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqindv[19] = (         short *)0;
#line 146 "dbprocess.pc"
                sqlstm.sqinds[19] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqharm[19] = (unsigned long )0;
#line 146 "dbprocess.pc"
                sqlstm.sqadto[19] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqtdso[19] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqhstv[20] = (unsigned char  *)&copy_flag;
#line 146 "dbprocess.pc"
                sqlstm.sqhstl[20] = (unsigned long )sizeof(int);
#line 146 "dbprocess.pc"
                sqlstm.sqhsts[20] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqindv[20] = (         short *)0;
#line 146 "dbprocess.pc"
                sqlstm.sqinds[20] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqharm[20] = (unsigned long )0;
#line 146 "dbprocess.pc"
                sqlstm.sqadto[20] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqtdso[20] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqhstv[21] = (unsigned char  *)&ftp_flag;
#line 146 "dbprocess.pc"
                sqlstm.sqhstl[21] = (unsigned long )sizeof(int);
#line 146 "dbprocess.pc"
                sqlstm.sqhsts[21] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqindv[21] = (         short *)0;
#line 146 "dbprocess.pc"
                sqlstm.sqinds[21] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqharm[21] = (unsigned long )0;
#line 146 "dbprocess.pc"
                sqlstm.sqadto[21] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqtdso[21] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqhstv[22] = (unsigned char  *)db_time;
#line 146 "dbprocess.pc"
                sqlstm.sqhstl[22] = (unsigned long )17;
#line 146 "dbprocess.pc"
                sqlstm.sqhsts[22] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqindv[22] = (         short *)0;
#line 146 "dbprocess.pc"
                sqlstm.sqinds[22] = (         int  )0;
#line 146 "dbprocess.pc"
                sqlstm.sqharm[22] = (unsigned long )0;
#line 146 "dbprocess.pc"
                sqlstm.sqadto[22] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqtdso[22] = (unsigned short )0;
#line 146 "dbprocess.pc"
                sqlstm.sqphsv = sqlstm.sqhstv;
#line 146 "dbprocess.pc"
                sqlstm.sqphsl = sqlstm.sqhstl;
#line 146 "dbprocess.pc"
                sqlstm.sqphss = sqlstm.sqhsts;
#line 146 "dbprocess.pc"
                sqlstm.sqpind = sqlstm.sqindv;
#line 146 "dbprocess.pc"
                sqlstm.sqpins = sqlstm.sqinds;
#line 146 "dbprocess.pc"
                sqlstm.sqparm = sqlstm.sqharm;
#line 146 "dbprocess.pc"
                sqlstm.sqparc = sqlstm.sqharc;
#line 146 "dbprocess.pc"
                sqlstm.sqpadto = sqlstm.sqadto;
#line 146 "dbprocess.pc"
                sqlstm.sqptdso = sqlstm.sqtdso;
#line 146 "dbprocess.pc"
                sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 146 "dbprocess.pc"
}

#line 146 "dbprocess.pc"

                if(sqlca.sqlcode)
                {
                  if(sqlca.sqlcode != 1403)
                     LOG_MSG("create_task_item_list_by_db error:%s .",sqlca.sqlerrm.sqlerrmc);
                  break;
                }
                struct TASK_ITEM *_task = _task_list;
				int _nflag = 0;
				while(_task)
				{
				   if( _task->sch_item.id == ftpparm_id){ 
				   _nflag = 1;
                                   break;
                                   }
                                   _task = _task->next;
				}
				if(_task==NULL) _task=(TASK_ITEM *)pcalloc(permanent_pool, sizeof(*_task));
		memset(&_task->sch_item,0,sizeof(_task->sch_item));		
                _task->sch_item.id=ftpparm_id;
                if ( _task->sch_item.id==0 )
                    continue;
                //memset(&_task->sch_item,0,sizeof(_task->sch_item));
                strcpy( _task->sch_item.month, trim(month,"*"));
                strcpy( _task->sch_item.day, trim(day,"*"));
                strcpy( _task->sch_item.hour, trim(hour,"*"));
                strcpy( _task->sch_item.min, trim(min,"*"));

                _task->sch_item.func=start_task;
                _task->sch_item.arg=_task->sch_item.id;

				memset(_task->ftp_host,0,sizeof(_task->ftp_host));
                strcpy( _task->ftp_host, trim(ftp_host,"NULL"));

				memset(_task->ftp_path,0,sizeof(_task->ftp_path));
                strcpy( _task->ftp_path, trim(ftp_path,"NULL"));

				memset(_task->local_path,0,sizeof(_task->local_path));
                strcpy( _task->local_path, trim(local_path,"NULL"));

				memset(_task->file,0,sizeof(_task->file));
                strcpy( _task->file, trim(file_pattern,"NULL"));

                _task->direction = direction;
                _task->del_src = del_src;
                _task->file_ctime_escape = file_modify_time_escape;
                _task->use_tmp = use_tmp;
                _task->copy_flag = copy_flag;
                _task->generate_db_cdr = generate_db_cdr;

				memset(_task->sh_file,0,sizeof(_task->sh_file));
                strcpy(_task->sh_file, trim(sh_file,"NO"));

				memset(_task->son_dir_mask,0,sizeof(_task->son_dir_mask));
                strcpy(_task->son_dir_mask,trim(son_dir_mask,"NULL"));

				memset(_task->cp_dir_1,0,sizeof(_task->cp_dir_1));
                strcpy(_task->cp_dir_1, trim(cp_dir_1,"NULL"));

				memset(_task->cp_dir_2,0,sizeof(_task->cp_dir_2));
                strcpy(_task->cp_dir_2, trim(cp_dir_2,"NULL"));
				
				memset(_task->cp_dir_3,0,sizeof(_task->cp_dir_3));
                strcpy(_task->cp_dir_3, trim(cp_dir_3,"NULL"));

				memset(_task->cp_dir_4,0,sizeof(_task->cp_dir_4));
                strcpy(_task->cp_dir_4, trim(cp_dir_4,"NULL"));
                _task->ftp_flag = ftp_flag;
                strcpy(g_db_time,(const char*)db_time);
                if(_nflag == 0)
                {
                   _tail->next = _task;
		   _tail = _task;
                   _tail->next = NULL;
                }
                nCount +=1;
        }
        /* EXEC SQL at db236 close cfg_cursor_updated; */ 
#line 223 "dbprocess.pc"

{
#line 223 "dbprocess.pc"
        struct sqlexd sqlstm;
#line 223 "dbprocess.pc"
        sqlstm.sqlvsn = 12;
#line 223 "dbprocess.pc"
        sqlstm.arrsiz = 23;
#line 223 "dbprocess.pc"
        sqlstm.sqladtp = &sqladt;
#line 223 "dbprocess.pc"
        sqlstm.sqltdsp = &sqltds;
#line 223 "dbprocess.pc"
        sqlstm.iters = (unsigned int  )1;
#line 223 "dbprocess.pc"
        sqlstm.offset = (unsigned int  )285;
#line 223 "dbprocess.pc"
        sqlstm.cud = sqlcud0;
#line 223 "dbprocess.pc"
        sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 223 "dbprocess.pc"
        sqlstm.sqlety = (unsigned short)256;
#line 223 "dbprocess.pc"
        sqlstm.occurs = (unsigned int  )0;
#line 223 "dbprocess.pc"
        sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 223 "dbprocess.pc"
}

#line 223 "dbprocess.pc"

        LOG_MSG("Get ftp task count [%d].", nCount);
//        printf("Get ftp task count [%d].\n", nCount);
        return nCount; 
}



int create_task_item_list_by_db(int program_id)
{
	/* EXEC SQL BEGIN DECLARE SECTION; */ 
#line 233 "dbprocess.pc"

		int nCount = 0;
		int prg_id;
		int ftpparm_id;
		int del_src;
		int copy_flag;
		int direction;
		int file_modify_time_escape;
		int generate_db_cdr;
		int use_tmp;
		char ftp_host[256];
		char ftp_path[256];
		char local_path[256];
		char file_pattern[128];		
		char month[16];
		char day[16];
		char hour[16];
		char min[16];		
		char sh_file[256];
		char son_dir_mask[256];
		char cp_dir_1[256];
		char cp_dir_2[256];
		char cp_dir_3[256];
		char cp_dir_4[256];
                int  ftp_flag;
                /* varchar db_time[17]; */ 
struct { unsigned short len; unsigned char arr[17]; } db_time;
#line 258 "dbprocess.pc"

	/* EXEC SQL END DECLARE SECTION; */ 
#line 259 "dbprocess.pc"

	prg_id = program_id;
	struct TASK_ITEM  *_cur=_task_list;

	/* EXEC SQL at db236 DECLARE cfg_cursor CURSOR for select ftpparm_id,ftp_host,ftp_path,local_path,Nvl(file_pattern,'^.*'),direction,Nvl(month,'*'),Nvl(day,'*'),Nvl(hour,'*'),Nvl(min,'*'),del_src,Nvl(sh_file,'NO'),Nvl(son_dir_mask,'NULL'),Nvl(cp_dir_1,'NULL'),Nvl(cp_dir_2,'NULL'),Nvl(cp_dir_3,'NULL'),Nvl(cp_dir_4,'NULL'), file_modify_time_escape,generate_db_cdr,use_tmp,nvl(copy_flag,0),ftp_flag,to_char(sysdate,'YYYYMMDDHH24MISS') from tb_cfg_filecollection where Program_id =:prg_id and Ftp_Flag=1; */ 
#line 263 "dbprocess.pc"
	
	/* EXEC SQL at db236 OPEN cfg_cursor; */ 
#line 264 "dbprocess.pc"

{
#line 264 "dbprocess.pc"
 struct sqlexd sqlstm;
#line 264 "dbprocess.pc"
 sqlstm.sqlvsn = 12;
#line 264 "dbprocess.pc"
 sqlstm.arrsiz = 23;
#line 264 "dbprocess.pc"
 sqlstm.sqladtp = &sqladt;
#line 264 "dbprocess.pc"
 sqlstm.sqltdsp = &sqltds;
#line 264 "dbprocess.pc"
 sqlstm.stmt = sq0007;
#line 264 "dbprocess.pc"
 sqlstm.iters = (unsigned int  )1;
#line 264 "dbprocess.pc"
 sqlstm.offset = (unsigned int  )305;
#line 264 "dbprocess.pc"
 sqlstm.selerr = (unsigned short)1;
#line 264 "dbprocess.pc"
 sqlstm.cud = sqlcud0;
#line 264 "dbprocess.pc"
 sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 264 "dbprocess.pc"
 sqlstm.sqlety = (unsigned short)256;
#line 264 "dbprocess.pc"
 sqlstm.occurs = (unsigned int  )0;
#line 264 "dbprocess.pc"
 sqlstm.sqcmod = (unsigned int )0;
#line 264 "dbprocess.pc"
 sqlstm.sqhstv[0] = (unsigned char  *)&prg_id;
#line 264 "dbprocess.pc"
 sqlstm.sqhstl[0] = (unsigned long )sizeof(int);
#line 264 "dbprocess.pc"
 sqlstm.sqhsts[0] = (         int  )0;
#line 264 "dbprocess.pc"
 sqlstm.sqindv[0] = (         short *)0;
#line 264 "dbprocess.pc"
 sqlstm.sqinds[0] = (         int  )0;
#line 264 "dbprocess.pc"
 sqlstm.sqharm[0] = (unsigned long )0;
#line 264 "dbprocess.pc"
 sqlstm.sqadto[0] = (unsigned short )0;
#line 264 "dbprocess.pc"
 sqlstm.sqtdso[0] = (unsigned short )0;
#line 264 "dbprocess.pc"
 sqlstm.sqphsv = sqlstm.sqhstv;
#line 264 "dbprocess.pc"
 sqlstm.sqphsl = sqlstm.sqhstl;
#line 264 "dbprocess.pc"
 sqlstm.sqphss = sqlstm.sqhsts;
#line 264 "dbprocess.pc"
 sqlstm.sqpind = sqlstm.sqindv;
#line 264 "dbprocess.pc"
 sqlstm.sqpins = sqlstm.sqinds;
#line 264 "dbprocess.pc"
 sqlstm.sqparm = sqlstm.sqharm;
#line 264 "dbprocess.pc"
 sqlstm.sqparc = sqlstm.sqharc;
#line 264 "dbprocess.pc"
 sqlstm.sqpadto = sqlstm.sqadto;
#line 264 "dbprocess.pc"
 sqlstm.sqptdso = sqlstm.sqtdso;
#line 264 "dbprocess.pc"
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 264 "dbprocess.pc"
}

#line 264 "dbprocess.pc"

	while(1) 
	{
		/* EXEC SQL at db236 FETCH cfg_cursor INTO :ftpparm_id,:ftp_host,:ftp_path,:local_path,:file_pattern,:direction,:month,:day,:hour,:min,:del_src,:sh_file,:son_dir_mask,:cp_dir_1,:cp_dir_2,:cp_dir_3,:cp_dir_4,:file_modify_time_escape,:generate_db_cdr,:use_tmp,:copy_flag,:ftp_flag,:db_time; */ 
#line 267 "dbprocess.pc"

{
#line 267 "dbprocess.pc"
  struct sqlexd sqlstm;
#line 267 "dbprocess.pc"
  sqlstm.sqlvsn = 12;
#line 267 "dbprocess.pc"
  sqlstm.arrsiz = 23;
#line 267 "dbprocess.pc"
  sqlstm.sqladtp = &sqladt;
#line 267 "dbprocess.pc"
  sqlstm.sqltdsp = &sqltds;
#line 267 "dbprocess.pc"
  sqlstm.iters = (unsigned int  )1;
#line 267 "dbprocess.pc"
  sqlstm.offset = (unsigned int  )329;
#line 267 "dbprocess.pc"
  sqlstm.selerr = (unsigned short)1;
#line 267 "dbprocess.pc"
  sqlstm.cud = sqlcud0;
#line 267 "dbprocess.pc"
  sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 267 "dbprocess.pc"
  sqlstm.sqlety = (unsigned short)256;
#line 267 "dbprocess.pc"
  sqlstm.occurs = (unsigned int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqfoff = (         int )0;
#line 267 "dbprocess.pc"
  sqlstm.sqfmod = (unsigned int )2;
#line 267 "dbprocess.pc"
  sqlstm.sqhstv[0] = (unsigned char  *)&ftpparm_id;
#line 267 "dbprocess.pc"
  sqlstm.sqhstl[0] = (unsigned long )sizeof(int);
#line 267 "dbprocess.pc"
  sqlstm.sqhsts[0] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqindv[0] = (         short *)0;
#line 267 "dbprocess.pc"
  sqlstm.sqinds[0] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqharm[0] = (unsigned long )0;
#line 267 "dbprocess.pc"
  sqlstm.sqadto[0] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqtdso[0] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqhstv[1] = (unsigned char  *)ftp_host;
#line 267 "dbprocess.pc"
  sqlstm.sqhstl[1] = (unsigned long )256;
#line 267 "dbprocess.pc"
  sqlstm.sqhsts[1] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqindv[1] = (         short *)0;
#line 267 "dbprocess.pc"
  sqlstm.sqinds[1] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqharm[1] = (unsigned long )0;
#line 267 "dbprocess.pc"
  sqlstm.sqadto[1] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqtdso[1] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqhstv[2] = (unsigned char  *)ftp_path;
#line 267 "dbprocess.pc"
  sqlstm.sqhstl[2] = (unsigned long )256;
#line 267 "dbprocess.pc"
  sqlstm.sqhsts[2] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqindv[2] = (         short *)0;
#line 267 "dbprocess.pc"
  sqlstm.sqinds[2] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqharm[2] = (unsigned long )0;
#line 267 "dbprocess.pc"
  sqlstm.sqadto[2] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqtdso[2] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqhstv[3] = (unsigned char  *)local_path;
#line 267 "dbprocess.pc"
  sqlstm.sqhstl[3] = (unsigned long )256;
#line 267 "dbprocess.pc"
  sqlstm.sqhsts[3] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqindv[3] = (         short *)0;
#line 267 "dbprocess.pc"
  sqlstm.sqinds[3] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqharm[3] = (unsigned long )0;
#line 267 "dbprocess.pc"
  sqlstm.sqadto[3] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqtdso[3] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqhstv[4] = (unsigned char  *)file_pattern;
#line 267 "dbprocess.pc"
  sqlstm.sqhstl[4] = (unsigned long )128;
#line 267 "dbprocess.pc"
  sqlstm.sqhsts[4] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqindv[4] = (         short *)0;
#line 267 "dbprocess.pc"
  sqlstm.sqinds[4] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqharm[4] = (unsigned long )0;
#line 267 "dbprocess.pc"
  sqlstm.sqadto[4] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqtdso[4] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqhstv[5] = (unsigned char  *)&direction;
#line 267 "dbprocess.pc"
  sqlstm.sqhstl[5] = (unsigned long )sizeof(int);
#line 267 "dbprocess.pc"
  sqlstm.sqhsts[5] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqindv[5] = (         short *)0;
#line 267 "dbprocess.pc"
  sqlstm.sqinds[5] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqharm[5] = (unsigned long )0;
#line 267 "dbprocess.pc"
  sqlstm.sqadto[5] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqtdso[5] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqhstv[6] = (unsigned char  *)month;
#line 267 "dbprocess.pc"
  sqlstm.sqhstl[6] = (unsigned long )16;
#line 267 "dbprocess.pc"
  sqlstm.sqhsts[6] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqindv[6] = (         short *)0;
#line 267 "dbprocess.pc"
  sqlstm.sqinds[6] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqharm[6] = (unsigned long )0;
#line 267 "dbprocess.pc"
  sqlstm.sqadto[6] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqtdso[6] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqhstv[7] = (unsigned char  *)day;
#line 267 "dbprocess.pc"
  sqlstm.sqhstl[7] = (unsigned long )16;
#line 267 "dbprocess.pc"
  sqlstm.sqhsts[7] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqindv[7] = (         short *)0;
#line 267 "dbprocess.pc"
  sqlstm.sqinds[7] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqharm[7] = (unsigned long )0;
#line 267 "dbprocess.pc"
  sqlstm.sqadto[7] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqtdso[7] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqhstv[8] = (unsigned char  *)hour;
#line 267 "dbprocess.pc"
  sqlstm.sqhstl[8] = (unsigned long )16;
#line 267 "dbprocess.pc"
  sqlstm.sqhsts[8] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqindv[8] = (         short *)0;
#line 267 "dbprocess.pc"
  sqlstm.sqinds[8] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqharm[8] = (unsigned long )0;
#line 267 "dbprocess.pc"
  sqlstm.sqadto[8] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqtdso[8] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqhstv[9] = (unsigned char  *)min;
#line 267 "dbprocess.pc"
  sqlstm.sqhstl[9] = (unsigned long )16;
#line 267 "dbprocess.pc"
  sqlstm.sqhsts[9] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqindv[9] = (         short *)0;
#line 267 "dbprocess.pc"
  sqlstm.sqinds[9] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqharm[9] = (unsigned long )0;
#line 267 "dbprocess.pc"
  sqlstm.sqadto[9] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqtdso[9] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqhstv[10] = (unsigned char  *)&del_src;
#line 267 "dbprocess.pc"
  sqlstm.sqhstl[10] = (unsigned long )sizeof(int);
#line 267 "dbprocess.pc"
  sqlstm.sqhsts[10] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqindv[10] = (         short *)0;
#line 267 "dbprocess.pc"
  sqlstm.sqinds[10] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqharm[10] = (unsigned long )0;
#line 267 "dbprocess.pc"
  sqlstm.sqadto[10] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqtdso[10] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqhstv[11] = (unsigned char  *)sh_file;
#line 267 "dbprocess.pc"
  sqlstm.sqhstl[11] = (unsigned long )256;
#line 267 "dbprocess.pc"
  sqlstm.sqhsts[11] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqindv[11] = (         short *)0;
#line 267 "dbprocess.pc"
  sqlstm.sqinds[11] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqharm[11] = (unsigned long )0;
#line 267 "dbprocess.pc"
  sqlstm.sqadto[11] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqtdso[11] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqhstv[12] = (unsigned char  *)son_dir_mask;
#line 267 "dbprocess.pc"
  sqlstm.sqhstl[12] = (unsigned long )256;
#line 267 "dbprocess.pc"
  sqlstm.sqhsts[12] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqindv[12] = (         short *)0;
#line 267 "dbprocess.pc"
  sqlstm.sqinds[12] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqharm[12] = (unsigned long )0;
#line 267 "dbprocess.pc"
  sqlstm.sqadto[12] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqtdso[12] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqhstv[13] = (unsigned char  *)cp_dir_1;
#line 267 "dbprocess.pc"
  sqlstm.sqhstl[13] = (unsigned long )256;
#line 267 "dbprocess.pc"
  sqlstm.sqhsts[13] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqindv[13] = (         short *)0;
#line 267 "dbprocess.pc"
  sqlstm.sqinds[13] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqharm[13] = (unsigned long )0;
#line 267 "dbprocess.pc"
  sqlstm.sqadto[13] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqtdso[13] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqhstv[14] = (unsigned char  *)cp_dir_2;
#line 267 "dbprocess.pc"
  sqlstm.sqhstl[14] = (unsigned long )256;
#line 267 "dbprocess.pc"
  sqlstm.sqhsts[14] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqindv[14] = (         short *)0;
#line 267 "dbprocess.pc"
  sqlstm.sqinds[14] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqharm[14] = (unsigned long )0;
#line 267 "dbprocess.pc"
  sqlstm.sqadto[14] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqtdso[14] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqhstv[15] = (unsigned char  *)cp_dir_3;
#line 267 "dbprocess.pc"
  sqlstm.sqhstl[15] = (unsigned long )256;
#line 267 "dbprocess.pc"
  sqlstm.sqhsts[15] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqindv[15] = (         short *)0;
#line 267 "dbprocess.pc"
  sqlstm.sqinds[15] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqharm[15] = (unsigned long )0;
#line 267 "dbprocess.pc"
  sqlstm.sqadto[15] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqtdso[15] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqhstv[16] = (unsigned char  *)cp_dir_4;
#line 267 "dbprocess.pc"
  sqlstm.sqhstl[16] = (unsigned long )256;
#line 267 "dbprocess.pc"
  sqlstm.sqhsts[16] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqindv[16] = (         short *)0;
#line 267 "dbprocess.pc"
  sqlstm.sqinds[16] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqharm[16] = (unsigned long )0;
#line 267 "dbprocess.pc"
  sqlstm.sqadto[16] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqtdso[16] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqhstv[17] = (unsigned char  *)&file_modify_time_escape;
#line 267 "dbprocess.pc"
  sqlstm.sqhstl[17] = (unsigned long )sizeof(int);
#line 267 "dbprocess.pc"
  sqlstm.sqhsts[17] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqindv[17] = (         short *)0;
#line 267 "dbprocess.pc"
  sqlstm.sqinds[17] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqharm[17] = (unsigned long )0;
#line 267 "dbprocess.pc"
  sqlstm.sqadto[17] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqtdso[17] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqhstv[18] = (unsigned char  *)&generate_db_cdr;
#line 267 "dbprocess.pc"
  sqlstm.sqhstl[18] = (unsigned long )sizeof(int);
#line 267 "dbprocess.pc"
  sqlstm.sqhsts[18] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqindv[18] = (         short *)0;
#line 267 "dbprocess.pc"
  sqlstm.sqinds[18] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqharm[18] = (unsigned long )0;
#line 267 "dbprocess.pc"
  sqlstm.sqadto[18] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqtdso[18] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqhstv[19] = (unsigned char  *)&use_tmp;
#line 267 "dbprocess.pc"
  sqlstm.sqhstl[19] = (unsigned long )sizeof(int);
#line 267 "dbprocess.pc"
  sqlstm.sqhsts[19] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqindv[19] = (         short *)0;
#line 267 "dbprocess.pc"
  sqlstm.sqinds[19] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqharm[19] = (unsigned long )0;
#line 267 "dbprocess.pc"
  sqlstm.sqadto[19] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqtdso[19] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqhstv[20] = (unsigned char  *)&copy_flag;
#line 267 "dbprocess.pc"
  sqlstm.sqhstl[20] = (unsigned long )sizeof(int);
#line 267 "dbprocess.pc"
  sqlstm.sqhsts[20] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqindv[20] = (         short *)0;
#line 267 "dbprocess.pc"
  sqlstm.sqinds[20] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqharm[20] = (unsigned long )0;
#line 267 "dbprocess.pc"
  sqlstm.sqadto[20] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqtdso[20] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqhstv[21] = (unsigned char  *)&ftp_flag;
#line 267 "dbprocess.pc"
  sqlstm.sqhstl[21] = (unsigned long )sizeof(int);
#line 267 "dbprocess.pc"
  sqlstm.sqhsts[21] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqindv[21] = (         short *)0;
#line 267 "dbprocess.pc"
  sqlstm.sqinds[21] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqharm[21] = (unsigned long )0;
#line 267 "dbprocess.pc"
  sqlstm.sqadto[21] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqtdso[21] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqhstv[22] = (unsigned char  *)&db_time;
#line 267 "dbprocess.pc"
  sqlstm.sqhstl[22] = (unsigned long )19;
#line 267 "dbprocess.pc"
  sqlstm.sqhsts[22] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqindv[22] = (         short *)0;
#line 267 "dbprocess.pc"
  sqlstm.sqinds[22] = (         int  )0;
#line 267 "dbprocess.pc"
  sqlstm.sqharm[22] = (unsigned long )0;
#line 267 "dbprocess.pc"
  sqlstm.sqadto[22] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqtdso[22] = (unsigned short )0;
#line 267 "dbprocess.pc"
  sqlstm.sqphsv = sqlstm.sqhstv;
#line 267 "dbprocess.pc"
  sqlstm.sqphsl = sqlstm.sqhstl;
#line 267 "dbprocess.pc"
  sqlstm.sqphss = sqlstm.sqhsts;
#line 267 "dbprocess.pc"
  sqlstm.sqpind = sqlstm.sqindv;
#line 267 "dbprocess.pc"
  sqlstm.sqpins = sqlstm.sqinds;
#line 267 "dbprocess.pc"
  sqlstm.sqparm = sqlstm.sqharm;
#line 267 "dbprocess.pc"
  sqlstm.sqparc = sqlstm.sqharc;
#line 267 "dbprocess.pc"
  sqlstm.sqpadto = sqlstm.sqadto;
#line 267 "dbprocess.pc"
  sqlstm.sqptdso = sqlstm.sqtdso;
#line 267 "dbprocess.pc"
  sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 267 "dbprocess.pc"
}

#line 267 "dbprocess.pc"

		if(sqlca.sqlcode)	
		{
			if(sqlca.sqlcode != 1403)
				LOG_MSG("create_task_item_list_by_db error:%s .",sqlca.sqlerrm.sqlerrmc);
			break;
		}
	
		struct TASK_ITEM *_task;
		_task=(TASK_ITEM *)pcalloc(permanent_pool, sizeof(*_task));
		_task->sch_item.id=ftpparm_id;
		if ( _task->sch_item.id==0 )
			continue;
		
		strcpy( _task->sch_item.month, trim(month,"*"));
		strcpy( _task->sch_item.day, trim(day,"*"));
		strcpy( _task->sch_item.hour, trim(hour,"*"));
		strcpy( _task->sch_item.min, trim(min,"*"));
		
		_task->sch_item.func=start_task;
		_task->sch_item.arg=_task->sch_item.id;
		strcpy( _task->ftp_host, trim(ftp_host,"NULL"));
		strcpy( _task->ftp_path, trim(ftp_path,"NULL"));
		strcpy( _task->local_path, trim(local_path,"NULL"));
		strcpy( _task->file, trim(file_pattern,"NULL"));	
		
		_task->direction = direction;		
		_task->del_src = del_src;
		_task->file_ctime_escape = file_modify_time_escape;
		_task->use_tmp = use_tmp;
		_task->copy_flag = copy_flag;
		_task->generate_db_cdr = generate_db_cdr;
		strcpy(_task->sh_file, trim(sh_file,"NO"));	 
		strcpy(_task->son_dir_mask,trim(son_dir_mask,"NULL"));
		strcpy(_task->cp_dir_1, trim(cp_dir_1,"NULL"));
		strcpy(_task->cp_dir_2, trim(cp_dir_2,"NULL"));
		strcpy(_task->cp_dir_3, trim(cp_dir_3,"NULL"));
		strcpy(_task->cp_dir_4, trim(cp_dir_4,"NULL"));
                _task->ftp_flag = ftp_flag;
                db_time.arr[db_time.len]='\0';
                strcpy(g_db_time,(const char*)db_time.arr);
		if (_cur!=NULL)
			_cur->next=_task;	
		else
			_task_list=_task;
		_cur=_task;
		_task->next=NULL;
		nCount +=1;
	}
	/* EXEC SQL at db236 close cfg_cursor; */ 
#line 316 "dbprocess.pc"

{
#line 316 "dbprocess.pc"
 struct sqlexd sqlstm;
#line 316 "dbprocess.pc"
 sqlstm.sqlvsn = 12;
#line 316 "dbprocess.pc"
 sqlstm.arrsiz = 23;
#line 316 "dbprocess.pc"
 sqlstm.sqladtp = &sqladt;
#line 316 "dbprocess.pc"
 sqlstm.sqltdsp = &sqltds;
#line 316 "dbprocess.pc"
 sqlstm.iters = (unsigned int  )1;
#line 316 "dbprocess.pc"
 sqlstm.offset = (unsigned int  )441;
#line 316 "dbprocess.pc"
 sqlstm.cud = sqlcud0;
#line 316 "dbprocess.pc"
 sqlstm.sqlest = (unsigned char  *)&sqlca;
#line 316 "dbprocess.pc"
 sqlstm.sqlety = (unsigned short)256;
#line 316 "dbprocess.pc"
 sqlstm.occurs = (unsigned int  )0;
#line 316 "dbprocess.pc"
 sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
#line 316 "dbprocess.pc"
}

#line 316 "dbprocess.pc"

	LOG_MSG("Get ftp task count [%d].", nCount);	
    printf("Get ftp task count [%d].\n", nCount);
	return nCount;
}   

