#include "stdafx.h"
#include "PaperDlg.h"

#include "../Atlas/Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern UNIT a_UnitArray[3];
extern BYTE a_nUnitIndex;

/////////////////////////////////////////////////////////////////////////////
// CPaperDlg dialog											
CPaperDlg::CPaperDlg(CWnd* pParent,long nWidth,long nHeight,long nXMargin,long nYMargin,long viewLower,long viewUpper,long nXGrid,long nYGrid)
	:CDialog(CPaperDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPaperDlg)
	m_viewLower = 0;
	m_viewUpper = 0;
	m_portrait  =TRUE;
	m_cxGrid = 0;
	m_cyGrid = 0;
	//}}AFX_DATA_INIT

	m_nWidth = nWidth;
	m_nHeight = nHeight;
	
	m_nXMargin = nXMargin;
	m_nYMargin = nYMargin;

	m_viewLower = viewLower;
	m_viewUpper = viewUpper;
	m_cxGrid = nXGrid;
	m_cyGrid = nYGrid;
}

void CPaperDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPaperDlg)
	DDX_Control(pDX, IDC_PAPERCOMBO, m_papercombo);
	DDX_Text(pDX, IDC_VIEWLOWER, m_viewLower);
	DDV_MinMaxInt(pDX, m_viewLower, 1, 400);
	DDX_Text(pDX, IDC_VIEWUPPER, m_viewUpper);
	DDV_MinMaxInt(pDX, m_viewUpper, 50, 10000);
	DDX_Text(pDX, IDC_CXGRID, m_cxGrid);
	DDV_MinMaxUInt(pDX, m_cxGrid, 36, 72000);
	DDX_Text(pDX, IDC_CYGRID, m_cyGrid);
	DDV_MinMaxUInt(pDX, m_cyGrid, 36, 72000);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPaperDlg, CDialog)
	//{{AFX_MSG_MAP(CPaperDlg)
	ON_CBN_SELCHANGE(IDC_PAPERCOMBO, OnSelchangePapercombo)
	ON_BN_CLICKED(IDC_PORTRAITRADIO, OnPortraitradio)
	ON_BN_CLICKED(IDC_LANDSCAPERADIO, OnLandscaperadio)
	ON_EN_CHANGE(IDC_PAPERWIDTH, OnChangePaperwidth)
	ON_EN_CHANGE(IDC_PAPERHEIGHT, OnChangePaperheight)
	ON_EN_CHANGE(IDC_XMARGIN, OnChangeXMargin)
	ON_EN_CHANGE(IDC_YMARGIN, OnChangeYMargin)
	ON_BN_CLICKED(IDC_STANDADRADIO, OnStandadradio)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPaperDlg message handlers

BOOL CPaperDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	GetDlgItem(IDC_UNITSHOW)->SetWindowText(a_UnitArray[a_nUnitIndex].unitname);

	UpdateHAndW();
		
	if(m_nHeight > m_nWidth)
	{
		((CButton*)GetDlgItem(IDC_PORTRAITRADIO))->SetCheck(TRUE);
		m_portrait = TRUE;
	}
	else
	{
		((CButton*)GetDlgItem(IDC_LANDSCAPERADIO))->SetCheck(TRUE);
		m_portrait = FALSE;
	}

	for(long I=0;I<sizeof(g_paper)/sizeof(PAPERSIZE);I++)
	{
		if((g_paper[I].width == m_nWidth  && g_paper[I].height == m_nHeight) || (g_paper[I].height == m_nWidth && g_paper[I].width  == m_nHeight))
		{
			((CButton*)GetDlgItem(IDC_STANDADRADIO ))->SetCheck(TRUE);
			((CButton*)GetDlgItem(IDC_CUSTOMRADIO))->SetCheck(FALSE);
			m_papercombo.SetCurSel(I);
			return TRUE;
		}
	}

	m_papercombo.SetCurSel(0);

	((CButton*)GetDlgItem(IDC_STANDADRADIO))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_CUSTOMRADIO))->SetCheck(TRUE);
	
	CenterWindow();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPaperDlg::OnSelchangePapercombo() 
{
	// TODO: Add your control notification handler code here
	
	long index=m_papercombo.GetCurSel();
	if(index != 0 && index != CB_ERR)
	{
		if(m_portrait==TRUE)
		{
			m_nHeight = g_paper[index].height;
			m_nWidth  = g_paper[index].width;

			m_nXMargin = m_nWidth /2;
			m_nYMargin = m_nHeight/2;
		}
		else
		{
			m_nWidth  = g_paper[index].height;
			m_nHeight = g_paper[index].width;

			m_nXMargin = m_nWidth /2;
			m_nYMargin = m_nHeight/2;
		}
		UpdateHAndW();
		((CButton*)GetDlgItem(IDC_STANDADRADIO))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_CUSTOMRADIO))->SetCheck(FALSE);
	}
	else if(index == 0)
	{
		((CButton*)GetDlgItem(IDC_CUSTOMRADIO))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_STANDADRADIO))->SetCheck(FALSE);
	}
}


