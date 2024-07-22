#include "stdafx.h"
#include "TDBGridCtrl.h"
#include "StatDlg.h"
#include "OrderKeyDlg.h"
#include "_Recordset.h"
#include "StyleDisp.h"
#include "Font.h"

#include <shellapi.h>

#include "Columns.h"
#include "Column.h"

#include "../Public/AttributeCtrl.h"
#include "../Public/ODBCDatabase.h"
#include "../Public/PropertyPane.h"

#include "../Geometry/Geom.h"
#include "../Mapper/Global.h"

#include <comutil.h>
#include <stdio.h>

#pragma comment(lib, "comsuppw.lib")
#pragma comment(lib, "kernel32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool CTDBGridCtrl::m_bEditable = false;

IMPLEMENT_DYNCREATE(CTDBGridCtrl, CFrameWndEx)

/////////////////////////////////////////////////////////////////////////////
// CTDBGridCtrl dialog
BEGIN_MESSAGE_MAP(CTDBGridCtrl, CFrameWndEx)
	//{{AFX_MSG_MAP(CTDBGridCtrl)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGING()
	ON_COMMAND(ID_GRID_DELETE, OnDelete)
	ON_COMMAND(ID_GRID_ORDER, OnOrder)
	ON_COMMAND(ID_GRID_ORDERASCE, OnOrderAsce)
	ON_COMMAND(ID_GRID_ORDERDESC, OnOrderDesc)
	ON_COMMAND(ID_GRID_FITTOHEADER, OnFitToHeader)
	ON_COMMAND(ID_GRID_FITTOWINDOW, OnFitToWindow)
	ON_COMMAND(ID_GRID_STAT, OnStat)
	ON_COMMAND(ID_GRID_INSERT, OnInsert)
	//}}AFX_MSG_MAP	
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CTDBGridCtrl, CTrueDBGridCtrl)
	ON_EVENT(CTDBGridCtrl, 88888, 11, RowColChange, VTS_PVARIANT VTS_I2)
	//	ON_EVENT(CTDBGridCtrl, 88888, 75, FilterChange, VTS_NONE)
	ON_EVENT(CTDBGridCtrl, 88888, -607, Mouseup, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CTDBGridCtrl, 88888, 76, FilterButtonClick, VTS_I2)
END_EVENTSINK_MAP()

CTDBGridCtrl::CTDBGridCtrl()
{
	m_pDatabase = nullptr;
	m_pCheckBox = nullptr;
}

