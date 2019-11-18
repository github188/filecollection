#ifndef FRAME_MUTEX_H
#define FRAME_MUTEX_H

#include <pthread.h>    //pThread-Mutex
#include <semaphore.h>  //Posix-Semaphoren
#include <fcntl.h>      //wg. "O_"-Konstanten
#include <sys/sem.h>    //System V - Semaphoren
#include "definitions.h"
#include "frame_utils.h"

/**Bibliothek mit Funktionen zur Realisierung einer Mutal Exclusion unter Nutzung verschiedener Verfahren.
  *- tAbstractMutex          Schnittstellendefinition
  *- tNullMutex              Mutex ohne Funktion
  *- tAtomicMutex            Mutex unter Nutzung atomarer Operationen
  *- tAtomicRWMutex          RW-Mutex unter Nutzung atomarer Operationen
  *- tPthreadMutex           pThread-Mutex zur threadübergreifenden Verwendung
  *- tPosixSemaphore         Vaterklasse für named und unnamed Posix-Semaphore
  *- tUnnamedPosixSemaphore  unnamed Posix-Semaphore
  *- tNamedPosixSemaphore    named Posix-Semaphore
  *- tSystemVSemaphore       "klassische" System-V-Semaphore
  *@author Dirk Henrici
  */

//Compiler-Flag "-lpthread" verwenden

/**Stellt ein allgemeines Interface für einen Mutex bzw. eine Semaphore bereit.
  *Abgeleitete Klassen realisieren dieses Interface komplett oder in Teilen
  */
class tAbstractMutex {
protected:
  bool _master;
  bool _connected;
  int _lockcount;
public:
  tAbstractMutex(bool master = false) : _master(master), _connected(false), _lockcount(0) {};
  virtual ~tAbstractMutex() {};
  void setmaster(bool master) {_master=master;};
  virtual int getmutexsize() {return 0;};
  virtual void connect(void* mutexptr = NULL) {_connected = true; mutexptr = mutexptr;};
  virtual bool islocked() {return _lockcount!=0;};
  virtual tERROR create();
  virtual tERROR wait(int nTimes);
  virtual tERROR remove();
  virtual tERROR acquire();
  virtual tERROR tryacquire();
  virtual tERROR acquire_read();
  virtual tERROR tryacquire_read();
  virtual tERROR acquire_upgrade();
  virtual tERROR tryacquire_upgrade();
  virtual tERROR release();
  virtual tERROR release_read();
  virtual tERROR release_downgrade();
};

/**Klasse zum Schützen des aktuellen Scope über Instantiierung der Klasse unter Angabe des zu verwendenden Mutex
  */
class tGuardCurrentScope {
  tAbstractMutex* _mutex;
public:
  tGuardCurrentScope(tAbstractMutex* mutex) : _mutex(mutex) {mutex->acquire();};
  ~tGuardCurrentScope() {_mutex->release();};
};

/**Sonderfall eines Mutex ohne Funktion
  */
class tNullMutex : public tAbstractMutex {
public:
  tNullMutex() : tAbstractMutex() {};
  virtual tERROR create();
  virtual tERROR remove();
  virtual tERROR acquire();
  virtual tERROR tryacquire();
  virtual tERROR acquire_read();
  virtual tERROR tryacquire_read();
  virtual tERROR acquire_upgrade();
  virtual tERROR tryacquire_upgrade();
  virtual tERROR release();
  virtual tERROR release_read();
  virtual tERROR release_downgrade();
};

/**Nutzung atomarer Variablen zur Realisierung einer einfachen "mutual exclusion".
  *Sollte nur für kurzzeitige Sperrungen verwendet werden, so dass es nur selten zu überlappendem Zugriff kommt.
  *Dafür sehr schnell.
  */
class tAtomicMutex : public tAbstractMutex {
  atomic_t* _mutexptr;
  atomic_t _localmutex;
public:
  tAtomicMutex(bool master = false) : tAbstractMutex(master) {};
  virtual ~tAtomicMutex() {};
  virtual int getmutexsize() {return sizeof(atomic_t);};
  virtual void connect(void* mutexptr = NULL);
  virtual tERROR create();
  virtual tERROR remove();
  virtual tERROR acquire();
  virtual tERROR release();
};