void CPaperDlg::OnPortraitradio() 
{
	// TODO: Add your control notification handler code here
	m_portrait=TRUE;
	if(m_nHeight < m_nWidth)
	{
		long t=m_nHeight;
		m_nHeight = m_nWidth;
		m_nWidth=t;

		long l = m_nXMargin;
		m_nXMargin = m_nYMargin;
		m_nYMargin = l;
	}
	UpdateHAndW();
}

void CPaperDlg::OnLandscaperadio() 
{
	// TODO: Add your control notification handler code here
	m_portrait=FALSE;
	if(m_nHeight > m_nWidth)
	{
		long t=m_nWidth;
		m_nWidth = m_nHeight;
		m_nHeight=t;

		long l = m_nXMargin;
		m_nXMargin = m_nYMargin;
		m_nYMargin = l;

	}
	UpdateHAndW();
}
void CPaperDlg::UpdateHAndW()
{
	CString str;
	float width,height;
	width  = m_nWidth /a_UnitArray[a_nUnitIndex].pointpro;
	height = m_nHeight/a_UnitArray[a_nUnitIndex].pointpro;	
	str.Format(_T("%.3f"),width);
	((CWnd*)GetDlgItem(IDC_PAPERWIDTH))->SetWindowText(str);
	str.Format(_T("%.3f"),height);
	((CWnd*)GetDlgItem(IDC_PAPERHEIGHT))->SetWindowText(str);

	float lrMargin,tbMargin;
	lrMargin = m_nXMargin/a_UnitArray[a_nUnitIndex].pointpro;
	tbMargin = m_nYMargin/a_UnitArray[a_nUnitIndex].pointpro;	
	str.Format(_T("%.3f"),lrMargin);
	((CWnd*)GetDlgItem(IDC_XMARGIN))->SetWindowText(str);
	str.Format(_T("%.3f"),tbMargin);
	((CWnd*)GetDlgItem(IDC_YMARGIN))->SetWindowText(str);
}

void CPaperDlg::OnChangePaperwidth() 
{
	// TODO: Add your control notification handler code here
	if(GetFocus() == (CWnd*)GetDlgItem(IDC_PAPERWIDTH))
	{	
		CString str;
		((CWnd*)GetDlgItem(IDC_PAPERWIDTH))->GetWindowText(str);
		float t=(float)_tcstod(str,nullptr);
		m_nWidth=(long)(t*a_UnitArray[a_nUnitIndex].pointpro);
	
		((CButton*)GetDlgItem(IDC_STANDADRADIO))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_CUSTOMRADIO))->SetCheck(TRUE);
	}
}

void CPaperDlg::OnChangePaperheight() 
{
	// TODO: Add your control notification handler code here
	if(GetFocus() == (CWnd*)GetDlgItem(IDC_PAPERHEIGHT))
	{	
		CString str;		
		((CWnd*)GetDlgItem(IDC_PAPERHEIGHT))->GetWindowText(str);
		float t=(float)_tcstod(str,nullptr);
		m_nHeight=(long)(t*a_UnitArray[a_nUnitIndex].pointpro);

		((CButton*)GetDlgItem(IDC_STANDADRADIO))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_CUSTOMRADIO))->SetCheck(TRUE);
	}
}

void CPaperDlg::OnChangeXMargin() 
{
	// TODO: Add your control notification handler code here
	if(GetFocus() == (CWnd*)GetDlgItem(IDC_XMARGIN))
	{	
		CString str;
		((CWnd*)GetDlgItem(IDC_XMARGIN))->GetWindowText(str);
		float t=(float)_tcstod(str,nullptr);
		m_nXMargin=(long)(t*a_UnitArray[a_nUnitIndex].pointpro);
	}
}

void CPaperDlg::OnChangeYMargin() 
{
	// TODO: Add your control notification handler code here
	if(GetFocus() == (CWnd*)GetDlgItem(IDC_YMARGIN))
	{	
		CString str;		
		((CWnd*)GetDlgItem(IDC_YMARGIN))->GetWindowText(str);
		float t=(float)_tcstod(str,nullptr);
		m_nYMargin=(long)(t*a_UnitArray[a_nUnitIndex].pointpro);
	}
}

void CPaperDlg::OnStandadradio() 
{
	// TODO: Add your control notification handler code here
	OnSelchangePapercombo();
}