BOOL CTDBGridCtrl::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, LPCTSTR lpszMenuName, DWORD dwExStyle, CCreateContext* pContext)
{
	if(CFrameWndEx::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, lpszMenuName, dwExStyle, pContext)==TRUE)
	{
		const WCHAR wcLicenseKey[] = {0x0047, 0x004C, 0x0005, 0x0008, 0x0001, 0x0005,	0x0002, 0x0008, 0x0001, 0x0004};
		const BSTR bstrLicense = ::SysAllocStringLen(wcLicenseKey, sizeof(wcLicenseKey)/sizeof(WCHAR));

		if(m_DBGrid.Create(nullptr, WS_CHILD|WS_VISIBLE, CRect(0, 0, 0, 0), this, 88888, nullptr, 0, bstrLicense)==TRUE)
		{
			const LONG nStyle = ::GetWindowLong(m_DBGrid.m_hWnd, GWL_EXSTYLE);
			::SetWindowLong(m_DBGrid.m_hWnd, GWL_EXSTYLE, nStyle&~WS_EX_CLIENTEDGE);

			if(m_Adodc.Create(nullptr, nullptr, WS_CHILD, CRect(0, 0, 0, 0), this, 999999, nullptr)==TRUE)
			{
				m_Adodc.SetCommandType(1);
				CStyleDisp style1;
				style1.SetBorderAppearance(0);
				CStyleDisp style2 = m_DBGrid.GetEvenRowStyle();
				style2.SetBackColor(0X00F5F5F5);

				CStyleDisp style3 = m_DBGrid.GetHeadingStyle();
				style3.SetAlignment(2);//horizontally center
				style3.SetVerticalAlignment(2);//vertically center
				//	style3.SetBackColor(0XCCCCCC);
				m_DBGrid.SetHeadingStyle(style3);
				COleFont headerfont = m_DBGrid.GetHeadFont();
				headerfont.SetWeight(550);
				//	CY fontsize = headerfont.GetSize();				
				m_DBGrid.SetHeadFont(headerfont);

				m_DBGrid.SetAllowArrows(TRUE);
				m_DBGrid.SetAppearance(2);//Track 3D
				m_DBGrid.SetBorderStyle(1);//Fixed Single
				m_DBGrid.SetEmptyRows(TRUE);
				m_DBGrid.EnableD2DSupport();
				m_DBGrid.SetDataSource(nullptr);
				m_DBGrid.BindProperty(0x9, &m_Adodc);
				m_DBGrid.SetWindowPos(&wndTop, 0, 30, 0, 0, SWP_NOSIZE);
				m_DBGrid.SetRecordSelectors(FALSE);
				m_DBGrid.SetMarqueeStyle(3);
				m_DBGrid.SetEditForeColor(0X00000000);
				m_DBGrid.SetEditBackColor(0X00FFFFFF);
				m_DBGrid.SetAlternatingRowStyle(TRUE);
				m_DBGrid.SetFilterBar(TRUE);
				m_DBGrid.SetFetchRowStyle(TRUE);
				CStyleDisp style4 = m_DBGrid.GetFilterBarStyle();
				style4.SetBorderSize(0);
				style4.SetBorderType(0);
				m_DBGrid.SetFilterBarStyle(style4);

				m_DBGrid.SetRowDividerStyle(6);//Light gray line
				m_DBGrid.SetHeadLines(1.5);
				m_DBGrid.SetEvenRowStyle(style2);
				m_DBGrid.SetSpringMode(TRUE);

				m_Adodc.ShowWindow(SW_HIDE);
				::SysFreeString(bstrLicense);
			}
			else
			{
				::SysFreeString(bstrLicense);
				return FALSE;
			}
		}
		else
		{
			::SysFreeString(bstrLicense);
			return FALSE;
		}

		CMFCToolBarComboBoxButton::SetFlatMode();
		const HINSTANCE hInstOld = AfxGetResourceHandle();
		HINSTANCE hInst = ::LoadLibrary(_T("Information.dll"));
		AfxSetResourceHandle(hInst);

		CWnd* pWnd = pParentWnd->GetParent();
		if(m_ToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_DBGRID)==TRUE)
		{
			m_ToolBar.LoadToolBar(IDR_DBGRID, 0, 0, TRUE);
			m_ToolBar.CleanUpLockedImages();
			m_ToolBar.LoadBitmap(IDB_DBGRID_TOOLBAR, 0, 0, TRUE);

			m_ToolBar.SetPaneStyle(m_ToolBar.GetPaneStyle()|CBRS_TOOLTIPS|CBRS_FLYBY);
			m_ToolBar.SetPaneStyle(m_ToolBar.GetPaneStyle()&~(CBRS_GRIPPER|CBRS_SIZE_DYNAMIC|CBRS_BORDER_TOP|CBRS_BORDER_BOTTOM|CBRS_BORDER_LEFT|CBRS_BORDER_RIGHT));
			m_ToolBar.SetOwner(this);
			m_ToolBar.SetRouteCommandsViaFrame(FALSE);
			m_ToolBar.AdjustLayout();
			m_ToolBar.AdjustSizeImmediate();

			if(CTDBGridCtrl::m_bEditable==false)
			{
				m_ToolBar.RemoveButton(m_ToolBar.CommandToIndex(ID_GRID_INSERT));
				m_ToolBar.RemoveButton(m_ToolBar.CommandToIndex(ID_GRID_DELETE));
			}
			m_ToolBar.RemoveButton(m_ToolBar.CommandToIndex(ID_GRID_GEOMALL));
			m_ToolBar.RemoveButton(m_ToolBar.CommandToIndex(ID_GRID_EXPORT));
			m_ToolBar.AdjustLayout();
			m_ToolBar.AdjustSize();
			m_ToolBar.AdjustSizeImmediate();
		}

		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CTDBGridCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	if(!CFrameWndEx::PreCreateWindow(cs))
		return FALSE;

	cs.style &= ~WS_EX_CLIENTEDGE;
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	return TRUE;
}

void CTDBGridCtrl::OnSize(UINT nType, int cx, int cy)
{
	CFrameWndEx::OnSize(nType, cx, cy);

	if(m_DBGrid.m_hWnd!=nullptr)
	{
		const int offset = m_ToolBar.m_hWnd==nullptr ? 0 : m_ToolBar.CalcFixedLayout(FALSE, TRUE).cy;;
		m_DBGrid.SetWindowPos(&wndTop, -1, offset, cx+2, cy-offset+1, SWP_SHOWWINDOW);
	}
}

