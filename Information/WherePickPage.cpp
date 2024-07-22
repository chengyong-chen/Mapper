#include "stdafx.h"
#include "WherePickPage.h"

#include "../Public/Fieldset.h"
#include "../Public/CellList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class CEdit;

/////////////////////////////////////////////////////////////////////////////
// CWherePickPage dialog
IMPLEMENT_DYNCREATE(CWherePickPage, CPropertyPage)

CWherePickPage::CWherePickPage()
	: CPropertyPage(CWherePickPage::IDD)
{
	//{{AFX_DATA_INIT(CWherePickPage)
	//}}AFX_DATA_INIT
}

void CWherePickPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWherePickPage)
	DDX_Control(pDX, IDC_LIST, m_ListCtrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWherePickPage, CPropertyPage)
	//{{AFX_MSG_MAP(CWherePickPage)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT_EX(LVN_BEGINLABELEDIT, OnBeginLabeledit)
	ON_NOTIFY_REFLECT_EX(LVN_ENDLABELEDIT, OnEndInPlaceEdit)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWherePickPage message handlers

/////////////////////////////////////////////////////////////////////////////
// CWherePickPage message handlers
BOOL CWherePickPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	//	DWORD dwStyle = LVS_REPORT | LVS_ALIGNLEFT | LVS_NOLABELWRAP | LVS_SHOWSELALWAYS | LVS_SINGLESEL | WS_CHILD | WS_VISIBLE | LVS_EX_FULLROWSELECT; 
	//	dwStyle &= ~LVS_SORTASCENDING;
	//	dwStyle &= ~LVS_SORTDESCENDING;
	//	dwStyle &= ~LVS_EDITLABELS;

	//	m_ListCtrl.ModifyStyle(dwStyle);
	CHeaderCtrl* pHeaderCtrl = (CHeaderCtrl*)m_ListCtrl.GetHeaderCtrl();
	if(pHeaderCtrl!=nullptr)
	{
		pHeaderCtrl->SetBitmapMargin(0);
		pHeaderCtrl->EnableWindow(FALSE);
	}
	m_ListCtrl.m_bEditble = true;

	LV_COLUMN lvcColumn;
	lvcColumn.mask = LVCF_FMT|LVCF_TEXT|LVCF_WIDTH;
	lvcColumn.fmt = LVCFMT_LEFT;

	lvcColumn.cx = 60;
	lvcColumn.pszText = (LPTSTR)"Logic";
	m_ListCtrl.InsertColumn(0, &lvcColumn);

	lvcColumn.cx = 90;
	lvcColumn.pszText = (LPTSTR)"Field";
	m_ListCtrl.InsertColumn(1, &lvcColumn);

	lvcColumn.cx = 60;
	lvcColumn.pszText = (LPTSTR)"Operation";
	m_ListCtrl.InsertColumn(2, &lvcColumn);

	lvcColumn.cx = 105;
	lvcColumn.pszText = (LPTSTR)"Value";
	m_ListCtrl.InsertColumn(3, &lvcColumn);

	//m_nList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CWherePickPage::OnSetActive()
{
	if(m_pDatabase!=nullptr&&m_strTable.IsEmpty()==FALSE&&m_strWhere!=nullptr)
	{
		m_ListCtrl.EnableWindow(TRUE);
		m_ListCtrl.DeleteAllItems();

		CString strWhere = *m_strWhere;
		strWhere.MakeUpper();
		CString strLogic;
		while(strWhere.IsEmpty()==FALSE)
		{
			const int nItem = m_ListCtrl.GetItemCount();
			const int index = m_ListCtrl.InsertItem(nItem, nullptr);
			m_ListCtrl.SetItemText(index, 0, strLogic);

			CString string;
			if(strWhere.Find(_T(" AND "))!=-1)
			{
				const int pos = strWhere.Find(_T(" AND "));
				string = strWhere.Left(pos);
				strWhere = strWhere.Mid(pos+5);
				strLogic = _T("并且");
			}
			else if(strWhere.Find(_T(" OR "))!=-1)
			{
				const int pos = strWhere.Find(_T(" OR "));
				string = strWhere.Left(pos);
				strWhere = strWhere.Mid(pos+4);

				strLogic = _T("或者");
			}
			else
			{
				string = strWhere;
				strWhere.Empty();
			}
			string.TrimLeft();
			const int pos1 = string.Find(_T("["));
			const int pos2 = string.Find(_T("]"));
			if(pos1!=-1&&pos2!=-1)
			{
				CString strField = string.Mid(pos1+1, pos2-pos1-1);
				m_ListCtrl.SetItemText(index, 1, strField);
				string = string.Mid(pos2+1);
			}
			else
			{
				const int pos = string.FindOneOf(_T(" <>="));
				if(pos!=-1)
				{
					CString strField = string.Left(pos);
					m_ListCtrl.SetItemText(index, 1, strField);
					string = string.Mid(pos);
				}
			}
			string.TrimLeft();

			CString strValue;
			if(string.Find(_T("<>"))!=-1)
			{
				strValue = string.Mid(2);
				strValue.TrimLeft();
				m_ListCtrl.SetItemText(index, 2, _T("不等于"));
			}
			else if(string.Find(_T(">="))!=-1)
			{
				strValue = string.Mid(2);
				strValue.TrimLeft();
				m_ListCtrl.SetItemText(index, 2, _T("大于等于"));
			}
			else if(string.Find(_T("<="))!=-1)
			{
				strValue = string.Mid(2);
				strValue.TrimLeft();
				m_ListCtrl.SetItemText(index, 2, _T("小于等于"));
			}
			else if(string.Find(_T(">"))!=-1)
			{
				strValue = string.Mid(1);
				strValue.TrimLeft();
				m_ListCtrl.SetItemText(index, 2, _T("大于"));
			}
			else if(string.Find(_T("<"))!=-1)
			{
				strValue = string.Mid(1);
				strValue.TrimLeft();
				m_ListCtrl.SetItemText(index, 2, _T("小于"));
			}
			else if(string.Find(_T("="))!=-1)
			{
				strValue = string.Mid(1);
				strValue.TrimLeft();
				if(strValue.Left(1)==_T("'")&&strValue.Right(1)==_T("'"))
				{
					strValue = strValue.Mid(1);
					strValue = strValue.Left(strValue.GetLength()-1);
				}
				m_ListCtrl.SetItemText(index, 2, _T("等于"));
			}
			else if(string.Find(_T("LIKE "))!=-1)
			{
				strValue = string.Mid(5);
				strValue.TrimLeft();
				strValue = strValue.Mid(1);
				strValue = strValue.Left(strValue.GetLength()-1);
				if(strValue.Left(1)==_T("%")&&strValue.Right(1)==_T("%"))
				{
					strValue = strValue.Mid(1);
					strValue = strValue.Left(strValue.GetLength()-1);
					m_ListCtrl.SetItemText(index, 2, _T("包含"));
				}
				else
				{
					m_ListCtrl.SetItemText(index, 2, _T("等于"));
				}
			}

			m_ListCtrl.SetItemText(index, 3, strValue);
		}
	}
	else
	{
		m_ListCtrl.EnableWindow(FALSE);
	}

	return CPropertyPage::OnSetActive();
}

