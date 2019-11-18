#include <cerrno>
#include <time.h>
#include "thread.h"
#include "frame_utils.h"
CThMutex::CThMutex()
{
	pthread_mutex_init(&m_tMutex, NULL);
}


CThMutex::~CThMutex()
{
	int iResult = pthread_mutex_destroy(&m_tMutex);
	if (iResult)
		throw CThreadException(strerror(iResult), __FILE__, __LINE__);
}


bool CThMutex::Lock()
{
	int iResult = pthread_mutex_lock(&m_tMutex);
	if (iResult)
		throw CThreadException(strerror(iResult), __FILE__, __LINE__);

	return true;
}


bool CThMutex::TryLock()
{
	int iResult = pthread_mutex_trylock(&m_tMutex);
	if (iResult)
	{
		if (iResult == EBUSY)
			return false;

		throw CThreadException(strerror(iResult), __FILE__, __LINE__);
	}

	return true;
}


bool CThMutex::Unlock()
{
	int iResult = pthread_mutex_unlock(&m_tMutex);
	if (iResult)
		throw CThreadException(strerror(iResult), __FILE__, __LINE__);

	return true;
}


CThSem::CThSem(int iValue)
{
	if (sem_init(&m_tSem, 0, iValue) < 0)
		throw CThreadException(strerror(errno), __FILE__, __LINE__);
}


CThSem::~CThSem()
{
	if (sem_destroy(&m_tSem) < 0)
		throw CThreadException(strerror(errno), __FILE__, __LINE__);
}


bool CThSem::Wait()
{
	sem_wait(&m_tSem);
	return true;
}


bool CThSem::TryWait()
{
	if (sem_trywait(&m_tSem) < 0)
	{
		if (errno == EAGAIN)
			return false;

		throw CThreadException(strerror(errno), __FILE__, __LINE__);
	}

	return true;
}


bool CThSem::Post()
{
	if (sem_post(&m_tSem) < 0)
		throw CThreadException(strerror(errno), __FILE__, __LINE__);

	return true;
}


int CThSem::GetValue()
{
	int iValue;
	sem_getvalue(&m_tSem, &iValue);
	return iValue;
}


CThCond::CThCond()
{
	pthread_cond_init(&m_tCond, NULL);
}


CThCond::~CThCond()
{
	int iResult = pthread_cond_destroy(&m_tCond);
	if (iResult)
		throw CThreadException(strerror(iResult), __FILE__, __LINE__);
}


bool CThCond::Signal()
{
	pthread_cond_signal(&m_tCond);
	return true;
}


bool CThCond::Broadcast()
{
	pthread_cond_broadcast(&m_tCond);
	return true;
}


bool CThCond::Wait(CThMutex& stThMutex)
{
	pthread_cond_wait(&m_tCond, &stThMutex.m_tMutex);
	return true;
}


bool CThCond::TimedWait(CThMutex& stThMutex, const struct timespec* pstAbsTime)
{
	int iResult = pthread_cond_timedwait(&m_tCond, &stThMutex.m_tMutex, pstAbsTime);
	if (iResult)
	{
		if (iResult == ETIMEDOUT || iResult == EINTR)
			return false;

		throw CThreadException(strerror(iResult), __FILE__, __LINE__);
	}

	return true;
}


CThRWLock::CThRWLock(bool bWriterFirst, int iMaxReader)
		: m_bWriterFirst(bWriterFirst), m_iMaxReader(iMaxReader)
{
	m_iWaitingReader = 0;
	m_iWaitingWriter = 0;
	m_iHoldingLock = 0;
}


CThRWLock::~CThRWLock()
{
	if (m_iWaitingReader != 0 || m_iWaitingWriter != 0 || m_iHoldingLock != 0)
		throw CThreadException("RWLock is busy.", __FILE__, __LINE__);
}


bool CThRWLock::ReadLock()
{
	m_stMutex.Lock();
	while ( (m_iHoldingLock < 0) || (m_iMaxReader > 0 && m_iHoldingLock == m_iMaxReader)
			|| (m_bWriterFirst && m_iWaitingWriter > 0) )
	{
		m_iWaitingReader++;
		m_stReaderCond.Wait(m_stMutex);
		m_iWaitingReader--;
	}

	m_iHoldingLock++;
	m_stMutex.Unlock();
	return true;
}


bool CThRWLock::WriteLock()
{
	m_stMutex.Lock();
	while (m_iHoldingLock != 0)
	{
		m_iWaitingWriter++;
		m_stWriterCond.Wait(m_stMutex);
		m_iWaitingWriter--;
	}

	m_iHoldingLock = -1;
	m_stMutex.Unlock();
	return true;
}


bool CThRWLock::Unlock()
{
	m_stMutex.Lock();
	if (m_iHoldingLock == -1)
		m_iHoldingLock = 0;
	else if (m_iHoldingLock > 0)
		m_iHoldingLock--;
	else
		throw CThreadException("unknown lock status while unlocking.", __FILE__, __LINE__);

	if (m_bWriterFirst)
	{
		if (m_iWaitingWriter > 0)
		{
			if (m_iHoldingLock == 0)
				m_stWriterCond.Signal();
		}
		else if (m_iWaitingReader > 0)
		{
			m_stReaderCond.Broadcast();
		}
	}
	else
	{
		if (m_iWaitingWriter > 0 && m_iHoldingLock == 0)
			m_stWriterCond.Signal();
		else if (m_iWaitingReader > 0)
			m_stReaderCond.Broadcast();
	}

	m_stMutex.Unlock();
	return true;
}


