#include "stdafx.h"
#include <stdlib.h>
#include "DBGridCtrl.h"
#include "columns.h"
#include "column.h"
#include "StatDlg.h"
#include "OrderKeyDlg.h"

#include "../Public/Fieldset.h"
#include "../Public/ODBCDatabase.h"

#include "../Geometry/Geom.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CDBGridCtrl, CFrameWndEx)

CDBGridCtrl::CDBGridCtrl()
{
	m_pDatabase = nullptr;
	m_keyVisible = true;
}

BEGIN_MESSAGE_MAP(CDBGridCtrl, CFrameWndEx)
	//{{AFX_MSG_MAP(CDBGridCtrl)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_GRID_ORDER, OnOrder)
	ON_COMMAND(ID_GRID_ORDERASCE, OnOrderAsce)
	ON_COMMAND(ID_GRID_ORDERDESC, OnOrderDesc)
	ON_COMMAND(ID_GRID_FITTOHEADER, OnFitToHeader)
	ON_COMMAND(ID_GRID_FITTOWINDOW, OnFitToWindow)
	ON_COMMAND(ID_GRID_STAT, OnStat)

	ON_COMMAND(ID_GRID_EXPORT, OnExport2Excel)
	ON_WM_WINDOWPOSCHANGING()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDBGridCtrl message handlers
BOOL CDBGridCtrl::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, LPCTSTR lpszMenuName, DWORD dwExStyle, CCreateContext* pContext)
{
	if(CFrameWndEx::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, lpszMenuName, dwExStyle, pContext)==TRUE)
	{
		if(m_DBGrid.Create(nullptr, nullptr, WS_CHILD|WS_VISIBLE, CRect(0, 0, 0, 0), this, 88888, nullptr)==TRUE)
		{
			const LONG nStyle = ::GetWindowLong(m_DBGrid.m_hWnd, GWL_EXSTYLE);
			::SetWindowLong(m_DBGrid.m_hWnd, GWL_EXSTYLE, nStyle&~WS_EX_CLIENTEDGE);

			if(m_Rdc.Create(nullptr, nullptr, WS_CHILD, CRect(0, 0, 0, 0), this, 999999, nullptr)==TRUE)
			{
				IUnknown* pCursor = m_Rdc.GetDSCCursor();
				if(pCursor!=nullptr)
				{
					m_DBGrid.SetDataSource(pCursor);
					m_DBGrid.BindProperty(0x9, &m_Rdc);
					pCursor->Release();
				}
				m_Rdc.ShowWindow(SW_HIDE);

				m_DBGrid.SetAppearance(0);
				m_DBGrid.SetBorderStyle(1);
				m_DBGrid.SetAllowUpdate(FALSE);
				m_DBGrid.SetAllowAddNew(FALSE);
				m_DBGrid.SetAllowDelete(FALSE);
				m_DBGrid.GetAllowArrows();
				m_DBGrid.SetRowDividerStyle(6);
				m_DBGrid.SetRecordSelectors(FALSE);
				m_DBGrid.SetHeadLines(1.5);
				m_DBGrid.SetColumnHeaders(TRUE);
				m_DBGrid.SetMarqueeUnique(TRUE);
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}

		CMFCToolBarComboBoxButton::SetFlatMode();
		const HINSTANCE hInstOld = AfxGetResourceHandle();
		HINSTANCE hInst = ::LoadLibrary(_T("Information.dll"));
		AfxSetResourceHandle(hInst);

		if(m_ToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_DBGRID)==TRUE)
		{
			m_ToolBar.LoadToolBar(IDR_DBGRID, 0, 0, TRUE);
			m_ToolBar.CleanUpLockedImages();
			m_ToolBar.LoadBitmap(IDB_DBGRID_TOOLBAR, 0, 0, TRUE);

			m_ToolBar.SetPaneStyle(m_ToolBar.GetPaneStyle()|CBRS_TOOLTIPS|CBRS_FLYBY);
			m_ToolBar.SetPaneStyle(m_ToolBar.GetPaneStyle()&~(CBRS_GRIPPER|CBRS_SIZE_DYNAMIC|CBRS_BORDER_TOP|CBRS_BORDER_BOTTOM|CBRS_BORDER_LEFT|CBRS_BORDER_RIGHT));
			m_ToolBar.SetOwner(this);
			m_ToolBar.SetRouteCommandsViaFrame(FALSE);// All commands will be routed via this control , not via the parent frame:
			m_ToolBar.AdjustLayout();
			m_ToolBar.AdjustSizeImmediate();

			m_ToolBar.RemoveButton(m_ToolBar.CommandToIndex(ID_GRID_INSERT));
			m_ToolBar.RemoveButton(m_ToolBar.CommandToIndex(ID_GRID_DELETE));
			m_ToolBar.RemoveButton(m_ToolBar.CommandToIndex(ID_GRID_QUERY));
			m_ToolBar.RemoveButton(m_ToolBar.CommandToIndex(ID_GRID_GEOMALL));
			m_ToolBar.AdjustLayout();
			m_ToolBar.AdjustSizeImmediate();

			m_ToolBar.RemoveButton(m_ToolBar.CommandToIndex(ID_GRID_GEOMRADIO));
			CMFCToolBarButton* pButton1 = m_ToolBar.GetButton(m_ToolBar.CommandToIndex(ID_GRID_GEOMRADIO));
			if(pButton1!=nullptr)
			{
				CRect rect;
				m_ToolBar.GetItemRect(m_ToolBar.CommandToIndex(ID_GRID_GEOMRADIO), &rect);
				rect.right = rect.left+100;
				pButton1->SetRect(rect);
				m_ToolBar.AdjustLayout();

				m_ToolBar.SetButtonStyle(m_ToolBar.CommandToIndex(ID_GRID_GEOMRADIO), TBBS_CHECKBOX|TBBS_CHECKED);

				pButton1->m_strText = "������ͼ";
				pButton1->m_bText = TRUE;
				pButton1->m_bImage = FALSE;
				pButton1->SetStyle(TBBS_CHECKBOX|TBBS_CHECKED);
				pButton1->m_bUserButton = TRUE;
				pButton1->EnableWindow(TRUE);
			}
		}

		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CDBGridCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	if(!CFrameWndEx::PreCreateWindow(cs))
		return FALSE;

	cs.style &= ~WS_EX_CLIENTEDGE;
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	return TRUE;
}

