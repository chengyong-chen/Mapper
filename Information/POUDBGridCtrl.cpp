#include "stdafx.h"
#include "POUDBGridCtrl.h"
#include "resource.h"

#include "Column.h"
#include "Columns.h"

#include "../Viewer/Global.h"
#include "../Public/Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CPOUDBGridCtrl, CDBGridCtrl)

CPOUDBGridCtrl::CPOUDBGridCtrl()
{
}

BEGIN_MESSAGE_MAP(CPOUDBGridCtrl, CDBGridCtrl)
	//{{AFX_MSG_MAP(CPOUDBGridCtrl)
	ON_COMMAND(ID_GRID_GEOM, OnGeometry)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPOUDBGridCtrl message handlers
void CPOUDBGridCtrl::PolyFilter(CPointF* pPoints, int nAnchors)
{
	/*
	 if(m_pDatabase == nullptr)
	 return;
	 if(m_pDatabase->IsOpen() == FALSE)
	 return;
	 if(m_nTableType != 2)
	 return;

	 if(pPoints == nullptr)
	 return;

	 CString strConnect = m_Rdc.GetConnect();
	 CString strOldSQL = m_Rdc.GetSql();
	 CString strSQL = strOldSQL.Left(strOldSQL.Find(_T("FROM")));
	 strSQL += _T("INTO Query ");
	 strSQL += strOldSQL.Mid(strOldSQL.Find(_T("FROM")));

	 m_Rdc.SetConnect(_T(""));
	 m_Rdc.SetSql(_T(""));
	 m_Rdc.Refresh();

	 try
	 {
	 m_pDatabase->ExecuteSQL(_T("DROP Table Query"));
	 }
	 catch(CDBException* ex)
	 {
		 ex->ReportError();
		 ex->Delete();
	 }
	 catch(CException* ex)
	{
		ex->Delete();
	}

	 try
	 {
	 m_pDatabase->ExecuteSQL(strSQL);

	 CRecordset rs(m_pDatabase);
	 rs.Open(CRecordset::dynaset,_T("Select * From QUERY"));//,CRecordset::none);
	 while(rs.IsEOF() == FALSE)
	 {
	 CPointF geoPoint;

	 CString strValue;
	 rs.GetFieldValue(_T("X"),strValue);
	 geoPoint.x = _tcstod(strValue,nullptr);
	 if(geoPoint.x == 0.0f)
	 {
	 rs.MoveNext();
	 continue;
	 }

	 rs.GetFieldValue(_T("Y"),strValue);
	 geoPoint.y = _tcstod(strValue,nullptr);
	 if(geoPoint.y == 0.0f)
	 {
	 rs.MoveNext();
	 continue;
	 }
	 if(PointInPolygon(geoPoint,pPoints,nAnchors) == false)
	 {
	 rs.Delete();
	 }

	 rs.MoveNext();
	 }
	 rs.
	 rs.Close();
	 }
	 catch(CDBException* ex)
	 {
	 ex->ReportError();
	 ex->Delete();
	 }
	 catch(CException* ex)
	{
		ex->Delete();
	}
	 m_Rdc.SetConnect(strConnect);
	 m_Rdc.SetSql(_T("Select * From QUERY"));
	 m_Rdc.Refresh();
	 MarkField();
	 */
}

void CPOUDBGridCtrl::PolyFilter(CPoint* pPoints, int nAnchors)
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

void CPOUDBGridCtrl::OnGeometry()
{
	if(IsWindowVisible()==FALSE)
		return;

	if(m_Rdc.GetConnect().IsEmpty()==TRUE)
		return;

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

	CFrameWnd* pMainFrame = (CFrameWnd*)AfxGetMainWnd();
	CView* pView1 = nullptr;
	CFrameWnd* pChildFrame = (CFrameWnd*)pMainFrame->GetActiveFrame();
	if(pChildFrame!=nullptr)
	{
		pView1 = pChildFrame->GetActiveView();
		if(pView1!=nullptr)
		{
			CRect rect;
			pView1->GetClientRect(rect);
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
	pMainFrame->SendMessage(WM_MATCHMAP, 0, (UINT)&geoPoint);
}

void CPOUDBGridCtrl::MarkField()
{
	CColumns cols = m_DBGrid.GetColumns();

	VARIANT index;
	index.vt = VT_BSTR;
	index.bstrVal = ::SysAllocString(m_strIdField);
	CColumn col = cols.GetItem(index);
	if(m_keyVisible==true)
	{
		col.SetWidth(60);
		col.SetBackColor(0X00DDDDDD);
		//	col.SetForeColor(0X00FFFF);
		//	col.SetAllowFocus(FALSE);
		SysFreeString(index.bstrVal);
	}
	else
	{
		col.SetVisible(FALSE);
	}

	index.vt = VT_BSTR;
	index.bstrVal = ::SysAllocString(L"X");
	cols = m_DBGrid.GetColumns();
	col = cols.GetItem(index);
	if(m_keyVisible==true)
	{
		col.SetWidth(60);
		col.SetBackColor(0X00DDDDDD);
		//	col.SetForeColor(0X00FFFF);
		//	col.SetAllowFocus(FALSE);
		SysFreeString(index.bstrVal);
	}
	else
	{
		col.SetVisible(FALSE);
	}

	index.vt = VT_BSTR;
	index.bstrVal = ::SysAllocString(L"Y");
	cols = m_DBGrid.GetColumns();
	col = cols.GetItem(index);
	if(m_keyVisible==true)
	{
		//		col.SetWidth(55);
		col.SetBackColor(0X00DDDDDD);
		//		col.SetForeColor(0X00FFFF);
		//		col.SetAllowFocus(FALSE);
		SysFreeString(index.bstrVal);
	}
	else
	{
		col.SetVisible(FALSE);
	}
}

BOOL CPOUDBGridCtrl::OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if(pHandlerInfo==nullptr)
	{
		if(nCode==CN_UPDATE_COMMAND_UI)
		{
			CCmdUI* pCmdUI = (CCmdUI*)pExtra;
			if(pCmdUI->m_bContinueRouting)
			{
				return CDBGridCtrl::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
			}

			ASSERT(!pCmdUI->m_bContinueRouting);

			switch(nId)
			{
			case ID_GRID_GEOM:
			{
				if(m_pDatabase==nullptr)
					pCmdUI->Enable(FALSE);
				else if(m_pDatabase->IsOpen()==FALSE)
					pCmdUI->Enable(FALSE);
				else if(m_strTable.IsEmpty()==TRUE)
					pCmdUI->Enable(FALSE);
				else
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
				return TRUE;
			}
			break;
			default:
				break;
			}
			pCmdUI->m_bContinueRouting = false;
		}
	}

	return CDBGridCtrl::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
}
