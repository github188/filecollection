/***************************************************************************
frame_mutex.cpp  -  description
-------------------
begin                : Mit Jun 25 2003
copyright            : (C) 2003 by Dirk Henrici
email                : henrici@informatik.uni-kl.de
***************************************************************************/

/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of license found in "COPYING".                     *
*                                                                         *
***************************************************************************/

#include "frame_mutex.h"
#include "thread.h"
tERROR tAbstractMutex::create() {return ERR_NotImplemented;};
tERROR tAbstractMutex::remove() {return ERR_NotImplemented;};
tERROR tAbstractMutex::acquire() {return ERR_NotImplemented;};
tERROR tAbstractMutex::tryacquire() {return ERR_NotImplemented;};
tERROR tAbstractMutex::wait(int nTimes) {return ERR_NotImplemented;};
tERROR tAbstractMutex::acquire_read() {return ERR_NotImplemented;};
tERROR tAbstractMutex::tryacquire_read() {return ERR_NotImplemented;};
tERROR tAbstractMutex::acquire_upgrade() {return ERR_NotImplemented;};
tERROR tAbstractMutex::tryacquire_upgrade() {return ERR_NotImplemented;};
tERROR tAbstractMutex::release() {return ERR_NotImplemented;};
tERROR tAbstractMutex::release_read() {return ERR_NotImplemented;};
tERROR tAbstractMutex::release_downgrade() {return ERR_NotImplemented;};
tERROR tNullMutex::create() {return ERR_OK;};
tERROR tNullMutex::remove() {return ERR_OK;};
tERROR tNullMutex::acquire() {return ERR_OK;};
tERROR tNullMutex::tryacquire() {return ERR_OK;};
tERROR tNullMutex::acquire_read() {return ERR_OK;};
tERROR tNullMutex::tryacquire_read() {return ERR_OK;};
tERROR tNullMutex::acquire_upgrade() {return ERR_OK;};
tERROR tNullMutex::tryacquire_upgrade() {return ERR_OK;};
tERROR tNullMutex::release() {return ERR_OK;};
tERROR tNullMutex::release_read() {return ERR_OK;};
tERROR tNullMutex::release_downgrade() {return ERR_OK;};

/**Erstellt einen Mutex
*/
void tAtomicMutex::connect(void* mutexptr) {
	//Standardmutex nutzen oder angegebenen Speicherbereich?
	if (mutexptr == NULL) {
		_mutexptr = &_localmutex;
	} else {
		_mutexptr = (atomic_t*)mutexptr;
	}
	tAbstractMutex::connect(mutexptr);
};

/**Initialisiert einen Mutex
*/
tERROR tAtomicMutex::create() {
	if (!_connected)
		connect();
	if (_master)
		atomic_setunlocked(_mutexptr);
	return ERR_OK;
}

/**Gibt einen Mutex frei
*/
tERROR tAtomicMutex::remove() {
	return ERR_OK;
};

/**Sperrt einen Mutex und wartet ggf. bis dies möglich ist
*/
tERROR tAtomicMutex::acquire() {
	if (!_connected)
		connect();
	atomic_lock(_mutexptr);
	_lockcount++;
	return ERR_OK;
};

/**Entsperrt einen Mutex
*/
tERROR tAtomicMutex::release() {
	if (!_connected)  connect();
	if (_lockcount>0) {
		_lockcount--;
	} else {
		debugprint('W',"tried to unlock mutex more often than to lock it", "tAtomicMutex::release");
	}
	atomic_unlock(_mutexptr);
	return ERR_OK;
};

/**Erstellt einen Mutex
*/
void tAtomicRWMutex::connect(void* mutexptr) {
	//Standardmutex nutzen oder angegebenen Speicherbereich?
	if (mutexptr == NULL) {
		_mutexptr = &_localmutex;
	} else {
		_mutexptr = (locktype*)mutexptr;
	}
	tAbstractMutex::connect(mutexptr);
};

/**Initialisiert einen Mutex
*/
tERROR tAtomicRWMutex::create() {
	if (!_connected)
		connect();
	if (_master)
		atomic_rwlock_setunlocked(_mutexptr);
	return ERR_OK;
}

/**Gibt einen Mutex frei
*/
tERROR tAtomicRWMutex::remove() {
	return ERR_OK;
};

/**Sperrt einen Mutex und wartet ggf. bis dies möglich ist
*/
tERROR tAtomicRWMutex::acquire() {
	if (!_connected)
		connect();
	atomic_writelock_acquire(_mutexptr);
	_lockcount++;
	return ERR_OK;
};