BOOL CWherePickPage::OnKillActive()
{
	if(m_pDatabase!=nullptr&&m_strTable.IsEmpty()==FALSE&&m_strWhere!=nullptr)
	{
		CString strWhere;
		const int nCount = m_ListCtrl.GetItemCount();
		for(int index = 0; index<nCount; index++)
		{
			CString strLogic = m_ListCtrl.GetItemText(index, 0);
			CString strField = m_ListCtrl.GetItemText(index, 1);
			CString strOperator = m_ListCtrl.GetItemText(index, 2);
			CString strValue = m_ListCtrl.GetItemText(index, 3);

			CString string;
			if(index!=0)
			{
				if(strLogic==_T("并且"))
					string += _T(" And ");
				else if(strLogic==_T("或者"))
					string += _T(" Or ");
			}

			string += _T("[")+strField+_T("]");

			CFieldset rs(m_pDatabase);
			if(rs.Open(nullptr, nullptr, m_strTable, nullptr, CRecordset::OpenType::snapshot)==TRUE)
			{
				const int nDateType = rs.GetFieldDateType(strField);
				switch(nDateType)
				{
				case 0:
					if(strOperator==_T("包含"))
					{
						string += _T(" Like '%")+strValue+_T("%'");
					}
					else if(strOperator==_T("等于"))
					{
						string += _T(" Like '")+strValue+_T("'");
					}
					strWhere += string;
					break;
				case 1:
					if(strOperator==_T("等于"))
					{
						string += _T("=")+strValue;
					}
					else if(strOperator==_T("不等于"))
					{
						string += _T("<>")+strValue;
					}
					else if(strOperator==_T("大于"))
					{
						string += _T(">")+strValue;
					}
					else if(strOperator==_T("大于等于"))
					{
						string += _T(">=")+strValue;
					}
					else if(strOperator==_T("小于"))
					{
						string += _T("<")+strValue;
					}
					else if(strOperator==_T("小于等于"))
					{
						string += _T("<=")+strValue;
					}
					strWhere += string;
					break;
				}

				rs.Close();
			}
		}

		if(m_strWhere!=nullptr)
		{
			*m_strWhere = strWhere;
		}
	}

	return CPropertyPage::OnKillActive();
}

