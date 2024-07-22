#include "stdafx.h"

#include "Vehicle.h"
#include "Port.h"
#include "ReplayTable.h"
#include "ReplayDlg.h"

#include "../Viewer/Global.h"
#include "../Coding/Instruction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern LPCTSTR lpszWarningTypes[];

IMPLEMENT_DYNAMIC(CReplayTable, CReplay)

CReplayTable::CReplayTable(CFrameWnd* pParents)
	: CReplay(pParents)
{
}

bool CReplayTable::Open()
{
	CPort::Open();

	if(m_database.IsOpen()==FALSE)
		return false;

	HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Navigate.dll"));
	AfxSetResourceHandle(hInst);

	static CString strSQL;
	CReplayDlg dlg(nullptr, this);
	if(dlg.DoModal()==IDOK)
	{
		m_dwId = dlg.m_dwVehicle;
		CString string1;
		CString string2;
		string1.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"), dlg.m_Date1.GetYear(), dlg.m_Date1.GetMonth(), dlg.m_Date1.GetDay(), dlg.m_Time1.GetHour(), dlg.m_Time1.GetMinute(), dlg.m_Time1.GetSecond());
		string2.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"), dlg.m_Date2.GetYear(), dlg.m_Date2.GetMonth(), dlg.m_Date2.GetDay(), dlg.m_Time2.GetHour(), dlg.m_Time2.GetMinute(), dlg.m_Time2.GetSecond());
		strSQL.Format(_T("SELECT GPS时间,经度,纬度,速度,高度,方向,状态,有效性 FROM RecvPos WHERE VehicleID=%d And GPS时间>{Ts '%s'} And GPS时间<{Ts '%s' } ORDER BY GPS时间"), m_dwId, string1, string2);

		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
	}
	else
	{
		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
		return false;
	}

	POSITION pos = RSlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CRS* RS = (CRS*)RSlist.GetNext(pos);
		if(RS->dwId==m_dwId)
		{
			AfxMessageBox(_T("软件正在回放此车的历史轨迹!"));
			return false;
		}
	}

	Content content;
	content.dwId = m_dwId;
	content.strUser = _T("");
	content.strCode = _T("");

	CRecordset rs(&m_database);
	CString strSQL3;
	strSQL3.Format(_T("SELECT DISTINCT 姓名,车牌号码 FROM %s WHERE ID=%d"), _T("View_Vehicle"), m_dwId);
	try
	{
		rs.Open(CRecordset::snapshot, strSQL3, CRecordset::readOnly);
		if(!rs.IsEOF())
		{
			CString strValue;

			rs.GetFieldValue(_T("姓名"), strValue);
			strValue.TrimRight(_T(" "));
			content.strUser = strValue;
			rs.GetFieldValue(_T("车牌号码"), strValue);
			strValue.TrimRight(_T(" "));
			content.strCode = strValue;
			m_strCode = strValue;
		}

		rs.Close();
	}
	catch(CDBException*ex)
	{
		ex->ReportError();
		ex->Delete();
	}

	if(dlg.m_bHashing==FALSE)
	{
		CFrameWnd* pAppFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
		if(pAppFrame->SendMessage(WM_VEHICLECONTROL, GPSSERVER_VEHICLE_NEW, (long)&content)==TRUE)
		{
			CRS* RS = new CRS;
			RS->dwId = m_dwId;
			RS->rs = new CRecordset(&m_database);
			try
			{
				RS->rs->Open(CRecordset::forwardOnly, strSQL, CRecordset::readOnly);
				RSlist.AddTail(RS);

				if(dlg.m_bHandle==TRUE)
				{
					pAppFrame->SendMessage(WM_VEHICLECONTROL, GPSSERVER_VEHICLE_TRACK, RS->dwId);
				}

				long nReplaySpeed = AfxGetApp()->GetProfileInt(_T("Socket"), _T("ReplaySpeed"), 200);
				pAppFrame->SetTimer(TIMER_REPLAY, nReplaySpeed, nullptr);
			}
			catch(CDBException*ex)
			{
				ex->ReportError();
				delete RS;
				RS = nullptr;
				ex->Delete();
			}
		}
	}
	else
	{
		POSITION pos2 = Hashinglist.GetHeadPosition();
		while(pos2!=nullptr)
		{
			void* object = Hashinglist.GetNext(pos2);
			if(object!=nullptr)
			{
				delete object;
				object = nullptr;
			}
		}
		Hashinglist.RemoveAll();

		if(m_imagelist1.m_hImageList==nullptr)
		{
			HINSTANCE hInstOld = AfxGetResourceHandle();
			HINSTANCE hInst = ::LoadLibrary(_T("Navigate.dll"));
			AfxSetResourceHandle(hInst);

			int nIcon1 = AfxGetApp()->GetProfileInt(_T("Navigate"), _T("Icon1"), 1);
			switch(nIcon1)
			{
			case 1:
				m_imagelist1.Create(IDB_VEHICLE1, 32, 0, RGB(255, 0, 255));
				break;
			case 2:
				m_imagelist1.Create(IDB_VEHICLE2, 32, 0, RGB(255, 0, 255));
				break;
			case 3:
				m_imagelist1.Create(IDB_VEHICLE3, 32, 0, RGB(255, 0, 255));
				break;
			case 4:
				m_imagelist1.Create(IDB_VEHICLE4, 32, 0, RGB(255, 0, 255));
				break;
			default:
				m_imagelist1.Create(IDB_VEHICLE1, 32, 0, RGB(255, 0, 255));
				break;
			}

			AfxSetResourceHandle(hInstOld);
			::FreeLibrary(hInst);
		}
		if(m_imagelist2.m_hImageList==nullptr)
		{
			HINSTANCE hInstOld = AfxGetResourceHandle();
			HINSTANCE hInst = ::LoadLibrary(_T("Navigate.dll"));
			AfxSetResourceHandle(hInst);

			int nIcon2 = AfxGetApp()->GetProfileInt(_T("Navigate"), _T("Icon2"), 2);
			switch(nIcon2)
			{
			case 1:
				m_imagelist2.Create(IDB_VEHICLE1, 32, 0, RGB(255, 0, 255));
				break;
			case 2:
				m_imagelist2.Create(IDB_VEHICLE2, 32, 0, RGB(255, 0, 255));
				break;
			case 3:
				m_imagelist2.Create(IDB_VEHICLE3, 32, 0, RGB(255, 0, 255));
				break;
			case 4:
				m_imagelist2.Create(IDB_VEHICLE4, 32, 0, RGB(255, 0, 255));
				break;
			default:
				m_imagelist2.Create(IDB_VEHICLE1, 32, 0, RGB(255, 0, 255));
				break;
			}

			AfxSetResourceHandle(hInstOld);
			::FreeLibrary(hInst);
		}
		/*
				CString string1;
				CString string2;
				string1.Format(_T("%d-%d-%d %d:%d:%d"),dlg.m_Date1.GetYear(),dlg.m_Date1.GetMonth(),dlg.m_Date1.GetDay(),dlg.m_Time1.GetHour(),dlg.m_Time1.GetMinute(),dlg.m_Time1.GetSecond());
				string2.Format(_T("%d-%d-%d %d:%d:%d"),dlg.m_Date2.GetYear(),dlg.m_Date2.GetMonth(),dlg.m_Date2.GetDay(),dlg.m_Time2.GetHour(),dlg.m_Time2.GetMinute(),dlg.m_Time2.GetSecond());
				CString strSQL;
				strSQL.Format(_T("Select MaxLon(经度) As myMaxLon,MinLon(经度) As myMinLon MaxLat(纬度) As myMaxLat,MinLat(纬度) As myMinLat FROM [%s]  WHERE ID = %d And 时间>{Ts '%s'} And 时间<{Ts '%s'},Order by 时间"),_T("PositionTable"),dlg.m_dwId,string1,string2);

				CRecordset rs(&m_database);
				try
				{
					 rs.Open(CRecordset::forwardOnly,strSQL,CRecordset::readOnly);

					double fMaxLon = 0;
					double fMinLon = 0;
					double fMaxLat = 0;
					double fMinLat = 0;
					  if(rs.IsEOF() == FALSE)
					{
						CString strValue;
						rs.GetFieldValue(_T("myMaxLon"),strValue);
						fMaxLon = _ttof(strValue);
						rs.GetFieldValue(_T("myMinLon"),strValue);
						fMinLon = _ttof(strValue);
						rs.GetFieldValue(_T("myMaxLat"),strValue);
						fMaxLat = _ttof(strValue);
						rs.GetFieldValue(_T("myMinLat"),strValue);
						fMinLat = _ttof(strValue);
					}
					rs.Close();
				}
				catch(CDBException* ex)
				{
					ex->ReportError();
					ex->Delete();
				}
		*/
		CRecordset rs(&m_database);
		try
		{
			TRACE(strSQL);
			// AfxMessageBox(strSQL);
			rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly);
			while(rs.IsEOF()==FALSE)
			{
				SMessage* message = new SMessage();
				Hashinglist.AddTail(message);

				CString strValue;
				CDBVariant varValue;
				rs.GetFieldValue(_T("时间"), varValue);
				if(varValue.m_pdate!=nullptr)
				{
					message->nTime = varValue.m_pdate->month*100000000;
					message->nTime += varValue.m_pdate->day*1000000;
					message->nTime += varValue.m_pdate->hour*10000;
					message->nTime += varValue.m_pdate->minute*100;
				}
				message->lng = _ttoi(strValue);
				rs.GetFieldValue(_T("经度"), strValue);
				message->lng = _tcstod(strValue, nullptr);
				rs.GetFieldValue(_T("纬度"), strValue);
				message->lat = _tcstod(strValue, nullptr);
				rs.GetFieldValue(_T("速度"), strValue);
				message->fSpeed = _tcstod(strValue, nullptr);
				rs.GetFieldValue(_T("方向"), strValue);
				message->nBearing = _tcstod(strValue, nullptr);
				rs.GetFieldValue(_T("状态"), strValue);
				message->dwWarning = _tcstod(strValue, nullptr);
				message->m_bAvailable = TRUE;

				rs.MoveNext();
			}

			rs.Close();

			CFrameWnd* pMainChild = (CFrameWnd*)AfxGetMainWnd()->SendMessage(WM_GETMAINCHILD, 0, 0);
			CWnd* pWnd = pMainChild->GetActiveView();
			if(pWnd!=nullptr)
				pWnd->Invalidate(TRUE);
		}
		catch(CDBException*ex)
		{
			ex->ReportError();
			ex->Delete();
		}
	}

	return true;
}