/**Sperrt einen Mutex zum Lesen und wartet ggf. bis dies möglich ist
*/
tERROR tAtomicRWMutex::acquire_read() {
	if (!_connected)
		connect();
	atomic_readlock_acquire(_mutexptr);
	_lockcount++;
	return ERR_OK;
};

/**Macht aus einem zum Lesen gesperrten Mutex ein zum Lesen gesperrtes
*/
tERROR tAtomicRWMutex::acquire_upgrade() {
	if (!_connected)  connect();
	if (_lockcount>0) {
		_lockcount--;
	} else {
		debugprint('W',"tried to unlock mutex more often than to lock it", "tAtomicMutex::release");
	}
	atomic_writelock_upgrade(_mutexptr);
	return ERR_OK;
};

/**Entsperrt einen Mutex
*/
tERROR tAtomicRWMutex::release() {
	if (!_connected)  connect();
	if (_lockcount>0) {
		_lockcount--;
	} else {
		debugprint('W',"tried to unlock mutex more often than to lock it", "tAtomicMutex::release");
	}
	atomic_writelock_release(_mutexptr);
	return ERR_OK;
};

/**Entsperrt einen zum Lesen gesperrten Mutex
*/
tERROR tAtomicRWMutex::release_read() {
	if (!_connected)  connect();
	if (_lockcount>0) {
		_lockcount--;
	} else {
		debugprint('W',"tried to unlock mutex more often than to lock it", "tAtomicMutex::release");
	}
	atomic_readlock_release(_mutexptr);
	return ERR_OK;
};

/**Macht aus einem zum Schreiben gesperrten Mutex wieder ein zum Lesen gesperrtes
*/
tERROR tAtomicRWMutex::release_downgrade() {
	if (!_connected)  connect();
	if (_lockcount>0) {
		_lockcount--;
	} else {
		debugprint('W',"tried to unlock mutex more often than to lock it", "tAtomicMutex::release");
	}
	atomic_writelock_downgrade(_mutexptr);
	return ERR_OK;
};

/**Erstellt einen Mutex
*/
void tPthreadMutex::connect(void* mutexptr) {
	//Standardmutex nutzen oder angegebenen Speicherbereich?
	if (mutexptr == NULL) {
		_mutexptr = &_localmutex;
	} else {
		_mutexptr = (pthread_mutex_t*)mutexptr;
	}
	tAbstractMutex::connect(mutexptr);
};

/**Initialisiert einen Mutex
*/
tERROR tPthreadMutex::create() {
	if (!_connected)
		connect();
	if (_master) {
		int errcode = pthread_mutex_init(_mutexptr, NULL);
		if (errcode!=0) {
			debugprint('E',"mutex creation failed (" + int2str(errcode) +")", "tPthreadMutex::create");
			return ERR_Mutex;
		}
	}
	return ERR_OK;
}

/**Gibt einen Mutex frei
*/
tERROR tPthreadMutex::remove() {
	if ((_connected) && (_master)) {
		int errcode = pthread_mutex_destroy(_mutexptr);
		if (errcode!=0) {
			debugprint('E',"destroying mutex failed (" + int2str(errcode) +")", "tPthreadMutex::remove");
			return ERR_Mutex;
		}
	}
	return ERR_OK;
};

/**Sperrt einen Mutex und wartet ggf. bis dies möglich ist
*/
tERROR tPthreadMutex::acquire() {
	if (!_connected)
		connect();
	int errcode = pthread_mutex_lock(_mutexptr);
	if (errcode!=0) {
		debugprint('E',"error locking mutex (" + int2str(errcode) +")", "tPthreadMutex::acquire");
		return ERR_Mutex;
	}
	_lockcount++;
	return ERR_OK;
};

/**Entsperrt einen Mutex
*/
tERROR tPthreadMutex::release() {
	if (!_connected)  connect();
	if (_lockcount>0) {
		_lockcount--;
	} else {
		debugprint('W',"tried to unlock mutex more often than to lock it", "tPthreadMutex::release");
	}
	int errcode = pthread_mutex_unlock(_mutexptr);
	if (errcode!=0) {
		debugprint('E',"error unlocking mutex (" + int2str(errcode) +")", "tPthreadMutex::release");
		return ERR_Mutex;
	}
	return ERR_OK;
};

