#include "stdafx.h"
#include "MessageEar.h"
#include "mmsystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMessageEar

CMessageEar::CMessageEar()
{
	m_nLine = 0;
}

CMessageEar::~CMessageEar()
{
}

BEGIN_MESSAGE_MAP(CMessageEar, CEdit)
	//{{AFX_MSG_MAP(CMessageEar)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMessageEar message handlers
void CMessageEar::AddMessage(CString& strMsg)
{
	const COleDateTime time = COleDateTime::GetCurrentTime();
	CString strTime;

	// CString file = "receivedmsg.wav";					
	// PlaySound(file, nullptr, SND_FILENAME|SND_ASYNC);

	if(500<m_nLine) // 信息滚动框显示信息的行数,不超过1000行
		m_strContent = m_strContent.Left(m_strContent.ReverseFind(_T('\n')));
	else
		m_nLine++;

	m_strContent.Insert(0, _T("\r\n"));
	m_strContent.Insert(0, strMsg);
	strTime.Format(_T("<%s>： "), time.Format(_T("%Y/%m/%d %H:%M")));
	m_strContent.Insert(0, strTime);

	SetWindowText(m_strContent);
}

void CMessageEar::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// CEdit::OnChar(nChar, nRepCnt, nFlags);
}
