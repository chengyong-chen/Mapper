#include "stdafx.h"
#include  <stdio.h>
#include "Global.h"
#include "KeyQueryEar.h"

#include "../Geometry/Geom.h"
#include "../Viewer/Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKeyQueryEar dialog
IMPLEMENT_DYNCREATE(CKeyQueryEar, CDialog)

CKeyQueryEar::CKeyQueryEar()
	: CDialog(CKeyQueryEar::IDD)
{
	//{{AFX_DATA_INIT(CKeyQueryEar)
	//}}AFX_DATA_INIT
}

void CKeyQueryEar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKeyQueryEar)
	DDX_Control(pDX, IDC_LISTCTRL, m_listCtrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CKeyQueryEar, CDialog)
	//{{AFX_MSG_MAP(CKeyQueryEar)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_QUERY, OnQuery)
	ON_NOTIFY(LVN_DELETEITEM, IDC_LISTCTRL, OnDeleteItemListCtrl)
	ON_NOTIFY(NM_DBLCLK, IDC_LISTCTRL, OnDblclkListCtrl)
	ON_CBN_SELCHANGE(IDC_CLASSCOMBO1, OnClass1Selchange)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETCOUNT, OnSetCount)
	ON_CBN_EDITUPDATE(IDC_MAPCOMBO, OnCbnEditupdateMapcombo)
	//	ON_CBN_SELCHANGE(IDC_MAPCOMBO, OnCbnSelchangeMapcombo)
END_MESSAGE_MAP()

void CKeyQueryEar::DDX_CBData(CDataExchange* pDX, int nIDC, DWORD& dwData)
{
	const HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
	if(pDX->m_bSaveAndValidate)
	{
		const int nSelected = ::SendMessage(hWndCtrl, CB_GETCURSEL, 0, 0L);
		dwData = ::SendMessage(hWndCtrl, CB_GETITEMDATA, nSelected, 0L);
	}
	else
	{
		const int nCount = ::SendMessage(hWndCtrl, CB_GETCOUNT, 0, 0L);
		for(int i = 0; i<nCount; i++)
		{
			const DWORD dwItemData = ::SendMessage(hWndCtrl, CB_GETITEMDATA, i, 0L);
			if(dwItemData==dwData)
			{
				::SendMessage(hWndCtrl, CB_SETCURSEL, i, 0L);
				break;
			}
		}
	}
}

BOOL CKeyQueryEar::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString strAll;
	strAll.LoadString(IDS_STRING_ALL);
	const HWND hMapWndCtrl = ::GetDlgItem(GetSafeHwnd(), IDC_MAPCOMBO);
	const int nMapIndex = ::SendMessage(hMapWndCtrl, CB_ADDSTRING, 0, (LPARAM)strAll.GetBuffer(0));
	::SendMessage(hMapWndCtrl, CB_SETITEMDATA, nMapIndex, 0XFFFF);
	((CComboBox*)GetDlgItem(IDC_MAPCOMBO))->SetCurSel(0);
	strAll.ReleaseBuffer();
	const HWND hClassWndCtrl1 = ::GetDlgItem(GetSafeHwnd(), IDC_CLASSCOMBO1);
	const int nClassIndex1 = ::SendMessage(hClassWndCtrl1, CB_ADDSTRING, 0, (LPARAM)strAll.GetBuffer(0));
	::SendMessage(hClassWndCtrl1, CB_SETITEMDATA, nClassIndex1, 0XFF);
	((CComboBox*)GetDlgItem(IDC_CLASSCOMBO1))->SetCurSel(0);
	strAll.ReleaseBuffer();
	const HWND hClassWndCtrl2 = ::GetDlgItem(GetSafeHwnd(), IDC_CLASSCOMBO2);
	const int nClassIndex2 = ::SendMessage(hClassWndCtrl2, CB_ADDSTRING, 0, (LPARAM)strAll.GetBuffer(0));
	::SendMessage(hClassWndCtrl2, CB_SETITEMDATA, nClassIndex2, 0XFF);
	((CComboBox*)GetDlgItem(IDC_CLASSCOMBO2))->SetCurSel(0);
	strAll.ReleaseBuffer();

	CString columName1;
	CString columName2;
	CString columName3;
	columName1.LoadString(IDS_KEYNAME);
	columName2.LoadString(IDS_KEYMAP);
	columName3.LoadString(IDS_KEYLAYER);

	LV_COLUMN lvcolumn;
	lvcolumn.mask = LVCF_FMT|LVCF_SUBITEM|LVCF_TEXT|LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;

	lvcolumn.iSubItem = 0;
	lvcolumn.pszText = columName1.GetBuffer(0);
	lvcolumn.cx = 100;
	m_listCtrl.InsertColumn(0, &lvcolumn);

	lvcolumn.iSubItem = 1;
	lvcolumn.pszText = columName2.GetBuffer(0);
	lvcolumn.cx = 80;
	m_listCtrl.InsertColumn(1, &lvcolumn);

	lvcolumn.iSubItem = 2;
	lvcolumn.pszText = columName3.GetBuffer(0);
	lvcolumn.cx = 80;
	m_listCtrl.InsertColumn(2, &lvcolumn);

	//m_nList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	OnClass1Selchange();
	columName1.ReleaseBuffer();
	columName2.ReleaseBuffer();
	columName3.ReleaseBuffer();
	return TRUE; // return TRUE unless you set the focus to a control	              // EXCEPTION: OCX Property Ears should return FALSE
}

