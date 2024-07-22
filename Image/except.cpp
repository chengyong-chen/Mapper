#include "stdafx.h"

#include "except.h"

#ifdef _WINDOWS
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

CTextException::CTextException(int Code, const char* pszErr)
{
#ifdef _WINDOWS
	SetErrorMode(0); // Restore normal error handling just in case
	// file system error checking was disabled.
#endif

	m_pszErr = new char[strlen(pszErr)+1];
	strcpy(m_pszErr, pszErr);

	m_Code = Code;
}

CTextException::CTextException(const CTextException& ex)
{
	m_Code = ex.GetCode();
	m_pszErr = new char[strlen((const char*)ex)+1];
	strcpy(m_pszErr, (const char*)ex);
}

CTextException::~CTextException()
{
	delete m_pszErr;
}

int CTextException::GetCode() const
{
	return m_Code;
}

CTextException::operator const char* () const
// This operator allows the exception to be treated as a string
// whenever needed.
{
	return m_pszErr;
}
