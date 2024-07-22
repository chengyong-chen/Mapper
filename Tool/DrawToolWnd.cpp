#include "stdafx.h"
#include "Resource.h"
#include "DrawToolWnd.h"

#include "Tool.h"
#include "SelectTool.h"
#include "EroseTool.h"
#include "TextTool.h"
#include "PolyTool.h"
#include "TextPolyTool.h"
#include "PRectTool.h"
#include "RRectTool.h"
#include "EllipseTool.h"
#include "MarkTool.h"
#include "BezierTool.h"
#include "SmearTool.h"
#include "SplitTool.h"
#include "ImageTool.h"
#include "PrintTool.h"
#include "RotateTool.h"
#include "ReflectTool.h"
#include "ScaleTool.h"
#include "SkewTool.h"
#include "ZoomTool.h"
#include "panTool.h"
#include "InfoTool.h"
#include "PositionTool.h"
#include "DatumTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern __declspec(dllexport) CSelectTool selectTool;
extern __declspec(dllexport) CEroseTool eroseTool;
extern __declspec(dllexport) CTextTool textTool;
extern __declspec(dllexport) CTextPolyTool textpolyTool;
extern __declspec(dllexport) CZoomTool zoominTool;
extern __declspec(dllexport) CZoomTool zoomoutTool;
extern __declspec(dllexport) CPanTool panTool;
extern __declspec(dllexport) CPRectTool prectTool;
extern __declspec(dllexport) CRRectTool rrectTool;
extern __declspec(dllexport) CEllipseTool ellipseTool;
extern __declspec(dllexport) CMarkTool markTool;
extern __declspec(dllexport) CPolyTool polyTool;
extern __declspec(dllexport) CBezierTool bezierTool;
extern __declspec(dllexport) CSmearTool smearTool;
extern __declspec(dllexport) CImageTool imageTool;
extern __declspec(dllexport) CSplitTool splitTool;
extern __declspec(dllexport) CPrintTool printTool;
extern __declspec(dllexport) CRotateTool rotateTool;
extern __declspec(dllexport) CReflectTool reflectTool;
extern __declspec(dllexport) CScaleTool scaleTool;
extern __declspec(dllexport) CSkewTool skewTool;
extern __declspec(dllexport) CInfoTool infoTool;
extern __declspec(dllexport) CPositionTool positionTool;
extern __declspec(dllexport) CDatumTool datumTool;

IMPLEMENT_DYNAMIC(CDrawToolWnd, CControlBar)

BEGIN_MESSAGE_MAP(CDrawToolWnd, CControlBar)
	//{{AFX_MSG_MAP(CDrawToolWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CDrawToolWnd::CDrawToolWnd()
{
	m_Tools[0][0] = &selectTool;
	m_Tools[0][1] = &eroseTool;
	m_Tools[1][0] = &textTool;
	m_Tools[1][1] = &textpolyTool;
	m_Tools[2][0] = &prectTool;
	m_Tools[2][1] = &rrectTool;
	m_Tools[3][0] = &ellipseTool;
	m_Tools[3][1] = &polyTool;
	m_Tools[4][0] = &smearTool;
	m_Tools[4][1] = &bezierTool;
	m_Tools[5][0] = &markTool;
	m_Tools[5][1] = &imageTool;
	m_Tools[6][0] = &rotateTool;
	m_Tools[6][1] = &reflectTool;
	m_Tools[7][0] = &scaleTool;
	m_Tools[7][1] = &skewTool;
	m_Tools[8][0] = &splitTool;
	m_Tools[8][1] = &panTool;
	m_Tools[9][0] = &zoominTool;
	m_Tools[9][1] = &zoomoutTool;

	zoominTool.m_bOut = false;
	zoomoutTool.m_bOut = true;
	m_ppTool = nullptr;
}

CDrawToolWnd::~CDrawToolWnd()
{
}

CSize CDrawToolWnd::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	ASSERT_VALID(this);

	if(IsFloating())
	{
		CFrameWnd* pFrameWnd = this->GetParentFrame();
		if(pFrameWnd!=AfxGetMainWnd())
		{
			pFrameWnd->ModifyStyle(WS_SYSMENU, 0);
		}
	}

	return CSize(51, 180);
}

void CDrawToolWnd::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
}

