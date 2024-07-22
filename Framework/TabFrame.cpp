#include "stdafx.h"
#include "TabFrame.h"
#include <AFXPRIV.H>

#include "gfx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabFrame

IMPLEMENT_GWDYNCREATE(CTabFrame, CFrameWndEx)

CTabFrame::CTabFrame(CObject& document)
	: m_mainview((CView&)document)
{
}

CTabFrame::~CTabFrame()
{
}

BEGIN_MESSAGE_MAP(CTabFrame, CFrameWndEx)
	//{{AFX_MSG_MAP(CTabFrame)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#ifdef _DEBUG
void CTabFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CTabFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG

int CTabFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CFrameWndEx::OnCreate(lpCreateStruct)==-1)
		return -1;

	return 0;
}
