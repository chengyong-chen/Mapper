#include "stdafx.h"

#include "POUTDBGridCtrl.h"
#include "SQLDlg.h"
#include "Columns.h"
#include "Column.h"
#include "TablePane.h"

#include "../Geometry/Geom.h"

#include "../Viewer/Global.h"
#include "../Mapper/Global.h"

#include "../Dataview/Global.h"
#include "../Dataview/viewinfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPOUTDBGridCtrl dialog
CPOUTDBGridCtrl::CPOUTDBGridCtrl()
{
}

BEGIN_MESSAGE_MAP(CPOUTDBGridCtrl, CTDBGridCtrl)
	//{{AFX_MSG_MAP(CPOUTDBGridCtrl)
	ON_WM_CREATE()
	ON_COMMAND(ID_GRID_INSERT, OnInsert)
	ON_COMMAND(ID_GRID_QUERY, OnQuery)
	ON_COMMAND(ID_GRID_GEOM, OnGeom)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CPOUTDBGridCtrl::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nId, CCreateContext* pContext)
{
	if(CTDBGridCtrl::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nullptr, 0, pContext)==TRUE)
	{
		const HINSTANCE hInstOld = AfxGetResourceHandle();
		HINSTANCE hInst = ::LoadLibrary(_T("Information.dll"));
		AfxSetResourceHandle(hInst);

		m_ToolBar.RemoveButton(m_ToolBar.CommandToIndex(ID_GRID_GEOMRADIO));
		CMFCToolBarButton* pCheckBox = DYNAMIC_DOWNCAST(CMFCToolBarButton, m_ToolBar.GetButton(m_ToolBar.CommandToIndex(ID_GRID_GEOMRADIO)));
		if(pCheckBox!=nullptr)
		{
			pCheckBox->ResetImageToDefault();
			pCheckBox->SetStyle(TBBS_CHECKBOX);
			pCheckBox->m_strText = "������ͼ";
			pCheckBox->EnableWindow(TRUE);
		}
		m_ToolBar.AdjustLayout();
		m_ToolBar.AdjustSize();

		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
		return TRUE;
	}
	else
		return FALSE;
}

DWORD CPOUTDBGridCtrl::Position(CView* pView, double WPARAM, double LPARAM)
{
	if(m_pHeaderProfile==nullptr)
		return 0;

	VARIANT varLng;
	VARIANT varLat;
	VARIANT varId;
	varId.vt = VT_BSTR;
	varLng.vt = VT_BSTR;
	varLat.vt = VT_BSTR;
	varId.bstrVal = ::SysAllocString(m_pHeaderProfile->m_strIdField);
	varLng.bstrVal = ::SysAllocString(L"X");
	varLat.bstrVal = ::SysAllocString(L"Y");

	CColumns cols = m_DBGrid.GetColumns();
	CColumn colID = cols.GetItem(varId);
	CColumn colLng = cols.GetItem(varLng);
	CColumn colLat = cols.GetItem(varLat);

	varLng.vt = VT_R8;
	varLat.vt = VT_R8;
	varLng.dblVal = WPARAM*constants::radianTodegree;
	varLat.dblVal = LPARAM*constants::radianTodegree;

	colLng.SetValue(varLng);
	colLat.SetValue(varLat);

	CTDBGridCtrl::Update();
	Sleep(1000);

	varId = colID.GetValue();
	return varId.uintVal;
}

