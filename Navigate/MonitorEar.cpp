#include "stdafx.h"
#include "MonitorEar.h"
#include "CallDlg.h"
#include "Vehicle.h"
#include "mmsystem.h"
#include "ReplayDlg.h"
#include "DispatchDlg.h"
#include "SearchDlg.h"
#include "MessageDlg.h"
#include "Port.h"
#include "PortSocket.h"
#include "CmdParamDlg.h"

#include "../Viewer/Global.h"
#include "../Coding/Instruction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

LPCTSTR lpszWarningTypes[] = {
	_T("正常状态"), // = 0          
	_T("低电压报警"), // = 1          
	_T("GPS天线断线报警"), // = 2          
	_T("故障服务请求122"), // = 3          
	_T("紧急求救报警"), // = 4          
	_T("防盗器报警"), // = 5          
	_T("火警"), // = 6          
	_T("越区报警"), // = 7	         
	_T("出入隧道"), // = 8	         
	_T("断电报警"), // = 9	         
	_T("超速报警"), // = 10	         				
	_T("震动报警"), // = 11	         				
	_T("超出限定道路报警"), // = 12	         				
	_T("遥控报警"), // = 13	         				
	_T("拖吊报警"), // = 14
	_T("超时报警"), // = 15
	_T("开门报警"), // = 16		
};
/////////////////////////////////////////////////////////////////////////////
// CMonitorEar dialog

#define TIMER_PULSE 3

bool bSelected = false;

CMonitorEar::CMonitorEar(CWnd* pParent /*=nullptr*/)
	: CDialog(CMonitorEar::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMonitorEar)
	//}}AFX_DATA_INIT
}

void CMonitorEar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMonitorEar)
	DDX_Control(pDX, IDC_MSFLEXGRID, m_VehGrid);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMonitorEar, CDialog)
	//{{AFX_MSG_MAP(CMonitorEar)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CALL, OnCall)
	ON_BN_CLICKED(IDC_ERASE, OnErase)
	ON_BN_CLICKED(IDC_TRACK, OnTrack)
	ON_BN_CLICKED(IDC_TRACE, OnTrace)
	ON_BN_CLICKED(IDC_TILE, OnTile)
	ON_BN_CLICKED(IDC_MESSAGE, OnMessage)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_COMMAND, OnMyCommand)
	ON_BN_CLICKED(IDC_SETUP, OnSetup)
	ON_COMMAND(ID_MENUITEM_TRACE, OnMenuitemTrace)
	ON_COMMAND(ID_MENUITEM_TRACE_STOP, OnMenuitemTraceStop)
	ON_WM_SHOWWINDOW()
	ON_COMMAND(ID_MENUITEM_SORT_AESC, OnMenuitemSortAesc)
	ON_COMMAND(ID_MENUITEM_SORT_DESC, OnMenuitemSortDesc)
	ON_COMMAND(ID_MENUITEM_DISPATCH, OnMenuitemDispatch)
	ON_COMMAND(ID_MENUITEM_SEARCH, OnMenuitemSearch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMonitorEar message handlers
BOOL CMonitorEar::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_VehGrid.SetTextMatrix(0, 0, false ? _T("目标编号") : _T("ID"));
	m_VehGrid.SetColWidth(0, 900);
	m_VehGrid.SetColAlignment(0, 3);

	m_VehGrid.SetTextMatrix(0, 1, false ? _T("车主") : _T("Driver"));
	m_VehGrid.SetColWidth(1, 800);
	m_VehGrid.SetColAlignment(1, 3);

	m_VehGrid.SetTextMatrix(0, 2, false ? _T("车牌号") : _T("Plate"));
	m_VehGrid.SetColWidth(2, 1000);
	m_VehGrid.SetColAlignment(2, 3);

	m_VehGrid.SetTextMatrix(0, 3, false ? _T("车辆类型") : _T("Model"));
	m_VehGrid.SetColWidth(3, 900);
	m_VehGrid.SetColAlignment(3, 3);

	m_VehGrid.SetTextMatrix(0, 4, false ? _T("颜色") : _T("Color"));
	m_VehGrid.SetColWidth(4, 700);
	m_VehGrid.SetColAlignment(4, 3);

	m_VehGrid.SetTextMatrix(0, 5, false ? _T("通讯号码") : _T("Phone"));
	m_VehGrid.SetColWidth(5, 1200);
	m_VehGrid.SetColAlignment(5, 3);

	m_VehGrid.SetTextMatrix(0, 6, false ? _T("经度") : _T("X"));
	m_VehGrid.SetColWidth(6, 950);
	m_VehGrid.SetColAlignment(6, 3);

	m_VehGrid.SetTextMatrix(0, 7, false ? _T("纬度") : _T("Y"));
	m_VehGrid.SetColWidth(7, 950);
	m_VehGrid.SetColAlignment(7, 3);

	m_VehGrid.SetTextMatrix(0, 8, false ? _T("速度") : _T("Speed"));
	m_VehGrid.SetColWidth(8, 750);
	m_VehGrid.SetColAlignment(8, 3);

	m_VehGrid.SetTextMatrix(0, 9, false ? _T("时间") : _T("Time"));
	m_VehGrid.SetColWidth(9, 900);
	m_VehGrid.SetColAlignment(9, 3);

	m_VehGrid.SetTextMatrix(0, 10, false ? _T("跟踪") : _T("Trace"));
	m_VehGrid.SetColWidth(10, 800);
	m_VehGrid.SetColAlignment(10, 3);

	m_VehGrid.SetTextMatrix(0, 11, false ? _T("轨迹") : _T("Track"));
	m_VehGrid.SetColWidth(11, 800);
	m_VehGrid.SetColAlignment(11, 3);

	m_VehGrid.SetTextMatrix(0, 12, false ? _T("状态") : _T("Status"));
	m_VehGrid.SetColWidth(12, 500);

	m_VehGrid.SetTextMatrix(0, 13, false ? _T("车机类型") : _T("IVU"));
	m_VehGrid.SetColWidth(13, 1000);

	m_VehGrid.SetTextMatrix(0, 14, _T(""));
	m_VehGrid.SetColWidth(14, 0);

	m_VehGrid.SetTextMatrix(0, 15, _T(""));
	m_VehGrid.SetColWidth(15, 0);

	m_VehGrid.SetTextMatrix(0, 16, _T(""));
	m_VehGrid.SetColWidth(16, 0);

	this->SetTimer(TIMER_PULSE, 60*1000, nullptr);//1分钟发送个链路监测数据包,并检测是否服务器已经终端

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CMonitorEar::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CButton* button1 = (CButton*)GetDlgItem(IDC_CALL);
	CButton* button2 = (CButton*)GetDlgItem(IDC_ERASE);

	CButton* button3 = (CButton*)GetDlgItem(IDC_TRACK);
	CButton* button4 = (CButton*)GetDlgItem(IDC_TRACE);

	CButton* button5 = (CButton*)GetDlgItem(IDC_COMMAND);
	CButton* button6 = (CButton*)GetDlgItem(IDC_SETUP);

	CButton* button7 = (CButton*)GetDlgItem(IDC_MESSAGE);
	CButton* button8 = (CButton*)GetDlgItem(IDC_TILE);

	if(button1!=nullptr)
		button1->SetWindowPos(&wndTop, cx-130, 05, 0, 0, SWP_NOSIZE);
	if(button3!=nullptr)
		button3->SetWindowPos(&wndTop, cx-130, 35, 0, 0, SWP_NOSIZE);
	if(button5!=nullptr)
		button5->SetWindowPos(&wndTop, cx-130, 65, 0, 0, SWP_NOSIZE);
	if(button6!=nullptr)
		button6->SetWindowPos(&wndTop, cx-130, 95, 0, 0, SWP_NOSIZE);

	if(button2!=nullptr)
		button2->SetWindowPos(&wndTop, cx-60, 05, 0, 0, SWP_NOSIZE);
	if(button4!=nullptr)
		button4->SetWindowPos(&wndTop, cx-60, 35, 0, 0, SWP_NOSIZE);
	if(button8!=nullptr)
		button8->SetWindowPos(&wndTop, cx-60, 65, 0, 0, SWP_NOSIZE);
	if(button7!=nullptr)
		button7->SetWindowPos(&wndTop, cx-60, 95, 0, 0, SWP_NOSIZE);

	if(m_VehGrid.m_hWnd!=nullptr)
		m_VehGrid.SetWindowPos(&wndTop, 0, 0, cx-132, cy+1, SWP_NOMOVE);

	CStatic* msg = (CStatic*)GetDlgItem(IDC_STATIC_MESSAGE);
	if(msg!=nullptr)
	{
		msg->SetWindowPos(&wndTop, cx-130, cy-30, 0, 0, SWP_NOSIZE);
	}
}