void CReplayTable::ReceiveMsg()
{
	if(m_database.IsOpen()==TRUE&&RSlist.GetCount()>0)
	{
		POSITION pos1 = RSlist.GetHeadPosition();
		POSITION pos2;
		while((pos2 = pos1)!=nullptr)
		{
			CRS* RS = (CRS*)RSlist.GetNext(pos1);
			if(RS->rs==nullptr)
				continue;
			if(RS->rs->IsOpen()==FALSE)
				continue;

			if(RS->rs->IsEOF()==FALSE)
			{
				try
				{
					CString strValue;

					CDBVariant varValue;
					RS->rs->GetFieldValue(_T("时间"), varValue);
					if(varValue.m_pdate!=nullptr)
					{
						m_message.nTime = varValue.m_pdate->month*100000000;
						m_message.nTime += varValue.m_pdate->day*1000000;
						m_message.nTime += (varValue.m_pdate->hour-4)*10000;
						m_message.nTime += varValue.m_pdate->minute*100;
					}

					RS->rs->GetFieldValue(_T("经度"), strValue);
					m_message.lng = _tcstod(strValue, nullptr);
					RS->rs->GetFieldValue(_T("纬度"), strValue);
					m_message.lat = _tcstod(strValue, nullptr);
					RS->rs->GetFieldValue(_T("速度"), strValue);
					m_message.fSpeed = _tcstod(strValue, nullptr);
					RS->rs->GetFieldValue(_T("海拔"), strValue);
					m_message.altitude = _tcstod(strValue, nullptr);
					RS->rs->GetFieldValue(_T("方向"), strValue);
					m_message.nBearing = _ttoi(strValue);
					RS->rs->GetFieldValue(_T("状态"), strValue);
					const DWORD dwAlarm = _ttoi(strValue);
					m_message.dwWarning = dwAlarm;
					RS->rs->GetFieldValue(_T("有效性"), strValue);
					m_message.m_bAvailable = TRUE; // _ttoi(strValue);
					m_message.m_dwVehicle = RS->dwId;
					m_message.m_wCommand = WM_VEHICLEMOVE;

					RS->rs->MoveNext();

					const CFrameWnd* pAppFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
					pAppFrame->SendMessage(WM_VEHICLEMOVE, RS->dwId, (LONG)&m_message);
				}
				catch(CDBException*ex)
				{
					CFrameWnd* pAppFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
					AfxGetMainWnd()->SendMessage(WM_VEHICLECONTROL, GPSSERVER_VEHICLE_EREASE, RS->dwId);
					Erease(RS->dwId);

					Close();
					ex->Delete();
				}
			}
			else
			{
				CFrameWnd* pAppFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
				AfxGetMainWnd()->SendMessage(WM_VEHICLECONTROL, GPSSERVER_VEHICLE_EREASE, RS->dwId);
				Erease(RS->dwId);
			}
		}
	}
	else if(Hashinglist.GetCount()<1)
		Close();
}