void CTDBGridCtrl::ClearGrid()
{
	m_DBGrid.ClearFields();
	m_DBGrid.SetDataSource(nullptr);
	m_DBGrid.Refresh();

	m_Adodc.SetRecordSource(_T(""));
	m_Adodc.SetConnectionString(_T(""));

	m_pDatabase = nullptr;
	m_pHeaderProfile = nullptr;
}

void CTDBGridCtrl::Fill(CDatabase* pDatabase, CHeaderProfile* pHeaderProfile, BOOL bReadOnly)
{
	if(pDatabase==m_pDatabase&&pHeaderProfile==m_pHeaderProfile)
		return;

	if(pDatabase!=m_pDatabase)
		SetDatabase(pDatabase);
	else if(pDatabase==nullptr)
		ClearGrid();
	else if(pDatabase->IsOpen()==FALSE)
		ClearGrid();

	if(pHeaderProfile==m_pHeaderProfile)
		ClearGrid();
	else if(pHeaderProfile==nullptr)
		ClearGrid();
	else if(pHeaderProfile->m_strKeyTable==_T(""))
		ClearGrid();
	else if(pHeaderProfile!=m_pHeaderProfile)
	{
		CString strSQL = pHeaderProfile->MakeSelectStatment();
		CODBCDatabase::ParseSQL(pDatabase, strSQL);
		if(m_Adodc.m_hWnd!=nullptr)
		{
			AfxGetApp()->BeginWaitCursor();
			try
			{
				m_Adodc.SetRecordSource(strSQL);//m_Adodc.SetSql(strSQL);
				m_Adodc.Refresh();
				LPUNKNOWN pCursor = (LPUNKNOWN)m_Adodc.GetRecordset();
				if(pCursor!=nullptr)
				{
					m_DBGrid.ClearFields();
					m_DBGrid.SetDataSource(pCursor); //bind the grid
				}
			}
			catch(CDBException*ex)
			{
				OutputDebugString(ex->m_strError);
				ex->Delete();
			}
			catch(CException* ex)
			{
				OutputDebugStringA("Error happened when load data to grid!");
				ex->Delete();
			}
			AfxGetApp()->EndWaitCursor();
		}

		char lpTable[255];
#ifdef _UNICODE
		::WideCharToMultiByte(CP_ACP, 0, pHeaderProfile->m_strKeyTable, -1, lpTable, 255, nullptr, nullptr);
#else
		strcpy_s(lpTable, strTable);
#endif

		CColumns columns = m_DBGrid.GetColumns();
		for(int nIndex = 0; nIndex<columns.GetCount(); nIndex++)
		{
			VARIANT var;
			var.vt = VT_I4;
			var.lVal = nIndex;

			CColumn column = columns.GetItem(var);
			column.SetFilterButton(TRUE);
		}
		m_pHeaderProfile = pHeaderProfile;

		MarkField();
		m_DBGrid.HoldFields();
	}
	else
	{
		m_DBGrid.SetDataSource(nullptr);
		m_DBGrid.ClearFields();
		m_DBGrid.Refresh();
		m_Adodc.SetRecordSource(_T(""));

		m_pHeaderProfile = nullptr;
	}
}

