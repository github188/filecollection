
#define CC_INLINE_MODIFIER inline
#define STATIC_INLINE_WORKS 1
#if defined(__cplusplus)
  #define GASNET_INLINE_MODIFIER(fnname) inline
#elif defined(STATIC_INLINE_WORKS)
  #define GASNET_INLINE_MODIFIER(fnname) static CC_INLINE_MODIFIER
#elif defined(CC_INLINE_MODIFIER)
  #define GASNET_INLINE_MODIFIER(fnname) CC_INLINE_MODIFIER
#endif
/* ------------------------------------------------------------------------------------ */
/* portable atomic increment/decrement

   these provide a special datatype (gasneti_atomic_t) representing an atomically
    updated unsigned integer value and a set of atomic ops
   atomicity is guaranteed only if ALL accesses to the gasneti_atomic_t data happen
    through the provided operations (i.e. it is an error to directly access the
    contents of a gasneti_atomic_t), and if the gasneti_atomic_t data is only
    addressable by the current process (e.g. not in a System V shared memory segment)

    atomic_init(v)      initializer for an gasneti_atomic_t to value v
    atomic_set(p,v)     atomically sets *p to value v
    atomic_read(p)      atomically read and return the value of *p
    atomic_increment(p) atomically increment *p (no return value)
    atomic_decrement(p) atomically decrement *p (no return value)
    atomic_dec_and_test(p)
      atomically decrement *p, return non-zero iff the new value is 0
 */

#if defined(SOLARIS) || /* SPARC seems to have no atomic ops */ \
    defined(AIX) || \
    defined(CRAYT3E) || /* TODO: no atomic ops on T3e? */       \
    defined(_SX) || /* NEC SX-6 atomics not available to user code? */ \
    defined(HPUX)    || /* HPUX seems to have no atomic ops */  \
    defined(__crayx1) || /* X1 atomics currently broken */ \
    (defined(__PGI) && defined(BROKEN_LINUX_ASM_ATOMIC_H)) || /* haven't implemented atomics for PGI */ \
    (defined(OSF) && !defined(__DECC) && !defined(__GNUC__)) /* only implemented for these compilers */
  #define GASNETI_USE_GENERIC_ATOMICOPS
#endif

