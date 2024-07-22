// FieldDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FieldDlg.h"
#include "Crack.h"

#include "../Public/ODBCDatabase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

long CFieldDlg::fieldtype_size[] = {
	dbBoolean, 1,
	dbByte, 1,
	dbInteger, 2,
	dbLong, 4,
	//	dbCurrency,	8,
	dbSingle, 4,
	dbDouble, 8,
	dbDate, 8,
	dbText, 255,
	//	dbLongBinary,	0,
	//	dbMemo,	0,
	//	dbGUID,	16
};

// CFieldDlg �Ի���

IMPLEMENT_DYNAMIC(CFieldDlg, CDialog)

CFieldDlg::CFieldDlg(CDatabase* pDatabase, LPCTSTR strTable, CWnd* pParent /*=nullptr*/)
	: CDialog(CFieldDlg::IDD, pParent)
{
	m_pDatabase = pDatabase;
	m_strTable = strTable;
	m_bModified = false;
}

CFieldDlg::~CFieldDlg()
{
}

void CFieldDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FIELDLIST, m_fieldlist);
}

BEGIN_MESSAGE_MAP(CFieldDlg, CDialog)
	ON_BN_CLICKED(IDC_NEW, OnNew)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_MOVEUP, OnMoveup)
	ON_BN_CLICKED(IDC_MOVEDOWN, OnMovedown)
	ON_NOTIFY(LVN_DELETEITEM, IDC_FIELDLIST, OnDeleteitemFieldlist)
	ON_NOTIFY(HDN_ITEMCHANGED, 0, OnItemchangedFieldlist)
END_MESSAGE_MAP()

// CFieldDlg ��Ϣ�������

