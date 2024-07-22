#include "stdafx.h"
#include "resource.h"

#include "columns.h"
#include "column.h"
#include "Global.h"

#include "ATTDBGridCtrl.h"

#include "../Geometry/Geom.h"

#include "../Mapper/Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CAATDBGridCtrl, CDBGridCtrl)

CAATDBGridCtrl::CAATDBGridCtrl()
{
}

BEGIN_MESSAGE_MAP(CAATDBGridCtrl, CDBGridCtrl)
	//{{AFX_MSG_MAP(CAATDBGridCtrl)
	ON_COMMAND(ID_GRID_GEOMALL, OnGeomAll)
	ON_COMMAND(ID_GRID_GEOM, OnGeom)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAATDBGridCtrl message handlers

void CAATDBGridCtrl::PolyFilter(CPointF* pPoints, int nAnchors)
{/*
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

 m_Rdc.SetConnect(strConnect);
 m_Rdc.SetSql(_T("Select * From QUERY"));
 m_Rdc.Refresh();
 MarkField();*/
}

void CAATDBGridCtrl::PolyFilter(CPoint* pPoints, int nAnchors)
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

void CAATDBGridCtrl::OnGeomAll()
{
	if(IsWindowVisible()==FALSE)
		return;

	if(m_pDatabase==nullptr)
		return;
	if(m_pDatabase->IsOpen()==FALSE)
		return;
	if(m_strTable.IsEmpty()==TRUE)
		return;

	CFrameWnd* pMainFrame = (CFrameWnd*)AfxGetMainWnd();
	CFrameWnd* pChildFrame = (CFrameWnd*)pMainFrame->GetActiveFrame();
	CView* pView = pChildFrame->GetActiveView();
	if(pView==nullptr)
		return;

	CClientDC dc(pView);
	CString strConnect = m_Rdc.GetConnect();

	CRecordset rs(m_pDatabase);
	try
	{
		const CString strSQL = m_Rdc.GetSql();
		rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly);
		while(rs.IsEOF()==FALSE)
		{
			CString strGeomID;
			rs.GetFieldValue(m_strIdField, strGeomID);
			const DWORD dwGeom = _ttol(strGeomID);;
			pView->SendMessage(WM_ACTIVEGEOM, 0, dwGeom);

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
}

void CAATDBGridCtrl::OnGeom()
{
	if(IsWindowVisible()==FALSE)
		return;

	if(m_Rdc.GetConnect().IsEmpty()==TRUE)
		return;

	VARIANT varId;
	varId.vt = VT_BSTR;
	varId.bstrVal = ::SysAllocString(m_strIdField);

	CColumns cols = m_DBGrid.GetColumns();
	CColumn colID = cols.GetItem(varId);

	CString strID = colID.GetText();
	DWORD dwId = _tcstoul(strID, nullptr, 10);

	CFrameWnd* pAppFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
	if(pAppFrame==nullptr)
		return;
	CMDIChildWnd* pCldFrame = (CMDIChildWnd*)pAppFrame->GetActiveFrame();
	if(pCldFrame==nullptr)
		return;
	CView* pView = (CView*)pCldFrame->GetActiveView();
	if(pView==nullptr)
		return;

	pView->SendMessage(WM_HIGHLIGHTATT, (UINT)(&m_strTable), dwId);
}

void CAATDBGridCtrl::MarkField(CString field)
{
	if(field.IsEmpty())
		return;

	try
	{
		VARIANT index;
		index.vt = VT_BSTR;
		index.bstrVal = ::SysAllocString(field);
		CColumns cols = m_DBGrid.GetColumns();
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
	}
	catch(CException*ex)
	{
		//		OutputDebugString();
		ex->Delete();
	}
}

BOOL CAATDBGridCtrl::OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
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
			case ID_GRID_GEOMALL:
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
					VARIANT varId;
					varId.vt = VT_BSTR;
					varId.bstrVal = ::SysAllocString(m_strIdField);

					CColumns cols = m_DBGrid.GetColumns();
					CColumn colID = cols.GetItem(varId);
					CString strID = colID.GetText();
					DWORD dwId = _tcstoul(strID, nullptr, 10);
					if(dwId==0)
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
		if(nCode==CN_COMMAND)
		{
		}
	}

	return CDBGridCtrl::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
}
