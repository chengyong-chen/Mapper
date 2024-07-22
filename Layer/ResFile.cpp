#include "stdafx.h"

#include "ResFile.h"
#include "io.h"
#include "time.h"
#include "Mmsystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CResFile::CResFile()
{
	m_nIndex        = -1;
	CurrentPosition = 0;
	m_PackHeader    = nullptr;
	m_hFile         = nullptr;
}

BOOL CResFile::Open(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError)
{
	if(CFile::Open(lpszFileName,nOpenFlags, pError) == FALSE)
	{
		m_hFile = nullptr;
		AfxMessageBox(_T("资源包不能被打开!"));
		return FALSE;
	}
    
	PACKHEADER  header;
	CFile::Read(&header,sizeof(PACKHEADER));
	long FileNumber=header.dwTotalFile;
	m_PackHeader=(PACKHEADER*)new char[sizeof(PACKHEADER)+1000*sizeof(FILEENTRY)];
	CFile::SeekToBegin();	
	CFile::Read(m_PackHeader,sizeof(PACKHEADER)+1000*sizeof(FILEENTRY));
	return TRUE;
}

void CResFile::Close()
{
	if(m_PackHeader != nullptr)
	{
		delete m_PackHeader;
		m_PackHeader = nullptr;
		m_nIndex=-1;
		CurrentPosition=0;
	}

	CFile::Close();
}

bool CResFile::OpenFile(CString strFile,UINT nFlags,CFileException* pError)
{   
	if(m_hFile == nullptr)
	{
		m_nIndex        = -1;
		CurrentPosition = 0;
		return false;
	}

	for(DWORD i=0; i<m_PackHeader->dwTotalFile; i++)
	{
		if(strFile.Find((m_PackHeader->feFileList[i]).szFileName) != -1)
		{
			m_nIndex = i;
			CurrentPosition = (m_PackHeader->feFileList[i]).dwPosition;
			CFile::Seek(CurrentPosition,CFile::begin);
			return TRUE;
		}
	}

	return false;
}

void CResFile::CloseFile()
{
	m_nIndex=-1;
	CurrentPosition=0;	
}

UINT CResFile::Read(void* lpBuf,UINT nCount)
{
	if(m_hFile != nullptr)
	{
		CFile::Seek(CurrentPosition,CFile::begin);
	
		long counts = CFile::Read(lpBuf, nCount);

		CurrentPosition = CurrentPosition + counts;
		return counts;
	}

	return 0;
}
