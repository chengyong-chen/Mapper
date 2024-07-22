#include "stdafx.h"
#include "Global.h"
#include "LogToWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CLogToWnd::CLogToWnd(CWnd* pParent)
	: m_pParent(pParent)
{
}

CLogToWnd::~CLogToWnd()
{
}

/////////////////////////////////////////////////////////////////////////////
// CLogToWnd message handlers
void CLogToWnd::OutputString(const char* str)
{
	if(m_pParent!=nullptr)
	{
		m_pParent->PostMessage(WM_LOGMESSAGE, 0, reinterpret_cast<LPARAM>(str));
	}
}
