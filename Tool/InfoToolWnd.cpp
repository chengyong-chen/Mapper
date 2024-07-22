#include "stdafx.h"

#include "InfoToolWnd.h"

#include "infoTool.h"
#include "insertTool.h"
#include "MarqueeTool.h"

#include "RadiusTool.h"
#include "BoundTool.h"
#include "PositionTool.h"
#include "RoundTool.h"
#include "BufferTool.h"
#include "RegionTool.h"

#include "ExtentDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern __declspec(dllexport) CInfoTool infoTool;
extern __declspec(dllexport) CInsertTool insertTool;
extern __declspec(dllexport) CMarqueeTool marqueeTool;
extern __declspec(dllexport) CRadiusTool radiusTool;
extern __declspec(dllexport) CBoundTool boundTool;
extern __declspec(dllexport) CPositionTool positionTool;
extern __declspec(dllexport) CRoundTool roundTool;
extern __declspec(dllexport) CBufferTool bufferTool;
extern __declspec(dllexport) CRegionTool regionTool;

IMPLEMENT_DYNAMIC(CInfoToolWnd, CControlBar)

BEGIN_MESSAGE_MAP(CInfoToolWnd, CControlBar)
	//{{AFX_MSG_MAP(CControlBar)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CInfoToolWnd::CInfoToolWnd()
{
	row = -1;
	col = -1;
	m_bVisible = true;
	m_pTool = nullptr;
}

CInfoToolWnd::~CInfoToolWnd()
{
}

CSize CInfoToolWnd::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	ASSERT_VALID(this);

	if(IsFloating())
	{
		CFrameWnd* pFrameWnd = this->GetParentFrame();
		if(pFrameWnd!=AfxGetMainWnd())
		{
			// Remove the SYSMENU functionality
			pFrameWnd->ModifyStyle(WS_SYSMENU, 0);
		}
	}

	CSize size;
	size.cx = 51;
	size.cy = 161;

	return size;
}

/////////////////////////////////////////////////////////////////////////////
// CInfoToolWnd message handlers

BOOL CInfoToolWnd::Create(CWnd* pParentWnd, DWORD dwStyle, UINT nId)
{
	if(pParentWnd!=nullptr)
		ASSERT_VALID(pParentWnd); // must have a parent

		// save the style
		//	m_dwStyle = (dwStyle & CBRS_ALL);
		//	if(nId == AFX_IDW_TOOLBAR)
		//		m_dwStyle |= CBRS_HIDE_INPLACE;

	m_dwStyle &= ~WS_SYSMENU;

	// create the HWND
	CRect rect;
	rect.SetRectEmpty();
	const LPCTSTR lpszClass = AfxRegisterWndClass(0, ::LoadCursor(nullptr, IDC_ARROW), (HBRUSH)(COLOR_BTNFACE+1), nullptr);
	if(!CWnd::Create(lpszClass, nullptr, dwStyle, rect, pParentWnd, nId))
		return FALSE;

	m_wndParent = pParentWnd;

	return TRUE;
}

void CInfoToolWnd::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
}

void CInfoToolWnd::OnPaint()
{
	CRect rect;
	GetClientRect(rect);
	CPaintDC dc(this); // device context for painting

	CDC mdc;
	CBitmap bitmap;

	bitmap.LoadBitmap(IDB_INFOTOOL);
	if(!mdc.CreateCompatibleDC(&dc))
	{
		bitmap.DeleteObject();
		return;
	}
	else
	{
		const HANDLE old = mdc.SelectObject(&bitmap);
		dc.BitBlt(0, 0, 51, 161, &mdc, 0, 0, SRCCOPY);

		const CRect cRect(col*26, row*18, (col+1)*26-1, (row+1)*18-1);
		dc.InvertRect(&cRect);

		mdc.SelectObject(old);
		bitmap.DeleteObject();
		mdc.DeleteDC();
	}
}

void CInfoToolWnd::OnLButtonDown(UINT nFlags, CPoint docPoint)
{
	if(docPoint.y%18==0||docPoint.x%26==0)
		return;
	const unsigned __int8 row1 = docPoint.y/18;
	const unsigned __int8 col1 = docPoint.x/26;
	if(row1==row&&col1==col)
		return;

	CDC* pDC = GetDC();

	CRect cRect(col*26, row*18, (col+1)*26-1, (row+1)*18-1);
	pDC->InvertRect(&cRect);
	if(m_pTool==nullptr)
		m_pTool = nullptr;
	else if(col1==0&&row1==0)
		*m_pTool = &infoTool;
	else if(col1==1&&row1==0)
		*m_pTool = &insertTool;
	else if(col1==0&&row1==1)
		*m_pTool = &positionTool;
	else if(col1==1&&row1==1)
		*m_pTool = &positionTool;
	else if(col1==0&&row1==2)
		*m_pTool = &marqueeTool;
	else if(col1==1&&row1==2)
		*m_pTool = &radiusTool;
	else if(col1==0&&row1==3)
		*m_pTool = &boundTool;
	else if(col1==1&&row1==3)
		*m_pTool = &roundTool;
	else if(col1==0&&row1==4)
		*m_pTool = &bufferTool;
	else if(col1==1&&row1==4)
		*m_pTool = &regionTool;

	cRect.SetRect(col1*26, row1*18, (col1+1)*26-1, (row1+1)*18-1);
	pDC->InvertRect(&cRect);

	ReleaseDC(pDC);
	col = col1;
	row = row1;

	//	m_wndParent->SendMessage(WM_SELINFOTOOL,0,0);
}

void CInfoToolWnd::OnLButtonDblClk(UINT nFlags, CPoint docPoint)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CControlBar::OnLButtonDblClk(nFlags, docPoint);

	if(docPoint.y%18==0||docPoint.x%26==0)
		return;
	const long row = docPoint.y/18;
	const long col = docPoint.x/26;

	if(col==1&&row==3)
	{
		CExtentDlg dlg(this, CRoundTool::Extent/10);
		if(dlg.DoModal()==IDOK)
		{
			CRoundTool::Extent = dlg.m_nExtent*10;
		}
	}

	if(col==1&&row==4)
	{
		CExtentDlg dlg(this, CBufferTool::Extent/10);
		if(dlg.DoModal()==IDOK)
		{
			CBufferTool::Extent = dlg.m_nExtent*10;
		}
	}
}

#ifdef _DEBUG

void CInfoToolWnd::AssertValid() const
{
	CControlBar::AssertValid();
}

void CInfoToolWnd::Dump(CDumpContext& dc) const
{
	CControlBar::Dump(dc);
}

#endif
