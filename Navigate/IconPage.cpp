// IconPage.cpp : implementation file
//

#include "stdafx.h"
#include "IconPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIconPage property page

IMPLEMENT_DYNCREATE(CIconPage, CPropertyPage)

CIconPage::CIconPage() : CPropertyPage(CIconPage::IDD)
{
	//{{AFX_DATA_INIT(CIconPage)
	//}}AFX_DATA_INIT
}

CIconPage::~CIconPage()
{
	if(m_imagelist.m_hImageList!=nullptr)
	{
		m_imagelist.DeleteImageList();
		m_imagelist.Detach();
	}
}

void CIconPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIconPage)
	DDX_Control(pDX, IDC_ICON1, m_Icon1);
	DDX_Control(pDX, IDC_ICON2, m_Icon2);
	DDX_Control(pDX, IDC_ICON3, m_Icon3);
	DDX_Control(pDX, IDC_ICON4, m_Icon4);
	DDX_Control(pDX, IDC_IMAGELIST1, m_ImageCtrl1);
	DDX_Control(pDX, IDC_IMAGELIST2, m_ImageCtrl2);
	DDX_Control(pDX, IDC_IMAGELIST3, m_ImageCtrl3);
	DDX_Control(pDX, IDC_IMAGELIST4, m_ImageCtrl4);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CIconPage, CPropertyPage)
	//{{AFX_MSG_MAP(CIconPage)
	ON_WM_PAINT()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_IMAGELIST1, OnItemchangedImagelist1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_IMAGELIST2, OnItemchangedImagelist2)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_IMAGELIST3, OnItemchangedImagelist3)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_IMAGELIST4, OnItemchangedImagelist4)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIconPage message handlers