void CWherePickPage::OnAdd()
{
	const int nItem = m_ListCtrl.GetItemCount();
	const int index = m_ListCtrl.InsertItem(nItem, nullptr);
	m_ListCtrl.SetItemText(index, 1, nullptr);
	m_ListCtrl.SetItemText(index, 2, nullptr);
	m_ListCtrl.SetItemText(index, 3, nullptr);
	if(nItem>0)
	{
		m_ListCtrl.SetItemText(index, 0, _T("并且"));
	}

	m_ListCtrl.SetFocus();
}

void CWherePickPage::OnDelete()
{
	POSITION pos = m_ListCtrl.GetFirstSelectedItemPosition();
	while(pos!=nullptr)
	{
		const int nItem = m_ListCtrl.GetNextSelectedItem(pos);
		m_ListCtrl.DeleteItem(nItem);
	}
	m_ListCtrl.SetFocus();
}

BOOL CWherePickPage::OnBeginLabeledit(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* plvDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM* plvItem = &plvDispInfo->item;
	const int nItem = plvItem->iItem;
	const int nCol = plvItem->iSubItem;

	if(nItem==-1)
		return FALSE;
	else if(nItem==0&&nCol==0)
		return FALSE;
	const CString textItem = m_ListCtrl.GetItemText(nItem, nCol);
	CRect rect;
	m_ListCtrl.GetItemRect(nItem, rect, LVIR_BOUNDS);

	for(long i = 0; i<nCol; i++)
	{
		rect.left += m_ListCtrl.GetColumnWidth(i);
	}
	rect.right = rect.left+m_ListCtrl.GetColumnWidth(nCol);
	const int nHeight = rect.Height();
	CCellList* pCellList = nullptr;
	if(nCol<3)
	{
		CCellList* CellList = new CCellList();
		pCellList->m_nRow = nItem;
		pCellList->m_nCol = nCol;
		if(pCellList->Create((WS_BORDER|WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST|CBS_AUTOHSCROLL)&(~LBS_SORT), rect, &m_ListCtrl, IDC_IPLIST)==TRUE)
		{
			pCellList = CellList;
		}
		else
		{
			delete CellList;
			CellList = nullptr;
			return FALSE;
		}
	}

	switch(nCol)
	{
	case 0:
	{
		pCellList->AddString(_T("并且"));
		pCellList->AddString(_T("或者"));

		rect.bottom = rect.bottom+2*nHeight+::GetSystemMetrics(SM_CYHSCROLL);
	}
	break;
	case 1:
		if(m_pDatabase!=nullptr&&m_pDatabase->IsOpen()==TRUE&&m_strTable.IsEmpty()==FALSE)
		{
			CFieldset rs(m_pDatabase);
			if(rs.Open(nullptr, nullptr, m_strTable, nullptr, CRecordset::OpenType::snapshot))
			{
				int nCount = 0;
				while(!rs.IsEOF())
				{
					CString strColumn = rs.m_strColumnName;
					const int nPos = strColumn.Find(_T('.'));
					if(nPos==-1)
					{
						pCellList->AddString(strColumn);
					}
					else
					{
						strColumn = strColumn.Left(nPos);
						pCellList->AddString(strColumn);
						strColumn = rs.m_strColumnName;
						strColumn = strColumn.Mid(nPos+1);
						pCellList->AddString(strColumn);
					}

					nCount++;
					rs.MoveNext();
				}

				rs.Close();
			}

			rect.bottom = rect.bottom+8*nHeight+::GetSystemMetrics(SM_CYHSCROLL);
		}
		break;
	case 2:
	{
		const CString strField = m_ListCtrl.GetItemText(nItem, 1);
		if(strField.IsEmpty()==FALSE)
		{
			CFieldset rs(m_pDatabase);
			if(rs.Open(nullptr, nullptr, m_strTable, nullptr, CRecordset::OpenType::snapshot)==TRUE)
			{
				const int nDateType = rs.GetFieldDateType(strField);
				switch(nDateType)
				{
				case 0:
				{
					pCellList->AddString(_T("包含"));
					pCellList->AddString(_T("等于"));
					rect.bottom = rect.bottom+2*nHeight+::GetSystemMetrics(SM_CYHSCROLL);
				}
				break;
				case 1:
				{
					pCellList->AddString(_T("等于"));
					pCellList->AddString(_T("大于"));
					pCellList->AddString(_T("大于等于"));
					pCellList->AddString(_T("小于"));
					pCellList->AddString(_T("小于等于"));
					pCellList->AddString(_T("不等于"));
					rect.bottom = rect.bottom+6*nHeight+::GetSystemMetrics(SM_CYHSCROLL);
				}
				break;
				}

				rs.Close();
			}
		}
		else
		{
			pCellList->PostMessage(WM_DESTROY, 0, 0);
			pCellList->DestroyWindow();
			delete pCellList;
			pCellList = nullptr;
		}
	}
	break;
	}

	if(pCellList!=nullptr)
	{
		pCellList->MoveWindow(rect);

		pCellList->SetFont(m_ListCtrl.GetFont());
		pCellList->SetItemHeight(-1, nHeight);

		pCellList->SetHorizontalExtent(0);
		if(textItem.IsEmpty()==FALSE)
		{
			if(pCellList->SelectString(-1, textItem)==CB_ERR)
			{
				pCellList->SetWindowText(textItem);
			}
		}
		else
			pCellList->SetCurSel(0);

		pCellList->SetFocus();

		*pResult = 1;
		return TRUE;
	}
	else
	{
		*pResult = 0;
		return FALSE;
	}
}