void CDBGridCtrl::OnSize(UINT nType, int cx, int cy)
{
	CFrameWndEx::OnSize(nType, cx, cy);

	if(m_DBGrid.m_hWnd!=nullptr)
	{
		const int offset = m_ToolBar.m_hWnd==nullptr ? 0 : m_ToolBar.CalcFixedLayout(FALSE, TRUE).cy;;
		m_DBGrid.SetWindowPos(&wndTop, -1, offset, cx+2, cy-offset+1, SWP_SHOWWINDOW);
	}
}

void CDBGridCtrl::Clear()
{
	m_nTableType = 0;
	m_pDatabase = nullptr;
	m_strTable.Empty();
	m_strFields.Empty();
	m_strWhere.Empty();

	m_Rdc.SetConnect(_T(""));
	m_Rdc.SetSql(_T(""));
	m_Rdc.Refresh();

	m_DBGrid.UpdateWindow();
}

void CDBGridCtrl::Fill(CDatabase* pDatabase, CString strTable, CString strFields, CString strWhere, CString strIdField)
{
	if(pDatabase!=nullptr&&pDatabase->IsOpen()==TRUE)
	{
		strTable.Replace(_T("["), nullptr);
		strTable.Replace(_T("]"), nullptr);

		m_pDatabase = pDatabase;
		m_strTable = strTable;
		m_strFields = strFields;
		m_strWhere = strWhere;
		const CString strConnectionString = ((CODBCDatabase*)pDatabase)->GetODBCConnectionString();
		m_Rdc.SetConnect(strConnectionString);

		CString strSQL;
		strSQL.Format(_T("Select %s FROM [%s]"), strFields, strTable);
		if(strWhere.IsEmpty()==FALSE)
		{
			strSQL = strSQL+_T(" WHERE ")+strWhere;
		}
		CODBCDatabase::ParseSQL(pDatabase, strSQL);

		m_Rdc.SetSql(strSQL);
		m_Rdc.Refresh();

		CFieldset fieldset(pDatabase);
		if(fieldset.Open(nullptr, nullptr, m_strTable, nullptr, CRecordset::OpenType::snapshot)==TRUE)
		{
			m_nTableType = fieldset.GetLinkType();

			fieldset.Close();
		}

		MarkField();
	}
	else
	{
		this->Clear();
	}
}