void CMonitorEar::OnCall()
{
	CPort* pPort = (CPort*)AfxGetMainWnd()->SendMessage(WM_GETPORT, 0, 0);
	if(pPort!=nullptr)
	{
		const HINSTANCE hInstOld = AfxGetResourceHandle();
		HINSTANCE hInst = ::LoadLibrary(_T("Navigate.dll"));
		AfxSetResourceHandle(hInst);

		CCallDlg dlg(this, pPort);
		if(dlg.DoModal()==IDOK)
		{
		}

		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
	}
}

void CMonitorEar::OnErase()
{
	const long index = m_VehGrid.GetRowSel();
	if(index!=-1)
	{
		const CString strID = m_VehGrid.GetTextMatrix(index, 0);
		const DWORD dwId = _ttoi(strID);

		AfxGetMainWnd()->SendMessage(WM_VEHICLECONTROL, GPSSERVER_VEHICLE_EREASE, dwId);

		CPort* pPort = (CPort*)AfxGetMainWnd()->SendMessage(WM_GETPORT, 0, 0);
		if(pPort!=nullptr)
		{
			pPort->SendControl(dwId, GPSCLIENT_VEHICLE_EREASE);
		}
	}
}

void CMonitorEar::OnTrack()
{
	const long index = m_VehGrid.GetRowSel();
	CString string = m_VehGrid.GetTextMatrix(index, 0);
	const DWORD dwId = _ttoi(string);

	string = m_VehGrid.GetTextMatrix(index, 10);
	if(string==_T("×"))
	{
		AfxGetMainWnd()->SendMessage(WM_VEHICLECONTROL, GPSSERVER_VEHICLE_TRACK, dwId);

		CPort* pPort = (CPort*)AfxGetMainWnd()->SendMessage(WM_GETPORT, 0, 0);
		if(pPort!=nullptr)
		{
			pPort->SendControl(dwId, GPSCLIENT_VEHICLE_TRACK);
		}
	}
	else if(string==_T("√"))
	{
		AfxGetMainWnd()->SendMessage(WM_VEHICLECONTROL, GPSSERVER_VEHICLE_UNTRACK, dwId);

		CPort* pPort = (CPort*)AfxGetMainWnd()->SendMessage(WM_GETPORT, 0, 0);
		if(pPort!=nullptr)
		{
			pPort->SendControl(dwId, GPSCLIENT_VEHICLE_UNTRACK);
		}
	}
}

void CMonitorEar::OnTrace()
{
	const long index = m_VehGrid.GetRowSel();
	CString string = m_VehGrid.GetTextMatrix(index, 0);
	const DWORD dwId = _ttoi(string);

	string = m_VehGrid.GetTextMatrix(index, 11);
	if(string==_T("×"))
		AfxGetMainWnd()->SendMessage(WM_VEHICLECONTROL, GPSSERVER_TRACE_DRAW, dwId);
	if(string==_T("√"))
		AfxGetMainWnd()->SendMessage(WM_VEHICLECONTROL, GPSSERVER_TRACE_UNDRAW, dwId);
}

void CMonitorEar::OnTile()
{
	CMDIFrameWnd* pFrameWnd = (CMDIFrameWnd*)AfxGetMainWnd();
	pFrameWnd->MDITile(MDITILE_SKIPDISABLED);
}

