// MapEdit.cpp : implementation file
//

#include "stdafx.h"
#include "Trasvr.h"
#include "MessageWnd.h"

#include "..\Smcom\Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TIMER_SHOW_MESSSAGE    100

const UINT WM_MAPEDITADDSTRING = WM_USER + 4;

/////////////////////////////////////////////////////////////////////////////
// CMessageWnd

CMessageWnd::CMessageWnd()
{	
	m_nLine = 0;
}

CMessageWnd::~CMessageWnd()
{
	while(m_msgShowQueue.empty() == FALSE)
	{
		delete m_msgShowQueue.front();
		m_msgShowQueue.pop();
	}
}


BEGIN_MESSAGE_MAP(CMessageWnd, CEdit)
	//{{AFX_MSG_MAP(CMessageWnd)
	ON_WM_CREATE()
	ON_WM_CHAR()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MAPEDITADDSTRING, OnAddNewString)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMessageWnd message handlers

void CMessageWnd::OnTimer(UINT nIDEvent) 
{
	switch(nIDEvent)
	{
	case TIMER_SHOW_MESSSAGE:
		{
			while(!m_msgShowQueue.empty())
			{
				m_nLine ++;
				if(500 < m_nLine) // 信息滚动框显示信息的行数,不超过1000行
				{
					m_strContent.Empty();
					m_nLine = 0;
				}
				CString* p = m_msgShowQueue.front();
				m_msgShowQueue.pop();
				if(p != nullptr) 
				{
					COleDateTime time = COleDateTime::GetCurrentTime();
					m_strContent.Insert(0, _T("\r\n"));	
					m_strContent.Insert(0, *p);
					m_strContent.Insert(0, time.Format(_T("<%Y/%m/%d %H:%M> ")));
					SetWindowText(m_strContent);

					delete p;
					p = nullptr;
				}
			}
		}
		return;
	}
	
	CEdit::OnTimer(nIDEvent);
}

void CMessageWnd::AddMsg(CString* pStr)
{
	if(pStr != nullptr) 
	{
		m_msgShowQueue.push(pStr);
	}
}

int CMessageWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if(CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	// LONG style = GetWindowLong(m_hWnd, GWL_STYLE)|WS_HSCROLL|WS_VSCROLL|ES_AUTOVSCROLL|ES_LEFT|ES_MULTILINE|ES_WANTRETURN;
	// SetWindowLong(m_hWnd, GWL_STYLE, style);
	// ModifyStyle(0, ES_AUTOHSCROLL |ES_AUTOVSCROLL|WS_HSCROLL|WS_VSCROLL);
	// ShowScrollBar(SB_VERT, TRUE);
	this->SetTimer(TIMER_SHOW_MESSSAGE, 200, nullptr);

	return 0;
}

void CMessageWnd::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{	
	// CEdit::OnChar(nChar, nRepCnt, nFlags);
}


//从其他线程发来更新数据命令。
LRESULT CMessageWnd::OnAddNewString(WPARAM pstr, LPARAM hMutex)
{
	AddMsg((CString*)(pstr));

	return 0;
}

BOOL CMessageWnd::PreCreateWindow(CREATESTRUCT &cs)
{
	cs.style |= (WS_CHILD|WS_VISIBLE|WS_VSCROLL|ES_LEFT|ES_MULTILINE|ES_AUTOHSCROLL|WS_EX_RIGHTSCROLLBAR);
	return CEdit::PreCreateWindow(cs);
}

