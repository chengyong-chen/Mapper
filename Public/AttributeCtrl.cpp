#include "stdafx.h"
#include "AttributeCtrl.h"

#include  <cstdio>

#include "../Public/ODBCDatabase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAttributeCtrl dialog
CAttributeCtrl::CAttributeCtrl()
{
	m_database = nullptr;
}

CAttributeCtrl::~CAttributeCtrl()
{
}

BEGIN_MESSAGE_MAP(CAttributeCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CAttributeCtrl)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttributeCtrl message handlers
int CAttributeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CListCtrl::OnCreate(lpCreateStruct)==-1)
		return -1;

	LV_COLUMN lvcColumn;
	lvcColumn.mask = LVCF_FMT|LVCF_TEXT|LVCF_WIDTH;
	lvcColumn.fmt = LVCFMT_LEFT;

	lvcColumn.cx = 100;
	lvcColumn.pszText = (LPTSTR)_T("Field");
	CListCtrl::InsertColumn(0, &lvcColumn);

	lvcColumn.cx = 150;
	lvcColumn.pszText = (LPTSTR)_T("Value");
	CListCtrl::InsertColumn(1, &lvcColumn);

	CListCtrl::SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	//	CListCtrl::SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CAttributeCtrl::Create(CWnd* pParent)
{
	DWORD dwStyle = LVS_REPORT|LVS_ALIGNLEFT|LVS_NOLABELWRAP|LVS_SHOWSELALWAYS|LVS_SINGLESEL|WS_CHILD|WS_VISIBLE|LVS_EX_FULLROWSELECT;
	dwStyle &= ~LVS_SORTASCENDING;
	dwStyle &= ~LVS_SORTDESCENDING;
	dwStyle &= ~LVS_EDITLABELS;

	CListCtrl::EnableToolTips(FALSE);
	return CListCtrl::Create(dwStyle, CRect(0, 0, 0, 0), pParent, 0);
}

BOOL CAttributeCtrl::OnTableChanged(CDatabase* pDatabase, CString strTable)
{
	if(pDatabase==nullptr)
		return FALSE;
	if(pDatabase->IsOpen()==FALSE)
		return FALSE;

	if(strTable.IsEmpty()==TRUE)
		return FALSE;

	if(m_database==pDatabase&&m_strTable==strTable)
		return FALSE;

	m_database = pDatabase;
	m_strTable = strTable;
	CListCtrl::DeleteAllItems();

	CPoint point;

	CRecordset rs(m_database);
	try
	{
		CString strSQL;
		strSQL.Format(_T("SELECT * FROM [%s]"), m_strTable);
		CODBCDatabase::ParseSQL(m_database, strSQL);

		rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly);
		const int fieldCount = rs.GetODBCFieldCount();
		for(short nIndex = 0; nIndex<fieldCount; nIndex++)
		{
			CODBCFieldInfo fieldinfo;
			rs.GetODBCFieldInfo(nIndex, fieldinfo);

			CListCtrl::InsertItem(nIndex, fieldinfo.m_strName);
		}
		rs.Close();
	}
	catch(CDBException*ex)
	{
		OutputDebugString(ex->m_strError);
		ex->ReportError();
		ex->Delete();
	}

	return TRUE;
}

BOOL CAttributeCtrl::ShowInfo(CString strIdField, const DWORD& dwId)
{
	if(m_database==nullptr)
		return FALSE;
	if(m_strTable.IsEmpty()==TRUE)
		return FALSE;

	CRecordset rs(m_database);
	try
	{
		CString strSQL;
		strSQL.Format(_T("SELECT * FROM [%s] WHERE %s=%d"), m_strTable, strIdField, dwId);
		CODBCDatabase::ParseSQL(m_database, strSQL);

		rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly);
		const int fieldCount = rs.GetODBCFieldCount();
		if(rs.IsEOF()==FALSE)
		{
			for(short nIndex = 0; nIndex<fieldCount; nIndex++)
			{
				CODBCFieldInfo fieldinfo;
				rs.GetODBCFieldInfo(nIndex, fieldinfo);

				if(fieldinfo.m_nSQLType==SQL_DATE||fieldinfo.m_nSQLType==SQL_TIME||fieldinfo.m_nSQLType==SQL_TIMESTAMP)
				{
				}
				else
				{
					CString strValue;
					rs.GetFieldValue(nIndex, strValue);
					CListCtrl::SetItemText(nIndex, 1, strValue);
				}
			}
		}

		rs.Close();
	}
	catch(CDBException*ex)
	{
		OutputDebugString(ex->m_strError);
		ex->ReportError();
		ex->Delete();
	}
	return TRUE;
}