void CMonitorEar::OnMessage()
{
	const long index = m_VehGrid.GetRowSel();
	if(index==-1)
		return;
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Navigate.dll"));
	AfxSetResourceHandle(hInst);
	const CString strID = m_VehGrid.GetTextMatrix(index, 0);
	DWORD dwId = _ttoi(strID);

	CMessageDlg dlg;
	dlg.m_dwId = dwId;
	if(dlg.DoModal()==IDOK)
	{
		dwId = dlg.m_dwId;
		CStringA string(dlg.m_strMessage);
		static CStringA strMsg;
		strMsg.Format("<- [%d] %s", dwId, string);
		AfxGetMainWnd()->SendMessage(WM_VEHICLEMESSAGE, dwId, (LONG)&strMsg);

		CPort* pPort = (CPort*)AfxGetMainWnd()->SendMessage(WM_GETPORT, 0, 0);
		if(pPort!=nullptr)
		{
			pPort->SendMessage(dwId, string);
		}
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

BEGIN_EVENTSINK_MAP(CMonitorEar, CDialog)
	//{{AFX_EVENTSINK_MAP(CMonitorEar)
	ON_EVENT(CMonitorEar, IDC_MSFLEXGRID, 69 /* SelChange */, OnSelChangeGrid, VTS_NONE)
	ON_EVENT(CMonitorEar, IDC_MSFLEXGRID, -607 /* MouseUp */, OnMouseUpMsflexgrid, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CMonitorEar, IDC_MSFLEXGRID, -605 /* MouseDown */, OnMouseDownMsflexgrid, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

BOOL CMonitorEar::ActivateVehicle(const DWORD& dwId)
{/*
	if(IsWindowVisible() == FALSE)
		return false;

	long oldIndex = m_VehGrid.GetRowSel();
	if(oldIndex != -1)
	{
		CString strID = m_VehGrid.GetTextMatrix(oldIndex,0);
		DWORD ID = _ttoi(strID);
		if(ID == dwId)
			return FALSE;
	}

	CString str1;
	str1.Format(_T("%d"),dwId);
	for(int index=1; index<m_VehGrid.GetRows(); index++)
	{
		CString str2 = m_VehGrid.GetTextMatrix(index, 0);
		if(str1 == str2)
		{
			m_VehGrid.SetRow(index);
			m_VehGrid.SetRowSel(index);
			OnSelChangeGrid();
			return TRUE;
		}
	}
*/
	return FALSE;
}

void CMonitorEar::OnSelChangeGrid()
{
	const long index = m_VehGrid.GetRowSel();

	CString string = m_VehGrid.GetTextMatrix(index, 10);
	if(string==_T("×"))
		((CButton*)GetDlgItem(IDC_TRACK))->SetWindowText(false ? _T("跟踪目标") : _T("Start Tracing"));
	if(string==_T("√"))
		((CButton*)GetDlgItem(IDC_TRACK))->SetWindowText(false ? _T("放弃跟踪") : _T("Stop Tracing"));

	string = m_VehGrid.GetTextMatrix(index, 11);
	if(string==_T("×"))
		((CButton*)GetDlgItem(IDC_TRACE))->SetWindowText(false ? _T("显示轨迹") : _T("Show Track"));
	if(string==_T("√"))
		((CButton*)GetDlgItem(IDC_TRACE))->SetWindowText(false ? _T("隐藏轨迹") : _T("Hide Track"));

	if(m_VehGrid.GetTextMatrix(index, 6).IsEmpty()||m_VehGrid.GetTextMatrix(index, 7).IsEmpty())
		return;
	const CString strID = m_VehGrid.GetTextMatrix(index, 0);
	const DWORD dwId = _ttoi(strID);
	AfxGetMainWnd()->SendMessage(WM_ACTIVATEVEHICLE, dwId, 0);
}

BOOL CMonitorEar::VehicleControl(UINT WPARAM, LONG LPARAM)
{
	long selIndex = m_VehGrid.GetRowSel();

	switch(WPARAM)
	{
	case SERVER_LOGIN_SUCCEED:
	{
		GetDlgItem(IDC_COMMAND)->EnableWindow(TRUE);
		GetDlgItem(IDC_TRACK)->EnableWindow(TRUE);
		GetDlgItem(IDC_TRACE)->EnableWindow(TRUE);
		GetDlgItem(IDC_CALL)->EnableWindow(TRUE);
		GetDlgItem(IDC_MESSAGE)->EnableWindow(TRUE);
		GetDlgItem(IDC_ERASE)->EnableWindow(TRUE);
		GetDlgItem(IDC_TILE)->EnableWindow(TRUE);
	}
	break;
	case SERVER_LOGIN_FAILED:
	{
		//	m_pPort->m_dwUser = 0;
	}
	break;
	case SERVER_DOWN:
	{
		int nCount = m_VehGrid.GetRows();
		for(int index = nCount-1; index>0; index--)
		{
			if(m_VehGrid.GetRows()==2)
			{
				m_VehGrid.SetTextMatrix(1, 0, _T(""));
				m_VehGrid.SetTextMatrix(1, 1, _T(""));
				m_VehGrid.SetTextMatrix(1, 2, _T(""));
				m_VehGrid.SetTextMatrix(1, 3, _T(""));
				m_VehGrid.SetTextMatrix(1, 4, _T(""));
				m_VehGrid.SetTextMatrix(1, 5, _T(""));
				m_VehGrid.SetTextMatrix(1, 6, _T(""));
				m_VehGrid.SetTextMatrix(1, 7, _T(""));
				m_VehGrid.SetTextMatrix(1, 8, _T(""));
				m_VehGrid.SetTextMatrix(1, 9, _T(""));
				m_VehGrid.SetTextMatrix(1, 10, _T(""));
				m_VehGrid.SetTextMatrix(1, 11, _T(""));
				m_VehGrid.SetTextMatrix(1, 12, _T(""));
				m_VehGrid.SetTextMatrix(1, 13, _T(""));
				m_VehGrid.SetTextMatrix(1, 14, _T(""));
				m_VehGrid.SetTextMatrix(1, 15, _T(""));
			}
			else
			{
				m_VehGrid.RemoveItem(index);
			}
		}
		GetDlgItem(IDC_COMMAND)->EnableWindow(FALSE);
		GetDlgItem(IDC_TRACK)->EnableWindow(FALSE);
		GetDlgItem(IDC_TRACE)->EnableWindow(FALSE);
		GetDlgItem(IDC_CALL)->EnableWindow(FALSE);
		GetDlgItem(IDC_MESSAGE)->EnableWindow(FALSE);
		GetDlgItem(IDC_ERASE)->EnableWindow(FALSE);
		GetDlgItem(IDC_TILE)->EnableWindow(FALSE);
	}
	break;
	case GPSSERVER_VEHICLE_NEW:
	{
		Content* content = (Content*)LPARAM;
		CString strID;
		strID.Format(_T("%d"), content->dwId);
		int index = 0;
		if(m_VehGrid.GetTextMatrix(1, 0).IsEmpty()==TRUE)
		{
			index = 1;
		}
		else
		{
			index = m_VehGrid.GetRows();
			VARIANT Var;
			Var.vt = VT_I4;
			Var.lVal = index;
			m_VehGrid.AddItem(strID, Var);
		}

		m_VehGrid.SetTextMatrix(index, 0, strID);
		m_VehGrid.SetTextMatrix(index, 1, content->strUser);
		m_VehGrid.SetTextMatrix(index, 2, content->strCode);
		m_VehGrid.SetTextMatrix(index, 3, content->strType);
		m_VehGrid.SetTextMatrix(index, 4, content->strColor);
		m_VehGrid.SetTextMatrix(index, 5, content->strDriver);
		m_VehGrid.SetTextMatrix(index, 6, _T(""));
		m_VehGrid.SetTextMatrix(index, 7, _T(""));
		m_VehGrid.SetTextMatrix(index, 10, _T("×"));
		m_VehGrid.SetTextMatrix(index, 11, _T("×"));
		m_VehGrid.SetTextMatrix(index, 12, _T(""));
		m_VehGrid.SetTextMatrix(index, 13, content->strIVU);
		m_VehGrid.SetTextMatrix(index, 14, content->strLink);
		m_VehGrid.SetTextMatrix(index, 15, content->strPass);
	}
	break;
	case GPSSERVER_VEHICLE_EREASE:
	{
		int nCount = m_VehGrid.GetRows();
		for(int index = nCount-1; index>0; index--)
		{
			CString strID = m_VehGrid.GetTextMatrix(index, 0);
			DWORD dwId = _ttoi(strID);
			if(dwId==LPARAM)
			{
				if(m_VehGrid.GetRows()==2)
				{
					m_VehGrid.SetTextMatrix(1, 0, _T(""));
					m_VehGrid.SetTextMatrix(1, 1, _T(""));
					m_VehGrid.SetTextMatrix(1, 2, _T(""));
					m_VehGrid.SetTextMatrix(1, 3, _T(""));
					m_VehGrid.SetTextMatrix(1, 4, _T(""));
					m_VehGrid.SetTextMatrix(1, 5, _T(""));
					m_VehGrid.SetTextMatrix(1, 6, _T(""));
					m_VehGrid.SetTextMatrix(1, 7, _T(""));
					m_VehGrid.SetTextMatrix(1, 8, _T(""));
					m_VehGrid.SetTextMatrix(1, 9, _T(""));
					m_VehGrid.SetTextMatrix(1, 10, _T(""));
					m_VehGrid.SetTextMatrix(1, 11, _T(""));
					m_VehGrid.SetTextMatrix(1, 12, _T(""));
					m_VehGrid.SetTextMatrix(1, 13, _T(""));
					m_VehGrid.SetTextMatrix(1, 14, _T(""));
					m_VehGrid.SetTextMatrix(1, 15, _T(""));
				}
				else
				{
					m_VehGrid.RemoveItem(index);
				}
				break;
			}
		}
	}
	break;
	case GPSSERVER_VEHICLE_TRACK:
	{
		for(int index = 1; index<m_VehGrid.GetRows(); index++)
		{
			CString strID = m_VehGrid.GetTextMatrix(index, 0);
			DWORD dwId = _ttoi((LPCTSTR)strID);
			if(dwId==LPARAM)
			{
				m_VehGrid.SetTextMatrix(index, 10, _T("√"));
				if(index==selIndex)
					((CButton*)GetDlgItem(IDC_TRACK))->SetWindowText(_T("放弃跟踪"));
				break;
			}
		}
	}
	break;
	case GPSSERVER_VEHICLE_UNTRACK:
	{
		for(int index = 1; index<m_VehGrid.GetRows(); index++)
		{
			CString strID = m_VehGrid.GetTextMatrix(index, 0);
			DWORD dwId = _ttoi(strID);
			if(dwId==LPARAM)
			{
				m_VehGrid.SetTextMatrix(index, 10, _T("×"));
				if(index==selIndex)
					((CButton*)GetDlgItem(IDC_TRACK))->SetWindowText(_T("跟踪目标"));
				break;
			}
		}
	}
	break;
	case GPSSERVER_TRACE_DRAW:
	{
		for(int index = 1; index<m_VehGrid.GetRows(); index++)
		{
			CString strID = m_VehGrid.GetTextMatrix(index, 0);
			DWORD dwId = _ttoi(strID);
			if(dwId==LPARAM)
			{
				m_VehGrid.SetTextMatrix(index, 11, _T("√"));
				if(index==selIndex)
					((CButton*)GetDlgItem(IDC_TRACE))->SetWindowText(_T("隐藏轨迹"));
				break;
			}
		}
	}
	break;
	case GPSSERVER_TRACE_UNDRAW:
	{
		for(int index = 1; index<m_VehGrid.GetRows(); index++)
		{
			CString strID = m_VehGrid.GetTextMatrix(index, 0);
			DWORD dwId = _ttoi((LPCTSTR)strID);
			if(dwId==LPARAM)
			{
				m_VehGrid.SetTextMatrix(index, 11, _T("×"));
				if(index==selIndex)
					((CButton*)GetDlgItem(IDC_TRACE))->SetWindowText(_T("显示轨迹"));
				break;
			}
		}
	}
	break;
	case GPSSERVER_VEHICLE_MESSAGE:
	{
		if(IsWindowVisible())
		{
			this->SetTimer(1, 500, nullptr);
		}
		break;
	}
	case GPSSERVER_VEHICLE_DISPATCH:
	{
		SMessage* pMsg = (SMessage*)LPARAM;
		if(nullptr==pMsg)
			break;

		DWORD dwId = 0;
		for(int row = 1; row<m_VehGrid.GetRows(); row++)
		{
			dwId = _ttoi(m_VehGrid.GetTextMatrix(row, 0));
			if(dwId==pMsg->m_dwVehicle)
			{
				CString strNum;
				strNum.Format(_T("%d"), pMsg->nTime);
				m_VehGrid.SetTextMatrix(row, 16, strNum);
				break;
			}
		}
		break;
	}
	case GPSCLIENT_VEHICLE_SORT:
	{
		SMessage* message = (SMessage*)LPARAM;
		m_VehGrid.SetRedraw(FALSE);

		bSelected = false;
		for(int row = 1; row<m_VehGrid.GetRows(); row++)
		{
			m_VehGrid.SetRow(row);
			for(int col = 1; col<m_VehGrid.GetCols(); col++)
			{
				m_VehGrid.SetCol(col);
				if(m_VehGrid.GetCellBackColor()!=0x00FFFFFF)
					m_VehGrid.SetCellBackColor(0x00FFFFFF);
				else
					break;
			}
		}

		CString str;
		double lat, lng;
		for(int index = 1; index<m_VehGrid.GetRows(); index++)
		{
			str = m_VehGrid.GetTextMatrix(index, 6);
			lng = _ttof((LPCTSTR)str);
			str = m_VehGrid.GetTextMatrix(index, 7);
			lat = _ttof((LPCTSTR)str);

			if(lng < message->lng-message->altitude
				||lng > message->lng+message->altitude
				||lat < message->lat-message->altitude
				||lat > message->lat+message->altitude)
				continue;

			bSelected = true;
			MoveGridItem(1, index);
			m_VehGrid.SetRow(1);
			for(int i = 1; i<m_VehGrid.GetCols(); i++)
			{
				m_VehGrid.SetCol(i);
				m_VehGrid.SetCellBackColor(0x00FFFFB0);
			}
		}
		m_VehGrid.SetRedraw(TRUE);
		m_VehGrid.Refresh();
		break;
	}
	default:
		break;
	}

	return TRUE;
}

BOOL CMonitorEar::VehicleMove(UINT WPARAM, LONG LPARAM)
{
	if(WPARAM==0||LPARAM==0)
		return FALSE;

	const SMessage* message = (SMessage*)LPARAM;
	for(int index = 1; index<m_VehGrid.GetRows(); index++)
	{
		CString strID = m_VehGrid.GetTextMatrix(index, 0);
		const DWORD dwId = _ttoi(strID);
		if(dwId==WPARAM)
		{
			CString string;
			string.Format(_T("%g"), message->lng);
			m_VehGrid.SetTextMatrix(index, 6, string);
			string.Format(_T("%g"), message->lat);
			m_VehGrid.SetTextMatrix(index, 7, string);
			string.Format(_T("%.2f"), message->fSpeed);
			m_VehGrid.SetTextMatrix(index, 8, string);

			int nHour = message->nTime/10000;
			const int nMinites = (message->nTime-nHour*10000)/100;
			const int nSecond = message->nTime-nHour*10000-nMinites*100;

			nHour = (nHour-8+24)%24;
			string.Format(_T("%02d:%02d:%02d"), nHour, nMinites, nSecond);
			m_VehGrid.SetTextMatrix(index, 9, string);
			break;
		}
	}

	return TRUE;
}

BOOL CMonitorEar::VehicleStatus(UINT WPARAM, LONG LPARAM)
{
	if(WPARAM==0||LPARAM==0)
		return FALSE;

	for(int index = 1; index<m_VehGrid.GetRows(); index++)
	{
		CString strID = m_VehGrid.GetTextMatrix(index, 0);
		const DWORD dwId = _ttoi(strID);
		if(dwId==WPARAM)
		{
			const CString strStatus = m_VehGrid.GetTextMatrix(index, 12);
			if(LPARAM!=_ttoi(strStatus))
			{
				switch(LPARAM)
				{
				case 0:
					break;
				case 1:
					break;
				case 2:
					break;
				}

				CString strStatus;
				strStatus.Format(_T("%d"), LPARAM);
				m_VehGrid.SetTextMatrix(index, 12, strStatus);
			}

			break;
		}
	}

	return TRUE;
}

void CMonitorEar::MoveFirst(DWORD dwVehicle)
{
	for(int index = 1; index<m_VehGrid.GetRows(); index++)
	{
		CString strID = m_VehGrid.GetTextMatrix(index, 0);
		const DWORD dwId = _ttoi(strID);
		if(dwId==dwVehicle)
		{
			if(index!=1)
			{
				MoveGridItem(1, index);
			}

			break;
		}
	}
}

void CMonitorEar::MoveGridItem(int idx1, int idx2)
{
	VARIANT Var;
	Var.vt = VT_I4;
	Var.lVal = idx1;
	m_VehGrid.AddItem(m_VehGrid.GetTextMatrix(idx2, 0), Var);
	m_VehGrid.SetTextMatrix(idx1, 1, m_VehGrid.GetTextMatrix(idx2+1, 1));
	m_VehGrid.SetTextMatrix(idx1, 2, m_VehGrid.GetTextMatrix(idx2+1, 2));
	m_VehGrid.SetTextMatrix(idx1, 3, m_VehGrid.GetTextMatrix(idx2+1, 3));
	m_VehGrid.SetTextMatrix(idx1, 4, m_VehGrid.GetTextMatrix(idx2+1, 4));
	m_VehGrid.SetTextMatrix(idx1, 5, m_VehGrid.GetTextMatrix(idx2+1, 5));
	m_VehGrid.SetTextMatrix(idx1, 6, m_VehGrid.GetTextMatrix(idx2+1, 6));
	m_VehGrid.SetTextMatrix(idx1, 7, m_VehGrid.GetTextMatrix(idx2+1, 7));
	m_VehGrid.SetTextMatrix(idx1, 8, m_VehGrid.GetTextMatrix(idx2+1, 8));
	m_VehGrid.SetTextMatrix(idx1, 9, m_VehGrid.GetTextMatrix(idx2+1, 9));
	m_VehGrid.SetTextMatrix(idx1, 10, m_VehGrid.GetTextMatrix(idx2+1, 10));
	m_VehGrid.SetTextMatrix(idx1, 11, m_VehGrid.GetTextMatrix(idx2+1, 11));
	m_VehGrid.SetTextMatrix(idx1, 12, m_VehGrid.GetTextMatrix(idx2+1, 12));
	m_VehGrid.SetTextMatrix(idx1, 13, m_VehGrid.GetTextMatrix(idx2+1, 13));
	m_VehGrid.SetTextMatrix(idx1, 14, m_VehGrid.GetTextMatrix(idx2+1, 14));
	m_VehGrid.SetTextMatrix(idx1, 15, m_VehGrid.GetTextMatrix(idx2+1, 15));
	m_VehGrid.RemoveItem(idx2+1);
}

void CMonitorEar::OnTimer(UINT nIDEvent)
{
	switch(nIDEvent)
	{
	case TIMER_PULSE:
	{
		try
		{
			CPort* pPort = (CPort*)AfxGetMainWnd()->SendMessage(WM_GETPORT, 0, 0);
			if(pPort!=nullptr)
			{
				if(CPortSocket::m_socket.IsTimerOut())
				{
					CString str = _T("--> 服务器可能意外中断！");
					AfxGetMainWnd()->SendMessage(WM_VEHICLEMESSAGE, 0, (LONG)&str);

					CPortSocket::m_socket.SetLogin(FALSE);
					CPortSocket::m_socket.Close();

					AfxGetMainWnd()->SendMessage(WM_VEHICLECONTROL, SERVER_DOWN, 0);
				}
				else
				{
					pPort->SendControl(0, CLIENT_PULSE);
				}
			}
		}
		catch(CException* ex)
		{
			ex->Delete();
		}
	}
	break;
	default:
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

BOOL CMonitorEar::DestroyWindow()
{
	PlaySound(nullptr, nullptr, SND_FILENAME|SND_ASYNC);

	return CDialog::DestroyWindow();
}

void CMonitorEar::OnMyCommand()
{
	CPoint point;
	CRect rect;
	GetDlgItem(IDC_COMMAND)->GetWindowRect(&rect);
	point.x = rect.left;
	point.y = rect.top;
	const HWND hWndTrack = m_hWnd;
	ASSERT(hWndTrack!=nullptr&&::IsWindow(hWndTrack));
	static CMenu Popup;
	static bool bCreated = false;
	if(bCreated==false)
		bCreated = true;
	else
		Popup.DestroyMenu();
	VERIFY(Popup.CreatePopupMenu());
	const long index = m_VehGrid.GetRowSel();
	if(index==-1)
		return;

	CString strIVU = m_VehGrid.GetTextMatrix(index, 13);
	//	if(LoadXMLCmds(strIVU, &Popup) == TRUE)
	{
		//		Popup.TrackPopupMenu(TPM_RIGHTALIGN |TPM_LEFTBUTTON, point.x, point.y, this);
	}
}

void CMonitorEar::OnSetup()
{
	_TCHAR rundir[MAX_PATH];
	if(::GetModuleFileName(AfxGetInstanceHandle(), rundir, MAX_PATH)==0)
		return;

	for(int i = _tcslen(rundir); i>=0; i--)
	{
		if(*(rundir+i)==_T('\\'))
		{
			*(rundir+i) = 0;
			break;
		}
	}

	CString strDir = (LPCTSTR)rundir;
	strDir += _T("\\TracarMgr.exe");
	const CStringA straDir(strDir);
	WinExec(straDir, SW_SHOWNORMAL);
}

BOOL CMonitorEar::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if(lParam==0&&wParam>=7000&&wParam<8000)
	{
		CWaitCursor wait;
		CPort* pPort = (CPort*)AfxGetMainWnd()->SendMessage(WM_GETPORT, 0, 0);
		if(pPort==nullptr)
			return CDialog::OnCommand(wParam, lParam);

		long index1 = m_VehGrid.GetRowSel();
		long index2 = m_VehGrid.GetRow();
		if(index1==-1||index2==-1)
			return CDialog::OnCommand(wParam, lParam);

		if(index1>index2)
		{
			long tem = index1;
			index1 = index2;
			index2 = tem;
		}

		CString strIVU = m_VehGrid.GetTextMatrix(index1, 13);
		if(strIVU.IsEmpty()==TRUE)
			return CDialog::OnCommand(wParam, lParam);

		CDatabase* pDatabase = &(pPort->m_database);
		if(pDatabase->IsOpen()==FALSE)
			return CDialog::OnCommand(wParam, lParam);

		DWORD dwCmdID;
		CString strCmd;
		CString strDrp;

		CRecordset rs(pDatabase);
		CString strSQL;
		strSQL.Format(_T("SELECT IVUCmd.* FROM IVU INNER JOIN IVUCmd ON IVU.ID = IVUCmd.IVU where IVU.类型='%s' And IVUCmd.掩码=%d"), strIVU, wParam);
		rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly);
		if(rs.IsEOF()==TRUE)
		{
			rs.Close();
			return CDialog::OnCommand(wParam, lParam);
		}
		else
		{
			CString strCmdID;
			CString strDangerous;
			rs.GetFieldValue(_T("ID"), strCmdID);
			rs.GetFieldValue(_T("含义"), strCmd);
			rs.GetFieldValue(_T("描述"), strDrp);
			rs.GetFieldValue(_T("Dangerous"), strDangerous);

			dwCmdID = _ttoi(strCmdID);

			rs.Close();

			if(strDangerous==_T('1'))
			{
				if(AfxMessageBox(_T("危险命令，是否继续操作？"), MB_YESNO|MB_ICONEXCLAMATION)!=IDYES)
				{
					return CDialog::OnCommand(wParam, lParam);
				}
			}
		}

		static CCoder::SParameter parameter;
		strSQL.Format(_T("Select * From CmdParam Where CmdID=%d"), dwCmdID);
		rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly);
		if(rs.IsEOF()==FALSE)
		{
			HINSTANCE hInstOld = AfxGetResourceHandle();
			HINSTANCE hInst = ::LoadLibrary(_T("Navigate.dll"));
			AfxSetResourceHandle(hInst);

			CCmdParamDlg dlg(this, &parameter);
			dlg.m_strTitle = strCmd;
			dlg.m_strDescription = strDrp;
			while(rs.IsEOF()==FALSE)
			{
				CString string;

				rs.GetFieldValue(_T("Name"), string);
				dlg.m_saParams.Add(string);

				rs.GetFieldValue(_T("Type"), string);
				dlg.m_saTypes.Add(string);

				rs.GetFieldValue(_T("DefVal"), string);
				dlg.m_saValues.Add(string);

				rs.MoveNext();
			}
			if(dlg.DoModal()!=IDOK)
			{
				AfxSetResourceHandle(hInstOld);
				::FreeLibrary(hInst);
				rs.Close();
				return CDialog::OnCommand(wParam, lParam);
			}

			AfxSetResourceHandle(hInstOld);
			::FreeLibrary(hInst);
		}
		rs.Close();

		parameter.m_password = m_VehGrid.GetTextMatrix(index1, 15);

		CString strNotSendVhc;
		strNotSendVhc.Empty();
		for(int i = index1; i<=index2; i++)
		{
			CString strVehicle = m_VehGrid.GetTextMatrix(i, 0);
			DWORD dwVehicle = _ttoi(strVehicle);
			if(strIVU!=m_VehGrid.GetTextMatrix(i, 13))
			{
				strNotSendVhc += strVehicle+_T("、");
				continue;
			}

			pPort->SendCommand(dwVehicle, wParam, strIVU, parameter);
		}
		if(!strNotSendVhc.IsEmpty())
		{
			strNotSendVhc.Insert(0, _T("由于车载终端类型不同，下列车辆命令未发送出去：\n"));
			AfxMessageBox(strNotSendVhc);
		}
	}

	return CDialog::OnCommand(wParam, lParam);
}

static int mouseCol = 0;

void CMonitorEar::OnMouseUpMsflexgrid(short Button, short Shift, long x, long y)
{
	mouseCol = m_VehGrid.GetMouseCol();
	if(Button!=2)
		return;

	static CMenu Popup, cmdPopup;
	static bool bCreated = false;

	int index = m_VehGrid.GetMouseRow();
	if(index==0)
	{
		HWND hWndTrack = m_hWnd;
		ASSERT(hWndTrack!=nullptr&&::IsWindow(hWndTrack));
		static CMenu Popup, cmdPopup;
		static bool bCreated = false;
		if(bCreated==false)
			bCreated = true;
		else
		{
			Popup.DestroyMenu();
			cmdPopup.DestroyMenu();
		}
		VERIFY(Popup.CreatePopupMenu());
		VERIFY(cmdPopup.CreatePopupMenu());

		if(bCreated==false)
			bCreated = true;
		else
		{
			Popup.DestroyMenu();
			cmdPopup.DestroyMenu();
		}
		VERIFY(Popup.CreatePopupMenu());
		VERIFY(cmdPopup.CreatePopupMenu());
		//	Popup.AppendMenu(MF_STRING, ID_MENUITEM_SORT_AESC, GetItemLan("ID_MENUITEM_SORT_AESC"));
		//	Popup.AppendMenu(MF_STRING, ID_MENUITEM_SORT_DESC, GetItemLan("ID_MENUITEM_SORT_DESC"));

		//		LoadXMLLans(&cmdPopup);pPopup->AppendMenu(MF_STRING, dwId, (LPCTSTR)strCmd);
		Popup.AppendMenu(MF_SEPARATOR);

		CPoint pt(x, y);
		ClientToScreen(&pt);
		Popup.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, pt.x, pt.y, this);
	}
	else if(index>0&&index<m_VehGrid.GetRows())
	{
		m_VehGrid.SetRow(index);
		m_VehGrid.SetCol(1);
		m_VehGrid.SetColSel(m_VehGrid.GetCols()-1);

		CString strIVU = m_VehGrid.GetTextMatrix(index, 13);
		if(strIVU.IsEmpty())
			return;

		HWND hWndTrack = m_hWnd;
		ASSERT(hWndTrack!=nullptr&&::IsWindow(hWndTrack));
		if(!bCreated)
			bCreated = true;
		else
		{
			Popup.DestroyMenu();
			cmdPopup.DestroyMenu();
		}
		VERIFY(Popup.CreatePopupMenu());
		VERIFY(cmdPopup.CreatePopupMenu());

		CPort* pPort = (CPort*)AfxGetMainWnd()->SendMessage(WM_GETPORT, 0, 0);
		if(pPort==nullptr)
			return;

		CDatabase* pDatabase = &(pPort->m_database);
		if(pDatabase->IsOpen()==FALSE)
			return;

		CRecordset rs(pDatabase);
		CString strSQL;
		strSQL.Format(_T("Select * From IVUCmd INNER JOIN IVU ON IVUCmd.IVU=IVU.ID  Where IVU.类型='%s'"), strIVU);
		rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly);
		while(rs.IsEOF()==FALSE)
		{
			CString strParam;
			rs.GetFieldValue(_T("掩码"), strParam);

			CString string;
			rs.GetFieldValue(_T("含义"), string);
			string.TrimLeft();
			string.TrimRight();

			cmdPopup.AppendMenu(MF_STRING, _ttoi(strParam), string);

			rs.MoveNext();
		}
		rs.Close();

		Popup.AppendMenu(MF_POPUP, (UINT)cmdPopup.m_hMenu, _T("车机命令(&V)"));
		Popup.AppendMenu(MF_SEPARATOR);
		Popup.AppendMenu(MF_STRING, ID_MENUITEM_TRACE, _T("轨迹回放(&T)"));
		Popup.AppendMenu(MF_STRING, ID_MENUITEM_TRACE_STOP, _T("停止回放(&C)"));
		Popup.AppendMenu(MF_SEPARATOR);
		Popup.AppendMenu(MF_STRING, ID_MENUITEM_SORT_AESC, _T("按列排序(升序)(&S)"));
		Popup.AppendMenu(MF_STRING, ID_MENUITEM_SORT_DESC, _T("按列排序(降序)(&J)"));
		Popup.AppendMenu(MF_SEPARATOR);
		Popup.AppendMenu(MF_STRING, ID_MENUITEM_DISPATCH, _T("派遣车辆(&D)"));
		Popup.AppendMenu(MF_STRING, ID_MENUITEM_SEARCH, _T("按列查找(&F)"));

		CPoint pt(x, y);
		ClientToScreen(&pt);
		Popup.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, pt.x, pt.y, this);
	}
}

