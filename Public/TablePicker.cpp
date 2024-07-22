#include "stdafx.h"
#include "TablePicker.h"
#include "Tableset.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPickTableDlg dialog

CPickTableDlg::CPickTableDlg(CWnd* pParent, CDatabase* database, LPCTSTR lpTable)
	: CDialog(CPickTableDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPickTableDlg)
	//}}AFX_DATA_INIT
	m_database = database;
	m_strTable = lpTable;
}

void CPickTableDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPickTableDlg)
	DDX_Control(pDX, IDC_TABLELIST, m_ctrlTable);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPickTableDlg, CDialog)
	//{{AFX_MSG_MAP(CPickTableDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPickTableDlg message handlers

BOOL CPickTableDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if(m_database==nullptr||m_database->IsOpen()==FALSE)
		return FALSE;

	LV_COLUMN lvcColumn;
	lvcColumn.mask = LVCF_FMT|LVCF_SUBITEM|LVCF_TEXT|LVCF_WIDTH;
	lvcColumn.fmt = LVCFMT_LEFT;

	CRect rect;
	m_ctrlTable.GetClientRect(&rect);
	const CSize size = rect.Size();
	const int nWidth = size.cx/3;
	lvcColumn.cx = nWidth;
	lvcColumn.pszText = (LPTSTR)"Name";
	lvcColumn.iSubItem = 0;
	m_ctrlTable.InsertColumn(0, &lvcColumn);

	lvcColumn.cx = nWidth;
	lvcColumn.pszText = (LPTSTR)"Type";
	lvcColumn.iSubItem = 1;
	m_ctrlTable.InsertColumn(1, &lvcColumn);

	lvcColumn.cx = nWidth;
	lvcColumn.pszText = (LPTSTR)"Owner";
	lvcColumn.iSubItem = 2;
	m_ctrlTable.InsertColumn(2, &lvcColumn);

	//m_nList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	try
	{
		int sel = -1;

		CTableset set(m_database);//CRecordset::OpenType::snapshot
		if(set.Open(nullptr, nullptr, nullptr, "'TABLE'", CRecordset::OpenType::snapshot)==TRUE)
		{
			while(set.IsEOF()==FALSE)
			{
				CString strTable = set.m_strTableName;
				CString strType = set.m_strTableType;
				CString strOwner = set.m_strTableOwner;
				set.MoveNext();

				if(strOwner==_T("sys"))
					continue;
				else
				{
					int nItem = m_ctrlTable.GetItemCount();
					nItem = m_ctrlTable.InsertItem(nItem, nullptr);
					m_ctrlTable.SetItemText(nItem, 0, strTable);
					m_ctrlTable.SetItemText(nItem, 1, strType);
					m_ctrlTable.SetItemText(nItem, 2, strOwner);

					if(strTable==m_strTable)
						sel = nItem;
				}
			}

			set.Close();
		}
		//CRecordset::OpenType::snapshot
		if(set.Open(nullptr, nullptr, nullptr, "'VIEW'", CRecordset::forwardOnly)==TRUE)
		{
			int sel = -1;
			while(set.IsEOF()==FALSE)
			{
				CString strTable = set.m_strTableName;
				CString strType = set.m_strTableType;
				CString strOwner = set.m_strTableOwner;
				set.MoveNext();
				if(strOwner==_T("sys"))
					continue;
				else if(strOwner==_T("INFORMATION_SCHEMA"))
					continue;
				else
				{
					int nItem;
					nItem = m_ctrlTable.GetItemCount();
					nItem = m_ctrlTable.InsertItem(nItem, nullptr);
					m_ctrlTable.SetItemText(nItem, 0, strTable);
					m_ctrlTable.SetItemText(nItem, 1, strType);
					m_ctrlTable.SetItemText(nItem, 2, strOwner);

					if(strTable==m_strTable)
						sel = nItem;
				}
			}
			set.Close();
		}
		if(sel!=-1)
		{
			m_ctrlTable.SetFocus();
			m_ctrlTable.SetItemState(sel, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		}
	}
	catch(CDBException*ex)
	{
		OutputDebugString(ex->m_strError);
		ex->ReportError();
		ex->Delete();
	}
	return TRUE;
}

void CPickTableDlg::OnOK()
{
	const int nItem = m_ctrlTable.GetNextItem(-1, LVNI_ALL|LVNI_SELECTED);
	m_strTable = m_ctrlTable.GetItemText(nItem, 0);
	CDialog::OnOK();
}
