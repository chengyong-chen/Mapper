#include "stdafx.h"
#include  <stdio.h>
#include  <stdlib.h>

#include "Global.h"
#include "GeomQueryEar.h"

#include "../Geometry/Geom.h"
#include "../Viewer/Global.h"

#include "../Dataview/Viewinfo.h"

#include "../Public/Function.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGeomQueryEar dialog
IMPLEMENT_DYNCREATE(CGeomQueryEar, CDialog)

CGeomQueryEar::CGeomQueryEar()
	: CDialog(CGeomQueryEar::IDD)
{
	//{{AFX_DATA_INIT(CGeomQueryEar)
	//}}AFX_DATA_INIT

	m_pDatabase = nullptr;
}

void CGeomQueryEar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGeomQueryEar)
	DDX_Control(pDX, IDC_LISTCTRL, m_listCtrl);
	DDX_Control(pDX, IDC_TYPECOMBO, m_TypeCombo);
	DDX_Control(pDX, IDC_ADMCODE, m_MarkCombo);
	DDX_Control(pDX, IDC_ROADCODE, m_RoadCombo);
	DDX_Control(pDX, IDC_POICODE1, m_POITypeCombo1);
	DDX_Control(pDX, IDC_POICODE2, m_POITypeCombo2);
	DDX_Control(pDX, IDC_POICODE3, m_POITypeCombo3);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGeomQueryEar, CDialog)
	//{{AFX_MSG_MAP(CGeomQueryEar)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_QUERY, OnQuery)
	ON_NOTIFY(LVN_DELETEITEM, IDC_LISTCTRL, OnDeleteItemListCtrl)
	ON_NOTIFY(NM_DBLCLK, IDC_LISTCTRL, OnDblclkListCtrl)
	//}}AFX_MSG_MAP
	ON_CBN_EDITUPDATE(IDC_MAPCOMBO, OnCbnEditupdateMapcombo)
	ON_CBN_SELCHANGE(IDC_TYPECOMBO, OnSelchangeType)
	ON_CBN_SELCHANGE(IDC_POICODE1, OnPOICom1SelChanged)
	ON_CBN_SELCHANGE(IDC_POICODE2, OnPOICom2SelChanged)
	//	ON_CBN_SELCHANGE(IDC_MAPCOMBO, OnCbnSelchangeMapcombo)
END_MESSAGE_MAP()

void CGeomQueryEar::DDX_CBData(CDataExchange* pDX, int nIDC, DWORD& dwData)
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

BOOL CGeomQueryEar::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString strAll;
	strAll.LoadString(IDS_STRING_ALL);
	const HWND hMapWndCtrl = ::GetDlgItem(GetSafeHwnd(), IDC_MAPCOMBO);
	const int nMapIndex = ::SendMessage(hMapWndCtrl, CB_ADDSTRING, 0, (LPARAM)strAll.GetBuffer(0));
	::SendMessage(hMapWndCtrl, CB_SETITEMDATA, nMapIndex, 0XFFFF);
	((CComboBox*)GetDlgItem(IDC_MAPCOMBO))->SetCurSel(0);
	strAll.ReleaseBuffer();

	CString columName1;
	CString columName2;
	columName1.LoadString(IDS_KEYNAME);
	columName2.LoadString(IDS_KEYMAP);

	LV_COLUMN lvcolumn;
	lvcolumn.mask = LVCF_FMT|LVCF_SUBITEM|LVCF_TEXT|LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;

	lvcolumn.iSubItem = 0;
	lvcolumn.pszText = columName1.GetBuffer(0);
	lvcolumn.cx = 120;
	m_listCtrl.InsertColumn(0, &lvcolumn);

	lvcolumn.iSubItem = 1;
	lvcolumn.pszText = columName2.GetBuffer(0);
	lvcolumn.cx = 100;
	m_listCtrl.InsertColumn(1, &lvcolumn);

	m_TypeCombo.AddString(_T("地名"));
	m_TypeCombo.AddString(_T("单位"));
	//	m_TypeCombo.AddString(_T("道路"));

	m_TypeCombo.SetCurSel(0);
	OnSelchangeType();

	columName1.ReleaseBuffer();
	columName2.ReleaseBuffer();
	return TRUE; // return TRUE unless you set the focus to a control	              // EXCEPTION: OCX Property Ears should return FALSE
}