void CMonitorEar::OnMenuitemTrace()
{
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Navigate.dll"));
	AfxSetResourceHandle(hInst);

	CStringA str1;
	CStringA str2;
	CReplayDlg dlg(nullptr, nullptr);
	if(dlg.DoModal()==IDOK)
	{
		str1.Format("%04d-%02d-%02d %02d:%02d:%02d", dlg.m_Date1.GetYear(), dlg.m_Date1.GetMonth(), dlg.m_Date1.GetDay(), dlg.m_Time1.GetHour(), dlg.m_Time1.GetMinute(), dlg.m_Time1.GetSecond());
		str2.Format("%04d-%02d-%02d %02d:%02d:%02d", dlg.m_Date2.GetYear(), dlg.m_Date2.GetMonth(), dlg.m_Date2.GetDay(), dlg.m_Time2.GetHour(), dlg.m_Time2.GetMinute(), dlg.m_Time2.GetSecond());
	}
	else
	{
		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
		return;
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);

	CPort* pPort = (CPort*)AfxGetMainWnd()->SendMessage(WM_GETPORT, 0, 0);
	if(pPort==nullptr)
		return;
	const int index = m_VehGrid.GetRowSel();
	if(index<=0||index>=m_VehGrid.GetRows())
		return;
	const DWORD dwId = _ttoi(m_VehGrid.GetTextMatrix(index, 0));
	const DWORD nReplaySpeed = AfxGetApp()->GetProfileInt(_T("Socket"), _T("ReplaySpeed"), 200);

	STRUCT_GPSCLIENT_REPLAY* lp = new STRUCT_GPSCLIENT_REPLAY(dwId, str1.GetBuffer(), str2.GetBuffer(), nReplaySpeed);
	CPortSocket::m_socket.SendMsg((char*)lp, sizeof(*lp));
	str1.ReleaseBuffer();
}

