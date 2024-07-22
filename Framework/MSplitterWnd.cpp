#include "Stdafx.h"
#include "MSplitterWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(MSplitterWnd, GSplitterWnd)

MSplitterWnd::MSplitterWnd(CRuler& ruler)
	: m_ruler(ruler)
{
}

BEGIN_MESSAGE_MAP(MSplitterWnd, GSplitterWnd)
	//{{AFX_MSG_MAP(MSplitterWnd)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

MSplitterWnd::~MSplitterWnd()
{
}

void MSplitterWnd::RecalcLayout()
{
	CSplitterWnd::RecalcLayout();

	CWnd* pMapWnd = GetPane(0, 0);
	if(pMapWnd!=nullptr&&m_ruler.m_bVisible==TRUE)
	{
		CRect rect;
		pMapWnd->GetWindowRect(rect);
		this->ScreenToClient(&rect);
		pMapWnd->SetWindowPos(&wndBottom, rect.left+13, rect.top+13, rect.Width()-13, rect.Height()-13, SWP_NOACTIVATE);
	}
}

void MSplitterWnd::OnPaint()
{
	GSplitterWnd::OnPaint();

	CWnd* pView = GetPane(0, 0);
	if(pView!=nullptr&&m_ruler.m_bVisible==TRUE)
	{
		CWnd* pFrame = this->GetParent();
		if(pFrame!=nullptr)
			pFrame->PostMessage(WM_PAINT, 0, 0);
	}
}

void MSplitterWnd::OnSize(UINT nType, int cx, int cy)
{
	GSplitterWnd::OnSize(nType, cx, cy);
}
