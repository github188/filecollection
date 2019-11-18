#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "memmanager.h"
/*****************************************************************
 *
 * Managing free storage blocks...
 */

union align {
    /* Types which are likely to have the longest RELEVANT alignment
     * restrictions...
     */

    char *cp;
    void (*f) (void);
    long l;
    FILE *fp;
    double d;
};

#define CLICK_SZ (sizeof(union align))

union block_hdr {
    union align a;
    /* Actual header... */
    struct {
      char *endp;
      union block_hdr *next;
      char *first_avail;
    } h;
};

static union block_hdr *block_freelist = NULL;


/* Get a completely new block from the system pool. Note that we rely on
   malloc() to provide aligned memory. */

static union block_hdr *malloc_block(int size)
{
    union block_hdr *blok;
    int request_size;

    request_size = size + sizeof(union block_hdr);
    blok = (union block_hdr *) malloc(request_size);
    if (blok == NULL) {
      fprintf(stderr, "Ouch!  malloc(%d) failed in malloc_block()\n",
                request_size);
      exit(1);
    }
    blok->h.next = NULL;
    blok->h.first_avail = (char *) (blok + 1);
    blok->h.endp = size + blok->h.first_avail;
    
    return blok;
}

/* Free a chain of blocks --- must be called with alarms blocked. */

static void free_blocks(union block_hdr *blok)
{
    /* First, put new blocks at the head of the free list ---
     * we'll eventually bash the 'next' pointer of the last block
     * in the chain to point to the free blocks we already had.
     */

    union block_hdr *old_free_list;

    if (blok == NULL)
       return;      /* Sanity check --- freeing empty pool? */
       
    old_free_list = block_freelist;
    block_freelist = blok;
    /*
     * Next, adjust first_avail pointers of each block --- have to do it
     * sooner or later, and it simplifies the search in new_block to do it
     * now.
     */

    while (blok->h.next != NULL) {
       blok->h.first_avail = (char *) (blok + 1);
       blok = blok->h.next;
    }
    blok->h.first_avail = (char *) (blok + 1);
    /* Finally, reset next pointer to get the old free blocks back */
    blok->h.next = old_free_list;
}


/* Get a new block, from our own free list if possible, from the system
 * if necessary.  Must be called with alarms blocked.
 */

static union block_hdr *new_block(int min_size)
{
    union block_hdr **lastptr = &block_freelist;
    union block_hdr *blok = block_freelist;

    /* First, see if we have anything of the required size
     * on the free list...
     */

    while (blok != NULL) {
        if (min_size + BLOCK_MINFREE <= blok->h.endp - blok->h.first_avail) {
          *lastptr = blok->h.next;
          blok->h.next = NULL;
          return blok;
          }
      else {
          lastptr = &blok->h.next;
          blok = blok->h.next;
        }
    }

    /* Nope. */
    min_size += BLOCK_MINFREE;
    blok = malloc_block((min_size > BLOCK_MINALLOC) ? min_size : BLOCK_MINALLOC);
    return blok;
}


/* Accounting */

static long bytes_in_block_list(union block_hdr *blok)
{
    long size = 0;

    while (blok) {
      size += blok->h.endp - (char *) (blok + 1);
      blok = blok->h.next;
    }

    return size;
}


/*****************************************************************
 *
 * Pool internals and management...
 * NB that subprocesses are not handled by the generic cleanup code,
 * basically because we don't want cleanups for multiple subprocesses
 * to result in multiple three-second pauses.
 */

struct pool {
    union block_hdr *first;
    union block_hdr *last;
    struct pool *sub_pools;
    struct pool *sub_next;
    struct pool *sub_prev;
    struct pool *parent;
    char *free_first_avail;
};

T *permanent_pool;
/* Each pool structure is allocated in the start of its own first block,
 * so we need to know how many bytes that is (once properly aligned...).
 * This also means that when a pool's sub-pool is destroyed, the storage
 * associated with it is *completely* gone, so we have to make sure it
 * gets taken off the parent's sub-pool list...
 */

