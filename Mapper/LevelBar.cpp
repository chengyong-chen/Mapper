#include "stdafx.h"

#include "LevelBar.h"
#include "GrfView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CLevelBar::CLevelBar(CWnd* pParent)
	: CDialog(CLevelBar::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLevelBar)
	//}}AFX_DATA_INIT

	m_fLevel = 1;
	m_minLevel = 0;
	m_maxLevel = 25;
}

CLevelBar::~CLevelBar()
{
}

void CLevelBar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLevelBar)
	DDX_Text(pDX, IDC_LEVEL, m_fLevel);
	DDV_MinMaxFloat(pDX, m_fLevel, m_minLevel, m_maxLevel);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLevelBar, CDialog)
	//{{AFX_MSG_MAP(CLevelBar)
	ON_COMMAND(IDC_EXCUTE, OnExcute)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CLevelBar::SetRange(float fMin, float fMax)
{
	m_minLevel = fMin;
	m_maxLevel = fMax;
}

void CLevelBar::SetLevel(float fLevel)
{
	if(fLevel!=m_fLevel)
	{		
		CString str;
		str.Format(_T("%.3f"), fLevel);
		GetDlgItem(IDC_LEVEL)->SetWindowText(str);
		m_fLevel = fLevel;
	}
}

void CLevelBar::OnExcute()
{
	UpdateData();

	CFrameWnd* pMainFrame = (CFrameWnd*)AfxGetMainWnd();
	CFrameWnd* pChildFrame = (CFrameWnd*)pMainFrame->GetActiveFrame();
	if(pChildFrame!=nullptr)
	{
		CWnd* pWnd = pChildFrame->GetActiveView();
		if(pWnd!=nullptr)
		{
			((CGrfView*)pWnd)->ChangeViewLevel(m_fLevel);
		}
	}
}

void CLevelBar::OnOK()
{
}

BOOL CLevelBar::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_KEYDOWN)
	{
		switch(pMsg->wParam)
		{
		case VK_ESCAPE:
			return TRUE;
			break;
		case VK_RETURN:
			if(pMsg->hwnd==GetDlgItem(IDC_LEVEL)->m_hWnd)
			{
				OnExcute();
			}
			else
			{
			}

			break;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}