void CDBGridCtrl::PolyFilter(CPoint* pPoints, int nAnchors)
{
	if(m_pDatabase==nullptr)
		return;
	if(m_pDatabase->IsOpen()==FALSE)
		return;
	if(m_nTableType!=2)
		return;

	if(pPoints==nullptr)
		return;
	const CString strOldSQL = m_Rdc.GetSql();
	CString strSQL = strOldSQL.Left(strOldSQL.Find(_T("FROM")));
	strSQL += _T("INTO Query ");
	strSQL += strOldSQL.Mid(strOldSQL.Find(_T("FROM")));

	m_Rdc.SetSql(_T(""));
	m_Rdc.Refresh();

	try
	{
		m_pDatabase->ExecuteSQL(_T("DROP Table Query"));
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

	try
	{
		m_pDatabase->ExecuteSQL(strSQL);

		CRecordset rs(m_pDatabase);
		rs.Open(CRecordset::dynaset, _T("Select * From QUERY"));//,CRecordset::none);
		while(rs.IsEOF()==FALSE)
		{
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
	m_Rdc.SetSql(_T("Select * From QUERY"));
	m_Rdc.Refresh();
}

void CDBGridCtrl::OnFitToHeader()
{
	CColumns columns = m_DBGrid.GetColumns();
	for(int nIndex = 0; nIndex<columns.GetCount(); nIndex++)
	{
		VARIANT Var;
		Var.vt = VT_I4;
		Var.lVal = nIndex;

		CColumn column = columns.GetItem(Var);
		//		column.AutoSize();
	}
}

void CDBGridCtrl::OnFitToWindow()
{
	CRect rect;
	m_DBGrid.GetClientRect(rect);

	CColumns columns = m_DBGrid.GetColumns();
	int TotWidth = columns.GetCount();
	for(int nIndex = 0; nIndex<columns.GetCount(); nIndex++)
	{
		VARIANT Var;
		Var.vt = VT_I4;
		Var.lVal = nIndex;

		CColumn column = columns.GetItem(Var);
		TotWidth += column.GetWidth();
	}
	if(TotWidth>rect.Width())
		return;

	int VarWidth = (rect.Width()-TotWidth)/columns.GetCount();

	for(int nIndex = 0; nIndex<columns.GetCount(); nIndex++)
	{
		VARIANT Var;
		Var.vt = VT_I4;
		Var.lVal = nIndex;

		CColumn column = columns.GetItem(Var);
		int nWidth = column.GetWidth()+VarWidth;
		nWidth = max(nWidth, 40);
		column.SetWidth(nWidth);
	}
}

void CDBGridCtrl::OnOrderAsce()
{
	if(m_Rdc.GetConnect().IsEmpty()==TRUE)
		return;

	int selCol = m_DBGrid.GetSelEndCol();
	if(selCol!=-1)
	{
		CColumns columns = m_DBGrid.GetColumns();

		VARIANT Var;
		Var.vt = VT_I4;
		Var.lVal = selCol;

		CColumn column = columns.GetItem(Var);
		CString strField = column.GetDataField();

		CString strSQL;
		if(m_strWhere.IsEmpty()==FALSE)
			strSQL.Format(_T("Select %s FROM [%s] WHERE %s Order by [%s] ASC"), m_strFields, m_strTable, m_strWhere, strField);
		else
			strSQL.Format(_T("Select %s FROM [%s] Order by [%s] ASC"), m_strFields, m_strTable, strField);
		CODBCDatabase::ParseSQL(m_pDatabase, strSQL);

		m_Rdc.SetSql(strSQL);

		m_Rdc.Refresh();

		MarkField();
	}
}

void CDBGridCtrl::OnOrderDesc()
{
	if(m_Rdc.GetConnect().IsEmpty()==TRUE)
		return;

	int selCol = m_DBGrid.GetSelEndCol();
	if(selCol!=-1)
	{
		CColumns columns = m_DBGrid.GetColumns();

		VARIANT Var;
		Var.vt = VT_I4;
		Var.lVal = selCol;

		CColumn column = columns.GetItem(Var);
		CString strField = column.GetDataField();

		CString strSQL;
		if(m_strWhere.IsEmpty()==FALSE)
			strSQL.Format(_T("Select %s FROM [%s] WHERE %s Order by [%s] DESC"), m_strFields, m_strTable, m_strWhere, strField);
		else
			strSQL.Format(_T("Select %s FROM [%s] Order by [%s] DESC"), m_strFields, m_strTable, strField);
		CODBCDatabase::ParseSQL(m_pDatabase, strSQL);

		m_Rdc.SetSql(strSQL);
		m_Rdc.Refresh();

		MarkField();
	}
}

void CDBGridCtrl::OnStat()
{
	if(m_pDatabase==nullptr)
		return;
	if(m_pDatabase->IsOpen()==FALSE)
		return;
	if(m_strTable.IsEmpty()==TRUE)
		return;

	int selCol = m_DBGrid.GetSelEndCol();
	if(selCol!=-1)
	{
		CColumns columns = m_DBGrid.GetColumns();
		VARIANT Var;
		Var.vt = VT_I4;
		Var.lVal = selCol;

		CColumn column = columns.GetItem(Var);
		CString strField = column.GetDataField();
		if(CODBCDatabase::IsFiledNumeric(m_pDatabase, m_strTable, strField)==false)
		{
			AfxMessageBox(_T("Not a numeral field!"));
			return;
		}

		CRecordset rs(m_pDatabase);
		try
		{
			CString strSQL;
			if(m_strWhere.IsEmpty()==FALSE)
				strSQL.Format(_T("Select COUNT(*) As myCount,Max([%s]) As myMax,Min([%s]) As myMin,AVG([%s]) As myAvg,SUM([%s]) As mySum FROM [%s] WHERE %s"), strField, strField, strField, strField, m_strTable, m_strWhere);
			else
				strSQL.Format(_T("Select COUNT(*) As myCount,Max([%s]) As myMax,Min([%s]) As myMin,AVG([%s]) As myAvg,SUM([%s]) As mySum FROM [%s]"), strField, strField, strField, strField, m_strTable);

			CODBCDatabase::ParseSQL(m_pDatabase, strSQL);
			rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly);
			CString strCount;
			CString strMax;
			CString strMin;
			CString strAvg;
			CString strSum;
			if(rs.IsEOF()==FALSE)
			{
				rs.GetFieldValue(_T("myCount"), strCount);
				rs.GetFieldValue(_T("myMax"), strMax);
				rs.GetFieldValue(_T("myMin"), strMin);
				rs.GetFieldValue(_T("myAvg"), strAvg);
				rs.GetFieldValue(_T("mySum"), strSum);
			}
			rs.Close();

			HINSTANCE hInstOld = AfxGetResourceHandle();
			HINSTANCE hInst = ::LoadLibrary(_T("Information.dll"));
			AfxSetResourceHandle(hInst);

			CStatDlg dlg(nullptr, strField, strCount, strMax, strMin, strAvg, strSum);
			dlg.DoModal();

			AfxSetResourceHandle(hInstOld);
			::FreeLibrary(hInst);
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

void CDBGridCtrl::OnOrder()
{
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Information.dll"));
	AfxSetResourceHandle(hInst);

	COrderkeyDlg dlg(nullptr, &m_DBGrid, nullptr);
	if(dlg.DoModal()==IDOK)
	{
		const CString strOrder = dlg.m_strOrder;

		CString strSQL;
		if(m_strWhere.IsEmpty()==FALSE)
			strSQL.Format(_T("Select %s FROM [%s] WHERE %s Order By %s"), m_strFields, m_strTable, m_strWhere, strOrder);
		else
			strSQL.Format(_T("Select %s FROM [%s] Order By %s"), m_strFields, m_strTable, strOrder);
		CODBCDatabase::ParseSQL(m_pDatabase, strSQL);

		m_Rdc.SetSql(strSQL);
		m_Rdc.Refresh();

		MarkField();
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

BOOL CDBGridCtrl::DestroyWindow()
{
	m_pDatabase = nullptr;
	m_strTable.Empty();

	m_DBGrid.SetDataSource(nullptr);
	//	m_DBGrid.BindProperty(0x9,nullptr);
	m_Rdc.SetConnect(_T(""));
	m_Rdc.SetSql(_T(""));

	m_DBGrid.PostMessage(WM_DESTROY, 0, 0);
	m_DBGrid.DestroyWindow();
	m_Rdc.PostMessage(WM_DESTROY, 0, 0);
	m_Rdc.DestroyWindow();

	return CFrameWndEx::DestroyWindow();
}

void CDBGridCtrl::OnExport2Excel()
{
	if(m_pDatabase==nullptr)
		return;
	if(m_pDatabase->IsOpen()==FALSE)
		return;
	if(m_strTable.IsEmpty()==TRUE)
		return;

	CFileDialog dlg(FALSE,
		_T("xls"),
		nullptr,
		OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST,
		_T("Microsoft Excel File (*.xls)|*.xls|"),
		nullptr
	);

	dlg.m_ofn.lpstrTitle = _T("Save as an Excel file");
	if(dlg.DoModal()!=IDOK)
		return;

	CString sExcelFile = dlg.GetPathName(); // Filename and path for the file to be created
	DeleteFile(sExcelFile);

	CDatabase database;
	CString sDriver = _T("MICROSOFT EXCEL DRIVER (*.XLS)"); // exactly the same name as in the ODBC-Manager

	CString strSQL;
	try
	{
		strSQL.Format(_T("DRIVER={%s};DSN='';FIRSTROWHASNAMES=1;READONLY=FALSE;CREATE_DB=\"%s\";DBQ=%s"), sDriver, sExcelFile, sExcelFile);
		if(database.OpenEx(strSQL, CDatabase::noOdbcDialog)==TRUE)
		{
			strSQL = "CREATE TABLE Copy (";
			CColumns columns = m_DBGrid.GetColumns();
			for(int nIndex = 0; nIndex<columns.GetCount(); nIndex++)
			{
				VARIANT Var;
				Var.vt = VT_I4;
				Var.lVal = nIndex;

				CColumn column = columns.GetItem(Var);
				CString caption = column.GetCaption();
				strSQL += caption+_T(" Text,");
			}

			strSQL = strSQL.Left(strSQL.GetLength()-1);
			strSQL += ')';

			database.ExecuteSQL(strSQL);

			CString strHead = _T("INSERT INTO Copy (");
			for(int nIndex = 0; nIndex<columns.GetCount(); nIndex++)
			{
				VARIANT Var;
				Var.vt = VT_I4;
				Var.lVal = nIndex;

				CColumn column = columns.GetItem(Var);
				CString caption = column.GetCaption();
				strHead += caption+_T(',');
			}

			strHead = strHead.Left(strHead.GetLength()-1);
			strHead += ") VALUES (";

			CString strOldSQL = m_Rdc.GetSql();
			CRecordset rsSource(m_pDatabase);
			rsSource.Open(CRecordset::snapshot, strOldSQL, CRecordset::readOnly);
			while(rsSource.IsEOF()==FALSE)
			{
				strSQL = strHead;
				for(short index = 0; index<rsSource.GetODBCFieldCount(); index++)
				{
					CString strValue;
					rsSource.GetFieldValue(index, strValue);
					if(strValue.IsEmpty()==TRUE)
					{
						strValue = _T("  ");
					}
					strSQL += _T('\'')+strValue+_T('\'')+_T(',');
				}
				strSQL = strSQL.Left(strSQL.GetLength()-1);
				strSQL += _T(')');

				database.ExecuteSQL(strSQL);

				rsSource.MoveNext();
			}
			rsSource.Close();
		}

		database.Close();
	}
	catch(CException* ex)
	{
		//		OutputDebugString(ex->m_strError);
		TRACE1("Driver not installed: %s", sDriver);
		ex->Delete();
	}
}

BOOL CDBGridCtrl::OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if(pHandlerInfo==nullptr)
	{
		if(nCode==CN_UPDATE_COMMAND_UI)
		{
			CCmdUI* pCmdUI = (CCmdUI*)pExtra;
			if(pCmdUI->m_bContinueRouting)
			{
				return CFrameWndEx::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
			}

			ASSERT(!pCmdUI->m_bContinueRouting);

			switch(nId)
			{
			case ID_GRID_FITTOWINDOW:
			case ID_GRID_ORDER:
			{
				if(m_pDatabase==nullptr)
					pCmdUI->Enable(FALSE);
				else if(m_pDatabase->IsOpen()==FALSE)
					pCmdUI->Enable(FALSE);
				else if(m_strTable.IsEmpty()==TRUE)
					pCmdUI->Enable(FALSE);
				else
					pCmdUI->Enable(TRUE);
				return TRUE;
			}
			break;
			case ID_GRID_FITTOHEADER:
			{
				if(m_pDatabase==nullptr)
					pCmdUI->Enable(FALSE);
				else if(m_pDatabase->IsOpen()==FALSE)
					pCmdUI->Enable(FALSE);
				else if(m_strTable.IsEmpty()==TRUE)
					pCmdUI->Enable(FALSE);
				else
				{
					CRect rect;
					m_DBGrid.GetClientRect(rect);

					CColumns columns = m_DBGrid.GetColumns();
					int TotWidth = columns.GetCount();
					for(int nIndex = 0; nIndex<columns.GetCount(); nIndex++)
					{
						VARIANT Var;
						Var.vt = VT_I4;
						Var.lVal = nIndex;

						CColumn column = columns.GetItem(Var);
						TotWidth += column.GetWidth();
					}
					if(TotWidth>rect.Width())
					{
						pCmdUI->Enable(FALSE);
					}
					else
					{
						pCmdUI->Enable(TRUE);
					}
				}
				return TRUE;
			}

			break;
			case ID_GRID_ORDERASCE:
			case ID_GRID_ORDERDESC:
			case ID_GRID_STAT:
			{
				if(m_pDatabase==nullptr)
					pCmdUI->Enable(FALSE);
				else if(m_pDatabase->IsOpen()==FALSE)
					pCmdUI->Enable(FALSE);
				else if(m_strTable.IsEmpty()==TRUE)
					pCmdUI->Enable(FALSE);
				else if(m_DBGrid.GetSelEndCol()==-1)
					pCmdUI->Enable(FALSE);
				else
					pCmdUI->Enable(TRUE);
				return TRUE;
			}
			break;
			default:
				break;
			}
			pCmdUI->m_bContinueRouting = false;
		}
		if(nCode==CN_COMMAND)
		{
		}
	}

	return CFrameWndEx::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
}
