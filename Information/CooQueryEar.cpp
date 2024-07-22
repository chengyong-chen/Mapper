#include "stdafx.h"
#include "CooQueryEar.h"

#include "DegreeCtrl.h"
#include "SexageCtrl.h"

#include "../Viewer/Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCooQueryEar dialog
IMPLEMENT_DYNCREATE(CCooQueryEar, CDialog)

CCooQueryEar::CCooQueryEar()
	: CDialog(CCooQueryEar::IDD)
{
	//{{AFX_DATA_INIT(CCooQueryEar)

	//}}AFX_DATA_INIT

	d_pCoorCtrl = nullptr;
}

void CCooQueryEar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCooQueryEar)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCooQueryEar, CDialog)
	//{{AFX_MSG_MAP(CCooQueryEar)
	ON_CBN_SELCHANGE(IDC_TYPECOMBO, OnSelchangeTypecombo)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_LOCATE, OnLocate)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCooQueryEar message handlers

BOOL CCooQueryEar::OnInitDialog()
{
	CDialog::OnInitDialog();

	((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->AddString(_T("Degree"));
	((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->AddString(_T("D-M-S"));

	((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->SetCurSel(0);
	OnSelchangeTypecombo();
	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Ears should return FALSE
}

void CCooQueryEar::OnSelchangeTypecombo()
{
	if(d_pCoorCtrl!=nullptr)
	{
		d_pCoorCtrl->PostMessage(WM_DESTROY, 0, 0);
		d_pCoorCtrl->DestroyWindow();
		delete d_pCoorCtrl;
		d_pCoorCtrl = nullptr;
	}
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Information.dll"));
	AfxSetResourceHandle(hInst);
	const long index = ((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->GetCurSel();
	switch(index)
	{
	case 0:
	{
		d_pCoorCtrl = new CDegreeCtrl((CWnd*)this, m_geoPoint);
		d_pCoorCtrl->Create(IDD_DEGREE, (CWnd*)this);
		d_pCoorCtrl->ShowWindow(SW_SHOWNORMAL);
	}
	break;
	case 1:
	{
		d_pCoorCtrl = new CSexageCtrl((CWnd*)this);
		d_pCoorCtrl->Create(IDD_SEXAGE, (CWnd*)this);
		d_pCoorCtrl->ShowWindow(SW_SHOWNORMAL);
	}
	break;
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);

	if(d_pCoorCtrl!=nullptr)
		d_pCoorCtrl->SetWindowPos(&wndTop, 6, 35, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);
}

void CCooQueryEar::OnDestroy()
{
	CDialog::OnDestroy();

	if(d_pCoorCtrl!=nullptr)
	{
		d_pCoorCtrl->PostMessage(WM_DESTROY, 0, 0);
		d_pCoorCtrl->DestroyWindow();
		delete d_pCoorCtrl;
		d_pCoorCtrl = nullptr;
	}
}

void CCooQueryEar::OnLocate()
{
	if(d_pCoorCtrl==nullptr)
		return;

	d_pCoorCtrl->SendMessage(WM_COMMAND, IDOK, 0);
	CFrameWnd* pMainFrame = (CFrameWnd*)AfxGetMainWnd();
	if(m_geoPoint.x<-180.f||m_geoPoint.x > 180.f||m_geoPoint.y<-90.f||m_geoPoint.y > 90.f)
	{
		AfxMessageBox(_T("Exceed the range of the global!"));
		return;
	}

	CView* pView1 = nullptr;
	CFrameWnd* pChildFrame = (CFrameWnd*)pMainFrame->GetActiveFrame();
	if(pChildFrame!=nullptr)
	{
		pView1 = pChildFrame->GetActiveView();
		if(pView1!=nullptr)
		{
			CRect rect;
			pView1->GetClientRect(rect);
			const CPoint cliPoint = rect.CenterPoint();
			const CPointF geoPoint = m_geoPoint;
			CString stringX;
			CString stringY;
			stringX.Format(_T("%.5f"), geoPoint.x);
			stringY.Format(_T("%.5f"), geoPoint.y);
			AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoX"), stringX);
			AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoY"), stringY);
			AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliX"), cliPoint.x);
			AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliY"), cliPoint.y);
		}
	}

	pMainFrame->SendMessage(WM_SETGEOSIGN, (UINT)&m_geoPoint, 0);
	pMainFrame->SendMessage(WM_MATCHMAP, 0, (UINT)&m_geoPoint);
}

void CCooQueryEar::OnClear()
{
	CFrameWnd* pMainFrame = (CFrameWnd*)AfxGetMainWnd();
	CPointF geoPoint(0.0f, 0.0f);
	pMainFrame->SendMessage(WM_SETGEOSIGN, (UINT)&geoPoint, 0);

	CFrameWnd* pChildFrame = (CFrameWnd*)pMainFrame->GetActiveFrame();
	if(pChildFrame!=nullptr)
	{
		CView* pView = pChildFrame->GetActiveView();
		if(pView!=nullptr)
		{
			pView->Invalidate(TRUE);
		}
	}
}

BOOL CCooQueryEar::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_KEYDOWN)
	{
		switch(pMsg->wParam)
		{
		case VK_ESCAPE:
			return TRUE;
			break;
		case VK_RETURN:
			OnLocate();
			break;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}
