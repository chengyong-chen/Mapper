#include "stdafx.h"

#include "datasrc.h"
#include "prognot.h"

#ifdef _WINDOWS
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif


CDataSource::CDataSource(IProgressNotification* pNotification)
{
	m_pszName = nullptr;
	m_FileSize = 0;
	m_pNotification = pNotification;
}

CDataSource::~CDataSource()
{
}

void CDataSource::Open(LPCTSTR pszName, int FileSize)
{
	// Data source may not be open already!
	ASSERT(!m_FileSize);

	m_pszName = new TCHAR[_tcslen(pszName)+1];
	_tcscpy(m_pszName, pszName);
	m_FileSize = FileSize;
	m_BytesRead = 0;
}

void CDataSource::Close()
{
	if(m_pNotification)
		// be smart and tell the world: ich habe fertig!
		m_pNotification->OnProgress(1);
	delete m_pszName;
	m_pszName = nullptr;
}

TCHAR* CDataSource::GetName() const
{
	return m_pszName;
}

BYTE* CDataSource::ReadNBytes(int n)
{
	const int OldBytesRead = m_BytesRead;
	m_BytesRead += n;
	if(m_BytesRead/1024>OldBytesRead/1024&&m_pNotification)
		m_pNotification->OnProgress(double(m_BytesRead)/m_FileSize);
	CheckEOF();
	return nullptr;
}

// Jo Hagelberg 15.4.99:
// for use by other libs that handle progress internally (eg libjpeg)
void CDataSource::OProgressNotification(double part) const
{
	if(m_pNotification)
	{
		m_pNotification->OnProgress(part);
	}
}
