#include "stdafx.h"

#include "ScaleBar.h"
#include "GrfView.h"

#include "../Utility/Number.hpp"
#include "../Utility/Language.hpp"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CScaleBar::CScaleBar(CWnd* pParent)
	: CDialog(CScaleBar::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScaleBar)
	//}}AFX_DATA_INIT

	m_fScale = 1;
	m_minScale = 10000000000;
	m_maxScale = 0;
}

CScaleBar::~CScaleBar()
{
}

void CScaleBar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScaleBar)
	DDX_Text(pDX, IDC_SCALE, m_fScale);
	DDV_MinMaxFloat(pDX, m_fScale, m_maxScale, m_minScale);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CScaleBar, CDialog)
	//{{AFX_MSG_MAP(CScaleBar)
	ON_COMMAND(IDC_EXCUTE, OnExcute)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CScaleBar::SetRange(float fMin, float fMax)
{
	m_minScale = fMin;
	m_maxScale = fMax;
}

void CScaleBar::SetScale(float fScale)
{
	if(fScale!=m_fScale)
	{
		char code[3];
		Language::GetLanguageCode(code);
		
		CString str(Number::abbrnum(fScale, code).c_str());
		GetDlgItem(IDC_SCALE)->SetWindowText(str);
		m_fScale = fScale;
	}
}

void CScaleBar::OnExcute()
{
	UpdateData();

	CFrameWnd* pMainFrame = (CFrameWnd*)AfxGetMainWnd();
	CFrameWnd* pChildFrame = (CFrameWnd*)pMainFrame->GetActiveFrame();
	if(pChildFrame!=nullptr)
	{
		CWnd* pWnd = pChildFrame->GetActiveView();
		if(pWnd!=nullptr)
		{
			((CGrfView*)pWnd)->ChangeViewScale(m_fScale);
		}
	}
}

void CScaleBar::OnOK()
{
}

BOOL CScaleBar::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_KEYDOWN)
	{
		switch(pMsg->wParam)
		{
		case VK_ESCAPE:
			return TRUE;
			break;
		case VK_RETURN:
			if(pMsg->hwnd==GetDlgItem(IDC_SCALE)->m_hWnd)
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