void CReplayTable::Close()
{
	CReplay::Close();

	POSITION pos1 = RSlist.GetHeadPosition();
	while(pos1!=nullptr)
	{
		CRS* RS = (CRS*)RSlist.GetNext(pos1);
		CFrameWnd* pAppFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
		AfxGetMainWnd()->SendMessage(WM_VEHICLECONTROL, GPSSERVER_VEHICLE_EREASE, RS->dwId);
	}

	POSITION pos2 = RSlist.GetHeadPosition();
	while(pos2!=nullptr)
	{
		const CRS* RS = (CRS*)RSlist.GetNext(pos2);
		delete RS;
	}
	RSlist.RemoveAll();

	POSITION Pos3 = Hashinglist.GetHeadPosition();
	while(Pos3!=nullptr)
	{
		const void* object = Hashinglist.GetNext(Pos3);
		delete object;
	}
	Hashinglist.RemoveAll();
}

void CReplayTable::Erease(const DWORD& dwId)
{
	POSITION pos1 = RSlist.GetHeadPosition();
	POSITION pos2 = RSlist.GetHeadPosition();
	while((pos2 = pos1)!=nullptr)
	{
		CRS* RS = (CRS*)RSlist.GetNext(pos1);
		if(RS->dwId==dwId)
		{
			delete RS;
			RSlist.RemoveAt(pos2);
			break;
		}
	}
	if(RSlist.GetCount()<1)
	{
		Close();
	}
}

