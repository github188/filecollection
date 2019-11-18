#include <pthread.h>
#include "myatomic_1.h"

#if defined(_REENTRANT) || defined(_THREAD_SAFE) || \
        defined(PTHREAD_MUTEX_INITIALIZER) ||           \
        defined(HAVE_PTHREAD) || defined(HAVE_PTHREAD_H)
    /* a version for pthreads which is independent of GASNet HSL's */
    pthread_mutex_t gasneti_atomicop_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif
