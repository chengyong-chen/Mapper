#include "stdafx.h"
#include "ATTTDBGridCtrl.h"
#include "SQLDlg.h"

#include "Columns.h"
#include "Column.h"
#include "Global.h"
#include "TablePane.h"

#include "../Public/ODBCDatabase.h"

#include "../Geometry/Geom.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CATTTDBGridCtrl dialog

CATTTDBGridCtrl::CATTTDBGridCtrl()
{
}

BEGIN_MESSAGE_MAP(CATTTDBGridCtrl, CTDBGridCtrl)
	//{{AFX_MSG_MAP(CATTTDBGridCtrl)
	ON_WM_CREATE()
	ON_COMMAND(ID_GRID_INSERT, OnInsert)
	ON_COMMAND(ID_GRID_QUERY, OnQuery)
	ON_COMMAND(ID_GRID_GEOM, OnGeom)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CATTTDBGridCtrl::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nId, CCreateContext* pContext)
{
	if(CTDBGridCtrl::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nullptr, 0, pContext)==TRUE)
	{
		const HINSTANCE hInstOld = AfxGetResourceHandle();
		HINSTANCE hInst = ::LoadLibrary(_T("Information.dll"));
		AfxSetResourceHandle(hInst);

		m_ToolBar.RemoveButton(m_ToolBar.CommandToIndex(ID_GRID_GEOMRADIO));
		m_ToolBar.AdjustLayout();
		m_ToolBar.AdjustSize();
		m_ToolBar.AdjustSizeImmediate();

		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
		return TRUE;
	}
	else
		return FALSE;
}

void CATTTDBGridCtrl::OnGeom()
{
	if(m_pHeaderProfile==nullptr)
		return;

	VARIANT varGeomId;
	varGeomId.vt = VT_BSTR;
	varGeomId.bstrVal = ::SysAllocString(m_pHeaderProfile->m_strIdField);

	CColumns cols = m_DBGrid.GetColumns();
	CColumn colID = cols.GetItem(varGeomId);

	CString strId = colID.GetText();
	DWORD dwId = _tcstoul(strId, nullptr, 10);

	CFrameWnd* pAppFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd* pCldFrame = (CMDIChildWnd*)pAppFrame->GetActiveFrame();
	CView* pView = (CView*)pCldFrame->GetActiveView();
	if(pView==nullptr)
		return;

	pView->SendMessage(WM_HIGHLIGHTATT, (UINT)(&m_pHeaderProfile->m_strKeyTable), dwId);
}

void CATTTDBGridCtrl::OnUpdateGeom(CCmdUI* pCmdUI)
{
	if(m_pDatabase==nullptr||m_pDatabase->IsOpen()==FALSE||m_pHeaderProfile==nullptr)
	{
		pCmdUI->Enable(FALSE);
	}
	else
	{
		CColumns cols = m_DBGrid.GetColumns();
		for(int index = 0; index<cols.GetCount(); index++)
		{
			VARIANT var;
			var.vt = VT_I4;
			var.lVal = index;
			CColumn col = cols.GetItem(var);
			CString strField = col.GetDataField();
			if(strField==m_pHeaderProfile->m_strIdField)
			{
				CString strId = col.GetText();
				DWORD dwId = _tcstoul(strId, nullptr, 10);
				if(dwId==0)
					pCmdUI->Enable(FALSE);
				else
					pCmdUI->Enable(TRUE);
			}
		}
	}
}