void CPOUTDBGridCtrl::OnGeom()
{
	VARIANT varLng;
	VARIANT varLat;
	varLng.vt = VT_BSTR;
	varLat.vt = VT_BSTR;
	varLng.bstrVal = ::SysAllocString(L"X");
	varLat.bstrVal = ::SysAllocString(L"Y");

	CColumns cols = m_DBGrid.GetColumns();
	CColumn colLng = cols.GetItem(varLng);
	CColumn colLat = cols.GetItem(varLat);
	CString strLng = colLng.GetText();
	CString strLat = colLat.GetText();

	CPointF geoPoint;
	geoPoint.x = _tcstod(strLng, nullptr);
	geoPoint.y = _tcstod(strLat, nullptr);
	if(geoPoint.x==0.0f&&geoPoint.y==0.0f)
		return;

	CString stringX;
	CString stringY;
	stringX.Format(_T("%.5f"), geoPoint.x);
	stringY.Format(_T("%.5f"), geoPoint.y);
	AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoX"), stringX);
	AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoY"), stringY);

	CFrameWnd* pMainFrame = (CFrameWnd*)AfxGetMainWnd();
	pMainFrame->SendMessage(WM_DESTROYHTML, 0, 0);

	CView* pView = nullptr;
	CFrameWnd* pChildFrame = (CFrameWnd*)pMainFrame->GetActiveFrame();
	if(pChildFrame!=nullptr)
	{
		pView = pChildFrame->GetActiveView();
		if(pView!=nullptr)
		{
			CRect rect;
			pView->GetClientRect(rect);
			CPoint cliPoint = rect.CenterPoint();
			CString stringX;
			CString stringY;
			stringX.Format(_T("%.5f"), geoPoint.x);
			stringY.Format(_T("%.5f"), geoPoint.y);
			AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoX"), stringX);
			AfxGetApp()->WriteProfileString(_T("NewView"), _T("GeoY"), stringY);
			AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliX"), cliPoint.x);
			AfxGetApp()->WriteProfileInt(_T("NewView"), _T("CliY"), cliPoint.y);
		}
	}

	pMainFrame->SendMessage(WM_SETGEOSIGN, (UINT)&geoPoint, 0);
	if(pView!=nullptr)
	{
		CMFCToolBarButton* pButton = m_ToolBar.GetButton(m_ToolBar.CommandToIndex(ID_GRID_GEOMRADIO));
		if(m_pCheckBox!=nullptr&&m_pCheckBox->GetCheck()==BST_UNCHECKED)
		{
			BOOL bIn = pView->SendMessage(WM_GEOPOINTINMAP, (UINT)&geoPoint, 0);
			if(bIn==TRUE)
			{
				CViewinfo* pViewinfo = (CViewinfo*)pView->SendMessage(WM_GETVIEWINFO, 0, 0);
				if(pViewinfo!=nullptr)
				{
					CPoint docPoint = pViewinfo->m_datainfo.GeoToDoc(geoPoint);
					Gdiplus::Point cliPoint = pViewinfo->DocToClient<Gdiplus::Point>(docPoint);

					CRect cliRect;
					pView->GetClientRect(cliRect);
					if(cliRect.PtInRect(CPoint(cliPoint.X, cliPoint.Y))==FALSE)
					{
						CPoint cliCenter = cliRect.CenterPoint();;
						pView->SendMessage(WM_FIXATEVIEW, (UINT)&docPoint, (LONG)&cliCenter);
					}
					else
					{
					}
					pMainFrame->SendMessage(WM_VIEWDRAWOVER, (UINT)pView, 0);
				}
			}
			else
				pMainFrame->SendMessage(WM_MATCHMAP, 0, (UINT)&geoPoint);
		}
		else
			pMainFrame->SendMessage(WM_MATCHMAP, 0, (UINT)&geoPoint);
	}
	else
		pMainFrame->SendMessage(WM_MATCHMAP, 0, (UINT)&geoPoint);
}

void CPOUTDBGridCtrl::OnQuery()
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
		CStringW strColumns = dlg.m_strColumns;
		const CStringW strTable = dlg.m_strTable;
		const CStringW strWhere = dlg.m_strWhere;

		if(strColumns.Find(_T('*'))==-1)
		{
			if(strColumns.Find(m_pHeaderProfile->m_strIdField)==-1||strColumns.Find(_T("X"))==-1||strColumns.Find(_T("Y"))==-1)
			{
				AfxMessageBox(_T("You haven't select ID,X and Y!"));
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
				((CTablePane*)pParent)->SQLQuery('P', m_pDatabase, strSQL, m_pHeaderProfile->m_strIdField);
			}
		}
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

