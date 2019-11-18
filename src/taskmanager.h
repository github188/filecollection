#ifndef TASK_MANAGER_HEADER
#define TASK_MANAGER_HEADER

/*Create Task List*/
void creat_task_list();

/*Load Task From INI File*/
int load_task();

/*Start And Stop Task*/
void start_task(int __id);

/*List Task*/
void list_task();
void stop_task();
void start_db_process(int msg_id);

#endif