void CGeomQueryEar::AddMapCombo(CMap<WORD, WORD&, CString, CString&>& mapIdMap) const
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

void CGeomQueryEar::OnOK()
{
}

void CGeomQueryEar::OnSize(UINT nType, int cx, int cy)
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

void CGeomQueryEar::OnDeleteItemListCtrl(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	const PLACE* place = (PLACE*)pNMListView->lParam;
	delete place;

	*pResult = 0;
}

BOOL CGeomQueryEar::PreTranslateMessage(MSG* pMsg)
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

void CGeomQueryEar::OnCbnEditupdateMapcombo()
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

void CGeomQueryEar::OnSetComboDatabase(CDatabase* pDatabase)
{
	if(pDatabase==nullptr)
		return;
	if(pDatabase==m_pDatabase)
		return;
	if(pDatabase->IsOpen()==FALSE)
		return;

	CString strAll;
	strAll.LoadString(IDS_STRING_ALL);

	if(m_MarkCombo.m_hWnd!=nullptr)
	{
		m_MarkCombo.AddString(strAll);
		m_MarkCombo.SetItemData(0, -1);

		try
		{
			CRecordset rs(pDatabase);
			rs.Open(CRecordset::snapshot, _T("Select * From ADMMARK_TYPE"), CRecordset::readOnly);
			while(rs.IsEOF()==FALSE)
			{
				CString strType;
				rs.GetFieldValue(_T("TYPE"), strType);
				CString strName;
				rs.GetFieldValue(_T("NAME"), strName);

				DWORD data = HextoDec(strType);
				int index = m_MarkCombo.AddString(strName);
				m_MarkCombo.SetItemData(index, data);

				rs.MoveNext();
			}
			rs.Close();
		}
		catch(CDBException*ex)
		{
			ex->Delete();
		}
		catch(CException*ex)
		{
			ex->Delete();
		}

		m_MarkCombo.SetCurSel(0);
	}

	if(m_RoadCombo.m_hWnd!=nullptr)
	{
		m_RoadCombo.AddString(strAll);
		m_RoadCombo.SetItemData(0, -1);

		try
		{
			CRecordset rs(pDatabase);
			rs.Open(CRecordset::snapshot, _T("Select * From ROAD_TYPE"), CRecordset::readOnly);
			while(rs.IsEOF()==FALSE)
			{
				CString strType;
				rs.GetFieldValue(_T("TYPE"), strType);
				CString strName;
				rs.GetFieldValue(_T("NAME"), strName);

				DWORD data = HextoDec(strType);
				int index = m_RoadCombo.AddString(strName);
				m_RoadCombo.SetItemData(index, data);

				rs.MoveNext();
			}
			rs.Close();
		}
		catch(CDBException*ex)
		{
			OutputDebugString(ex->m_strError);
			ex->Delete();
		}
		catch(CException*ex)
		{
			ex->Delete();
		}

		m_RoadCombo.SetCurSel(0);
	}

	if(m_POITypeCombo1.m_hWnd!=nullptr)
	{
		m_POITypeCombo1.AddString(strAll);
		m_POITypeCombo1.SetItemData(0, -1);

		try
		{
			CRecordset rs(pDatabase);
			rs.Open(CRecordset::snapshot, _T("Select * From POI_CODE1"), CRecordset::readOnly);
			while(rs.IsEOF()==FALSE)
			{
				CString strCODE;
				rs.GetFieldValue(_T("CODE1"), strCODE);
				CString strName;
				rs.GetFieldValue(_T("NAME"), strName);

				DWORD data = HextoDec(strCODE);
				int index = m_POITypeCombo1.AddString(strName);
				m_POITypeCombo1.SetItemData(index, data);

				rs.MoveNext();
			}
			rs.Close();
		}
		catch(CDBException*ex)
		{
			OutputDebugString(ex->m_strError);
			ex->Delete();
		}
		catch(CException*ex)
		{
			ex->Delete();
		}

		m_POITypeCombo1.SetCurSel(0);
		m_POITypeCombo2.EnableWindow(FALSE);
		m_POITypeCombo3.EnableWindow(FALSE);
	}

	m_pDatabase = pDatabase;
}

