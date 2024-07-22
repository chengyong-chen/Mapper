// InfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "information.h"
#include "AttributeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAttributeDlg dialog
#define IDC_EDIT_EXTRA 200
#define IDC_STATIC_EXTRA 300

CAttributeDlg::CAttributeDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CAttributeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAttributeDlg)
		
	//}}AFX_DATA_INIT
	m_database = nullptr;
}

void CAttributeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAttributeDlg)
	DDX_Control(pDX, IDC_TYPE, m_TypeCombo);
	DDX_Control(pDX, IDC_LIST, m_ListCtrl);
	//}}AFX_DATA_MAP
/*
	if(!m_pSet->m_listValue.IsEmpty())
	{
		UINT nField=0;
		POSITION posValue = m_pSet->m_listValue.GetHeadPosition();
		while(posValue)
		{
			DDX_FieldText(pDX, IDC_EDIT_EXTRA+nField,m_pSet->m_listValue.GetNext(posValue), m_pSet);
			nField++;
		}
	}*/
}


BEGIN_MESSAGE_MAP(CAttributeDlg, CDialog)
	//{{AFX_MSG_MAP(CAttributeDlg)
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_TYPE, OnSelchangeType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttributeDlg message handlers

BOOL CAttributeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	LV_COLUMN lvcColumn;
	lvcColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvcColumn.fmt = LVCFMT_LEFT;

	lvcColumn.cx = 100;
	lvcColumn.pszText = (LPTSTR)"Field";
	m_ListCtrl.InsertColumn(0, &lvcColumn);

	lvcColumn.cx = 150;
	lvcColumn.pszText = (LPTSTR)"Value";
	m_ListCtrl.InsertColumn(1, &lvcColumn);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CAttributeDlg::OnTableChanged(CDatabase* pDatabase,CString strTable) 
{
	if(pDatabase == nullptr)
		return FALSE;
	if(pDatabase->IsOpen() == FALSE)
		return;

	if(strTable.IsEmpty() == TRUE)
		return FALSE;

	m_database = pDatabase;
	m_strTable = strTable;
	m_ListCtrl.DeleteAllItems();

	CPoint point;
	CRecordset rs(m_database);
	try
	{
		CString strSQL;
		strSQL.Format(_T("SELECT * FROM [%s]"),_T(m_strTable));
		CODBCDatabase::ParseSQL(m_database,strSQL);
	
		rs.Open(CRecordset::snapshot,strSQL,CRecordset::readOnly);	
		int fieldCount = rs.GetODBCFieldCount();
		int nItem = 0;
		for(short nIndex=0; nIndex<fieldCount;nIndex++)
		{
			CODBCFieldInfo fieldinfo; 
			rs.GetODBCFieldInfo(nIndex, fieldinfo);
			
			m_ListCtrl.InsertItem(nItem,fieldinfo.m_strName);
			nItem++;
		}		

		rs.Close();
	}
	catch(CDBException* ex)
	{
		OutputDebugString(ex->m_strError);
		ex->ReportError();
		ex->Delete();
	}
	catch(CException* ex)
	{
		ex->Delete();
	}
	return TRUE;
}

BOOL CAttributeDlg::ShowInfo(DWORD dwMapId,DWORD dwGeomID)
{
	if(m_database == nullptr)
		return FALSE;
	if(m_strTable.IsEmpty() == TRUE)
		return FALSE;

	CRecordset rs(m_database);
	try
	{
		CString strSQL;
		strSQL.Format(_T("SELECT * FROM [%s] WHERE GID=%d"),m_strTable,dwGeomID);
		if(m_strFilter.IsEmpty() == FALSE)
		{
			strSQL += _T( " And ");
			strSQL += m_strFilter;
		}
		CODBCDatabase::ParseSQL(m_database,strSQL);
		
		rs.Open(CRecordset::snapshot,strSQL,CRecordset::readOnly);	
		int fieldCount = rs.GetODBCFieldCount();
		while(!rs.IsEOF()) 
		{				
			for(short nIndex=2; nIndex<fieldCount;nIndex++)
			{
				CString strValue;
				rs.GetFieldValue(nIndex,strValue);
				m_ListCtrl.SetItemText(nIndex-2,1,strValue);			
			}		
			rs.MoveNext();
		}

		rs.Close();
	}
	catch(CDBException* ex)
	{
		OutputDebugString(ex->m_strError);
		ex->ReportError();
		ex->Delete();
		return FALSE;
	}

	return TRUE;
}
void CAttributeDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	if(m_ListCtrl.m_hWnd != nullptr)
	{
		m_ListCtrl.SetWindowPos(&wndTop, 0, 0, cx, cy-30, SWP_NOMOVE | SWP_SHOWWINDOW);
		if(cx>104)
		{
			m_ListCtrl.SetColumnWidth(1,cx-104);
		}
	}	

	CWnd* pWnd = GetDlgItem(IDC_TYPE);
	if(pWnd != nullptr)
	{
		pWnd->SetWindowPos(&wndTop, 0,0,cx,20, SWP_NOMOVE|SWP_SHOWWINDOW);
	}
}

void CAttributeDlg::OnSelchangeType() 
{
	// TODO: Add your control notification handler code here
	
}
