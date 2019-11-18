/***************************************************************************
                           utils.h  -  description
                             -------------------
    begin                : Mon Mai 12 2003
    copyright            : (C) 2003 by Dirk Henrici
    email                : henrici@informatik.uni-kl.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of license found in "COPYING".                     *
 *                                                                         *
 ***************************************************************************/

#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <cerrno>
#include <sched.h>
#include "myatomic_1.h"
#include <signal.h>
#include <sys/sem.h>
using namespace std;
/**Hilfsfunktionen fr andere Module.
  *In "frame_utils" finden sich:
  *- debugprint*      Zentrale Ausgabe von Fehlermeldungen, kann dort in SysLog o.. umgelenkt werden
  *- *2str            Funktionen zur Umwandlung von Variableninhalten in Strings
  *- crc32            Funktionen zur Bildung von CRC-Prfsummen
  *- setsignalhandler Signalhandler umsetzen
  *- semaphore_*      Direktes Ansprechen von System-V-Semaphoren um einen Prozess auf einen anderen warten zu lassen
  *- atomic_*         Nutzung atomarer Operationen zur Realisierung eines einfachen, schnellen "mutual exclusion"-Locks
  *
  *@author Dirk Henrici
  */
//!Definition eines Signalhandlers
typedef void sigfunc(int);

//!Auf "atomic_t" basierende Sperrvariable
typedef atomic_t tAtomicLock;

//!Auf "atomic_t" basierende Strukur, mit der RW-Lock realisiert wird
struct tAtomicRWLock {
  atomic_t lock;
  atomic_t readcount;
};

//#define MIN(a,b) ((a)>(b)? (b): (a))
//#define MAX(a,b) ((a)>(b)? (a): (b))
string GetValue(const string &content,const string &key,const string & split);
int Alarm(int type, const char *module,const char * msg);
void debugprint(char relevance, string subject, string func = "");
void debugprintunix(char relevance, string subject, string func = "");
string int2str(int value);
string dbl2str(double value) ;
string ptr2str(void* pointer);
string time2str(time_t t, const char *fmt);
time_t str2time(const char *str);
double str2double(double& t,const string & ss);                                                              
short str2short(short& t,const string & ss);                                                  
int str2int(int& t,const string & ss);   
char* rtrim(char * str);
char* ltrim(char *str);
char* trim(char *str,char *def = "");
unsigned long crc32(const char *string);
unsigned long crc32(const char *buffer, unsigned long len);
unsigned long crc32(string str);
unsigned long hash(const char *string);

sigfunc* setsignalhandler(int signo, sigfunc* newfunc);
int semaphore_create(bool master = false, string name = "", int mode = 0666);
void semaphore_remove(int semid);
void semaphore_wait(int semid, int change = -1);
void semaphore_signal(int semid);
int semaphore_getvalue(int semid);

//Mutual Exclusion

/**Setzt eine Sperrvariable auf "unlocked"
  */
inline void atomic_setunlocked(atomic_t* lockvar) {
  atomic_set(lockvar, 1);
}

/**Wartet darauf, da eine Variable null wird.
  *Im ungesperrten Zustand hat die Variable den Wert 1; warten mehrere, so wird die Variable negativ.
  */
inline void atomic_lock(atomic_t* lockvar) {
  //so lange prfen, bis Variable null wird

  while (!atomic_dec_and_test(lockvar)) {
    //Dekrementierung rckgngig machen
    atomic_inc(lockvar);
    //Warten, Rechenzeit abgeben
    if (sched_yield()==-1)
      debugprintunix('E',"error yielding process", "atomic_lock");
  }
}

/**Gegenoperation zu atomic_lock
  */
inline void atomic_unlock(atomic_t* lockvar) {
  atomic_inc(lockvar);
}

//RW-Lock

/**Setzt eine RW-Sperrvariable in den Ausgangszustand
  */inline void atomic_rwlock_setunlocked(tAtomicRWLock* lockvar) {
  atomic_setunlocked(&(lockvar->lock));
  atomic_set(&(lockvar->readcount), 1);
};

/**Setzt eine RW-Sperrvariable zum Lesen
  *(unbenutzt entspricht 1)
  */
inline void atomic_readlock_acquire(tAtomicRWLock* lockvar) {
  //!Wait until nobody writes
  atomic_lock(&(lockvar->lock));
  //!Increment counter
  atomic_inc(&(lockvar->readcount));
  //!Done
  atomic_unlock(&(lockvar->lock));
}

/**Entsperrt eine zum Lesen gesperrte Variable
  *(unbenutzt entspricht 1)
  */
inline void atomic_readlock_release(tAtomicRWLock* lockvar) {
  if (atomic_dec_and_test(&(lockvar->readcount)))
    debugprint('E', "decrementing read count more often than incrementing it", "atomic_readlock_release");
}

/**Sperrt eine RW-Sperrvariable zum Schreiben
  *(unbenutzt entspricht 1)
  */
inline void atomic_writelock_acquire(tAtomicRWLock* lockvar) {
  //!Wait until nobody writes or changes
  atomic_lock(&(lockvar->lock));
  //!Wait until nobody else reads
  atomic_lock(&(lockvar->readcount));
  atomic_unlock(&(lockvar->readcount));
}

/**Entsperrt eine zum Schreiben gesperrte Variable
  */
inline void atomic_writelock_release(tAtomicRWLock* lockvar) {
  atomic_unlock(&(lockvar->lock));
}

/**Sperrt ein schon zum Lesen gesperrtes RW-Lock auch zum Schreiben
  */
inline void atomic_writelock_upgrade(tAtomicRWLock* lockvar) {
  //!Wait until nobody writes or changes
  atomic_lock(&(lockvar->lock));
  //!Eigenes Readlock aufgeben
  atomic_readlock_release(lockvar);
  //!Wait until nobody else reads
  atomic_lock(&(lockvar->readcount));
  atomic_unlock(&(lockvar->readcount));
}

/**Hebt die Sperrung zum Schreiben auf, behlt aber die zum Lesen
  */
inline void atomic_writelock_downgrade(tAtomicRWLock* lockvar) {
  atomic_inc(&(lockvar->readcount)); // entspricht readlock_acquire
  atomic_writelock_release(lockvar);
}

#endif