void CGeomQueryEar::OnSelchangeType()
{
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_TYPECOMBO);
	if(pComboBox!=nullptr)
	{
		const int index = pComboBox->GetCurSel();
		if(index!=-1)
		{
			CString str;
			pComboBox->GetLBText(index, str);
			if(str==_T("地名"))
			{
				GetDlgItem(IDC_STATIC2)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC3)->ShowWindow(SW_HIDE);

				m_POITypeCombo1.ShowWindow(SW_HIDE);
				m_POITypeCombo2.ShowWindow(SW_HIDE);
				m_POITypeCombo3.ShowWindow(SW_HIDE);

				m_RoadCombo.ShowWindow(SW_HIDE);

				GetDlgItem(IDC_STATIC1)->SetWindowText(_T("地名类别"));
				m_MarkCombo.ShowWindow(SW_SHOW);
			}
			else if(str==_T("单位"))
			{
				GetDlgItem(IDC_STATIC1)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_STATIC2)->ShowWindow(SW_SHOW);
				GetDlgItem(IDC_STATIC3)->ShowWindow(SW_SHOW);

				m_POITypeCombo1.ShowWindow(SW_SHOW);
				m_POITypeCombo2.ShowWindow(SW_SHOW);
				m_POITypeCombo3.ShowWindow(SW_SHOW);

				m_MarkCombo.ShowWindow(SW_HIDE);
				m_RoadCombo.ShowWindow(SW_HIDE);

				GetDlgItem(IDC_STATIC1)->SetWindowText(_T("单位类别1"));
			}
			else if(str==_T("道路"))
			{
				GetDlgItem(IDC_STATIC2)->ShowWindow(SW_HIDE);
				GetDlgItem(IDC_STATIC3)->ShowWindow(SW_HIDE);

				m_MarkCombo.ShowWindow(SW_HIDE);
				m_POITypeCombo1.ShowWindow(SW_HIDE);
				m_POITypeCombo2.ShowWindow(SW_HIDE);
				m_POITypeCombo3.ShowWindow(SW_HIDE);

				m_RoadCombo.ShowWindow(SW_SHOW);
				GetDlgItem(IDC_STATIC1)->SetWindowText(_T("道路类别"));
			}
		}
	}
}

void CGeomQueryEar::OnPOICom1SelChanged()
{
	m_POITypeCombo2.ResetContent();
	m_POITypeCombo2.ResetContent();
	const int index = m_POITypeCombo1.GetCurSel();
	if(index==0)
	{
		m_POITypeCombo2.EnableWindow(FALSE);
		m_POITypeCombo3.EnableWindow(FALSE);
	}
	else if(m_pDatabase!=nullptr&&m_pDatabase->IsOpen()==TRUE)
	{
		const DWORD data = m_POITypeCombo1.GetItemData(index);

		try
		{
			CString strSQL;
			strSQL.Format(_T("Select * From POI_CODE2 Where CODE1='%X'"), data);
			CRecordset rs(m_pDatabase);
			rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly);
			while(rs.IsEOF()==FALSE)
			{
				rs.MoveNext();
			}
			if(rs.GetRecordCount()>1)
			{
				CString strAll;
				strAll.LoadString(IDS_STRING_ALL);

				m_POITypeCombo2.AddString(strAll);
				m_POITypeCombo2.SetItemData(0, -1);

				rs.MoveFirst();
				while(rs.IsEOF()==FALSE)
				{
					CString strCODE;
					rs.GetFieldValue(_T("CODE2"), strCODE);
					CString strName;
					rs.GetFieldValue(_T("NAME"), strName);
					const DWORD data = HextoDec(strCODE);
					const int index = m_POITypeCombo2.AddString(strName);
					m_POITypeCombo2.SetItemData(index, data);

					rs.MoveNext();
				}

				m_POITypeCombo2.EnableWindow(TRUE);
				m_POITypeCombo3.EnableWindow(FALSE);
				m_POITypeCombo2.SetCurSel(0);
			}
			else
			{
				m_POITypeCombo2.EnableWindow(FALSE);
				m_POITypeCombo3.EnableWindow(FALSE);
			}

			rs.Close();
		}
		catch(CDBException*ex)
		{
			OutputDebugString(ex->m_strError);
			ex->ReportError();
			ex->Delete();
		}
		catch(CException*ex)
		{
			ex->Delete();
		}
	}
}