BOOL CFieldDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect rect;
	m_fieldlist.GetClientRect(rect);

	LV_COLUMN lvcolumn;
	lvcolumn.mask = LVCF_FMT|LVCF_SUBITEM|LVCF_TEXT|LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;

	lvcolumn.pszText = (LPTSTR)"Name";
	lvcolumn.iSubItem = 0;
	lvcolumn.cx = rect.Width()/3;
	m_fieldlist.InsertColumn(0, &lvcolumn);

	lvcolumn.pszText = (LPTSTR)"Type";
	lvcolumn.iSubItem = 1;
	lvcolumn.cx = rect.Width()/3;
	m_fieldlist.InsertColumn(1, &lvcolumn);

	lvcolumn.pszText = (LPTSTR)"Description";
	lvcolumn.iSubItem = 2;
	lvcolumn.cx = rect.Width()/3;
	m_fieldlist.InsertColumn(2, &lvcolumn);

	//m_nList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	for(int I = 0; I<sizeof(fieldtype_size)/sizeof(long)/2; I++)
	{
		((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString(CCrack::strFieldType((short)fieldtype_size[I*2]));
	}

	if(m_strTable.IsEmpty()==FALSE)
	{
		OnTableChanged(m_strTable);
	}
	else
	{
		/*	GetDlgItem(IDC_NEW)->EnableWindow(FALSE);
			GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);
			GetDlgItem(IDC_MOVEUP)->EnableWindow(FALSE);
			GetDlgItem(IDC_MOVEDOWN)->EnableWindow(FALSE);

			GetDlgItem(IDC_SIZE)->EnableWindow(FALSE);
			GetDlgItem(IDC_DEFAULT)->EnableWindow(FALSE);
			GetDlgItem(IDC_COMBO1)->EnableWindow(FALSE);
			GetDlgItem(IDC_COMBO2)->EnableWindow(FALSE);*/
	}
	return TRUE; // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CFieldDlg::OnTableChanged(CString strTable)
{
	m_fieldlist.DeleteAllItems();

	if(m_pDatabase==nullptr)
		return;

	if(m_pDatabase->IsOpen()==FALSE)
		return;

	m_bModified = false;

	CRecordset rs(m_pDatabase);
	try
	{
		CString strSQL;
		strSQL.Format(_T("SELECT * FROM [%s]"), strTable);
		CODBCDatabase::ParseSQL(m_pDatabase, strSQL);

		rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly);
		for(short nIndex = 0; nIndex<rs.GetODBCFieldCount(); nIndex++)
		{
			CODBCFieldInfo* pFieldInfo = new CODBCFieldInfo;
			rs.GetODBCFieldInfo(nIndex, *pFieldInfo);

			TCHAR stext[126];
			LV_ITEM lvitem;
			lvitem.mask = LVIF_TEXT;
			lvitem.iItem = nIndex;

			lvitem.iSubItem = 0;
			wsprintf(stext, pFieldInfo->m_strName);
			lvitem.pszText = stext;
			int Item = m_fieldlist.InsertItem(&lvitem);
			m_fieldlist.SetItemData(Item, (DWORD)pFieldInfo);

			lvitem.iSubItem = 1;
			//			wsprintf(stext,CCrack::strFieldType(pFieldInfo->m_nType));
			lvitem.pszText = stext;
			m_fieldlist.SetItem(&lvitem);
		}

		rs.Close();

		m_strTable = strTable;
	}
	catch(CDaoException*ex)
	{
		//		OutputDebugString(ex->m_strError);
		ex->ReportError();
		ex->Delete();
	}
	catch(CException*ex)
	{
		ex->Delete();
	}
}

void CFieldDlg::OnNew()
{
	CDaoFieldInfo* pFieldInfo = new CDaoFieldInfo;
	pFieldInfo->m_strName = _T("New");
	pFieldInfo->m_lSize = 50;
	pFieldInfo->m_bAllowZeroLength = TRUE;
	pFieldInfo->m_nType = dbText;
	pFieldInfo->m_lAttributes = dbVariableField;

	int nItem = m_fieldlist.GetItemCount();
	nItem = m_fieldlist.InsertItem(nItem, nullptr);
	m_fieldlist.SetItemData(nItem, (DWORD)pFieldInfo);

	m_fieldlist.SetItemText(nItem, 0, pFieldInfo->m_strName);

	m_bModified = true;

	//	m_fieldlist.SetItemText(nItem,1,strType);			
	//	m_fieldlist.SetItemText(nItem,2,strOwner);			
}

void CFieldDlg::OnDelete()
{
	POSITION pos = m_fieldlist.GetFirstSelectedItemPosition();
	while(pos!=nullptr)
	{
		const int nItem = m_fieldlist.GetNextSelectedItem(pos);
		m_fieldlist.DeleteItem(nItem);
	}
	m_fieldlist.SetFocus();

	m_bModified = true;
}

void CFieldDlg::OnMoveup()
{
	m_bModified = true;
}

void CFieldDlg::OnMovedown()
{
	m_bModified = true;
}

void CFieldDlg::OnDeleteitemFieldlist(NMHDR* pNMHDR, LRESULT* pResult)
{
	const LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	const CDaoFieldInfo* pFieldInfo = (CDaoFieldInfo*)pNMLV->lParam;
	delete pFieldInfo;
	pFieldInfo = nullptr;

	*pResult = 0;
}

void CFieldDlg::OnItemchangedFieldlist(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	const long hItem = phdr->iItem;
	CDaoFieldInfo* pFieldInfo = (CDaoFieldInfo*)m_fieldlist.GetItemData(hItem);
	if(pFieldInfo!=nullptr)
	{
		GetDlgItem(IDC_NEW)->EnableWindow(FALSE);
		GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);
		GetDlgItem(IDC_MOVEUP)->EnableWindow(FALSE);
		GetDlgItem(IDC_MOVEDOWN)->EnableWindow(FALSE);

		GetDlgItem(IDC_SIZE)->EnableWindow(FALSE);
		GetDlgItem(IDC_DEFAULT)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO1)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO2)->EnableWindow(FALSE);
	}

	*pResult = 0;
}/*
LPCTSTR CCrack::strFieldType(short sType)
{
	switch(sType)
	{

case SQL_LONGVARCHAR:
case SQL_CHAR:
case SQL_VARCHAR:
	break;
case SQL_FLOAT:
case SQL_REAL:
case SQL_DOUBLE:
	break;
case SQL_DECIMAL:
case SQL_NUMERIC:
	break;
case SQL_TIMESTAMP:
case SQL_DATE:
case SQL_TIME:
	break;
case SQL_TINYINT:
case SQL_SMALLINT:
case SQL_INTEGER:
case SQL_BIGINT:
	break;

		case (dbBoolean):
			return _T("Bool");
		case (dbByte):
			return _T("Byte");
		case (dbInteger):
			return _T("Integer");
		case (dbLong):
			return _T("Long");
		case (dbCurrency):
			return _T("Currency");
		case (dbSingle):
			return _T("Single");
		case (dbDouble):
			return _T("Double");
		case (dbDate):
			return _T("Date");
		case (dbText):
			return _T("Text");
		case (dbLongBinary):
			return _T("Long Binary");
		case (dbMemo):
			return _T("Memo");
		case (dbGUID):
			return _T("GUID");
	}
	return _T("Unknown");
}
*/