/**Kombiniert zwei atomare Variablen zur Realisierung eines einfachen RW-Locks
  *Sollte nur für kurzzeitige Sperrungen verwendet werden, so dass es nur selten zu überlappendem Zugriff kommt.
  */
class tAtomicRWMutex : public tAbstractMutex {
  typedef tAtomicRWLock locktype;
  locktype* _mutexptr;
  locktype _localmutex;
public:
  tAtomicRWMutex(bool master = false) : tAbstractMutex(master) {};
  virtual ~tAtomicRWMutex() {};
  virtual int getmutexsize() {return sizeof(locktype);};
  virtual void connect(void* mutexptr = NULL);
  virtual tERROR create();
  virtual tERROR remove();
  virtual tERROR acquire();
  virtual tERROR acquire_read();
  virtual tERROR acquire_upgrade();
  virtual tERROR release();
  virtual tERROR release_read();
  virtual tERROR release_downgrade();
};

/**Mutex zur Verwendung innerhalb von Threads.
  *Gemäß Unix Network Programming (Stevens S. 745, 27.8, 1. Absatz) ist ein pthread-Mutex auch zwischen Prozessen verwendbar. In eigenen Tests führte dies unter Linux jedoch zu SegmentationFaults, wenn ein Prozeß auf einen anderen warten sollte.
  */
class tPthreadMutex : public tAbstractMutex {
  pthread_mutex_t* _mutexptr;
  pthread_mutex_t _localmutex;
public:
  tPthreadMutex(bool master = false) : tAbstractMutex(master) {};
  virtual ~tPthreadMutex() {};
  virtual int getmutexsize() {return sizeof(pthread_mutex_t);};
  virtual void connect(void* mutexptr = NULL);
  virtual tERROR create();
  virtual tERROR remove();
  virtual tERROR acquire();
  virtual tERROR release();
};

/**Posix-Semaphore.
  *Evtl. muß "-lrt" als Compilerflag angegeben werden.
  */
class tPosixSemaphore : public tAbstractMutex {
protected:
  sem_t* _semptr;
  sem_t _localsem;
public:
  tPosixSemaphore(bool master = false) : tAbstractMutex(master) {};
  virtual ~tPosixSemaphore() {};
  virtual tERROR acquire();
  virtual tERROR release();
};

/**Unnamed Posix-Semaphore.
  *Je nach Attribut nur innerhalb eines Prozesses (==0) oder prozeßübergreifend (!=0) verwendbar.
  *Letztere Option bei Linux zur Zeit nicht implementiert.
  */
class tUnnamedPosixSemaphore : public tPosixSemaphore {
  int _pshared;
public:
  tUnnamedPosixSemaphore(bool master = false, int pshared=0) : tPosixSemaphore(master), _pshared(pshared) {};
  virtual ~tUnnamedPosixSemaphore() {};
  virtual int getmutexsize() {return sizeof(sem_t);};
  virtual void connect(void* mutexptr = NULL);
  virtual tERROR create();
  virtual tERROR remove();
  void setattr(int pshared = 0) {_pshared = pshared;};
};

/**Named Posix-Semaphore.
  *Evtl. muß "-lrt" als Compilerflag angegeben werden.
  */
class tNamedPosixSemaphore : public tPosixSemaphore {
  sem_t* _semptr;
  sem_t _localsem;
  string _name;
  int _mode;
public:
  tNamedPosixSemaphore(bool master = false, string name = "", int mode = 0666) : tPosixSemaphore(master), _name(name), _mode(mode) {};
  virtual ~tNamedPosixSemaphore() {};
  virtual tERROR create();
  virtual tERROR remove();
  void setattr(string name, int mode = 0) {_name = name; _mode = mode;};
};

/**Semaphore für Interprozeß-Kommunikation
  */
class tSystemVSemaphore : public tAbstractMutex {
  int _semid;
  string _name;
  int _mode;
public:

  union semun {
    int val;
    struct semid_ds* buf;
    unsigned short* array;
    struct seminfo* __buf;
  };

  tSystemVSemaphore(bool master = false, string name = "", int mode = 0666) : tAbstractMutex(master), _name(name), _mode(mode) {};
  virtual ~tSystemVSemaphore() {};
  virtual tERROR create();
  virtual tERROR wait(int nTimes);
  virtual tERROR remove();
  virtual tERROR acquire();
  virtual tERROR tryacquire();
  virtual tERROR release();
};

#endif