void CGeomQueryEar::OnPOICom2SelChanged()
{
	m_POITypeCombo3.ResetContent();
	const int index2 = m_POITypeCombo2.GetCurSel();
	if(index2==0)
	{
		m_POITypeCombo3.EnableWindow(FALSE);
	}
	else if(m_pDatabase!=nullptr&&m_pDatabase->IsOpen()==TRUE)
	{
		const int index1 = m_POITypeCombo1.GetCurSel();
		const DWORD CODE1 = m_POITypeCombo1.GetItemData(index1);
		const DWORD CODE2 = m_POITypeCombo2.GetItemData(index2);

		CString strSQL;
		strSQL.Format(_T("Select * From POI_CODE3 Where CODE1='%X'And CODE2='%X'"), CODE1, CODE2);
		CRecordset rs(m_pDatabase);
		try
		{
			rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly);
			while(rs.IsEOF()==FALSE)
			{
				rs.MoveNext();
			}
			if(rs.GetRecordCount()>1)
			{
				CString strAll;
				strAll.LoadString(IDS_STRING_ALL);

				m_POITypeCombo3.AddString(strAll);
				m_POITypeCombo3.SetItemData(0, -1);

				rs.MoveFirst();
				while(rs.IsEOF()==FALSE)
				{
					CString strCODE;
					rs.GetFieldValue(_T("CODE3"), strCODE);
					CString strName;
					rs.GetFieldValue(_T("NAME"), strName);
					const DWORD data = HextoDec(strCODE);
					const int index = m_POITypeCombo3.AddString(strName);
					m_POITypeCombo3.SetItemData(index, data);

					rs.MoveNext();
				}

				m_POITypeCombo3.EnableWindow(TRUE);
				m_POITypeCombo3.SetCurSel(0);
			}
			else
			{
				m_POITypeCombo3.EnableWindow(FALSE);
			}

			rs.Close();
		}
		catch(CDBException*ex)
		{
			OutputDebugString(ex->m_strError);
			rs.Close();
			ex->ReportError();
			ex->Delete();
		}
		catch(CException*ex)
		{
			ex->Delete();
		}
	}
}

