#include "stdafx.h"
#include "PyramidMap.h"
#include "Downloader.h"


// CDownloader

CDownloader::CDownloader()
{
}

CDownloader::~CDownloader()
{
}


BEGIN_MESSAGE_MAP(CDownloader, CAsyncMonikerFile)
	//{{AFX_MSG_MAP(CDownloader)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDownloader member functions

void CDownloader::OnDataAvailable(DWORD dwSize, DWORD bscfFlag) 
{
	if ((bscfFlag & BSCF_LASTDATANOTIFICATION) != 0)
	{
		char* m_buffer = (char*)malloc(dwSize*sizeof(char) + 1);
		try
		{
			UINT nBytesRead = Read(m_buffer,dwSize);
			m_buffer[nBytesRead] = '\0';
			CFile file;
			if( file.Open( "Ticker2.txt", CFile::modeCreate | CFile::modeWrite ) ) 
			{				
				file.Write(m_buffer, _tcslen(m_buffer));
				file.Close();
			
			}			
			free(m_buffer);
		}
		catch(CFileException* pe)
		{
			OutputDebugString(ex->m_strError+"\r\n");
			pe->Delete ();
		}
	}

	CAsyncMonikerFile::OnDataAvailable(dwSize, bscfFlag);
}

void CDownloader::OnProgress(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, LPCTSTR szStatusText) 
{

	CAsyncMonikerFile::OnProgress(ulProgress, ulProgressMax, ulStatusCode, szStatusText);
}


void CDownloader::WaitFinished(void)
{
	m_abort=true;
	while(m_bAsyncloading == false)
	{
		Sleep(10);
	}
}