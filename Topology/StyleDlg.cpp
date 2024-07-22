#include "stdafx.h"
#include "StyleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////
// CStyleDlg dialog

CStyleDlg::CStyleDlg(CWnd* pParent, COLORREF colorNode, COLORREF colorEdge)
	: CDialog(CStyleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStyleDlg)
	m_colorNode = colorNode;
	m_colorEdge = colorEdge;
	//}}AFX_DATA_INIT
}

void CStyleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStyleDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CStyleDlg, CDialog)
	//{{AFX_MSG_MAP(CStyleDlg)
	ON_WM_PAINT()
	ON_BN_DOUBLECLICKED(IDC_NODECOLOR, OnDCNodeColor)
	ON_BN_DOUBLECLICKED(IDC_EDGECOLOR, OnDCEdgeColor)
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStyleDlg message handlers
void CStyleDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CWnd* pWndNode = (CWnd*)GetDlgItem(IDC_NODECOLOR);
	if(pWndNode!=nullptr)
	{
		CRect rect;
		pWndNode->GetClientRect(rect);
		rect.InflateRect(-1, -1);

		CClientDC dc(pWndNode);
		dc.FillSolidRect(rect, m_colorNode);
	}

	CWnd* pWndEdge = (CWnd*)GetDlgItem(IDC_EDGECOLOR);
	if(pWndEdge!=nullptr)
	{
		CRect rect;
		pWndEdge->GetClientRect(rect);
		rect.InflateRect(-1, -1);

		CClientDC dc(pWndEdge);
		dc.FillSolidRect(rect, m_colorEdge);
	}
}

void CStyleDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your message handler code here and/or call default
	if(lpDrawItemStruct->itemID==LB_ERR)
		return;

	if(lpDrawItemStruct->CtlID==IDC_NODECOLOR)
	{
		CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
		CRect rect = lpDrawItemStruct->rcItem;
		rect.InflateRect(-1, -1);
		pDC->FillSolidRect(rect, m_colorNode);
	}

	if(lpDrawItemStruct->CtlID==IDC_EDGECOLOR)
	{
		CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
		CRect rect = lpDrawItemStruct->rcItem;
		rect.InflateRect(-1, -1);
		pDC->FillSolidRect(rect, m_colorEdge);
	}

	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CStyleDlg::OnDCNodeColor()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CColorDialog pDlg(m_colorNode, CC_FULLOPEN);
	if(pDlg.DoModal()==IDOK)
	{
		m_colorNode = pDlg.GetColor();
		GetDlgItem(IDC_NODECOLOR)->Invalidate();
	}
}

void CStyleDlg::OnDCEdgeColor()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CColorDialog pDlg(m_colorEdge, CC_FULLOPEN);
	if(pDlg.DoModal()==IDOK)
	{
		m_colorEdge = pDlg.GetColor();
		GetDlgItem(IDC_EDGECOLOR)->Invalidate();
	}
}