void CMonitorEar::OnMenuitemTraceStop()
{
	CPort* pPort = (CPort*)AfxGetMainWnd()->SendMessage(WM_GETPORT, 0, 0);
	if(pPort==nullptr)
		return;
	const int index = m_VehGrid.GetRowSel();
	if(index<=0||index>=m_VehGrid.GetRows())
		return;
	const DWORD dwId = _ttoi(m_VehGrid.GetTextMatrix(index, 0));
	pPort->SendControl(dwId, GPSCLIENT_VEHICLE_REPLAY_STOP);
}

void CMonitorEar::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if(!IsWindowVisible())
	{
		this->KillTimer(1);
		Sleep(200);
		CStatic* msg = (CStatic*)GetDlgItem(IDC_STATIC_MESSAGE);
		if(msg!=nullptr)
			msg->ShowWindow(SW_HIDE);
	}
}

void CMonitorEar::OnMouseDownMsflexgrid(short Button, short Shift, long x, long y)
{
	mouseCol = m_VehGrid.GetMouseCol();
	if(Button!=1)
		return;

	if(bSelected)
	{
		bSelected = false;
		m_VehGrid.SetRedraw(FALSE);
		for(int row = 1; row<m_VehGrid.GetRows(); row++)
		{
			m_VehGrid.SetRow(row);
			for(int col = 1; col<m_VehGrid.GetCols(); col++)
			{
				m_VehGrid.SetCol(col);
				if(m_VehGrid.GetCellBackColor()!=0x00FFFFFF)
					m_VehGrid.SetCellBackColor(0x00FFFFFF);
			}
		}
		m_VehGrid.SetRedraw(TRUE);
		m_VehGrid.Refresh();
	}

	if(m_VehGrid.GetMouseRow()!=0)
		return;
	DoSort(1);
}