void CPOUTDBGridCtrl::MarkField()
{
	if(m_pHeaderProfile == nullptr)
		return;
	
	try
	{
		VARIANT index;
		index.vt = VT_BSTR;
		index.bstrVal = ::SysAllocString(m_pHeaderProfile->m_strIdField);
		CColumns cols = m_DBGrid.GetColumns();
		CColumn col = cols.GetItem(index);
		if(CTDBGridCtrl::m_bEditable==true)
		{
			col.SetBackColor(0X00DDDDDD);
			//	col.SetForeColor(0X00FFFF);
			col.SetWidth(60);
			col.SetAllowFocus(FALSE);
			SysFreeString(index.bstrVal);
		}
		else
		{
			col.SetVisible(FALSE);
		}
		col.SetLocked(true);

		index.vt = VT_BSTR;
		index.bstrVal = ::SysAllocString(L"X");
		cols = m_DBGrid.GetColumns();
		col = cols.GetItem(index);
		if(CTDBGridCtrl::m_bEditable==true)
		{
			col.SetBackColor(0X00DDDDDD);
			//		col.SetForeColor(0X00FFFF);
			col.SetWidth(60);
			//		col.SetAllowSizing(false);
			//		col.SetAllowFocus(false);
			SysFreeString(index.bstrVal);
		}
		else
		{
			col.SetVisible(FALSE);
		}
		col.SetLocked(true);

		index.vt = VT_BSTR;
		index.bstrVal = ::SysAllocString(L"Y");
		cols = m_DBGrid.GetColumns();
		col = cols.GetItem(index);
		if(CTDBGridCtrl::m_bEditable==true)
		{
			col.SetBackColor(0X00DDDDDD);
			//		col.SetForeColor(0X00FFFF);
			//		col.SetWidth(55);
			//		col.AutoSize();
			//		col.SetAllowSizing(false);
			//		col.SetAllowFocus(false);
			SysFreeString(index.bstrVal);
		}
		else
		{
			col.SetVisible(FALSE);
		}
		col.SetLocked(true);
	}
	catch(CException*ex)
	{
		//		OutputDebugString(ex->m_strError);
		ex->Delete();
		return;
	}
}

void CPOUTDBGridCtrl::OnInsert()
{
	if(m_pDatabase!=nullptr&&m_pDatabase->IsOpen()==TRUE&&m_pHeaderProfile!=nullptr)
	{
		CString strSQL;
		strSQL.Format(_T("Insert Into [%s] (X,Y) VALUES(%g,%g)"), m_pHeaderProfile->m_strKeyTable, 0.0f, 0.0f);

		m_pDatabase->ExecuteSQL(strSQL);
	}

	CTDBGridCtrl::OnInsert();
}

BOOL CPOUTDBGridCtrl::OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
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
				else
				{
					try
					{
						VARIANT varLng;
						VARIANT varLat;
						varLng.vt = VT_BSTR;
						varLat.vt = VT_BSTR;
						varLng.bstrVal = ::SysAllocString(L"X");
						varLat.bstrVal = ::SysAllocString(L"Y");

						CColumns cols = m_DBGrid.GetColumns();
						CColumn colLng = cols.GetItem(varLng);
						CColumn colLat = cols.GetItem(varLat);
						CString strLng = colLng.GetText();
						CString strLat = colLat.GetText();

						double x = _tcstod(strLng, nullptr);
						double y = _tcstod(strLat, nullptr);
						if(x==0.0f||y==0.0f)
							pCmdUI->Enable(FALSE);
						else
							pCmdUI->Enable(TRUE);
					}
					catch(CException* ex)
					{
						OutputDebugStringA("Error happened when activate POU record!");
						//	OutputDebugString(_T("Error when export data to pdf "));
						ex->Delete();
					}
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
