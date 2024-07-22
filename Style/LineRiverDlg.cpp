#include "stdafx.h"

#include "Color.h"
#include "Dash.h"
#include "Line.h"
#include "LineRiver.h"
#include "LineRiverDlg.h"

#include "ColorDlg.h"
#include "DashDlg.h"

#include "../Public/Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BYTE d_nUnitIndex;
extern UNIT g_UnitArray[3];

CLineRiverDlg::CLineRiverDlg(CWnd* pParent /*=nullptr*/, CLineRiver* pline)
	: CDialog(CLineRiverDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLineRiverDlg)
	d_miterlimit = 30;
	d_fFWidth = 0.0f;
	d_fTWidth = 0.0f;
	//}}AFX_DATA_INIT

	d_pLine = pline;
	d_fFWidth = (float)(d_pLine->m_nFWidth/(g_UnitArray[d_nUnitIndex].pointpro*10));
	d_fTWidth = (float)(d_pLine->m_nTWidth/(g_UnitArray[d_nUnitIndex].pointpro*10));
}

void CLineRiverDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLineRiverDlg)
	DDX_Text(pDX, IDC_MITERLIMIT, d_miterlimit);
	DDV_MinMaxInt(pDX, d_miterlimit, 0, 100);
	DDX_Text(pDX, IDC_FWIDTH, d_fFWidth);
	DDX_Text(pDX, IDC_TWIDTH, d_fTWidth);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLineRiverDlg, CDialog)
	//{{AFX_MSG_MAP(CLineRiverDlg)
	ON_BN_DOUBLECLICKED(IDC_DASH, OnDCDash)
	ON_BN_DOUBLECLICKED(IDC_COLOR, OnDCColor)
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLineRiverDlg message handlers

BOOL CLineRiverDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	switch(d_pLine->m_bCapindex)
	{
	case 0:
		((CButton*)GetDlgItem(IDC_CAPFLAT))->SetCheck(TRUE);
		break;
	case 1:
		((CButton*)GetDlgItem(IDC_CAPROUND))->SetCheck(TRUE);
		break;
	case 2:
		((CButton*)GetDlgItem(IDC_CAPSQUARE))->SetCheck(TRUE);
		break;
	}

	switch(d_pLine->m_bJoinindex)
	{
	case 0:
		((CButton*)GetDlgItem(IDC_JOINBEVEL))->SetCheck(TRUE);
		break;
	case 1:
		((CButton*)GetDlgItem(IDC_JOINROUND))->SetCheck(TRUE);
		break;
	case 2:
		((CButton*)GetDlgItem(IDC_JOINMITER))->SetCheck(TRUE);
		break;
	}

	GetDlgItem(IDC_UNITSHOW)->SetWindowText(g_UnitArray[d_nUnitIndex].unitname);

	SetWindowPos(&wndTop, 8, 45, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);

	return TRUE;
}

void CLineRiverDlg::OnOK()
{
	CDialog::OnOK();

	d_pLine->m_nFWidth = (unsigned short)(round(d_fFWidth*10*g_UnitArray[d_nUnitIndex].pointpro));
	d_pLine->m_nTWidth = (unsigned short)(round(d_fTWidth*10*g_UnitArray[d_nUnitIndex].pointpro));

	if((((CButton*)GetDlgItem(IDC_CAPFLAT))->GetState()&0X0003)) d_pLine->m_bCapindex = 0;
	if((((CButton*)GetDlgItem(IDC_CAPROUND))->GetState()&0X0003)) d_pLine->m_bCapindex = 1;
	if((((CButton*)GetDlgItem(IDC_CAPSQUARE))->GetState()&0X0003)) d_pLine->m_bCapindex = 2;

	if((((CButton*)GetDlgItem(IDC_JOINBEVEL))->GetState()&0X0003)) d_pLine->m_bJoinindex = 0;
	if((((CButton*)GetDlgItem(IDC_JOINROUND))->GetState()&0X0003)) d_pLine->m_bJoinindex = 1;
	if((((CButton*)GetDlgItem(IDC_JOINMITER))->GetState()&0X0003)) d_pLine->m_bJoinindex = 2;
}