void CKeyQueryEar::AddMapCombo(CMap<WORD, WORD&, CString, CString&>& mapIdMap) const
{
	const HWND hMapWndCtrl = ::GetDlgItem(GetSafeHwnd(), IDC_MAPCOMBO);
	POSITION pos = mapIdMap.GetStartPosition();
	while(pos!=nullptr)
	{
		WORD wMap;
		CString strMap;
		mapIdMap.GetNextAssoc(pos, wMap, strMap);
		const int nMapIndex = ::SendMessage(hMapWndCtrl, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)strMap);
		::SendMessage(hMapWndCtrl, CB_SETITEMDATA, nMapIndex, wMap);
	}
}

void CKeyQueryEar::AddClassCombo1(CMap<DWORD, DWORD&, CString, CString&>& mapIdClass) const
{
	const HWND hClassWndCtrl1 = ::GetDlgItem(GetSafeHwnd(), IDC_CLASSCOMBO1);
	POSITION pos = mapIdClass.GetStartPosition();
	while(pos!=nullptr)
	{
		DWORD dwClass;
		CString strClass;
		mapIdClass.GetNextAssoc(pos, dwClass, strClass);
		const int nClassIndex = ::SendMessage(hClassWndCtrl1, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)strClass);
		::SendMessage(hClassWndCtrl1, CB_SETITEMDATA, nClassIndex, dwClass);
	}
}

void CKeyQueryEar::OnOK()
{
}

void CKeyQueryEar::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if(m_listCtrl.m_hWnd!=nullptr)
	{
		CRect rect;
		m_listCtrl.GetWindowRect(rect);
		this->ScreenToClient(rect);

		m_listCtrl.SetWindowPos(&wndTop, 0, 0, cx, cy-rect.top, SWP_NOMOVE);
	}
}

void CKeyQueryEar::OnQuery()
{
	UpdateData(TRUE);

	m_listCtrl.DeleteAllItems();

	CString strKey;
	GetDlgItem(IDC_KEY)->GetWindowText(strKey);

	while(strKey.Find(_T(" +"))!=-1)
	{
		strKey.Replace(_T(" +"), _T("+"));
	}
	while(strKey.Find(_T("+ "))!=-1)
	{
		strKey.Replace(_T("+ "), _T("+"));
	}
	while(strKey.Find(_T("& "))!=-1)
	{
		strKey.Replace(_T("& "), _T("&"));
	}
	while(strKey.Find(_T(" &"))!=-1)
	{
		strKey.Replace(_T(" &"), _T("&"));
	}
	while(strKey.Find(_T("| "))!=-1)
	{
		strKey.Replace(_T("| "), _T("|"));
	}
	while(strKey.Find(_T(" |"))!=-1)
	{
		strKey.Replace(_T(" |"), _T("|"));
	}
	while(strKey.Find(_T("  "))!=-1)
	{
		strKey.Replace(_T("  "), _T(" "));
	}

	if(strKey.IsEmpty()==FALSE)
	{
		WORD wMap = 0XFFFF;
		int index = ((CComboBox*)GetDlgItem(IDC_MAPCOMBO))->GetCurSel();
		if(index!=LB_ERR)
			wMap = ((CComboBox*)GetDlgItem(IDC_MAPCOMBO))->GetItemData(index);
		else
		{
			CString str;
			GetDlgItem(IDC_MAPCOMBO)->GetWindowText(str);
			if(str.IsEmpty()==FALSE)
			{
				index = ((CComboBox*)GetDlgItem(IDC_MAPCOMBO))->FindString(-1, str);
				if(index!=LB_ERR)
				{
					wMap = ((CComboBox*)GetDlgItem(IDC_MAPCOMBO))->GetItemData(index);
				}
			}
		}
		const WORD wClass1 = 0XFFFF;
		const WORD wClass2 = 0XFFFF;
		DWORD dwClass = wClass1;
		dwClass = dwClass<<8;
		dwClass = dwClass|wClass2;

		CString strCtrl;
		strCtrl.Format(_T("%4X %8X "), wMap, dwClass);
		strKey = strCtrl+strKey;

		CFrameWnd* pAppFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
		if(pAppFrame!=nullptr)
		{
			AfxGetApp()->BeginWaitCursor();
			const int count = pAppFrame->SendMessage(WM_KEYQUERY, (UINT)&strKey, (UINT)&m_listCtrl);
			this->OnSetCount(count, 0);
			AfxGetApp()->EndWaitCursor();
		}
	}
}