void CGeomQueryEar::OnQuery()
{
	if(m_pDatabase==nullptr)
		return;
	if(m_pDatabase->IsOpen()==FALSE)
		return;

	UpdateData(TRUE);

	m_listCtrl.DeleteAllItems();

	CString strKey;
	GetDlgItem(IDC_KEY)->GetWindowText(strKey);

	if(strKey.IsEmpty()==FALSE)
	{
		CString strAll;
		strAll.LoadString(IDS_STRING_ALL);

		CString strSQL;
		int index = m_TypeCombo.GetCurSel();
		if(index!=-1)
		{
			CString str;
			m_TypeCombo.GetLBText(index, str);

			if(str==_T("地名"))
			{
				strSQL.Format(_T("Select B.MAP,B.NAME,A.X,A.Y from AdmMark_DATA A,AdmMark_Attribute B WHERE A.ID=b.ID And B.Name Like '%c%s%c'"), '%', strKey, '%');
				int index = m_MarkCombo.GetCurSel();
				DWORD data = m_MarkCombo.GetItemData(index);
				if(data!=-1)
				{
					CString str;
					str.Format(_T(" And A.Type='%X'"), data);
					strSQL += str;
				}

				CString strMap;
				GetDlgItem(IDC_MAPCOMBO)->GetWindowText(strMap);
				if(strMap!=strAll&&strMap.IsEmpty()==FALSE)
				{
					CString str;
					str.Format(_T(" And B.MAP='%s'"), strMap);
					strSQL += str;
				}
			}
			else if(str==_T("单位"))
			{
				strSQL.Format(_T("Select X,Y,NAME,MAP From POI Where NAME Like '%c%s%c'"), _T('\%'), strKey, _T('\%'));

				int index1 = m_POITypeCombo1.GetCurSel();
				if(index1!=0)
				{
					DWORD data = m_POITypeCombo1.GetItemData(index1);
					CString str;
					str.Format(_T(" And CODE1='%X'"), data);
					strSQL += str;

					int index2 = m_POITypeCombo2.GetCurSel();
					if(m_POITypeCombo2.IsWindowEnabled()==TRUE&&index2!=0)
					{
						DWORD data = m_POITypeCombo2.GetItemData(index2);
						CString str;
						str.Format(_T(" And CODE2='%X'"), data);
						strSQL += str;

						int index3 = m_POITypeCombo3.GetCurSel();
						if(m_POITypeCombo3.IsWindowEnabled()==TRUE&&index3!=0)
						{
							DWORD data = m_POITypeCombo3.GetItemData(index3);
							CString str;
							str.Format(_T(" And %sCODE3='%X'"), data);
							strSQL += str;
						}
					}
				}

				CString strMap;
				GetDlgItem(IDC_MAPCOMBO)->GetWindowText(strMap);
				if(strMap!=strAll&&strMap.IsEmpty()==FALSE)
				{
					CString str;
					str.Format(_T(" And MAP='%s'"), strMap);
					strSQL += str;
				}
			}
			else if(str==_T("道路"))
			{
				strSQL = _T("Select ID,Type,NAME,MAP From ROAD Where");
				int index = m_RoadCombo.GetCurSel();
				DWORD data = m_RoadCombo.GetItemData(index);
				if(data!=-1)
				{
					CString str;
					str.Format(_T(" CODE='%X'"), data);
					strSQL += str;
				}

				CString strMap;
				GetDlgItem(IDC_MAPCOMBO)->GetWindowText(strMap);
				if(strMap!=strAll&&strMap.IsEmpty()==FALSE)
				{
					CString str;
					str.Format(_T(" And B.MAP='%s'"), strMap);
					strSQL += str;
				}

				if(strKey.IsEmpty()==FALSE)
				{
					CString str;
					str.Format(_T(" And B.NAME Like '%c%s%c'"), _T('\%'), strKey, _T('\%'));
					strSQL += str;
				}
			}

			int count = 0;
			m_listCtrl.DeleteAllItems();
			try
			{
				CRecordset rs(m_pDatabase);
				rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly);
				while(rs.IsEOF()==FALSE)
				{
					PLACE* place = new PLACE;
					place->wMap = -1;
					place->wLayer = -1;

					int nItem = m_listCtrl.GetItemCount();
					int index = m_listCtrl.InsertItem(nItem, nullptr);
					m_listCtrl.SetItemData(index, (DWORD)place);

					if(str==_T("地名"))
					{
						CString strValue;
						rs.GetFieldValue(_T("X"), strValue);
						place->dLng = _tcstod(strValue, nullptr);

						rs.GetFieldValue(_T("Y"), strValue);
						place->dLat = _tcstod(strValue, nullptr);
					}
					else if(str==_T("单位"))
					{
						CString strValue;
						rs.GetFieldValue(_T("X"), strValue);
						place->dLng = _tcstod(strValue, nullptr);

						rs.GetFieldValue(_T("Y"), strValue);
						place->dLat = _tcstod(strValue, nullptr);
					}
					else if(str==_T("道路"))
					{
						CString strID;
						rs.GetFieldValue(_T("ID"), strID);
						place->dwGeom = _ttoi(strID);

						//	CString strType;
						//	rs.GetFieldValue(_T("TYPE"),strType);
						//	place->dwType = _ttoi(strType);
					}

					CString strName;
					rs.GetFieldValue(_T("NAME"), strName);
					m_listCtrl.SetItemText(nItem, 0, strName);

					CString strMap;
					rs.GetFieldValue(_T("MAP"), strMap);
					m_listCtrl.SetItemText(nItem, 1, strMap);

					count++;
					rs.MoveNext();
				}
				rs.Close();
			}
			catch(CDBException*ex)
			{
				OutputDebugString(ex->m_strError);
				ex->ReportError();
				ex->Delete();
			}
			catch(CException*ex)
			{
				ex->Delete();
			}

			CString strCount;
			strCount.Format(_T("%d "), count);
			GetDlgItem(IDC_COUNT)->SetWindowText(strCount);
		}
	}
}

void CGeomQueryEar::OnDblclkListCtrl(NMHDR* pNMHDR, LRESULT* pResult)
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

	if(place->dwGeom==-1)
	{
		CPointF geoPoint;
		geoPoint.x = place->dLng;
		geoPoint.y = place->dLat;

		pFrame->SendMessage(WM_SETGEOSIGN, (UINT)&geoPoint, 0);
		pFrame->SendMessage(WM_MATCHMAP, 0, (UINT)&geoPoint);
	}
	else if(place->wMap!=-1)
	{
		pFrame->SendMessage(WM_FLASHGEOM, 3, (UINT)place);
	}

	*pResult = 0;
}