/**Sperrt einen Mutex und wartet ggf. bis dies möglich ist
*/
tERROR tPosixSemaphore::acquire() {
	if (!_connected)
		connect();
	int errcode;
	do {errcode = sem_wait(_semptr);} while (errcode==EINTR);
	if (errcode!=0) {
		debugprint('E',"error locking semaphore", "tPosixSempahore::acquire");
		return ERR_Mutex;
	}
	_lockcount++;
	return ERR_OK;
};

/**Entsperrt eine Semaphore
*/
tERROR tPosixSemaphore::release() {
	if (!_connected)  connect();
	if (_lockcount>0) {
		_lockcount--;
	} else {
		debugprint('W',"tried to unlock semaphore more often than to lock it", "tPosixSempahore::release");
	}
	int errcode = sem_post(_semptr);
	if (errcode!=0) {
		debugprint('E',"error unlocking semaphore", "tPosixSempahore::release");
		return ERR_Mutex;
	}
	return ERR_OK;
};

/**Erstellt eine Semaphore
*/
void tUnnamedPosixSemaphore::connect(void* mutexptr) {
	//Standardsemaphore nutzen oder angegebenen Speicherbereich?
	if (mutexptr == NULL) {
		_semptr = &_localsem;
	} else {
		_semptr = (sem_t*)mutexptr;
	}
	tAbstractMutex::connect(mutexptr);
};

/**Initialisiert eine Semaphore
*/
tERROR tUnnamedPosixSemaphore::create() {
	if (!_connected)
		connect();
	if (_master) {
		int errcode = sem_init(_semptr, _pshared, 1); //2. Arg ungleich Null ==> nicht auf Proze?beschränkt; 3. Arg: Anzahl gelichzeitig erlaubter Zugriffe
		if (errcode!=0) {
			debugprintunix('E',"semaphore creation failed", "tUnnamedPosixSempahore::create");
			return ERR_Mutex;
		}
	}
	return ERR_OK;
}

/**Gibt eine Semaphore frei
*/
tERROR tUnnamedPosixSemaphore::remove() {
	if ((_connected) && (_master)) {
		int errcode = sem_destroy(_semptr);
		if (errcode!=0) {
			debugprintunix('E',"destroying semaphore failed", "tUnnamedPosixSempahore::remove");
			return ERR_Mutex;
		}
	}
	return ERR_OK;
};

/**Initialisiert eine Semaphore
*/
tERROR tNamedPosixSemaphore::create() {
	if (!_connected)
		connect();
	if (_master) {
		sem_unlink(_name.c_str());
		_semptr = sem_open(_name.c_str(), O_CREAT | O_EXCL, _mode, 1); //Letzter Parameter: initial value
	} else {
		_semptr = sem_open(_name.c_str(), 0);
	}
	if (((void*)_semptr==(char*)-1)||(_semptr==NULL)) {
		debugprintunix('E',"semaphore creation failed", "tNamedPosixSempahore::create");
		return ERR_Mutex;
	} else {
		return ERR_OK;
	}
}

/**Gibt eine Semaphore frei
*/
tERROR tNamedPosixSemaphore::remove() {
	if (_connected) {
		int errcode = sem_unlink(_name.c_str());
		if (errcode!=0) {
			debugprintunix('E',"destroying semaphore failed", "tNamedPosixSempahore::remove");
		} else {
			return ERR_OK;
		}
	}
	return ERR_Mutex;
};

/**Initialisiert einen Mutex
*/
tERROR tSystemVSemaphore::create() {
	if (!_connected) 
	{
		connect();
	}
	if (_master) 
	{
		//Wenn Semaphore schon existiert: löschen
		debugprint('I',"_master is true", "tSystemVSemaphore::create"); 
		if ((_semid = semget(crc32(_name), 1, 0)) != -1)
			semctl(_semid, 0, IPC_RMID);
		//Semaphore anlegen
		_semid = semget(crc32(_name), 1, _mode | IPC_CREAT); //Arg 2: Anzahl Semaphoren
	} 
	else 
	{
		debugprint('I',"_master is false", "tSystemVSemaphore::create");
		_semid = semget(crc32(_name), 1, 0);
		if(_semid ==-1)
		{	_semid = semget(crc32(_name), 1, _mode | IPC_CREAT);
			_master = true;			
			
		}

	}
	if (_semid==-1) 
	{
		debugprintunix('E',"semaphore creation failed :" + _name, "tSystemVSemaphore::create");
		return ERR_Mutex;
	}
	
	if (_master) 
	{
        	union semun sem_union;
        	sem_union.val = 1;	
        	if (semctl(_semid, 0, SETVAL, sem_union) == -1)
        	{
        		debugprintunix('E',"semaphore semctl  SETVAL:" + _name, "tSystemVSemaphore::create");
        	}
		debugprint('I',string("semaphore name: [") + _name + "], create by master semid is: " + int2str(_semid), "tSystemVSemaphore::create");	

	}
	else
	{
		debugprint('I',string("semaphore name: [") + _name + "], attach semid is: " + int2str(_semid), "tSystemVSemaphore::create");	

	}	

	 return ERR_OK;
}

