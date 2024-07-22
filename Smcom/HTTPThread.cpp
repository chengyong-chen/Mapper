#include "stdafx.h"
#include <afxwin.h>

#include "ComThread.h"
#include "HTTPThread.h"
#include "HTTPSession.h"

#include "../Socket/ExStdCFunction.h"
#include <tchar.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHTTPThread

IMPLEMENT_DYNCREATE(CHTTPThread, CComThread)

CHTTPThread::CHTTPThread()
	: CComThread()
{
	m_pHTTPSession = nullptr;
}

CHTTPThread::~CHTTPThread()
{
}

BOOL CHTTPThread::InitInstance()
{
	m_pMainWnd = nullptr;

	CString host, name, aliase, ip;
	if(GetLocalHostInfos(host, name, aliase, ip)==FALSE)
	{
		return FALSE;
	}

	m_pHTTPSession = new CHTTPSession(_T("Trasvr software"), PRE_CONFIG_INTERNET_ACCESS);//LOCAL_CONFIG_INTERNET_ACCESS 直接上网
	//	if(m_pHTTPSession->Open(_T("http://www.starg3.com/starg3/outputnew.php"),"?id=66000138&pw=000000&nm=10&tz=8") == TRUE)
	if(m_pHTTPSession->Open(_T("http://www.starg3.com/starg3/outputgrp.php"), _T("?id=99000001&pw=000000&tz=0"))==TRUE)
	{
		m_bValid = true;
		PutMsgToMessageWnd(false ? _T("基于HTTP的通讯服务打开成功") : _T("Succeed opening HTTP's communication service!"));

		return CComThread::InitInstance();
	}
	else
	{
		PutMsgToMessageWnd(false ? _T("基于HTTP的通讯服务打开失败") : _T("Failed when openning HTTP's communication service"));

		m_pHTTPSession->Close();
		delete m_pHTTPSession;
		m_pHTTPSession = nullptr;
		return FALSE;
	}
}

int CHTTPThread::ExitInstance()
{
	if(m_pHTTPSession!=nullptr)
	{
		m_pHTTPSession->Close();
		delete m_pHTTPSession;
		m_pHTTPSession = nullptr;
	}

	return CComThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CHTTPThread, CComThread)
	//{{AFX_MSG_MAP(CHTTPThread)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHTTPThread message handlers
