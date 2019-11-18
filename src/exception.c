/***************************************************************************
                          exception.cpp  -  description
                             -------------------
    begin                : Fri Jul 20 2001
    copyright            : (C) 2001 by Mark
    email                : alben@yeah.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "exception.h"

CException::CException(int iErrorNo, const char* sErrorMsg, const char* sFile, int iLine)
{
	m_iErrorNo = iErrorNo;
	if (sErrorMsg)
		m_strErrorMsg = sErrorMsg;
	if (sFile)
		m_strFile = sFile;
	m_iLine = iLine;
}


CException::CException(const char* sErrorMsg, const char* sFile, int iLine)
{
	m_iErrorNo = EMPTY_ERROR_NO;
	if (sErrorMsg)
		m_strErrorMsg = sErrorMsg;
	if (sFile)
		m_strFile = sFile;
	m_iLine = iLine;
}


ostream& operator << (ostream& os, const CException& e)
{
	if (e.m_iErrorNo != CException::EMPTY_ERROR_NO)
		os << e.m_iErrorNo << ":";

	os << e.m_strErrorMsg;

	if (!e.m_strFile.empty() && e.m_iLine != CException::EMPTY_ERROR_NO)
		os << " [" << e.m_strFile << ":" << e.m_iLine << "]";

	return os;
}


ostream& operator << (ostream& os, CMemException& e)
{
	return os << (CException&)e;
}


ostream& operator << (ostream& os, CFileException& e)
{
	return os << (CException&)e;
}


ostream& operator << (ostream& os, CFileLockException& e)
{
	return os << (CException&)e;
}


ostream& operator << (ostream& os, CHtmlException& e)
{
	return os << (CException&)e;
}


ostream& operator << (ostream& os, CSemException& e)
{
	return os << (CException&)e;
}


ostream& operator << (ostream& os, CShmException& e)
{
	return os << (CException&)e;
}


ostream& operator << (ostream& os, CSocketException& e)
{
	return os << (CException&)e;
}


ostream& operator << (ostream& os, CThreadException& e)
{
	return os << (CException&)e;
}


ostream& operator << (ostream& os, CNBTcpSvrException& e)
{
	return os << (CException&)e;
}


ostream& operator << (ostream& os, CPreforkSvrException& e)
{
	return os << (CException&)e;
}

ostream& operator << (ostream& os, CDBException& e)
{
	return os << (CException&)e;
}