#ifdef GASNETI_USE_GENERIC_ATOMICOPS
  typedef unsigned int uint32_t;
  /* a very slow but portable implementation of atomic ops */
  typedef struct { volatile uint32_t ctr; } atomic_t;
  #define atomic_read(p)      ((p)->ctr)
  #define ATOMIC_INIT(v)      { (v) }
  #ifdef _INCLUDED_GASNET_H
    extern void *gasneti_patomicop_lock; /* bug 693: avoid header dependency cycle */

    #define atomic_set(p,v) do {                          \
        gasnet_hsl_lock((gasnet_hsl_t*)gasneti_patomicop_lock);   \
        (p)->ctr = (v);                                           \
        gasnet_hsl_unlock((gasnet_hsl_t*)gasneti_patomicop_lock); \
      } while (0)
    #define atomic_inc(p) do {                      \
        gasnet_hsl_lock((gasnet_hsl_t*)gasneti_patomicop_lock);   \
        ((p)->ctr)++;                                             \
        gasnet_hsl_unlock((gasnet_hsl_t*)gasneti_patomicop_lock); \
      } while (0)
    #define atomic_dec(p) do {                      \
        gasnet_hsl_lock((gasnet_hsl_t*)gasneti_patomicop_lock);   \
        ((p)->ctr)--;                                             \
        gasnet_hsl_unlock((gasnet_hsl_t*)gasneti_patomicop_lock); \
      } while (0)
    extern int atomic_dec_and_test(atomic_t *p);
    #define GASNETI_GENERIC_DEC_AND_TEST_DEF                     \
    int atomic_dec_and_test(atomic_t *p) { \
      uint32_t newval;                                           \
      gasnet_hsl_lock((gasnet_hsl_t*)gasneti_patomicop_lock);    \
      newval = p->ctr - 1;                                       \
      p->ctr = newval;                                           \
      gasnet_hsl_unlock((gasnet_hsl_t*)gasneti_patomicop_lock);  \
      return (newval == 0);                                      \
    }
  #elif defined(_REENTRANT) || defined(_THREAD_SAFE) || \
        defined(PTHREAD_MUTEX_INITIALIZER) ||           \
        defined(HAVE_PTHREAD) || defined(HAVE_PTHREAD_H)
    /* a version for pthreads which is independent of GASNet HSL's */
    //#include <pthread.h>
    extern pthread_mutex_t gasneti_atomicop_mutex;// = PTHREAD_MUTEX_INITIALIZER;

    #define atomic_set(p,v) do {               \
        pthread_mutex_lock(&gasneti_atomicop_mutex);   \
        (p)->ctr = (v);                                \
        pthread_mutex_unlock(&gasneti_atomicop_mutex); \
      } while (0)
    #define atomic_inc(p) do {           \
        pthread_mutex_lock(&gasneti_atomicop_mutex);   \
        ((p)->ctr)++;                                  \
        pthread_mutex_unlock(&gasneti_atomicop_mutex); \
      } while (0)
    #define atomic_dec(p) do {           \
        pthread_mutex_lock(&gasneti_atomicop_mutex);   \
        ((p)->ctr)--;                                  \
        pthread_mutex_unlock(&gasneti_atomicop_mutex); \
      } while (0)
    GASNET_INLINE_MODIFIER(atomic_dec_and_test)
    int atomic_dec_and_test(atomic_t *p) {
      uint32_t newval;
      pthread_mutex_lock(&gasneti_atomicop_mutex);
      newval = p->ctr - 1;
      p->ctr = newval;
      pthread_mutex_unlock(&gasneti_atomicop_mutex);
      return (newval == 0);
    }
  #else
    /* only one thread - everything atomic by definition */
    #define PTHREAD_MUTEX_INITIALIZER ERROR: include pthread.h before gasnet_tools.h
    int pthread_mutex_lock; /* attempt to generate a linker error if pthreads are in use */
    #define atomic_set(p,v)     ((p)->ctr = (v))
    #define atomic_inc(p) (((p)->ctr)++)
    #define atomic_dec(p) (((p)->ctr)--)
    #define atomic_dec_and_test(p) ((--((p)->ctr)) == 0)
  #endif
