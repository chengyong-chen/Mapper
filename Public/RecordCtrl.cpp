#include "stdafx.h"
#include "resource.h"

#include "RecordCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CRecordCtrl::CRecordCtrl()
{
	m_pDatabase = nullptr;
	m_nFieldCount = 0;
	m_pFieldInfor = nullptr;
}

CRecordCtrl::~CRecordCtrl()
{
	delete[] m_pFieldInfor;
}

BEGIN_MESSAGE_MAP(CRecordCtrl, CListCtrlEx)
	//{{AFX_MSG_MAP(CRecordCtrl)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecordCtrl message handlers
int CRecordCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CListCtrlEx::OnCreate(lpCreateStruct);

	CString columName1;
	CString columName2;
	columName1.LoadString(IDS_FIELD);
	columName2.LoadString(IDS_VALUE);

	LV_COLUMN lvcColumn;
	lvcColumn.mask = LVCF_FMT|LVCF_TEXT|LVCF_WIDTH;
	lvcColumn.fmt = LVCFMT_LEFT;

	lvcColumn.cx = 100;
	lvcColumn.pszText = columName1.GetBuffer(0);
	CListCtrlEx::InsertColumn(0, &lvcColumn);

	lvcColumn.cx = 150;
	lvcColumn.pszText = columName2.GetBuffer(0);
	CListCtrlEx::InsertColumn(1, &lvcColumn);

	CListCtrlEx::SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	columName1.ReleaseBuffer();
	columName2.ReleaseBuffer();
	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CRecordCtrl::Create(CWnd* pParent)
{
	DWORD dwStyle = LVS_REPORT|LVS_ALIGNLEFT|LVS_NOLABELWRAP|LVS_SHOWSELALWAYS|LVS_SINGLESEL|WS_CHILD|WS_VISIBLE|LVS_EX_FULLROWSELECT;
	dwStyle &= ~LVS_SORTASCENDING;
	dwStyle &= ~LVS_SORTDESCENDING;
	dwStyle &= ~LVS_EDITLABELS;

	CListCtrl::EnableToolTips(FALSE);
	return CListCtrl::Create(dwStyle, CRect(0, 0, 0, 0), pParent, 0);
}

void CRecordCtrl::ActivateRec(CDatabase* pDatabase, CString strTable, CString strIdField, const DWORD& dwId)
{
	if(pDatabase==nullptr)
		return;
	if(strTable.IsEmpty()==TRUE)
		return;
	if(dwId==-1)
		return;

	DisposeRec();

	CString strSQL;
	strSQL.Format(_T("Select * From %s where %s=%d"), strTable, strIdField, dwId);

	if(pDatabase!=m_pDatabase||strTable!=m_strTable)
	{
		delete[] m_pFieldInfor;
		m_pFieldInfor = nullptr;

		CListCtrlEx::DeleteAllItems();

		try
		{
			_variant_t varRowsAff;
			m_pDatabase = pDatabase;
			m_strTable = strTable;
		}
		catch(CDBException* ex)
		{
			//	AfxMessageBox(ex->m_strError);
			OutputDebugString(ex->m_strError);
			ex->Delete();
		}
	}

	try
	{
		_variant_t varRowsAff;
		m_dwId = dwId;
	}
	catch(CDBException* ex)
	{
		//	AfxMessageBox(ex->m_strError);
		OutputDebugString(ex->m_strError);
		ex->Delete();
	}
}

void CRecordCtrl::DisposeRec() const
{
	if(m_pDatabase==nullptr)
		return;
	if(m_strTable.IsEmpty()==TRUE)
		return;
	if(m_dwId==-1)
		return;

	CString strFields;
	CString strValues;
	int nItem = 0;
	for(short nIndex = 0; nIndex<m_nFieldCount; nIndex++)
	{
		if(m_pFieldInfor[nIndex].strName=="ID")
		{
			continue;
		}

		CString strValue = CListCtrlEx::GetItemText(nItem++, 1);
		if(strValue==m_pFieldInfor[nIndex].strValue)
			continue;

		switch(m_pFieldInfor[nIndex].nType)
		{
			case ADOCG::adBigInt:
			case ADOCG::adDecimal:
			case ADOCG::adDouble:
			case ADOCG::adGUID:
			case ADOCG::adSingle:
			case ADOCG::adSmallInt:
			case ADOCG::adTinyInt:
			case ADOCG::adUnsignedBigInt:
			case ADOCG::adUnsignedInt:
			case ADOCG::adUnsignedSmallInt:
			case ADOCG::adUnsignedTinyInt:
			case ADOCG::adInteger:
			case ADOCG::adNumeric:
			case ADOCG::adVarNumeric:
			case ADOCG::adBoolean:
			case ADOCG::adCurrency:
			{
				strFields += (m_pFieldInfor[nIndex].strName+_T(','));
				strValues += strValue+_T(',');
			}
			break;
			case ADOCG::adBinary:
			case ADOCG::adVarBinary:
			case ADOCG::adLongVarBinary:
			{
			}
			break;
			case ADOCG::adBSTR:
			case ADOCG::adChapter:
			case ADOCG::adChar:
			case ADOCG::adDate:
			case ADOCG::adDBDate:
			case ADOCG::adDBTime:
			case ADOCG::adDBTimeStamp:
			case ADOCG::adEmpty:
			case ADOCG::adError:
			case ADOCG::adFileTime:
			case ADOCG::adIDispatch:
			case ADOCG::adIUnknown:
			case ADOCG::adLongVarChar:
			case ADOCG::adLongVarWChar:
			case ADOCG::adPropVariant:
			case ADOCG::adUserDefined:
			case ADOCG::adVarChar:
			case ADOCG::adVariant:
			case ADOCG::adVarWChar:
			case ADOCG::adWChar:
			{
				strFields += (m_pFieldInfor[nIndex].strName+_T(','));
				strValues += (_T('\'')+strValue+_T("\',"));
			}
			break;
			default:
				break;
		}

		if(strValues.IsEmpty()==FALSE)
		{
			strValues = strValues.Left(strValues.GetLength()-1);
		}
		if(strFields.IsEmpty()==FALSE)
		{
			strFields = strFields.Left(strFields.GetLength()-1);
		}

		if(strValues.IsEmpty()==FALSE&&strFields.IsEmpty()==FALSE)
		{
			CString strSQL;
			strSQL.Format(_T("Update %s (%s) Values(%s) where ID=%d"), m_strTable, strFields, strValues, m_dwId);

			_variant_t varRowsAff;
			/*m_pDatabase->Execute(LPCTSTR(strSQL), &varRowsAff, ADOCG::adCmdText);*/
		}
	}
}
