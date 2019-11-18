#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>
#include <iostream>
using namespace std;

class CException
{
public:
	enum { EMPTY_ERROR_NO = 32767, EMPTY_LINE = -1, };

protected:
	int m_iErrorNo;
	string m_strErrorMsg;
	string m_strFile;
	int m_iLine;

public:
	CException(int iErrorNo, const char* sErrorMsg, const char* sFile = NULL, int iLine = EMPTY_LINE);
	CException(const char* sErrorMsg, const char* sFile = NULL, int iLine = EMPTY_LINE);
	int ErrorNo() { return m_iErrorNo; }
	virtual ~CException() {}
  friend ostream& operator << (ostream& os, const CException& e);
};


class CMemException : public CException
{
public:
	CMemException(const char* sErrorMsg, const char* sFile = NULL, int iLine = CException::EMPTY_LINE)
			: CException(sErrorMsg, sFile, iLine) {}
	virtual ~CMemException() {}
	friend ostream& operator << (ostream& os, CMemException& e);
};


class CFileException : public CException
{
public:
	CFileException(const char* sErrorMsg, const char* sFile = NULL, int iLine = CException::EMPTY_LINE)
			: CException(sErrorMsg, sFile, iLine) {}
	virtual ~CFileException() {}
	friend ostream& operator << (ostream& os, CFileException& e);
};


class CFileLockException : public CException
{
public:
	CFileLockException(const char* sErrorMsg, const char* sFile = NULL, int iLine = CException::EMPTY_LINE)
			: CException(sErrorMsg, sFile, iLine) {}
	virtual ~CFileLockException() {}
	friend ostream& operator << (ostream& os, CFileLockException& e);
};


class CHtmlException : public CException
{
public:
	CHtmlException(const char* sErrorMsg, const char* sFile = NULL, int iLine = CException::EMPTY_LINE)
			: CException(sErrorMsg, sFile, iLine) {}
	virtual ~CHtmlException() {}
	friend ostream& operator << (ostream& os, CHtmlException& e);
};


class CSemException : public CException
{
public:
	CSemException(const char* sErrorMsg, const char* sFile = NULL, int iLine = CException::EMPTY_LINE)
			: CException(sErrorMsg, sFile, iLine) {}
	virtual ~CSemException() {}
	friend ostream& operator << (ostream& os, CSemException& e);
};


class CShmException : public CException
{
public:
	CShmException(const char* sErrorMsg, const char* sFile = NULL, int iLine = CException::EMPTY_LINE)
			: CException(sErrorMsg, sFile, iLine) {}
	virtual ~CShmException() {}
	friend ostream& operator << (ostream& os, CShmException& e);
};


class CSocketException : public CException
{
public:
	CSocketException(const char* sErrorMsg, const char* sFile = NULL, int iLine = CException::EMPTY_LINE)
			: CException(sErrorMsg, sFile, iLine) {}
	virtual ~CSocketException() {}
	friend ostream& operator << (ostream& os, CSocketException& e);
};


class CThreadException : public CException
{
public:
	CThreadException(const char* sErrorMsg, const char* sFile = NULL, int iLine = CException::EMPTY_LINE)
			: CException(sErrorMsg, sFile, iLine) {}
	virtual ~CThreadException() {}
	friend ostream& operator << (ostream& os, CThreadException& e);
};


class CNBTcpSvrException : public CException
{
public:
	CNBTcpSvrException(const char* sErrorMsg, const char* sFile = NULL, int iLine = CException::EMPTY_LINE)
			: CException(sErrorMsg, sFile, iLine) {}
	virtual ~CNBTcpSvrException() {}
	friend ostream& operator << (ostream& os, CNBTcpSvrException& e);
};


class CPreforkSvrException : public CException
{
public:
	CPreforkSvrException(const char* sErrorMsg, const char* sFile = NULL, int iLine = CException::EMPTY_LINE)
			: CException(sErrorMsg, sFile, iLine) {}
	virtual ~CPreforkSvrException() {}
	friend ostream& operator << (ostream& os, CPreforkSvrException& e);
};

class CDBException : public CException
{
public:
	CDBException(const char* sErrorMsg, const char* sFile = NULL, int iLine = CException::EMPTY_LINE)
			: CException(sErrorMsg, sFile, iLine) {}
	virtual ~CDBException() {}
	friend ostream& operator << (ostream& os, CDBException& e);
};
#endif