#define POOL_HDR_CLICKS (1 + ((sizeof(T) - 1) / CLICK_SZ))
#define POOL_HDR_BYTES (POOL_HDR_CLICKS * CLICK_SZ)

T *make_sub_pool(T *p)
{
    union block_hdr *blok;
    T *new_pool;

    blok = new_block(POOL_HDR_BYTES);
    new_pool = (T *) blok->h.first_avail;
    blok->h.first_avail += POOL_HDR_BYTES;

    memset((char *) new_pool, '\0', sizeof(T));
    new_pool->free_first_avail = blok->h.first_avail;
    new_pool->first = new_pool->last = blok;

    if (p) {
        new_pool->parent = p;
        new_pool->sub_next = p->sub_pools;
        if (new_pool->sub_next)
            new_pool->sub_next->sub_prev = new_pool;
        p->sub_pools = new_pool;
    }
    
    return new_pool;
}

T *init_alloc(void)
{
    permanent_pool = make_sub_pool(NULL);
    return permanent_pool;
}

void clear_pool(T *a)
{
    while (a->sub_pools)
        destroy_pool(a->sub_pools);

    free_blocks(a->first->h.next);
    a->first->h.next = NULL;
    a->last = a->first;
    a->first->h.first_avail = a->free_first_avail;
    
}

void destroy_pool(T *a)
{
    clear_pool(a);

    if (a->parent) {
      if (a->parent->sub_pools == a)
        a->parent->sub_pools = a->sub_next;
      if (a->sub_prev)
        a->sub_prev->sub_next = a->sub_next;
      if (a->sub_next)
        a->sub_next->sub_prev = a->sub_prev;
    }
    free_blocks(a->first);
}

long bytes_in_pool(T *p)
{
    return bytes_in_block_list(p->first);
}
long bytes_in_free_blocks(void)
{
    return bytes_in_block_list(block_freelist);
}

/*****************************************************************
 *
 * Allocating stuff...
 */


void * palloc(T *a, int reqsize)
{

    /* Round up requested size to an even number of alignment units (core clicks)
     */

    int nclicks = 1 + ((reqsize - 1) / CLICK_SZ);
    int size = nclicks * CLICK_SZ;

    /* First, see if we have space in the block most recently
     * allocated to this pool
     */

    union block_hdr *blok = a->last;
    char *first_avail = blok->h.first_avail;
    char *new_first_avail;

    if (reqsize <= 0)
        return NULL;

    new_first_avail = first_avail + size;

    if (new_first_avail <= blok->h.endp) {
      blok->h.first_avail = new_first_avail;
      return (void *) first_avail;
    }

    /* Nope --- get a new one that's guaranteed to be big enough */
    blok = new_block(size);
    a->last->h.next = blok;
    a->last = blok;
    first_avail = blok->h.first_avail;
    blok->h.first_avail += size;

    return (void *) first_avail;

}

void *pcalloc(T *a, int size)
{
    void *res = palloc(a, size);
    memset(res, '\0', size);
    return res;
}

char *pstrdup(T *a, const char *s)
{
    char *res;
    size_t len;

    if (s == NULL)
      return NULL;
    len = strlen(s) + 1;
    res = (char *)palloc(a, len);
    memcpy(res, s, len);
    return res;
}

char *pstrndup(T *a, const char *s, int n)
{
    char *res;

    if (s == NULL)
      return NULL;
    res = (char *)palloc(a, n + 1);
    memcpy(res, s, n);
    res[n] = '\0';
    return res;
}

char *pstrcat(T *a,...)
{
    char *cp, *argp, *res;

    /* Pass one --- find length of required string */

    int len = 0;
    va_list adummy;

    va_start(adummy, a);

    while ((cp = va_arg(adummy, char *)) != NULL)
       len += strlen(cp);

    va_end(adummy);

    /* Allocate the required string */

    res = (char *) palloc(a, len + 1);
    cp = res;
    *cp = '\0';

    /* Pass two --- copy the argument strings into the result space */

    va_start(adummy, a);

    while ((argp = va_arg(adummy, char *)) != NULL) {
      strcpy(cp, argp);
      cp += strlen(argp);
    }

    va_end(adummy);

    /* Return the result string */

    return res;
}
