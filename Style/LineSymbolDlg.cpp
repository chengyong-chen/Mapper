#include "stdafx.h"

#include "Line.h"
#include "LineSymbol.h"
#include "LineSymbolDlg.h"
#include "Symbol.h"
#include "SymbolLine.h"
#include "SymbolMid.hpp"
#include "Midtable.hpp"

#include "../Geometry/Poly.h"
#include "../DataView/ViewMonitor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CString d_strLineMidtable;

/////////////////////////////////////////////////////////////////////////////
// CLineSymbolDlg dialog

CLineSymbolDlg::CLineSymbolDlg(CWnd* pParent, CMidtable<CSymbolMid<CSymbolLine>>& tagtable, CLineSymbol* pLine)
	: TSymbolMidDlg(CLineSymbolDlg::IDD, pParent, tagtable, pLine)
{
	//{{AFX_DATA_INIT(CLineSymbolDlg)
	//}}AFX_DATA_INIT
}

void CLineSymbolDlg::DoDataExchange(CDataExchange* pDX)
{
	TSymbolMidDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpotFlashDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLineSymbolDlg, TSymbolMidDlg)
	//{{AFX_MSG_MAP(CLineSymbolDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLineSymbolDlg message handlers

BOOL CLineSymbolDlg::OnInitDialog()
{
	TSymbolMidDlg::OnInitDialog();

	SetWindowPos(&wndTop, 8, 45, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CLineSymbolDlg::DrawPreview()
{
	CSymbol* symbol = nullptr;
	POSITION pos = m_listctrl.GetFirstSelectedItemPosition();
	if(pos==nullptr)
	{
		TRACE0("No items were selected!\n");
		return;
	}
	else
	{
		const long nItem = m_listctrl.GetNextSelectedItem(pos);
		symbol = (CSymbol*)m_listctrl.GetItemData(nItem);
	}

	CWnd* pWnd = GetDlgItem(IDC_PREVIEW);
	if(pWnd==nullptr)
		return;

	CRect box;
	pWnd->GetClientRect(box);

	CRect rect(box);
	rect.InflateRect(-1, -1);
	CClientDC dc(pWnd);
	dc.FillSolidRect(box, GetSysColor(COLOR_3DFACE));
	dc.SetMapMode(MM_ANISOTROPIC);

	CPoly* poly = new CPoly();
	poly->m_nAnchors = 2;
	poly->m_pPoints = new CPoint[2];
	poly->m_pPoints[0] = CPoint(rect.left*10000, rect.top*10000+rect.Height()*10000/2);
	poly->m_pPoints[1] = CPoint(rect.right*10000, rect.top*10000+rect.Height()*10000/2);

	Gdiplus::Graphics g(dc.m_hDC);
	CDatainfo datainfo;
	CViewMonitor viewinfo(datainfo);
	viewinfo.m_ptViewPos = CPoint(0, 0);
	viewinfo.m_sizeView = box.Size();
	viewinfo.m_xFactorMapToView = 1.f;
	viewinfo.m_yFactorMapToView = 1.f;
	Context context;
	context.ratioLine = m_nScale/100.f;
	context.tolerance = viewinfo.GetPrecision(10);
	((CSymbolLine*)symbol)->Draw(g, viewinfo, poly, context, m_nScale/100.f);
	g.ReleaseHDC(dc.m_hDC);

	delete poly;
	poly = nullptr;
}

void CLineSymbolDlg::OnOK()
{
	TSymbolMidDlg::OnOK();

	POSITION pos = m_listctrl.GetFirstSelectedItemPosition();
	if(pos==nullptr)
		return;
	const long indexSymbol = m_listctrl.GetNextSelectedItem(pos);
	CSymbol* pSymbol = (CSymbol*)m_listctrl.GetItemData(indexSymbol);
	if(pSymbol==nullptr)
		return;

	m_pXSymbol->m_nWidth = ((CSymbolLine*)pSymbol)->m_nWidth;
}
