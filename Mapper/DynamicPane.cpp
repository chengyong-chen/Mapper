#include "stdafx.h"
#include "DynamicPane.h"
#include "DynamicDlg.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CDynamicPane, CDockablePane)

CDynamicPane::CDynamicPane()
{
}

CDynamicPane::~CDynamicPane()
{
}

BEGIN_MESSAGE_MAP(CDynamicPane, CDockablePane)
	ON_WM_CREATE()
END_MESSAGE_MAP()

int CDynamicPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CDockablePane::OnCreate(lpCreateStruct)==-1)
		return -1;

	m_DynamicDlg = new CDynamicDlg;
	if(m_DynamicDlg->Create(IDD_DYNAMIC, this))
	{
		m_DynamicDlg->ShowWindow(SW_SHOWDEFAULT);
	}
	else
	{
		delete m_DynamicDlg;
		m_DynamicDlg = nullptr;
	}
	return 0;
}

