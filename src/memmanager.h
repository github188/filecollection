#ifndef MEMORY_MANAGER_HEADER
#define MEMORY_MANAGER_HEADER

#define BLOCK_MINALLOC 8192
#define BLOCK_MINFREE 4096
#define T pool
typedef struct T T;

extern T *permanent_pool;

T *make_sub_pool(T *);
void destroy_pool(T *);
pool *init_alloc(void);

/* Clearing out EVERYTHING in an pool... destroys any sub-pools */
void clear_pool(T *);

void *palloc(T*, int nbytes);
void *pcalloc(T*, int nbytes);
char *pstrdup(T*, const char *s);
/* make a nul terminated copy of the n characters starting with s */
char *pstrndup(T*, const char *s, int n);
char *pstrcat(T*,...);

#endif