/**Gibt einen Mutex frei
*/
tERROR tSystemVSemaphore::remove() {
	if ((_connected) && (_master)) {
		_semid = semctl(_semid, 0, IPC_RMID);
		if (_semid==-1) {
			debugprintunix('E',"destroying semaphore failed", "tSystemVSemaphore::remove");
		} else {
			return ERR_OK;
		}
	}
	return ERR_Mutex;
};

/**Sperrt einen Mutex und wartet ggf. bis dies möglich ist
*/
tERROR tSystemVSemaphore::acquire() {
	if (!_connected) {
		connect();
	}
	//if (_lockcount==0) 
	{
		struct sembuf sb = {0, -1, SEM_UNDO | IPC_NOWAIT}; //1. Parameter: Sem-Nr.; 2. Parameter: gewünschte Wertänderung; 3. Parameter: Flags
		if (semop(_semid, &sb, 1) == -1) {    //3. Parameter: Anzahl zu bearbeitender Semaphoren
			debugprintunix('E',"error locking semaphore:" + int2str(_semid), "tSystemVSemaphore::acquire");
			return ERR_Mutex;
		}
	}
	//_lockcount++;
	return ERR_OK;
};

tERROR tSystemVSemaphore::wait( int nTimes) 
{
	if (!_connected) {
		connect();
	}
	int nTryTimer = 0;       
	if(nTimes<=0)
	{
loop:
		struct sembuf sb = {0, -1, SEM_UNDO}; //1. Parameter: Sem-Nr.; 2. Parameter: gewünschte Wertänderung; 3. Parameter: Flags
		if(semop(_semid, &sb, 1) < 0)
		{
			debugprintunix('E'," tid: "+int2str((int)pthread_self()) +" lock mutex semaphore:" + int2str(_semid), "tSystemVSemaphore::wait");
			if(errno == EINTR)
				goto loop;
			return ERR_Mutex;
		}
		return ERR_OK;
	}
    else
    {
       struct sembuf sb = {0, -1, SEM_UNDO |IPC_NOWAIT}; //1. Parameter: Sem-Nr.; 2. Parameter: gewünschte Wertänderung; 3. Parameter: Flags
    	while (semop(_semid, &sb, 1) < 0) 
    	{    //3. Parameter: Anzahl zu bearbeitender Semaphoren     	
    		debugprintunix('E'," tid: "+int2str((int)pthread_self()) +" locking error semaphore:" + int2str(_semid), "tSystemVSemaphore::wait");
    		if(++nTryTimer >nTimes || errno != EAGAIN)
    			return ERR_Mutex;
    		usleep_r(10);
    	}
    	return ERR_OK;
   }
};

/**Versucht, eine Semaphore zu sperren
*/
tERROR tSystemVSemaphore::tryacquire() {
	if (!_connected) {
		connect();
	}
	//if (_lockcount==0) 
	{
		struct sembuf sb = {0, -1, SEM_UNDO | IPC_NOWAIT};
		if (semop(_semid, &sb, 1) == -1) {
			if (errno!=EAGAIN)
				debugprintunix('E',"error locking semaphore:" + int2str(_semid), "tSystemVSemaphore::tryacquire");
			return ERR_Mutex;
		}
	}
	//_lockcount++;
	return ERR_OK;
};

/**Entsperrt einen Mutex
*/
tERROR tSystemVSemaphore::release() {
	if (!_connected)  
		connect();
	if (_lockcount>0) {
		_lockcount--;
	} else {
		//debugprint('W',"tried to unlock mutex more often than to lock it", "tSystemVSemaphore::release");
	}
	//if (_lockcount==0) 
	{
loop:	
		struct sembuf sb = {0, 1, SEM_UNDO};
		if (semop(_semid, &sb, 1) == -1) 
		{
			debugprintunix('E',"error unlocking semaphore", "tSystemVSemaphore::release");
			if(errno == EINTR)
				goto loop;
			return ERR_Mutex;
		}
	}
	//debugprint('I', "success release semaphore :" + int2str(_semid)+ " by thread: " + int2str(pthread_self()), "tSystemVSemaphore::release");
	return ERR_OK;
};