bool CTDBGridCtrl::AddNew(CString strFields, CString strValues)
{
	if(m_pDatabase==nullptr)
		return false;
	if(m_pDatabase->IsOpen()==FALSE)
		return false;

	try
	{
		int nCount1 = 0;
		int nCount2 = 0;
		int pos1 = strFields.Find(_T(','), 0);
		int pos2 = strValues.Find(_T(','), 0);
		while(pos1!=-1)
		{
			nCount1++;
			pos1 = strFields.Find(_T(','), pos1+1);
		}
		while(pos2!=-1)
		{
			nCount2++;
			pos2 = strValues.Find(_T(','), pos2+1);
		}

		if(nCount1!=nCount2)
			return false;

		SAFEARRAYBOUND rgsaField;
		SAFEARRAYBOUND rgsaValue;
		rgsaField.lLbound = 0;
		rgsaValue.lLbound = 0;
		rgsaField.cElements = nCount1+1;
		rgsaValue.cElements = nCount2+1;
		SAFEARRAY* psaField = SafeArrayCreate(VT_VARIANT, 1, &rgsaField);
		SAFEARRAY* psaValue = SafeArrayCreate(VT_VARIANT, 1, &rgsaValue);

		for(long index = 0; index<nCount1+1; index++)
		{
			int pos1 = strFields.Find(_T(','));
			int pos2 = strValues.Find(_T(','));

			CString strField;
			CString strValue;

			if(pos1!=-1)
			{
				strField = strFields.Left(pos1);
				strFields = strFields.Mid(pos1+1);
			}
			else
			{
				strField = strFields;
				strFields.Empty();
			}
			if(pos2!=-1)
			{
				strValue = strValues.Left(pos2);
				strValues = strValues.Mid(pos2+1);
			}
			else
			{
				strValue = strValues;
				strValues.Empty();
			}

			strField.Replace(_T("["), nullptr);
			strField.Replace(_T("]"), nullptr);
			strValue.Replace(_T("'"), nullptr);

			_variant_t varField = _bstr_t(strField);
			_variant_t varValue = _bstr_t(strValue);

			HRESULT hr1 = SafeArrayPutElement(psaField, &index, &varField);
			HRESULT hr2 = SafeArrayPutElement(psaValue, &index, &varValue);
			if(SUCCEEDED(hr1)==FALSE||SUCCEEDED(hr2)==FALSE)
			{
				::SafeArrayDestroy(psaField);
				::SafeArrayDestroy(psaValue);
				return false;
			}
		}

		VARIANT varFields, varValues;
		varFields.vt = VT_VARIANT|VT_ARRAY;
		varValues.vt = VT_VARIANT|VT_ARRAY;
		V_ARRAY(&varFields) = psaField;
		V_ARRAY(&varValues) = psaValue;

		C_Recordset rs = m_Adodc.GetRecordset();
		rs.AddNew(varFields, varValues);

		::SafeArrayDestroy(psaField);
		::SafeArrayDestroy(psaValue);

		return true;
	}
	catch(CException*ex)
	{
		TCHAR buff[MAX_PATH+1];
		memset(buff, 0, MAX_PATH+1);
		ex->GetErrorMessage(buff, MAX_PATH);
		OutputDebugString(buff);
		ex->Delete();
	}
	return false;
}

bool CTDBGridCtrl::Delete(const DWORD& dwId)
{
	if(m_pDatabase==nullptr)
		return false;
	if(m_pDatabase->IsOpen()==FALSE)
		return false;
	if(m_pHeaderProfile==nullptr)
		return false;

	C_Recordset rs = m_Adodc.GetRecordset();
	VARIANT origin = rs.GetBookmark();
	CString strCriteria;
	strCriteria.Format(_T("ID=%d"), dwId);

	VARIANT var;
	var.vt = VT_I4;
	var.lVal = 0;
	rs.MoveFirst();
	rs.Find((LPCTSTR)strCriteria, 0, ADOCG::adSearchForward, var);
	if(rs.GetEof()==TRUE)
	{
		rs.SetBookmark(origin);
		return false;
	}
	else
	{
		VARIANT current = rs.GetBookmark();

		rs.Delete(ADOCG::adAffectCurrent);

		if(rs.GetEof()!=TRUE)
			rs.MoveNext();

		if(rs.GetEof()==TRUE)
		{
			if(origin.lVal==current.lVal)
				rs.MoveFirst();
			else
				rs.SetBookmark(origin);
		}
		return true;
	}
}

void CTDBGridCtrl::Update()
{
	if(IsWindowVisible()==FALSE)
		return;

	m_DBGrid.Update();
}

void CTDBGridCtrl::OnShortCut()
{
	if(m_pDatabase!=nullptr)
	{
		CString strConnect = m_pDatabase->GetConnect();
		strConnect = strConnect.Mid(strConnect.Find(_T("DBQ="))+4);
		strConnect = strConnect.Left(strConnect.Find(_T(";")));

		strConnect += _T(".mdb");
		ShellExecute(nullptr, nullptr, strConnect, nullptr, nullptr, SW_SHOWMAXIMIZED);
	}
}

void CTDBGridCtrl::OnDelete()
{
	if(IsWindowVisible()==FALSE)
		return;

	if(m_Adodc.GetConnectionString().IsEmpty()==TRUE)
		return;

	m_DBGrid.Delete();
}

