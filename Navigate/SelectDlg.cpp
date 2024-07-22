#include "stdafx.h"
#include "SelectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectDlg dialog

CSelectDlg::CSelectDlg(CWnd* pParent, CDatabase* pDatabase, DWORD dwUserID)
	: CDialog(CSelectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectDlg)

	//}}AFX_DATA_INIT
	m_pDatabase = pDatabase;
	m_dwUser = dwUserID;
}

void CSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Control(pDX, IDC_USERLIST, m_listctrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSelectDlg, CDialog)
	//{{AFX_MSG_MAP(CSelectDlg)
	ON_BN_CLICKED(IDC_SEARCH, OnSearch)
	ON_BN_CLICKED(IDC_ALL, OnVehicle)
	ON_BN_CLICKED(IDC_GROUP, OnGroup)
	ON_BN_CLICKED(IDC_VEHICLE, OnVehicle)
	ON_NOTIFY(NM_CLICK, IDC_USERLIST, OnClick)
	ON_NOTIFY(NM_DBLCLK, IDC_USERLIST, OnDblclk)
	ON_NOTIFY(NM_RCLICK, IDC_USERLIST, OnRclick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectDlg message handlers
BOOL CSelectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	/*
		if(m_pDatabase == nullptr)
			return FALSE;
		if(m_pDatabase->IsOpen() == FALSE)
			return FALSE;

		ListView_SetExtendedListViewStyle(m_listctrl.m_hWnd, LVS_EX_FULLROWSELECT);


	//	GetDlgItem(IDC_VEHICLE)->EnableWindow(FALSE);
	//	GetDlgItem(IDC_VEHICLE)->ShowWindow(SW_HIDE);

	//	GetDlgItem(IDC_GROUP)->EnableWindow(FALSE);
	//	GetDlgItem(IDC_GROUP)->ShowWindow(SW_HIDE);


		CenterWindow();
		((CComboBox*)GetDlgItem(IDC_FIELDCOMBO))->SetCurSel(0);
		((CComboBox*)GetDlgItem(IDC_OPERATERCOMBO))->SetCurSel(0);
		((CButton*)GetDlgItem(IDC_VEHICLE))->SetCheck(TRUE);
		OnVehicle();
	*/
	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// CSelectDlg message handlers
void CSelectDlg::OnSearch()
{
	CString strColumn;
	const int nIndex1 = ((CComboBox*)GetDlgItem(IDC_FIELDCOMBO))->GetCurSel();
	if(nIndex1==-1)
		return;

	((CComboBox*)GetDlgItem(IDC_FIELDCOMBO))->GetLBText(nIndex1, strColumn);
	if(strColumn.IsEmpty()==TRUE)
		return;

	CString strValue;
	GetDlgItem(IDC_VALUE)->GetWindowText(strValue);
	if(strValue.IsEmpty()==TRUE)
		return;

	CString strOperator;
	const int nIndex2 = ((CComboBox*)GetDlgItem(IDC_OPERATERCOMBO))->GetCurSel();
	if(nIndex2==-1)
		return;

	CString strSQL;
	((CComboBox*)GetDlgItem(IDC_OPERATERCOMBO))->GetLBText(nIndex2, strOperator);
	if(strOperator==_T("包含"))
	{
		strSQL.Format(_T("SELECT DISTINCT ID,姓名,车牌号码 FROM View_Vehicle WHERE UserID = %d And %s Like '%c%s%c' ORDER BY ID"), m_dwUser, strColumn, '%', strValue, '%');
	}
	else if(strOperator==_T("等于"))
	{
		strSQL.Format(_T("SELECT DISTINCT ID,姓名,车牌号码 FROM View_Vehicle WHERE UserID = %d And %s = '%s' ORDER BY ID"), m_dwUser, strColumn, strValue);
	}
	else
		return;

	m_listctrl.DeleteAllItems();

	try
	{
		CRecordset rs(m_pDatabase);
		if(rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly)==TRUE)
		{
			while(!rs.IsEOF())
			{
				const int nItem = m_listctrl.GetItemCount();
				const int index = m_listctrl.InsertItem(nItem, nullptr);

				CString strValue;
				rs.GetFieldValue(_T("ID"), strValue);
				m_listctrl.SetItemText(nItem, 0, strValue);
				m_listctrl.SetItemData(index, _ttoi(strValue));

				rs.GetFieldValue(_T("姓名"), strValue);
				strValue.TrimRight(_T(" "));
				m_listctrl.SetItemText(nItem, 1, strValue);

				rs.GetFieldValue(_T("车牌号码"), strValue);
				strValue.TrimRight(_T(" "));
				m_listctrl.SetItemText(nItem, 2, strValue);

				rs.MoveNext();
			}

			rs.Close();
		}
	}
	catch(CDBException*ex)
	{
		ex->ReportError();
		ex->Delete();
	}
}