void CATTTDBGridCtrl::OnQuery()
{
	if(m_pDatabase==nullptr)
		return;
	if(m_pDatabase->IsOpen()==FALSE)
		return;
	if(m_pHeaderProfile==nullptr)
		return;
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Information.dll"));
	AfxSetResourceHandle(hInst);

	CSQLDlg dlg(this, m_pDatabase, m_pHeaderProfile->m_strKeyTable);
	if(dlg.DoModal()==IDOK)
	{
		CString strColumns = dlg.m_strColumns;
		const CString strTable = dlg.m_strTable;
		const CString strWhere = dlg.m_strWhere;

		if(strColumns.Find(_T('*'))==-1)
		{
			if(strColumns.Find(m_pHeaderProfile->m_strIdField)==-1)
			{
				AfxMessageBox(_T("You haven's select the ID field!"));
				strColumns = '*';
			}
		}

		CString strSQL;
		if(strWhere.IsEmpty()==FALSE)
			strSQL.Format(_T("Select %s FROM [%s] WHERE %s"), strColumns, strTable, strWhere);
		else
			strSQL.Format(_T("Select %s FROM [%s]"), strColumns, strTable);

		CWnd* pParent = this->GetParent();
		if(pParent!=nullptr)
		{
			pParent = pParent->GetParent();
			if(pParent!=nullptr)
			{
				((CTablePane*)pParent)->SQLQuery('A', m_pDatabase, strSQL, m_pHeaderProfile->m_strIdField);
			}
		}
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

void CATTTDBGridCtrl::MarkField()
{
	VARIANT index;
	index.vt = VT_BSTR;
	index.bstrVal = ::SysAllocString(m_pHeaderProfile->m_strIdField);
	CColumns cols = m_DBGrid.GetColumns();
	CColumn col = cols.GetItem(index);
	if(CTDBGridCtrl::m_bEditable==true)
	{
		col.SetBackColor(0X00DDDDDD);
		//		col.SetForeColor(0X00FFFF);
		col.SetWidth(60);
		col.SetAllowFocus(FALSE);
		SysFreeString(index.bstrVal);
	}
	else
	{
		col.SetVisible(FALSE);
	}
}

void CATTTDBGridCtrl::OnInsert()
{
	if(m_pHeaderProfile==nullptr)
		return;

	CString strSQL;
	strSQL.Format(_T("Insert Into [%s] default values"), m_pHeaderProfile->m_strKeyTable, 0);
	if(m_pDatabase!=nullptr)
	{
		CODBCDatabase::ParseSQL(m_pDatabase, strSQL);
		m_pDatabase->ExecuteSQL(strSQL);
	}

	CTDBGridCtrl::OnInsert();
}

BOOL CATTTDBGridCtrl::OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if(pHandlerInfo==nullptr)
	{
		if(nCode==CN_UPDATE_COMMAND_UI)
		{
			CCmdUI* pCmdUI = (CCmdUI*)pExtra;
			if(pCmdUI->m_bContinueRouting)
			{
				return CTDBGridCtrl::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
			}

			ASSERT(!pCmdUI->m_bContinueRouting);
			switch(nId)
			{
			case ID_GRID_GEOM:
			{
				if(m_pDatabase==nullptr||m_pDatabase->IsOpen()==FALSE||m_pHeaderProfile==nullptr)
				{
					pCmdUI->Enable(FALSE);
				}
				else// this slow down the whole programm
				{
					//CColumns cols = m_DBGrid.GetColumns();
					//for(int index=0;index<cols.GetCount();index++)
					//{
					//	VARIANT var;
					//	var.vt = VT_I4;
					//	var.lVal = index;
					//	CColumn col = cols.GetItem(var);
					//	CString strField = col.GetDataField();
					//	if(strField ==))
					//	{
					//		CString strId = col.GetText();
					//		DWORD dwId = _tcstoul(strId,nullptr,10);
					//		if(dwId == 0)
					//			pCmdUI->Enable(FALSE);
					//		else
					//			pCmdUI->Enable(TRUE);
					//	}
					//}
				}
				return TRUE;
			}
			break;
			case ID_GRID_QUERY:
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
			case ID_GRID_INSERT:
			{
				if(m_pDatabase==nullptr)
					pCmdUI->Enable(FALSE);
				else if(m_pDatabase->IsOpen()==FALSE)
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
	}

	return CTDBGridCtrl::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
}

BOOL CATTTDBGridCtrl::DestroyWindow()
{
	return CTDBGridCtrl::DestroyWindow();
}
