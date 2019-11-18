#ifndef FILE_PROCESS_HEADER
#define FILE_PROCESS_HEADER
char * get_file_name(const char *__pathname, char *__buf, int __size);
char * get_dir_name(const char *__pathname, char *__buf, int __size);
int if_file_read(const char *__pathname);
int if_file_write(const char *__pathname);
int if_file_exist(const char *__pathname);
int if_isfile(const char *__pathname);
int if_isdir(const char *__pathname);
int if_dir_read(const char *__pathname);
int if_dir_write(const char *__pathname);
long get_file_size(const char *__pathname);
int cp_file(const char *__src, const char *__dest, int __if_over);
time_t get_file_last_modify(const char *__pathname);
int GetFileRows(const char *__pathname);
#endif