void CSelectDlg::OnGroup()
{
	GetDlgItem(IDC_FIELDCOMBO)->EnableWindow(FALSE);
	GetDlgItem(IDC_OPERATERCOMBO)->EnableWindow(FALSE);
	GetDlgItem(IDC_VALUE)->EnableWindow(FALSE);
	GetDlgItem(IDC_SEARCH)->EnableWindow(FALSE);
	GetDlgItem(IDC_ALL)->EnableWindow(FALSE);

	m_listctrl.DeleteAllItems();

	m_listctrl.DeleteColumn(0);
	m_listctrl.DeleteColumn(0);
	m_listctrl.DeleteColumn(0);

	LV_COLUMN lvcolumn;
	lvcolumn.mask = LVCF_FMT|LVCF_SUBITEM|LVCF_TEXT|LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_CENTER;

	lvcolumn.pszText = (LPTSTR)"ID";
	lvcolumn.iSubItem = 0;
	lvcolumn.cx = 45;
	m_listctrl.InsertColumn(0, &lvcolumn);

	CRect rect;
	m_listctrl.GetClientRect(rect);

	lvcolumn.fmt = LVCFMT_LEFT;
	lvcolumn.pszText = (LPTSTR)"组名";
	lvcolumn.iSubItem = 1;
	lvcolumn.cx = rect.Width()-45;
	m_listctrl.InsertColumn(1, &lvcolumn);

	try
	{
		CString strSQL;
		strSQL.Format(_T("SELECT Group.ID,Group.组名 FROM UserGroup,Group WHERE UserGroup.UserID = %d and UserGroup.GroupID = Group.ID"), m_dwUser);

		CRecordset rs(m_pDatabase);
		if(rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly)==TRUE)
		{
			while(!rs.IsEOF())
			{
				int nItem = m_listctrl.GetItemCount();
				int index = m_listctrl.InsertItem(nItem, nullptr);

				CString strValue;
				rs.GetFieldValue(_T("ID"), strValue);
				strValue.TrimRight(_T(" "));
				m_listctrl.SetItemText(nItem, 0, strValue);
				m_listctrl.SetItemData(index, _ttoi(strValue));

				rs.GetFieldValue(_T("组名"), strValue);
				strValue.TrimRight(_T(" "));
				m_listctrl.SetItemText(nItem, 1, strValue);

				rs.MoveNext();
			}

			rs.Close();
		}
	}
	catch(CDBException*ex)
	{
		ex->ReportError();
		ex->Delete();
	}
}