#else
  #if defined(LINUX) && defined(__INTEL_COMPILER) && defined(__ia64__)
    /* Intel compiler's inline assembly broken on Itanium (bug 384) - use intrinsics instead */
    #include <ia64intrin.h>
    typedef struct { volatile uint32_t ctr; } atomic_t;
    #define atomic_inc(p) _InterlockedIncrement((volatile int *)&((p)->ctr))
    #define atomic_dec(p) _InterlockedDecrement((volatile int *)&((p)->ctr))
    #define atomic_read(p)      ((p)->ctr)
    #define atomic_set(p,v)     ((p)->ctr = (v))
    #define ATOMIC_INIT(v)      { (v) }
    #define atomic_dec_and_test(p) \
                                        (_InterlockedDecrement((volatile int *)&((p)->ctr)) == 0)
  #elif defined(LINUX)
    #include <linux/config.h>
    #if defined(BROKEN_LINUX_ASM_ATOMIC_H) || \
        (!defined(GASNETI_UNI_BUILD) && !defined(CONFIG_SMP))
      /* some versions of the linux kernel ship with a broken atomic.h
         this code based on a non-broken version of the header.
         Also force using this code if this is a gasnet-smp build and the
         linux/config.h settings disagree (due to system config problem or
         cross-compiling on a uniprocessor frontend for smp nodes)
       */
      #if defined(__i386__) || defined(__x86_64__) /* x86 and Athlon/Opteron */
        #ifdef GASNETI_UNI_BUILD
          #define GASNETI_LOCK ""
        #else
          #define GASNETI_LOCK "lock ; "
        #endif
        typedef struct { volatile int counter; } atomic_t;
        #define atomic_read(p)      ((p)->counter)
        #define ATOMIC_INIT(v)      { (v) }
        #define atomic_set(p,v)     ((p)->counter = (v))
        GASNET_INLINE_MODIFIER(atomic_inc)
        void atomic_inc(atomic_t *v) {
          __asm__ __volatile__(
                  GASNETI_LOCK "incl %0"
                  :"=m" (v->counter)
                  :"m" (v->counter));
        }
        GASNET_INLINE_MODIFIER(atomic_dec)
        void atomic_dec(atomic_t *v) {
          __asm__ __volatile__(
                  GASNETI_LOCK "decl %0"
                  :"=m" (v->counter)
                  :"m" (v->counter));
        }
        GASNET_INLINE_MODIFIER(atomic_dec_and_test)
        int atomic_dec_and_test(atomic_t *v) {
            unsigned char c;
            __asm__ __volatile__(
	            GASNETI_LOCK "decl %0; sete %1"
	            :"=m" (v->counter), "=qm" (c)
	            :"m" (v->counter) : "memory");
            return (c != 0);
        }
      #elif defined(__ia64__)
        #if GASNET_DEBUG
          #include <stdio.h>
          #include <stdlib.h>
          #define GASNETI_CMPXCHG_BUGCHECK_DECL  int _cmpxchg_bugcheck_count = 128;
          #define GASNETI_CMPXCHG_BUGCHECK(v) do {                                         \
              if (_cmpxchg_bugcheck_count-- <= 0) {                                        \
                void *ip;                                                                  \
                asm ("mov %0=ip" : "=r"(ip));                                              \
                fprintf(stderr,"CMPXCHG_BUGCHECK: stuck at %p on word %p\n", ip, (v));     \
                abort();                                                                   \
              }                                                                            \
            } while (0)
        #else
          #define GASNETI_CMPXCHG_BUGCHECK_DECL
          #define GASNETI_CMPXCHG_BUGCHECK(v)  ((void)0)
        #endif

        GASNET_INLINE_MODIFIER(gasneti_cmpxchg)
        int32_t gasneti_cmpxchg(int32_t volatile *ptr, int32_t oldval, int32_t newval) {                                                                                      \
          int64_t _o_, _r_;
           _o_ = (int64_t)oldval;
           __asm__ __volatile__ ("mov ar.ccv=%0;;" :: "rO"(_o_));
           __asm__ __volatile__ ("mf; cmpxchg4.acq %0=[%1],%2,ar.ccv"
                                  : "=r"(_r_) : "r"(ptr), "r"(newval) : "memory");
          return (int32_t) _r_;
        }
        GASNET_INLINE_MODIFIER(gasneti_atomic_addandfetch_32)
        int32_t gasneti_atomic_addandfetch_32(int32_t volatile *v, int32_t op) {
          int32_t oldctr, newctr;
          GASNETI_CMPXCHG_BUGCHECK_DECL

          do {
            GASNETI_CMPXCHG_BUGCHECK(v);
            oldctr = *v;
            newctr = oldctr + op;
          } while (gasneti_cmpxchg(v, oldctr, newctr) != oldctr);
          return newctr;
        }
        typedef struct { volatile int32_t ctr; } atomic_t;
        #define atomic_inc(p) (gasneti_atomic_addandfetch_32(&((p)->ctr),1))
        #define atomic_dec(p) (gasneti_atomic_addandfetch_32(&((p)->ctr),-1))
        #define atomic_read(p)      ((p)->ctr)
        #define atomic_set(p,v)     ((p)->ctr = (v))
        #define ATOMIC_INIT(v)      { (v) }
        #define atomic_dec_and_test(p) (gasneti_atomic_addandfetch_32(&((p)->ctr),-1) == 0)
      #else
        #error you have broken Linux system headers and an unrecognized CPU. barf...
      #endif
    #else
      #ifdef __alpha__
        /* work-around for a puzzling header bug in alpha Linux */
        #define extern static
      #endif
      #ifdef __cplusplus
        /* work around a really stupid C++ header bug observed in HP Linux */
        #define new new_
      #endif
      #include <asm/atomic.h>
      #ifdef __alpha__
        #undef extern
      #endif
      #ifdef __cplusplus
        #undef new
      #endif
    #endif
  #elif defined(FREEBSD)
    #include <machine/atomic.h>
    typedef struct { volatile uint32_t ctr; } atomic_t;
    #define atomic_inc(p) atomic_add_int(&((p)->ctr),1)
    #define atomic_dec(p) atomic_subtract_int(&((p)->ctr),1)
    #define atomic_read(p)      ((p)->ctr)
    #define atomic_set(p,v)     ((p)->ctr = (v))
    #define ATOMIC_INIT(v)      { (v) }
    /* FreeBSD is lacking atomic ops that return a value */
    #ifdef __i386__
      GASNET_INLINE_MODIFIER(_gasneti_atomic_decrement_and_test)
      int _gasneti_atomic_decrement_and_test(volatile uint32_t *ctr) {                                                       \
	unsigned char c;
        __asm__ __volatile__(
	        _STRINGIFY(MPLOCKED) "decl %0; sete %1"
	        :"=m" (*ctr), "=qm" (c)
	        :"m" (*ctr) : "memory");
        return (c != 0);
      }
    #else
      #error need to implement atomic_dec_and_test for FreeBSD on your CPU
    #endif
    #define atomic_dec_and_test(p) \
           _gasneti_atomic_decrement_and_test(&((p)->ctr))
  #elif defined(CYGWIN)
    #include <windows.h>
    typedef struct { volatile uint32_t ctr; } atomic_t;
    #define atomic_inc(p) InterlockedIncrement((LONG *)&((p)->ctr))
    #define atomic_dec(p) InterlockedDecrement((LONG *)&((p)->ctr))
    #define atomic_read(p)      ((p)->ctr)
    #define atomic_set(p,v)     ((p)->ctr = (v))
    #define ATOMIC_INIT(v)      { (v) }
    #define atomic_dec_and_test(p) \
                                        (InterlockedDecrement((LONG *)&((p)->ctr)) == 0)
  #elif defined(AIX)
    #include <sys/atomic_op.h>
    typedef struct { volatile int ctr; } atomic_t;
    #define atomic_inc(p) (fetch_and_add((atomic_p)&((p)->ctr),1))
    #define atomic_dec(p) (fetch_and_add((atomic_p)&((p)->ctr),-1))
    #define atomic_read(p)      ((p)->ctr)
    #define atomic_set(p,v)     ((p)->ctr = (v))
    #define ATOMIC_INIT(v)      { (v) }
    #define atomic_dec_and_test(p) \
                                        (fetch_and_add((atomic_p)&((p)->ctr),-1) == 1) /* TODO */
  #elif defined(OSF)
   #ifdef __DECC
     /* OSF atomics are compiler built-ins */
     #include <sys/machine/builtins.h>
     typedef struct { volatile int32_t ctr; } atomic_t;
     #define atomic_inc(p) (__ATOMIC_INCREMENT_LONG(&((p)->ctr)))
     #define atomic_dec(p) (__ATOMIC_DECREMENT_LONG(&((p)->ctr)))
     #define atomic_read(p)      ((p)->ctr)
     #define atomic_set(p,v)     ((p)->ctr = (v))
     #define ATOMIC_INIT(v)      { (v) }
     #define atomic_dec_and_test(p) \
                                        (__ATOMIC_DECREMENT_LONG(&((p)->ctr)) == 1)
   #elif defined(__GNUC__)
      static __inline__ int32_t gasneti_atomic_addandfetch_32(int32_t volatile *v, int32_t op) {
        register int32_t volatile * addr = (int32_t volatile *)v;
        register int32_t temp;
        register int32_t result;
        __asm__ __volatile__(
          "1: \n\t"
          "ldl_l %0, 0(%2)\n\t"
          "addl %0, %3, %0\n\t"
          "mov %0, %1\n\t"
          "stl_c %0, 0(%2)\n\t"
          "beq %0, 1b\n\t"
          "nop\n"
          : "=&r" (temp), "=&r" (result) /* outputs */
          : "r" (addr), "r" (op)         /* inputs */
          : "memory", "cc");             /* kills */
        return result;
      }
     typedef struct { volatile int32_t ctr; } atomic_t;
     #define atomic_inc(p) (gasneti_atomic_addandfetch_32(&((p)->ctr),1))
     #define atomic_dec(p) (gasneti_atomic_addandfetch_32(&((p)->ctr),-1))
     #define atomic_read(p)      ((p)->ctr)
     #define atomic_set(p,v)     ((p)->ctr = (v))
     #define ATOMIC_INIT(v)      { (v) }
     #define atomic_dec_and_test(p) (gasneti_atomic_addandfetch_32(&((p)->ctr),-1) == 0)
   #endif
  #elif defined(IRIX)
    #include <mutex.h>
    typedef __uint32_t atomic_t;
    #define atomic_inc(p) (test_then_add32((p),1))
    #define atomic_dec(p) (test_then_add32((p),(uint32_t)-1))
    #define atomic_read(p)      (*(p))
    #define atomic_set(p,v)     (*(p) = (v))
    #define ATOMIC_INIT(v)      (v)
    #define atomic_dec_and_test(p) \
                                        (add_then_test32((p),(uint32_t)-1) == 0)
  #elif defined(__crayx1) /* This works on X1, but NOT the T3E */
    #include <intrinsics.h>
    typedef volatile long atomic_t;
    /* DOB: man pages for atomic ops claim gsync is required for using atomic ops,
       but trying to do so leads to crashes. Using atomic ops without gync gives
       incorrect results (testtools fails)
     */
    #if 1
      #define gasneti_atomic_presync()  ((void)0)
      #define gasneti_atomic_postsync() ((void)0)
    #elif 0
      #define gasneti_atomic_presync()  _gsync(0)
      #define gasneti_atomic_postsync() _gsync(0)
    #else
      #define gasneti_atomic_presync()  _msync_msp(0)
      #define gasneti_atomic_postsync() _msync_msp(0)
    #endif
    #define atomic_inc(p)	\
      (gasneti_atomic_presync(),_amo_aadd((p),(long)1),gasneti_atomic_postsync())
    #define atomic_dec(p)	\
      (gasneti_atomic_presync(),_amo_aadd((p),(long)1),gasneti_atomic_postsync())
    #define atomic_read(p)      (*(p))
    #define atomic_set(p,v)     (*(p) = (v))
    #define ATOMIC_INIT(v)      (v)
    GASNET_INLINE_MODIFIER(atomic_dec_and_test)
    int atomic_dec_and_test(atomic_t *p) {
       int retval;
       gasneti_atomic_presync();
       retval = _amo_afadd((p),(long)-1) == 0;
       gasneti_atomic_postsync();
       return retval;
    }
  #elif defined(_SX)
    /* these are disabled for now because they don't link */
    typedef struct { volatile uint32_t ctr; } atomic_t;
   #if 0
    #include <sys/mplock.h>
    #define atomic_inc(p) (atomic_add4(((p)->ctr),1))
    #define atomic_dec(p) (atomic_add4(((p)->ctr),-1))
    #define atomic_read(p)      (atomic_read4((p)->ctr))
    #define atomic_set(p,v)     (atomic_set4((p)->ctr,(v)))
    #define ATOMIC_INIT(v)      { (v) }
    #define atomic_dec_and_test(p) \
                                        (atomic_add4(((p)->ctr),-1) == 0)
   #else
    #define atomic_inc(p) (muadd(&((p)->ctr),1))
    #define atomic_dec(p) (muadd(&((p)->ctr),-1))
    #define atomic_read(p)      (muget(&((p)->ctr)))
    #define atomic_set(p,v)     (muset(&((p)->ctr),(v)))
    #define ATOMIC_INIT(v)      { (v) }
    #define atomic_dec_and_test(p) \
                                        (muadd(&((p)->ctr),-1) == 0)
   #endif
  #elif 0 && defined(SOLARIS)
    /* $%*(! Solaris has atomic functions in the kernel but refuses to expose them
       to the user... after all, what application would be interested in performance? */
    #include <sys/atomic.h>
    typedef struct { volatile uint32_t ctr; } atomic_t;
    #define atomic_inc(p) (atomic_add_32((uint32_t *)&((p)->ctr),1))
    #define atomic_read(p)      ((p)->ctr)
    #define atomic_set(p,v)     ((p)->ctr = (v))
    #define ATOMIC_INIT(v)      { (v) }
  #elif defined(__APPLE__) && defined(__MACH__) && defined(__ppc__)
    #if defined(__xlC__)
      /* XLC machine code functions are very rigid, thus we produce all
       * three read-modify-write ops as distinct functions in order to
       * get anything near to optimal code.
       */
      static void gasneti_atomic_inc_32(int32_t volatile *v);
      #pragma mc_func gasneti_atomic_inc_32 {\
	/* ARGS: r3 = v  LOCAL: r2 = tmp */ \
	"7c401828"	/* 0: lwarx	r2,0,r3		*/ \
	"38420001"	/*    addi	r2,r2,0x1	*/ \
	"7c40192d"	/*    stwcx.	r2,0,r3		*/ \
	"40a2fff4"	/*    bne-	0b		*/ \
	"4c00012c"	/*    isync			*/ \
      }
      #pragma reg_killed_by gasneti_atomic_inc_32

      static void gasneti_atomic_dec_32(int32_t volatile *v);
      #pragma mc_func gasneti_atomic_dec_32 {\
	/* ARGS: r3 = v  LOCAL: r2 = tmp */ \
	"7c401828"	/* 0: lwarx	r2,0,r3		*/ \
	"3842ffff"	/*    subi	r2,r2,0x1	*/ \
	"7c40192d"	/*    stwcx.	r2,0,r3		*/ \
	"40a2fff4"	/*    bne-	0b		*/ \
	"4c00012c"	/*    isync			*/ \
      }
      #pragma reg_killed_by gasneti_atomic_dec_32

      static int32_t gasneti_atomic_decandfetch_32(int32_t volatile *v);
      #pragma mc_func gasneti_atomic_decandfetch_32 {\
	/* ARGS: r3 = v  LOCAL: r2 = tmp */ \
	"7c401828"	/* 0: lwarx	r2,0,r3		*/ \
	"3842ffff"	/*    subi	r2,r2,0x1	*/ \
	"7c40192d"	/*    stwcx.	r2,0,r3		*/ \
	"40a2fff4"	/*    bne-	0b		*/ \
	"4c00012c"	/*    isync			*/ \
	"7c431378"	/*    mr	r3,r2		*/ \
	/* RETURN in r3 = result after dec */ \
      }
      #pragma reg_killed_by gasneti_atomic_decandfetch_32

      typedef struct { volatile int32_t ctr; } atomic_t;
      #define atomic_inc(p) (gasneti_atomic_inc_32(&((p)->ctr)))
      #define atomic_dec(p) (gasneti_atomic_dec_32(&((p)->ctr)))
      #define atomic_read(p)      ((p)->ctr)
      #define atomic_set(p,v)     ((p)->ctr = (v))
      #define ATOMIC_INIT(v)      { (v) }
      #define atomic_dec_and_test(p) (gasneti_atomic_decandfetch_32(&((p)->ctr)) == 0)
    #else
      static __inline__ int32_t gasneti_atomic_addandfetch_32(int32_t volatile *v, int32_t op) {
        register int32_t volatile * addr = (int32_t volatile *)v;
        register int32_t result;
        __asm__ __volatile__ (
          "0:\t"
          "lwarx    %0,0,%1 \n\t"
          "add%I2   %0,%0,%2 \n\t"
          "stwcx.   %0,0,%1 \n\t"
          "bne-     0b \n\t"
          "isync"
          : "=&b"(result)		/* constraint b = not in r0 */
          : "r" (addr), "Ir"(op)
          : "cr0", "memory");
        return result;
      }
      typedef struct { volatile int32_t ctr; } atomic_t;
      #define atomic_inc(p) (gasneti_atomic_addandfetch_32(&((p)->ctr),1))
      #define atomic_dec(p) (gasneti_atomic_addandfetch_32(&((p)->ctr),-1))
      #define atomic_read(p)      ((p)->ctr)
      #define atomic_set(p,v)     ((p)->ctr = (v))
      #define ATOMIC_INIT(v)      { (v) }
      #define atomic_dec_and_test(p) (gasneti_atomic_addandfetch_32(&((p)->ctr),-1) == 0)
    #endif
  #else
    #error Need to implement atomic increment/decrement for this platform...
  #endif
#endif