void CKeyQueryEar::OnDblclkListCtrl(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pnmListView = (NM_LISTVIEW*)pNMHDR;
	const int index = pnmListView->iItem;
	if(index==-1)
		return;

	PLACE* place = (PLACE*)m_listCtrl.GetItemData(index);
	if(place==nullptr)
		return;

	CFrameWnd* pFrame = (CFrameWnd*)AfxGetMainWnd();
	if(pFrame==nullptr)
	{
		AfxMessageBox(_T("No map file!"));
		*pResult = 0;
		return;
	}

	pFrame->SendMessage(WM_FLASHGEOM, 3, (UINT)place);

	*pResult = 0;
}

void CKeyQueryEar::OnDeleteItemListCtrl(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	const PLACE* place = (PLACE*)pNMListView->lParam;
	delete place;

	*pResult = 0;
}

LONG CKeyQueryEar::OnSetCount(UINT WPARAM, LONG LPARAM)
{
	CString strCount;
	strCount.Format(_T("%d "), WPARAM);
	GetDlgItem(IDC_COUNT)->SetWindowText(strCount);
	return 0;
}

BOOL CKeyQueryEar::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_KEYDOWN)
	{
		switch(pMsg->wParam)
		{
		case VK_ESCAPE:
			return TRUE;
			break;
		case VK_RETURN:
			if(pMsg->hwnd==GetDlgItem(IDC_KEY)->m_hWnd)
			{
				OnQuery();
			}
			else
			{
			}

			break;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CKeyQueryEar::OnClass1Selchange()
{
	const int index = ((CComboBox*)GetDlgItem(IDC_CLASSCOMBO1))->GetCurSel();
	const DWORD dwData = ((CComboBox*)GetDlgItem(IDC_CLASSCOMBO1))->GetItemData(index);
	if(dwData&0XFF==0XFF)
	{
		((CComboBox*)GetDlgItem(IDC_CLASSCOMBO2))->EnableWindow(FALSE);
	}
	else
	{
		((CComboBox*)GetDlgItem(IDC_CLASSCOMBO2))->EnableWindow(TRUE);

		CString strAll;
		strAll.LoadString(IDS_STRING_ALL);
		const HWND hClassWndCtrl2 = ::GetDlgItem(GetSafeHwnd(), IDC_CLASSCOMBO2);
		const int nClassIndex2 = ::SendMessage(hClassWndCtrl2, CB_ADDSTRING, 0, (LPARAM)strAll.GetBuffer(0));
		::SendMessage(hClassWndCtrl2, CB_SETITEMDATA, nClassIndex2, 0XFF);
		((CComboBox*)GetDlgItem(IDC_CLASSCOMBO2))->SetCurSel(0);
		strAll.ReleaseBuffer();
	}
}

void CKeyQueryEar::OnCbnEditupdateMapcombo()
{
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_MAPCOMBO);
	if(pComboBox==nullptr)
		return;

	CString str;
	GetDlgItem(IDC_MAPCOMBO)->GetWindowText(str);
	const int nLength = str.GetLength();
	const DWORD dwCurSel = pComboBox->GetEditSel();
	const WORD dStart = LOWORD(dwCurSel);
	const WORD dEnd = HIWORD(dwCurSel);

	if(pComboBox->SelectString(-1, str)!=CB_ERR)
	{
		if(dEnd<nLength&&dwCurSel!=CB_ERR)
			pComboBox->SetEditSel(dStart, dEnd);
		else
			pComboBox->SetEditSel(nLength, -1);
	}
	else
	{
		pComboBox->SetCurSel(0);
		pComboBox->SetEditSel(dStart, dEnd);
	}
}