void CSelectDlg::OnVehicle()
{
	GetDlgItem(IDC_FIELDCOMBO)->EnableWindow(TRUE);
	GetDlgItem(IDC_OPERATERCOMBO)->EnableWindow(TRUE);
	GetDlgItem(IDC_VALUE)->EnableWindow(TRUE);
	GetDlgItem(IDC_SEARCH)->EnableWindow(TRUE);
	GetDlgItem(IDC_ALL)->EnableWindow(TRUE);

	m_listctrl.DeleteAllItems();

	m_listctrl.DeleteColumn(0);
	m_listctrl.DeleteColumn(0);
	m_listctrl.DeleteColumn(0);

	LV_COLUMN lvcolumn;
	lvcolumn.mask = LVCF_FMT|LVCF_SUBITEM|LVCF_TEXT|LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_CENTER;

	lvcolumn.pszText = (LPTSTR)"ID";
	lvcolumn.iSubItem = 0;
	lvcolumn.cx = 45;
	m_listctrl.InsertColumn(0, &lvcolumn);

	lvcolumn.fmt = LVCFMT_LEFT;
	lvcolumn.pszText = (LPTSTR)"车主";
	lvcolumn.iSubItem = 1;
	lvcolumn.cx = 60;
	m_listctrl.InsertColumn(1, &lvcolumn);

	lvcolumn.fmt = LVCFMT_LEFT;
	lvcolumn.pszText = (LPTSTR)"车牌号";
	lvcolumn.iSubItem = 2;
	lvcolumn.cx = 95;
	m_listctrl.InsertColumn(2, &lvcolumn);

	try
	{
		CString strSQL;
		strSQL.Format(_T("SELECT DISTINCT ID,姓名,车牌号码 FROM View_Vehicle WHERE UserID = %d ORDER BY ID"), m_dwUser);

		CRecordset rs(m_pDatabase);
		if(rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly)==TRUE)
		{
			while(rs.IsEOF()==FALSE)
			{
				const int nItem = m_listctrl.GetItemCount();
				const int index = m_listctrl.InsertItem(nItem, nullptr);

				CString strValue;
				rs.GetFieldValue(_T("ID"), strValue);
				strValue.TrimRight(_T(" "));
				m_listctrl.SetItemText(nItem, 0, strValue);
				m_listctrl.SetItemData(index, _ttoi(strValue));

				rs.GetFieldValue(_T("姓名"), strValue);
				strValue.TrimRight(_T(" "));
				m_listctrl.SetItemText(nItem, 1, strValue);

				rs.GetFieldValue(_T("车牌号码"), strValue);
				strValue.TrimRight(_T(" "));
				m_listctrl.SetItemText(nItem, 2, strValue);

				rs.MoveNext();
			}

			rs.Close();
		}
	}
	catch(CDBException*ex)
	{
		ex->ReportError();
		ex->Delete();
	}

	((CButton*)GetDlgItem(IDC_VEHICLE))->SetCheck(TRUE);
}

void CSelectDlg::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult)
{
	CPoint pt;
	GetCursorPos(&pt);
	m_listctrl.ScreenToClient(&pt);
	const int index = m_listctrl.HitTest(pt);
	if(index<0)
		return;

	::SendMessage(GetParent()->m_hWnd, WM_SELDLG_DBCLICK, index, (LPARAM)&m_listctrl);
}

void CSelectDlg::SelectItemByData(const DWORD& id)
{
	if(id<0)
		return;

	for(int i = 0; i<m_listctrl.GetItemCount(); i++)
	{
		const DWORD dwVal = m_listctrl.GetItemData(i);
		if(dwVal==id)
			m_listctrl.SetItemState(i, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		else
			m_listctrl.SetItemState(i, ~(LVIS_SELECTED|LVIS_FOCUSED), LVIS_SELECTED|LVIS_FOCUSED);
	}
}

void CSelectDlg::OnClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	CPoint pt;
	GetCursorPos(&pt);
	m_listctrl.ScreenToClient(&pt);
	const int index = m_listctrl.HitTest(pt);
	if(index<0)
		return;
	const CString strID = m_listctrl.GetItemText(index, 0);
	::SendMessage(GetParent()->m_hWnd, WM_SELDLG_CLICK, _ttoi(strID), (LPARAM)&m_listctrl);
}

void CSelectDlg::OnRclick(NMHDR* pNMHDR, LRESULT* pResult)
{
	CPoint pt;
	GetCursorPos(&pt);
	m_listctrl.ScreenToClient(&pt);
	const int index = m_listctrl.HitTest(pt);
	if(index<0)
		return;

	::SendMessage(GetParent()->m_hWnd, WM_SELDLG_RCLICK, index, (LPARAM)&m_listctrl);
}