BOOL CWherePickPage::OnEndInPlaceEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* plvDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM* plvItem = &plvDispInfo->item;

	if(plvItem->iSubItem==1&&m_pDatabase!=nullptr&&m_pDatabase->IsOpen()==TRUE&&m_strTable.IsEmpty()==FALSE)
	{
		long nType1 = -1;
		long nType2 = -1;

		CFieldset rs(m_pDatabase);
		if(rs.Open(nullptr, nullptr, m_strTable, nullptr, CRecordset::OpenType::snapshot))
		{
			const CString textItem = m_ListCtrl.GetItemText(plvItem->iItem, plvItem->iSubItem);
			nType1 = rs.GetFieldDateType(textItem);

			rs.Close();
		}
		if(rs.Open(nullptr, nullptr, nullptr, m_strTable, CRecordset::OpenOptions::readOnly))
		{
			nType2 = rs.GetFieldDateType(plvItem->pszText);
			rs.Close();
		}
		if(nType1!=nType2)
		{
			if(nType2==0)
				m_ListCtrl.SetItemText(plvItem->iItem, 2, _T("包含"));
			else
				m_ListCtrl.SetItemText(plvItem->iItem, 2, _T("等于"));
		}
	}
	m_ListCtrl.SetItemText(plvItem->iItem, plvItem->iSubItem, plvItem->pszText);

	CRect rect;
	m_ListCtrl.GetItemRect(plvItem->iItem, rect, LVIR_BOUNDS);
	for(long i = 0; i<plvItem->iSubItem; i++)
	{
		rect.left += m_ListCtrl.GetColumnWidth(i);
	}
	rect.right = rect.left+m_ListCtrl.GetColumnWidth(plvItem->iSubItem);
	m_ListCtrl.InvalidateRect(rect, TRUE);

	*pResult = 0;
	return FALSE;
}

BOOL CWherePickPage::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_KEYDOWN)
	{
		switch(pMsg->wParam)
		{
		case VK_ESCAPE:
		case VK_RETURN:
			return TRUE;
			break;
		}
	}

	return CPropertyPage::PreTranslateMessage(pMsg);
}
