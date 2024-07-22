#include "stdafx.h"
#include "Global.h"
#include "string"
#include "OutputPane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

COutputPane::COutputPane()
	: m_logtownd(this)
{
}

COutputPane::~COutputPane()
{
	m_logtownd.Unintialize();
}

BEGIN_MESSAGE_MAP(COutputPane, CDockablePane)
	ON_WM_CONTEXTMENU()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_MESSAGE(WM_LOGMESSAGE, OnLogMessage)
END_MESSAGE_MAP()

int COutputPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CDockablePane::OnCreate(lpCreateStruct)==-1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();
	if(m_messagebox.Create(LBS_NOINTEGRALHEIGHT|WS_CHILD|WS_VISIBLE|WS_VSCROLL, rectDummy, this, 2)==FALSE)
	{
		TRACE0("Failed to create output windows\n");
		return -1; // fail to create
	}
	else if(m_logtownd.Initialize()!=0)
		return -1;
	else
		return 0;
}

void COutputPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	m_messagebox.SetWindowPos(nullptr, -1, -1, cx, cy, SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);
}

BOOL COutputPane::DestroyWindow()
{
	m_logtownd.Unintialize();

	return CDockablePane::DestroyWindow();
}

void COutputPane::OnEditClear()
{
	MessageBox(_T("Clear output"));
}

LONG COutputPane::OnLogMessage(WPARAM, LPARAM lParam)
{
	while(m_messagebox.GetCount()>100)
	{
		m_messagebox.DeleteString(0);
	}
	const std::string converted((const char*)lParam);
	COleDateTime time = COleDateTime::GetCurrentTime();
	CStringA string;
	string.Format("%s:  %s\n",CStringA(time.Format(_T("%H:%M:%S"))),converted.c_str());
	string.Trim("\r");
	string.Trim("\n");
	m_messagebox.AddString(CString(string));
	m_messagebox.SetCaretIndex(m_messagebox.GetCount()-1);
	return 1L;
}