void CTDBGridCtrl::OnInsert()
{
	if(IsWindowVisible()==FALSE)
		return;

	if(m_pDatabase==nullptr)
		return;
	if(m_pDatabase->IsOpen()==FALSE)
		return;
	if(m_pHeaderProfile==nullptr)
		return;

	Sleep(1000);

	AfxGetApp()->BeginWaitCursor();
	m_Adodc.Refresh();
	LPUNKNOWN pCursor = (LPUNKNOWN)m_Adodc.GetRecordset();
	if(pCursor!=nullptr)
	{
		m_DBGrid.SetDataSource(pCursor);
	}
	AfxGetApp()->EndWaitCursor();
	MarkField();

	if(m_pDatabase!=nullptr&&m_pDatabase->IsOpen()==TRUE)
	{
		CString strSQL;
		strSQL.Format(_T("Select Max(%s) As maxId FROM [%s]"), m_pHeaderProfile->m_strIdField, m_pHeaderProfile->m_strKeyTable);
		CODBCDatabase::ParseSQL(m_pDatabase, strSQL);

		CRecordset rs(m_pDatabase);
		if(rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly)==TRUE)
		{
			if(rs.IsEOF()==FALSE)
			{
				CString maxId;
				rs.GetFieldValue((short)0, maxId);
				if(maxId.IsEmpty()==FALSE)
				{
					CString strWhere;
					strWhere.Format(_T("%s=%s"), m_pHeaderProfile->m_strIdField, maxId);

					C_Recordset rs = m_Adodc.GetRecordset();
					VARIANT origin = rs.GetBookmark();

					VARIANT var;
					var.vt = VT_I4;
					var.lVal = 0;
					rs.Find((LPCTSTR)strWhere, 1, ADOCG::adSearchForward, var);
					if(rs.GetEof()==TRUE)
					{
						rs.SetBookmark(origin);
						AfxMessageBox(_T("No new created records!"));
					}
				}
			}
			rs.Close();
		}
	}
}

bool CTDBGridCtrl::SetDatabase(CDatabase* pDatabase)
{
	if(pDatabase==m_pDatabase)
		return false;

	this->ClearGrid();
	if(pDatabase!=nullptr&&pDatabase->IsOpen()==TRUE)
	{
		const CString strConnectionString = ((CODBCDatabase*)pDatabase)->GetADOConnectionString();
		m_Adodc.SetConnectionString(strConnectionString);
		m_pDatabase = pDatabase;
		return true;
	}
	else
		return false;
}

void CTDBGridCtrl::OnFitToHeader()
{
	CColumns columns = m_DBGrid.GetColumns();
	for(int nIndex = 0; nIndex<columns.GetCount(); nIndex++)
	{
		VARIANT var;
		var.vt = VT_I4;
		var.lVal = nIndex;

		CColumn column = columns.GetItem(var);
		column.AutoSize();
	}
}

void CTDBGridCtrl::OnFitToWindow()
{
	CRect rect;
	m_DBGrid.GetClientRect(rect);

	CColumns columns = m_DBGrid.GetColumns();
	int TotWidth = columns.GetCount();
	for(int nIndex = 0; nIndex<columns.GetCount(); nIndex++)
	{
		VARIANT var;
		var.vt = VT_I4;
		var.lVal = nIndex;

		CColumn column = columns.GetItem(var);
		TotWidth += column.GetWidth();
	}
	if(TotWidth>rect.Width())
		return;

	int VarWidth = (rect.Width()-TotWidth)/columns.GetCount();

	for(int nIndex = 0; nIndex<columns.GetCount(); nIndex++)
	{
		VARIANT var;
		var.vt = VT_I4;
		var.lVal = nIndex;

		CColumn column = columns.GetItem(var);
		int nWidth = column.GetWidth()+VarWidth;
		nWidth =max(nWidth, 40);

		column.SetWidth(nWidth);
	}
}

void CTDBGridCtrl::OnOrderAsce()
{
	C_Recordset rs = m_Adodc.GetRecordset();
	if(rs==nullptr)
		return;

	int selCol = m_DBGrid.GetSelEndCol();
	if(selCol!=-1)
	{
		CColumns columns = m_DBGrid.GetColumns();

		VARIANT var;
		var.vt = VT_I4;
		var.lVal = selCol;

		CColumn column = columns.GetItem(var);
		CString strField = column.GetDataField();

		CString strOrder;
		strOrder.Format(_T("[%s] ASC"), strField);
		CODBCDatabase::ParseSQL(m_pDatabase, strOrder);
		rs.SetSort(strOrder);
	}
}

