#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>
#include <semaphore.h>
#include "frame_mutex.h"
#include "exception.h"

inline void usleep_r(unsigned long usec)
{
	struct timespec rqtp;
	rqtp.tv_sec = usec/1000000;       
	rqtp.tv_nsec = usec%1000000*1000;
	nanosleep(&rqtp, NULL);
}

class CThCond;
class CThMutex
{
	friend class CThCond;

protected:
	pthread_mutex_t m_tMutex;

public:
	CThMutex();
	virtual ~CThMutex();

	bool Lock();
	bool TryLock();
	bool Unlock();
};


class CThSem
{
protected:
	sem_t m_tSem;

public:
	CThSem(int iValue = 0);
	virtual ~CThSem();

	bool Wait();
	bool TryWait();
	bool Post();
	int GetValue();
};


class CThCond
{
protected:
	pthread_cond_t m_tCond;

public:
	CThCond();
	virtual ~CThCond();

	bool Signal();
	bool Broadcast();
	bool Wait(CThMutex& stThMutex);
	bool TimedWait(CThMutex& stThMutex, const struct timespec* pstAbsTime);
};


class CThRWLock
{
public:
	CThRWLock(bool bWriterFirst = false, int iMaxReader = 0);	//0 means no limit
	~CThRWLock();

	bool ReadLock();
	bool WriteLock();
	bool Unlock();

protected:
	const bool m_bWriterFirst;
	const int m_iMaxReader;

	CThMutex m_stMutex;
	CThCond m_stReaderCond;
	CThCond m_stWriterCond;
	int m_iWaitingReader;
	int m_iWaitingWriter;
	int m_iHoldingLock;	//-1 means writer holds the lock, >0 means reader count hloding the lock
};

class CLock
{
private:
	pthread_mutex_t *m_tMutex;
	int m_err;
public:
	CLock(pthread_mutex_t &mutex)
	{
		 m_tMutex=&mutex;
		 m_err = pthread_mutex_lock(m_tMutex);
		 if(m_err != 0)
	 	{
			debugprint('I', " pthread_mutex_lock error:" + int2str(errno) +",  " + strerror(errno), "CLock::CLock");
	 	}
	}
	~CLock()
	{
		if(m_tMutex)
		{
			m_err = pthread_mutex_unlock(m_tMutex);
			if(m_err != 0)
			{
				debugprint('I', " pthread_mutex_unlock error:" + int2str(errno) +",  " + strerror(errno), "CLock::~CLock");
			}
		}
	}
};

class CSemLock
{
private:
	tSystemVSemaphore *m_pSem;
	bool m_bLock;
public:
	CSemLock(tSystemVSemaphore *pSem)
	{
		 m_pSem=pSem;
		 m_bLock = false;		 
	}
	~CSemLock()
	{
		if(m_bLock && m_pSem != NULL)
		 	m_pSem->release();
	}
	bool TryLock( int nTimes)
	{
		if(m_pSem!= NULL)
		 	m_bLock = (m_pSem->wait(nTimes) == ERR_OK);
		return m_bLock;
	}
};

#endif