void CReplayTable::Draw(CWnd* pWnd, const CViewinfo& viewinfo)
{
	if(m_imagelist1.m_hImageList==nullptr&&m_imagelist2.m_hImageList==nullptr)
		return;

	CDC* pDC = pWnd->GetDC();
	pDC->SaveDC();
	pDC->SetViewportOrg(0, 0);
	pDC->SetWindowOrg(0, 0);
	pDC->SetMapMode(MM_TEXT);

	CRect cliRect;
	pWnd->GetClientRect(cliRect);

	POSITION pos = Hashinglist.GetHeadPosition();
	while(pos!=nullptr)
	{
		SMessage* message = (SMessage*)Hashinglist.GetNext(pos);
		const CPointF geoPoint = CPointF(message->lng, message->lat);
		Gdiplus::PointF cliPoint = viewinfo.WGS84ToClient(geoPoint);

		if(cliRect.PtInRect(CPoint(cliPoint.X, cliPoint.Y))==FALSE)
			continue;

		cliPoint.X -= 16;
		cliPoint.Y -= 16;
		if(message->dwWarning==0)
		{
			if(message->fSpeed<4.0f)
			{
				m_imagelist1.Draw(pDC, 17, CPoint(cliPoint.X, cliPoint.Y), ILD_TRANSPARENT);
			}
			else
			{
				const int n = (int)(((float)message->nBearing+7.5f)/22.5f);
				m_imagelist1.Draw(pDC, n, CPoint(cliPoint.X, cliPoint.Y), ILD_TRANSPARENT);
			}
		}
		else
		{
			if(message->fSpeed<4.0f)
			{
				m_imagelist2.Draw(pDC, 17, CPoint(cliPoint.X, cliPoint.Y), ILD_TRANSPARENT);
			}
			else
			{
				const int n = (int)(((float)message->nBearing+7.5f)/22.5f);
				m_imagelist2.Draw(pDC, n, CPoint(cliPoint.X, cliPoint.Y), ILD_TRANSPARENT);
			}
		}
	}

	pDC->RestoreDC(-1);
	pWnd->ReleaseDC(pDC);
}

CString CReplayTable::PickTag(CWnd* pWnd, const CViewinfo& viewinfo, const CPoint& docPoint)
{
	CString strTag;
	const Gdiplus::Point point = viewinfo.DocToClient<Gdiplus::Point>(docPoint);
	POSITION pos = Hashinglist.GetHeadPosition();
	while(pos!=nullptr)
	{
		SMessage* message = (SMessage*)Hashinglist.GetNext(pos);
		const CPointF geoPoint = CPointF(message->lng, message->lat);
		const Gdiplus::PointF cliPoint = viewinfo.WGS84ToClient(geoPoint);
		if(abs(point.X-cliPoint.X)<16&&abs(point.Y-cliPoint.Y)<16)
		{
			long nTime = message->nTime;
			const int month = nTime/100000000;
			nTime = nTime-month*100000000;
			const int day = nTime/1000000;
			nTime = nTime-day*1000000;
			const int hour = nTime/10000;
			nTime = nTime-hour*10000;
			const int minute = nTime/100;
			nTime = nTime-minute*100;
			CString strTime;
			strTime.Format(_T("%d月%d日%d点%d分"), month, day, hour, minute);

			strTag.Format(_T("车牌号:%s  时间:%s  速度:%.2f"), m_strCode, strTime, message->fSpeed);
			if(message->dwWarning!=0)
			{
				if(message->dwWarning<=16)
				{
					CString strAlarm;
					strAlarm.Format(_T(" 报警类型:%s"), lpszWarningTypes[message->dwWarning]);

					strTag = strTag+strAlarm;
				}
			}

			break;
		}
	}

	return strTag;
};