void CMonitorEar::DoSort(UINT uType)
{
	int row = m_VehGrid.GetMouseRow();
	const int col = mouseCol; // m_VehGrid.GetMouseCol();
	const int cols = m_VehGrid.GetCols();
	const int rows = m_VehGrid.GetRows();
	if(col>=0&&col<cols&&rows > 2)
	{
		m_VehGrid.SetRedraw(FALSE);
		int min = 1;
		for(int i = 1; i<rows-1; i++)
		{
			for(int j = i+1; j<rows; j++)
			{
				if(uType==0)
				{
					if(m_VehGrid.GetTextMatrix(min, col).CompareNoCase(m_VehGrid.GetTextMatrix(j, col))>0)
						min = j;
				}
				else
				{
					if(m_VehGrid.GetTextMatrix(min, col).CompareNoCase(m_VehGrid.GetTextMatrix(j, col))<0)
						min = j;
				}
			}

			if(min!=i)
				MoveGridItem(i, min);
			min = i+1;
		}
		m_VehGrid.SetRedraw(TRUE);
		m_VehGrid.Refresh();
	}
}

void CMonitorEar::OnMenuitemSortAesc()
{
	DoSort(0);
}

void CMonitorEar::OnMenuitemSortDesc()
{
	DoSort(1);
}