BOOL CIconPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Navigate.dll"));
	AfxSetResourceHandle(hInst);

	if(m_imagelist.Create(IDB_PREVIEW, 32, 1, RGB(255, 0, 255))==true)
	{
		m_ImageCtrl1.SetImageList(&m_imagelist, LVSIL_NORMAL);
		m_ImageCtrl2.SetImageList(&m_imagelist, LVSIL_NORMAL);
		m_ImageCtrl3.SetImageList(&m_imagelist, LVSIL_NORMAL);
		m_ImageCtrl4.SetImageList(&m_imagelist, LVSIL_NORMAL);

		m_imagelist.SetOverlayImage(0, 1);

		m_ImageCtrl1.SetIconSpacing(32, 32);
		m_ImageCtrl2.SetIconSpacing(32, 32);
		m_ImageCtrl3.SetIconSpacing(32, 32);
		m_ImageCtrl4.SetIconSpacing(32, 32);
	}

	for(int index = 0; index<4; index++)
	{
		LV_ITEM lvi;
		lvi.mask = LVIF_IMAGE;
		lvi.iItem = index;
		lvi.iImage = index;
		lvi.iSubItem = 0;

		m_ImageCtrl1.InsertItem(&lvi);
		m_ImageCtrl2.InsertItem(&lvi);
		m_ImageCtrl3.InsertItem(&lvi);
		m_ImageCtrl4.InsertItem(&lvi);
	}
	const int nIcon1 = AfxGetApp()->GetProfileInt(_T("Navigate"), _T("Icon1"), 1);
	const int nIcon2 = AfxGetApp()->GetProfileInt(_T("Navigate"), _T("Icon2"), 2);
	const int nIcon3 = AfxGetApp()->GetProfileInt(_T("Navigate"), _T("Icon3"), 3);
	const int nIcon4 = AfxGetApp()->GetProfileInt(_T("Navigate"), _T("Icon4"), 4);

	m_ImageCtrl1.SetItemState(nIcon1-1, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	m_ImageCtrl2.SetItemState(nIcon2-1, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	m_ImageCtrl3.SetItemState(nIcon3-1, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	m_ImageCtrl4.SetItemState(nIcon4-1, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	const CString strPort = AfxGetApp()->GetProfileString(_T("Navigate"), _T("Port"), _T("GPS Receiver"));
	if(strPort==_T("GPS Receiver"))
	{
		GetDlgItem(IDC_STATIC1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC4)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_IMAGELIST2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ICON2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_IMAGELIST3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ICON3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_IMAGELIST4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ICON4)->ShowWindow(SW_HIDE);
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CIconPage::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CPaintDC dc1(&m_Icon1);
	POSITION pos1 = m_ImageCtrl1.GetFirstSelectedItemPosition();
	while(pos1!=nullptr)
	{
		const int nItem = m_ImageCtrl1.GetNextSelectedItem(pos1);
		m_imagelist.Draw(&dc1, nItem, CPoint(0, 0), ILD_NORMAL|ILD_TRANSPARENT);
	}

	CPaintDC dc2(&m_Icon2);
	POSITION pos2 = m_ImageCtrl2.GetFirstSelectedItemPosition();
	while(pos2!=nullptr)
	{
		const int nItem = m_ImageCtrl2.GetNextSelectedItem(pos2);
		m_imagelist.Draw(&dc2, nItem, CPoint(0, 0), ILD_NORMAL|ILD_TRANSPARENT);
	}

	CPaintDC dc3(&m_Icon3);
	POSITION Pos3 = m_ImageCtrl3.GetFirstSelectedItemPosition();
	while(Pos3!=nullptr)
	{
		const int nItem = m_ImageCtrl3.GetNextSelectedItem(Pos3);
		m_imagelist.Draw(&dc3, nItem, CPoint(0, 0), ILD_NORMAL|ILD_TRANSPARENT);
	}

	CPaintDC dc4(&m_Icon4);
	POSITION Pos4 = m_ImageCtrl4.GetFirstSelectedItemPosition();
	while(Pos4!=nullptr)
	{
		const int nItem = m_ImageCtrl4.GetNextSelectedItem(Pos4);
		m_imagelist.Draw(&dc4, nItem, CPoint(0, 0), ILD_NORMAL|ILD_TRANSPARENT);
	}
}

void CIconPage::OnItemchangedImagelist1(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	const int index = pNMListView->iItem;
	if(index!=-1&&pNMListView->uNewState!=0)
	{
		m_Icon1.Invalidate();
		CPaintDC dc1(&m_Icon1);
		POSITION pos1 = m_ImageCtrl1.GetFirstSelectedItemPosition();
		while(pos1!=nullptr)
		{
			const int nItem = m_ImageCtrl1.GetNextSelectedItem(pos1);

			dc1.FillSolidRect(CRect(0, 0, 32, 32), 0XFFFFFF);
			m_imagelist.Draw(&dc1, nItem, CPoint(0, 0), ILD_NORMAL|ILD_TRANSPARENT);

			AfxGetApp()->WriteProfileInt(_T("Navigate"), _T("Icon1"), nItem+1);
		}
	}

	*pResult = 0;
}

void CIconPage::OnItemchangedImagelist2(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	const int index = pNMListView->iItem;
	if(index!=-1&&pNMListView->uNewState!=0)
	{
		m_Icon2.Invalidate();
		CPaintDC dc2(&m_Icon2);
		POSITION pos2 = m_ImageCtrl2.GetFirstSelectedItemPosition();
		while(pos2!=nullptr)
		{
			const int nItem = m_ImageCtrl2.GetNextSelectedItem(pos2);

			dc2.FillSolidRect(CRect(0, 0, 32, 32), 0XFFFFFF);
			m_imagelist.Draw(&dc2, nItem, CPoint(0, 0), ILD_NORMAL|ILD_TRANSPARENT);

			AfxGetApp()->WriteProfileInt(_T("Navigate"), _T("Icon2"), nItem+1);
		}
	}

	*pResult = 0;
}

void CIconPage::OnItemchangedImagelist3(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	const int index = pNMListView->iItem;
	if(index!=-1&&pNMListView->uNewState!=0)
	{
		m_Icon3.Invalidate();
		CPaintDC dc3(&m_Icon3);
		POSITION Pos3 = m_ImageCtrl3.GetFirstSelectedItemPosition();
		while(Pos3!=nullptr)
		{
			const int nItem = m_ImageCtrl3.GetNextSelectedItem(Pos3);

			dc3.FillSolidRect(CRect(0, 0, 32, 32), 0XFFFFFF);
			m_imagelist.Draw(&dc3, nItem, CPoint(0, 0), ILD_NORMAL|ILD_TRANSPARENT);

			AfxGetApp()->WriteProfileInt(_T("Navigate"), _T("Icon3"), nItem+1);
		}
	}

	*pResult = 0;
}

void CIconPage::OnItemchangedImagelist4(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	const int index = pNMListView->iItem;
	if(index!=-1&&pNMListView->uNewState!=0)
	{
		m_Icon4.Invalidate();
		CPaintDC dc4(&m_Icon4);
		POSITION Pos4 = m_ImageCtrl4.GetFirstSelectedItemPosition();
		while(Pos4!=nullptr)
		{
			const int nItem = m_ImageCtrl4.GetNextSelectedItem(Pos4);

			dc4.FillSolidRect(CRect(0, 0, 32, 32), 0XFFFFFF);
			m_imagelist.Draw(&dc4, nItem, CPoint(0, 0), ILD_NORMAL|ILD_TRANSPARENT);

			AfxGetApp()->WriteProfileInt(_T("Navigate"), _T("Icon4"), nItem+1);
		}
	}

	*pResult = 0;
}