void CLineRiverDlg::OnDCDash()
{
	CDashDlg dlg(nullptr, d_pLine->m_dash.GetDate());
	if(dlg.DoModal()==IDOK)
	{
		d_pLine->m_dash.m_segment[0] = round(dlg.d_segon1*g_UnitArray[d_nUnitIndex].pointpro*10);
		d_pLine->m_dash.m_segment[1] = round(dlg.d_segoff1*g_UnitArray[d_nUnitIndex].pointpro*10);
		d_pLine->m_dash.m_segment[2] = round(dlg.d_segon2*g_UnitArray[d_nUnitIndex].pointpro*10);
		d_pLine->m_dash.m_segment[3] = round(dlg.d_segoff2*g_UnitArray[d_nUnitIndex].pointpro*10);
		d_pLine->m_dash.m_segment[4] = round(dlg.d_segon3*g_UnitArray[d_nUnitIndex].pointpro*10);
		d_pLine->m_dash.m_segment[5] = round(dlg.d_segoff3*g_UnitArray[d_nUnitIndex].pointpro*10);
		d_pLine->m_dash.m_segment[6] = round(dlg.d_segon4*g_UnitArray[d_nUnitIndex].pointpro*10);
		d_pLine->m_dash.m_segment[7] = round(dlg.d_segoff4*g_UnitArray[d_nUnitIndex].pointpro*10);
		GetDlgItem(IDC_DASH)->Invalidate();
	}
}

void CLineRiverDlg::OnDCColor()
{
	// TODO: Add your control notification handler code here
	CColorDlg dlg(nullptr, d_pLine->m_pColor, RGB(0, 0, 0));
	if(dlg.DoModal()==IDOK)
	{
		delete d_pLine->m_pColor;
		d_pLine->m_pColor = dlg.d_pColor;
		dlg.d_pColor = nullptr;
		GetDlgItem(IDC_COLOR)->Invalidate();
	}
}

void CLineRiverDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your message handler code here and/or call default
	if(lpDrawItemStruct->itemID==LB_ERR)
		return;

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rect = lpDrawItemStruct->rcItem;

	if(lpDrawItemStruct->CtlID==IDC_COLOR)
	{
		rect.InflateRect(-1, -1);
		if(d_pLine->m_pColor!=nullptr)
			pDC->FillSolidRect(rect, d_pLine->m_pColor->GetMonitorRGB());
		else
			pDC->FillSolidRect(rect, RGB(0, 0, 0));
	}

	if(lpDrawItemStruct->CtlID==IDC_DASH)
	{
		pDC->FillSolidRect(rect, GetSysColor(COLOR_3DFACE));

		CPen pen;
		DWORD segment[8];
		unsigned short segcount = d_pLine->m_dash.GetCount();
		if(segcount!=0)
		{
			memcpy(segment, d_pLine->m_dash.GetDate(), 32);
			segcount = d_pLine->m_dash.GetCount();
			long length = 0;
			for(unsigned short i = 0; i<segcount; i++)
				length += (long)segment[i];
			long j = 1;
			while(rect.Width()<length*2)
			{
				length /= 2;
				j *= 2;
			}
			for(unsigned short i = 0; i<segcount; i++)
				segment[i] /= j;
		}

		long nPenStyle = 0;
		nPenStyle |= PS_ENDCAP_FLAT;
		nPenStyle |= PS_JOIN_BEVEL;
		nPenStyle |= PS_GEOMETRIC;
		if(segcount!=0)
		{
			nPenStyle |= PS_USERSTYLE;
		}

		LOGBRUSH pLogBrush;
		pLogBrush.lbStyle = BS_SOLID;
		pLogBrush.lbColor = RGB(0, 0, 0);
		pLogBrush.lbHatch = 0;

		pen.CreatePen(nPenStyle, 1, &pLogBrush, segcount, segcount!=0 ? segment : nullptr);

		CPen* oldpen = pDC->SelectObject(&pen);
		pDC->MoveTo(rect.left, rect.top+(rect.bottom-rect.top)/2);
		pDC->LineTo(rect.right, rect.top+(rect.bottom-rect.top)/2);
		pDC->SelectObject(oldpen);
		pen.DeleteObject();
	}

	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}
