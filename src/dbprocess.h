#ifndef _SQL_H
#define _SQL_H
enum{
	db_ok = 0,
        db_nofound,
        db_error  ,
        db_breaken,
    };
int connect_oracle(const char *file);
int disconnect_oracle();
int CheckSqlResult();
int ExecuteSql(const char *sql,bool bCommit);
int Commit() ;
int Rollback();
int create_task_item_list_by_db(int program_id);
int create_task_item_list_by_db_updated(int program_id);
#endif