void CMonitorEar::OnMenuitemDispatch()
{
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Navigate.dll"));
	AfxSetResourceHandle(hInst);

	CDispatch1Dlg dlg(this);
	dlg.m_pGrid = &m_VehGrid;
	if(dlg.DoModal()==IDOK)
	{
	}
	else
	{
		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
		return;
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

void CMonitorEar::OnMenuitemSearch()
{
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Navigate.dll"));
	AfxSetResourceHandle(hInst);

	CSearchDlg dlg(this);
	if(dlg.DoModal()==IDOK)
	{
	}
	else
	{
		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
		return;
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);

	dlg.m_strKeyField.MakeLower();
	bSelected = false;
	for(int row = 1; row<m_VehGrid.GetRows(); row++)
	{
		m_VehGrid.SetRow(row);
		for(int col = 1; col<m_VehGrid.GetCols(); col++)
		{
			m_VehGrid.SetCol(col);
			if(m_VehGrid.GetCellBackColor()!=0x00FFFFFF)
				m_VehGrid.SetCellBackColor(0x00FFFFFF);
			else
				break;
		}
	}

	int row = m_VehGrid.GetMouseRow();
	const int col = mouseCol; // m_VehGrid.GetMouseCol();
	const int cols = m_VehGrid.GetCols();
	const int rows = m_VehGrid.GetRows();
	if(col>=0&&col<cols&&rows > 2)
	{
		m_VehGrid.SetRedraw(FALSE);
		for(int i = 1; i<rows; i++)
		{
			CString strText = m_VehGrid.GetTextMatrix(i, mouseCol);
			strText.MakeLower();
			if(strText.Find(dlg.m_strKeyField)!=-1)
			{
				bSelected = true;
				MoveGridItem(1, i);
				m_VehGrid.SetRow(1);
				for(int i = 1; i<m_VehGrid.GetCols(); i++)
				{
					m_VehGrid.SetCol(i);
					m_VehGrid.SetCellBackColor(0x00FFFFB0);
				}
			}
		}
		m_VehGrid.SetRedraw(TRUE);
		m_VehGrid.Refresh();
	}
}