BOOL CDrawToolWnd::Create(CWnd* pParentWnd, DWORD dwStyle, UINT nId)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_bitmap.LoadBitmap(IDB_DRAWTOOL);

	m_wndParent = pParentWnd;

	// save the style
	//	m_dwStyle = (dwStyle & CBRS_ALL);
	//	if(nId == AFX_IDW_TOOLBAR)
	//		m_dwStyle |= CBRS_HIDE_INPLACE;

	m_dwStyle &= ~WS_SYSMENU;

	// create the HWND
	CRect rect;
	rect.SetRectEmpty();
	const LPCTSTR lpszClass = AfxRegisterWndClass(0, ::LoadCursor(nullptr, IDC_ARROW), (HBRUSH)(COLOR_BTNFACE+1), nullptr);
	return CWnd::Create(lpszClass, nullptr, dwStyle, rect, pParentWnd, nId);
}

void CDrawToolWnd::OnPaint()
{
	CPaintDC dc(this);

	CDC mdc;
	if(!mdc.CreateCompatibleDC(&dc))
		return;
	const HANDLE oldBmp = mdc.SelectObject(&m_bitmap);
	dc.BitBlt(0, 0, 51, 180, &mdc, 0, 0, SRCCOPY);
	if(m_ppTool!=nullptr)
	{
		for(int row = 0; row<10; row++)
		{
			for(int col = 0; col<2; col++)
			{
				if(*m_ppTool==m_Tools[row][col])
				{
					CRect cRect(col*26, row*18, (col+1)*26-1, (row+1)*18-1);
					dc.InvertRect(&cRect);
					break;
				}
			}
		}
	}
	mdc.SelectObject(oldBmp);
	mdc.DeleteDC();
}

void CDrawToolWnd::SwitchSource(CTool** ppTool)
{
	if(ppTool==m_ppTool)
		return;

	CClientDC dc(this);
	CDC mdc;
	if(!mdc.CreateCompatibleDC(&dc))
		return;
	const HANDLE oldBmp = mdc.SelectObject(&m_bitmap);
	dc.BitBlt(0, 0, 51, 180, &mdc, 0, 0, SRCCOPY);
	if(ppTool!=nullptr)
	{
		for(int row = 0; row<10; row++)
		{
			for(int col = 0; col<2; col++)
			{
				if(*ppTool==m_Tools[row][col])
				{
					CRect cRect(col*26, row*18, (col+1)*26-1, (row+1)*18-1);
					dc.InvertRect(&cRect);
					break;
				}
			}
		}
	}
	mdc.SelectObject(oldBmp);
	mdc.DeleteDC();

	m_ppTool = ppTool;
}

void CDrawToolWnd::SwitchToolTo(CTool* pTool)
{
	if(m_ppTool==nullptr)
		return;
	if(*m_ppTool==pTool)
		return;

	CDC* pDC = GetDC();
	for(int row = 0; row<10; row++)
	{
		for(int col = 0; col<2; col++)
		{
			if(*m_ppTool==m_Tools[row][col])
			{
				CRect cRect(col*26, row*18, (col+1)*26-1, (row+1)*18-1);
				pDC->InvertRect(&cRect);
				break;
			}
		}
	}

	if(pTool!=nullptr)
	{
		for(int row = 0; row<10; row++)
		{
			for(int col = 0; col<2; col++)
			{
				if(pTool==m_Tools[row][col])
				{
					CRect cRect(col*26, row*18, (col+1)*26-1, (row+1)*18-1);
					pDC->InvertRect(&cRect);
					break;
				}
			}
		}
	}
	*m_ppTool = pTool;
	ReleaseDC(pDC);
}

void CDrawToolWnd::OnLButtonDown(UINT nFlags, CPoint docPoint)
{
	if(docPoint.y%18==0||docPoint.x%26==0)
		return;
	const unsigned __int8 row = docPoint.y/18;
	const unsigned __int8 col = docPoint.x/26;
	if(row>9)
		return;
	else if(col>1)
		return;
	else if(m_ppTool!=nullptr)
	{
		SwitchToolTo(m_Tools[row][col]);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDrawToolWnd diagnostics

#ifdef _DEBUG

void CDrawToolWnd::AssertValid() const
{
	CControlBar::AssertValid();
}

void CDrawToolWnd::Dump(CDumpContext& dc) const
{
	CControlBar::Dump(dc);
}

#endif

BOOL CDrawToolWnd::DestroyWindow()
{
	m_bitmap.DeleteObject();

	return CControlBar::DestroyWindow();
}