void CTDBGridCtrl::OnOrderDesc()
{
	C_Recordset rs = m_Adodc.GetRecordset();
	if(rs==nullptr)
		return;

	int selCol = m_DBGrid.GetSelEndCol();
	if(selCol!=-1)
	{
		CColumns columns = m_DBGrid.GetColumns();

		VARIANT var;
		var.vt = VT_I4;
		var.lVal = selCol;

		CColumn column = columns.GetItem(var);
		CString strField = column.GetDataField();
		CString strOrder;
		strOrder.Format(_T("[%s] DESC"), strField);
		CODBCDatabase::ParseSQL(m_pDatabase, strOrder);
		rs.SetSort(strOrder);
	}
}

void CTDBGridCtrl::OnStat()
{
	if(m_pDatabase==nullptr)
		return;
	if(m_pDatabase->IsOpen()==FALSE)
		return;
	if(m_pHeaderProfile==nullptr)
		return;

	int selCol = m_DBGrid.GetSelEndCol();
	if(selCol!=-1)
	{
		CColumns columns = m_DBGrid.GetColumns();
		VARIANT var;
		var.vt = VT_I4;
		var.lVal = selCol;

		CColumn column = columns.GetItem(var);
		CString strField = column.GetDataField();
		if(CODBCDatabase::IsFiledNumeric(m_pDatabase, m_pHeaderProfile->m_strKeyTable, strField)==false)
		{
			AfxMessageBox(_T("Not a numeral field!"));
			return;
		}

		try
		{
			CString strSQL;
			strSQL.Format(_T("Select COUNT(*) As myCount,Max([%s]) As myMax,Min([%s]) As myMin,AVG([%s]) As myAvg,SUM([%s]) As mySum FROM [%s]"), strField, strField, strField, strField, m_pHeaderProfile->m_strKeyTable);
			if(m_pHeaderProfile->m_strWhere.IsEmpty()==FALSE)
			{
				strSQL += _T(" Where ");
				strSQL += m_pHeaderProfile->m_strWhere;
			}
			CODBCDatabase::ParseSQL(m_pDatabase, strSQL);

			CRecordset rs(m_pDatabase);
			if(rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly)==TRUE)
			{
				if(rs.IsEOF()==FALSE)
				{
					CString strCount;
					CString strMax;
					CString strMin;
					CString strAvg;
					CString strSum;

					rs.GetFieldValue((short)0,/*_T("myCount")*/strCount);//Oracal2??��3?��?��?????3?
					rs.GetFieldValue((short)1,/*_T("myMax")*/strMax);
					rs.GetFieldValue((short)2,/*_T("myMin")*/strMin);
					rs.GetFieldValue((short)3,/*_T("myAvg")*/strAvg);
					rs.GetFieldValue((short)4,/*_T("mySum")*/strSum);

					HINSTANCE hInstOld = AfxGetResourceHandle();
					HINSTANCE hInst = ::LoadLibrary(_T("Information.dll"));
					AfxSetResourceHandle(hInst);

					CStatDlg dlg(nullptr, strField, strCount, strMax, strMin, strAvg, strSum);
					dlg.DoModal();

					AfxSetResourceHandle(hInstOld);
					::FreeLibrary(hInst);
				}
				rs.Close();
			}
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

void CTDBGridCtrl::OnOrder()
{
	C_Recordset rs = m_Adodc.GetRecordset();
	if(rs==nullptr)
		return;
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Information.dll"));
	AfxSetResourceHandle(hInst);

	COrderkeyDlg dlg(nullptr, nullptr, &m_DBGrid);
	if(dlg.DoModal()==IDOK)
	{
		CString strOrder = dlg.m_strOrder;
		CODBCDatabase::ParseSQL(m_pDatabase, strOrder);
		rs.SetSort(strOrder);
	}
	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

void CTDBGridCtrl::Refresh()
{
	if(m_hWnd!=nullptr)
	{
		Sleep(1000);
		m_Adodc.Refresh();
	}
}

BOOL CTDBGridCtrl::OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
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
			case ID_GRID_QUERY:
			case ID_GRID_INSERT:
			case ID_GRID_DELETE:
			case ID_GRID_ORDER:
			case ID_GRID_FITTOHEADER:
			{
				if(m_pDatabase==nullptr)
					pCmdUI->Enable(FALSE);
				else if(m_pDatabase->IsOpen()==FALSE)
					pCmdUI->Enable(FALSE);
				else if(m_pHeaderProfile==nullptr)
					pCmdUI->Enable(FALSE);
				else
					pCmdUI->Enable(TRUE);
				return TRUE;
			}
			break;
			case ID_GRID_FITTOWINDOW:
			{
				if(m_pDatabase==nullptr)
					pCmdUI->Enable(FALSE);
				else if(m_pDatabase->IsOpen()==FALSE)
					pCmdUI->Enable(FALSE);
				else if(m_pHeaderProfile==nullptr)
					pCmdUI->Enable(FALSE);
				else//this slows down the whole programm
				{
					/*CRect rect;
					m_DBGrid.GetClientRect(rect);

					CColumns columns = m_DBGrid.GetColumns();
					int TotWidth = columns.GetCount();
					for(int nIndex=0;nIndex<columns.GetCount();nIndex++)
					{
						VARIANT var;
						var.vt = VT_I4;
						var.lVal = nIndex;

						CColumn column = columns.GetItem(var);
						TotWidth += column.GetWidth();
					}
					if(TotWidth>rect.Width())
					{
						pCmdUI->Enable(FALSE);
					}
					else
					{
						pCmdUI->Enable(TRUE);
					}*/
				}
				return TRUE;
			}
			break;
			case ID_GRID_ORDERASCE:
			case ID_GRID_ORDERDESC:
			{
				if(m_pDatabase==nullptr)
					pCmdUI->Enable(FALSE);
				else if(m_pDatabase->IsOpen()==FALSE)
					pCmdUI->Enable(FALSE);
				else if(m_pHeaderProfile==nullptr)
					pCmdUI->Enable(FALSE);
				else if(m_DBGrid.GetSelEndCol()==-1)
					pCmdUI->Enable(FALSE);
				else
					pCmdUI->Enable(TRUE);
				return TRUE;
			}
			break;
			case ID_GRID_STAT:
			{
				if(m_pDatabase==nullptr)
					pCmdUI->Enable(FALSE);
				else if(m_pDatabase->IsOpen()==FALSE)
					pCmdUI->Enable(FALSE);
				else if(m_pHeaderProfile==nullptr)
					pCmdUI->Enable(FALSE);
				else
				{
					const int selCol = m_DBGrid.GetSelEndCol();
					if(selCol!=-1)
						pCmdUI->Enable(TRUE);
					else
						pCmdUI->Enable(FALSE);
				}
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

DWORD CTDBGridCtrl::ActiveId()
{
	if(m_pDatabase==nullptr)
		return 0XFFFFFFFF;
	if(m_pDatabase->IsOpen()==FALSE)
		return 0XFFFFFFFF;
	if(m_pHeaderProfile==nullptr)
		return 0XFFFFFFFF;

	else
	{
		VARIANT varGeomID;
		varGeomID.vt = VT_BSTR;
		varGeomID.bstrVal = ::SysAllocString(m_pHeaderProfile->m_strIdField);

		CColumns cols = m_DBGrid.GetColumns();
		CColumn colGeomID = cols.GetItem(varGeomID);

		CString strGeomID = colGeomID.GetText();
		return _tcstoul(strGeomID, nullptr, 10);
	}
}

bool CTDBGridCtrl::ActivateRec(CString strIdField, DWORD dwId, BOOL bRemind)
{
	if(m_pDatabase==nullptr)
		return false;
	if(m_pDatabase->IsOpen()==FALSE)
		return false;

	C_Recordset rs = m_Adodc.GetRecordset();
	VARIANT origin = rs.GetBookmark();
	CString strCriteria;
	strCriteria.Format(_T("%s=%d"), strIdField, dwId);

	VARIANT var;
	var.vt = VT_I4;
	var.lVal = 0;
	rs.MoveFirst();
	rs.Find((LPCTSTR)strCriteria, 0, ADOCG::adSearchForward, var);
	if(rs.GetEof()==TRUE)
	{
		rs.SetBookmark(origin);
		if(bRemind==TRUE)
		{
			AfxMessageBox(_T("No matched record!"));
		}
		return false;
	}
	else
	{
		return true;
	}
}

BOOL CTDBGridCtrl::DestroyWindow()
{
	::SendMessage(GetParent()->GetParent()->m_hWnd, WM_MDIDESTROY, (WPARAM)m_hWnd, 0L);

	m_ToolBar.SetOwner(nullptr);

	if(m_pCheckBox!=nullptr)
	{
		m_pCheckBox->PostMessage(WM_DESTROY, 0, 0);
		m_pCheckBox->DestroyWindow();
		delete m_pCheckBox;
		m_pCheckBox = nullptr;
	}
	if(m_ToolBar.m_hWnd!=nullptr)
	{
		m_ToolBar.PostMessage(WM_DESTROY, 0, 0);
		m_ToolBar.DestroyWindow();
	}
	m_pDatabase = nullptr;
	m_DBGrid.SetDataSource(nullptr);
	//	m_DBGrid.BindProperty(0x9,nullptr);
	m_Adodc.SetRecordSource(_T(""));
	m_Adodc.SetConnectionString(_T(""));
	m_DBGrid.PostMessage(WM_DESTROY, 0, 0);
	m_DBGrid.DestroyWindow();
	m_Adodc.PostMessage(WM_DESTROY, 0, 0);
	m_Adodc.DestroyWindow();

	return CFrameWndEx::DestroyWindow();
}

//void CTDBGridCtrl::FilterChange()
//{
//	
//}
void CTDBGridCtrl::RowColChange(VARIANT* LastRow, short LastCol)
{
	DWORD nFlags = V_I4(LastRow);
	if(IsWindowVisible()==FALSE)
		return;

	if(m_pDatabase==nullptr)
		return;
	if(m_pDatabase->IsOpen()==FALSE)
		return;
	if(m_pHeaderProfile==nullptr)
		return;

	CFrameWnd* pAppFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
	if(pAppFrame==nullptr)
		return;

	VARIANT varId;
	varId.vt = VT_BSTR;
	varId.bstrVal = ::SysAllocString(m_pHeaderProfile->m_strIdField);

	try
	{
		CColumns cols = m_DBGrid.GetColumns();

		CColumn colID = cols.GetItem(varId);
		CString strID = colID.GetText();
		DWORD dwId = _tcstoul(strID, nullptr, 10);

		CPropertyPane* pPropertyPane = (CPropertyPane*)AfxGetMainWnd()->SendMessage(WM_GETPANE, 'I', 0);
		if(pPropertyPane!=nullptr&&pPropertyPane->IsVisible()==TRUE)
		{
			CAttributeCtrl* pAttributeCtrl = (CAttributeCtrl*)pPropertyPane->GetTabWnd('A');
			if(pAttributeCtrl!=nullptr)
			{
				pAttributeCtrl->OnTableChanged(m_pDatabase, m_pHeaderProfile->m_strKeyTable);
				pAttributeCtrl->ShowInfo(m_pHeaderProfile->m_strIdField,  dwId);
			}
		}
	}
	catch(CException*ex)
	{
		//		OutputDebugString(ex->m_strError);
		ex->Delete();
		return;
	}
}

void CTDBGridCtrl::Mouseup(short nButton, short nShiftState, long x, long y)
{
	int where = m_DBGrid.PointAt(x, -y);
	//	m_DBGrid.SetEditActive(TRUE);
}

void CTDBGridCtrl::FilterButtonClick(short ColIndex)
{
	C_Recordset rs = m_Adodc.GetRecordset();
	if(rs==nullptr)
		return;

	CString oldfilter = rs.GetFilter().bstrVal;
	CString newfilter = _T("");
	CColumns cols = m_DBGrid.GetColumns();
	for(int index = 0; index<cols.GetCount(); index++)
	{
		VARIANT var;
		var.vt = VT_I4;
		var.lVal = index;
		CColumn col = cols.GetItem(var);
		CString value = col.GetFilterText();
		if(value.IsEmpty()==TRUE)
			continue;

		value.Trim();
		if(value.IsEmpty()==TRUE)
		{
			col.SetFilterText(nullptr);
			continue;
		}
		CString field = col.GetDataField();
		if(field.IsEmpty()==TRUE)
			continue;

		SWORD sqltype = CODBCDatabase::GetFieldSQLType(m_pDatabase, m_pHeaderProfile->m_strKeyTable, field);
		CString stat = CODBCDatabase::SmartClause(sqltype, field, value);
		if(stat.IsEmpty())
			continue;

		newfilter += stat+_T(" AND ");
	}

	newfilter = newfilter.TrimRight(_T(" AND "));
	if(newfilter.CompareNoCase(oldfilter)!=0)
	{
		VARIANT var;
		var.vt = VT_BSTR;
		var.bstrVal = ::SysAllocString(newfilter);

		//	m_DBGrid.HoldFields();
		int col = m_DBGrid.GetCol();
		m_Adodc.GetRecordset().SetFilter(var);
		m_DBGrid.SetCol(col);
	}
}
