#include "stdafx.h"
#include  <stdio.h>
#include  <stdlib.h>
#include "mmsystem.h"

#include "../Coding/Instruction.h"
#include "../Coding/Coder.h"
#include "Port.h"
#include "PortSocket.h"
#include "Vehicle.h"
#include "LoginDlg.h"

#include "../Public/Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CPortSocket, CPort)

CGPSClientSocket CPortSocket::m_socket;

static HWND m_hParentsWnd = nullptr;
/////////////////////////////////////////////////////////////////////////////
// CPortSocket

CPortSocket::CPortSocket(CFrameWnd* pParents)
	: CPort(pParents)
{
	m_message.bRotateMap = false;
	m_hParentsWnd = pParents->m_hWnd;
}

bool CPortSocket::Open()
{
	CPort::Open();

	if(m_database.IsOpen()==FALSE)
	{
		CString strDSN = AfxGetApp()->GetProfileString(_T("VechileDB"), _T("DSN"), nullptr);
		CString strServerName = AfxGetApp()->GetProfileString(_T("VechileDB"), _T("ServerName"), nullptr);
		CString strUID = AfxGetApp()->GetProfileString(_T("VechileDB"), _T("UID"), nullptr);
		CString strPWD = AfxGetApp()->GetProfileString(_T("VechileDB"), _T("PWD"), nullptr);
		if(strDSN.IsEmpty()==FALSE&&strServerName.IsEmpty()==FALSE)
		{
			CString strConnect;
			strConnect.Format(_T("DSN=%s;DBQ=%s;UID=%s;PWD=%s"), strDSN, strServerName, strUID, strPWD);
			if(m_database.OpenEx(strConnect, CDatabase::noOdbcDialog)==FALSE)
			{
				AfxMessageBox(_T("位于车辆监控服务器上的数据库无法打开!"));
				return false;
			}
		}
	}

	CRecordset rs(&m_database);
	try
	{
		if(rs.Open(CRecordset::snapshot, _T("Select * From IVU"), CRecordset::readOnly)==TRUE)
		{
			while(rs.IsEOF()==FALSE)
			{
				CStringA strValue;
				rs.GetFieldValue(_T("类型"), strValue);
				strValue.TrimLeft();
				strValue.TrimRight();
				CCoder* pCoder = CCoder::GetCoder(strValue);
				if(pCoder!=nullptr)
				{
					pCoder->FetchStatus(&m_database);
				}

				rs.MoveNext();
			}

			rs.Close();
		}
	}
	catch(CDBException*ex)
	{
		ex->ReportError();
		ex->Delete();
		rs.Close();
	}

	CString strServer = AfxGetApp()->GetProfileString(_T("GPSServer"), _T("Server"), _T("localhost"));
	CStringA straServer(strServer);
	int nPort = AfxGetApp()->GetProfileInt(_T("GPSServer"), _T("Port"), 1501);
	if(!m_socket.ConnectToServer(straServer.GetBuffer(), nPort))
	{
		straServer.ReleaseBuffer();
		AfxMessageBox(_T("服务器连接失败!"));
		return false;
	}
	straServer.ReleaseBuffer();
	m_socket.SetProcessFunction(CallbackRecvFromSocket);

	HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Navigate.dll"));
	AfxSetResourceHandle(hInst);

	CString strUser = AfxGetApp()->GetProfileString(_T("GPSServer"), _T("Username"), _T(""));
	CString strPWD = AfxGetApp()->GetProfileString(_T("GPSServer"), _T("Password"), _T(""));
	CLoginDlg dlg(nullptr, strUser, _T(""));
	if(dlg.DoModal()==IDOK)
	{
		strUser = dlg.m_strUsername;
		strPWD = dlg.m_strPassword;

		CStringA straUser(strUser);
		CStringA straPWD(strPWD);

		STRUCT_CLIENT_LOGIN* lp = new STRUCT_CLIENT_LOGIN(straUser.GetBuffer(), straPWD.GetBuffer());
		m_socket.SendMsg((char*)lp, sizeof(*lp));
		straUser.ReleaseBuffer();

		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);

		AfxGetApp()->WriteProfileString(_T("GPSServer"), _T("Username"), strUser);
		return true;
	}
	else
	{
		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
		return false;
	}
}

void CPortSocket::Close()
{
	CPort::Close();

	m_database.Close();

	STRUCT_CLIENT_LOGOFF* lp = new STRUCT_CLIENT_LOGOFF();
	m_socket.SendMsg((char*)lp, sizeof(*lp));

	Sleep(200);
	m_socket.SetLogin(FALSE);
	m_socket.Close();

	PlaySound(nullptr, nullptr, SND_FILENAME|SND_ASYNC);

	AfxGetApp()->WriteProfileString(_T("GPSServer"), _T("PWD"), nullptr);
}

void CPortSocket::CallbackErrorFromSocket(long param)
{
	::SendMessage(m_hParentsWnd, WM_VEHICLECONTROL, SERVER_DOWN, 0);
}

void CPortSocket::CallbackRecvFromSocket(long param)
{
	char* lp = (char*)param;
	if(lp==nullptr)
		return;

	STRUCT_MSG_HEADER hdr;
	memcpy((char*)&hdr, lp, sizeof(STRUCT_MSG_HEADER));

	switch(hdr.wCmdId)
	{
	case SERVER_LOGIN_FAILED:
	{
		STRUCT_SERVER_REPLY_LOGIN msg;
		memcpy((char*)&msg, lp, hdr.dwTotalLen);

		switch(msg.dwErr)
		{
		case 0X0001:
			AfxMessageBox(_T("用户名不存在！"));
			break;
		case 0X0002:
			AfxMessageBox(_T("你的密码不正确！"));
			break;
		case 0X0003:
			AfxMessageBox(_T("你的软件版本太低，请重新安装新版本！"));
			break;
		case 0X0004:
			AfxMessageBox(_T("已有同名的用户登录，\n请换一个用户名重新登录！"));
			break;
		case 0X0005:
			AfxMessageBox(_T("服务器允许的连接数已满，请稍后再试。\n若需更多连接允许，请与软件提供商联系！"));
			break;
		case 0X0006:
			AfxMessageBox(_T("服务器端数据库连接出错！"));
			break;
		}

		CPortSocket::m_socket.SetLogin(FALSE);
		::SendMessage(m_hParentsWnd, WM_VEHICLECONTROL, SERVER_LOGIN_FAILED, 0);
		//	CPortSocket::m_socket.Terminate();因为关闭前需要向服务器发送关闭指令
	}
	break;
	case SERVER_LOGIN_SUCCEED:
	{
		STRUCT_SERVER_REPLY_LOGIN msg;
		memcpy((char*)&msg, lp, hdr.dwTotalLen);
		CPortSocket::m_dwUser = msg.dwUserId;

		CPortSocket::m_socket.SetLogin(TRUE);
		::SendMessage(m_hParentsWnd, WM_VEHICLECONTROL, SERVER_LOGIN_SUCCEED, 0);
	}
	break;
	case SERVER_LOGOFF:
	{
		CPortSocket::m_socket.SetLogin(FALSE);
		CPortSocket::m_socket.Close();

		::SendMessage(m_hParentsWnd, WM_VEHICLECONTROL, SERVER_DOWN, 0);
		AfxMessageBox(_T("GPS服务器要求你退出!"));
		return;
	}
	break;
	case SERVER_DOWN:
	{
		CPortSocket::m_socket.SetLogin(FALSE);
		CPortSocket::m_socket.Close();

		::SendMessage(m_hParentsWnd, WM_VEHICLECONTROL, SERVER_DOWN, 0);
		AfxMessageBox(_T("GPS服务器关闭!"));
	}
	break;
	case SERVER_PULSE:
	{
		CPortSocket::m_socket.ResetPulsCounter();
	}
	break;
	case GPSSERVER_VEHICLE_NEW:
	{
		STRUCT_GPSSERVER_VEHICLE_NEW msg;
		memcpy((char*)&msg, lp, hdr.dwTotalLen);

		Content* pCon = new Content;
		pCon->dwId = msg.dwVehicle;
		pCon->strCode = msg.vhcCode;
		pCon->strColor = msg.vhcColor;
		pCon->strDriver = msg.ownIVU;
		pCon->strLink = msg.vhcIVU;
		pCon->strType = msg.vhcType;
		pCon->strUser = msg.ownName;
		pCon->strIVU = msg.strIVU;
		pCon->strPass = msg.vhcPass;
#ifdef _TEST_VERSION
		pCon->strUser = _T("*******");
		pCon->strCode = _T("*******");
		pCon->strDriver = _T("*******");
		pCon->strType = _T("邮政车");
#endif
		::SendMessage(m_hParentsWnd, WM_VEHICLECONTROL, GPSSERVER_VEHICLE_NEW, (LONG)pCon);

		delete pCon;
		pCon = nullptr;
	}
	break;
	case GPSSERVER_VEHICLE_MOVE:
	{
		STRUCT_GPSSERVER_VEHICLE_MOVE msg;
		memcpy((char*)&msg, lp, hdr.dwTotalLen);
		static SMessage move;
		move.lng = msg.dbLng;
		move.lat = msg.dbLat;
		move.altitude = msg.flHeight;
		move.fSpeed = msg.flVeo;
		move.nBearing = msg.wDirect;
		move.dwStatus = msg.dwStatus;
		move.m_dwVehicle = msg.dwVehicle;
		move.m_bAvailable = true; // msg.bAV;
		COleDateTime oleTime = msg.gpsTime-LOCAL_SYS_TIME_SPAN;
		CString strMapTime = oleTime.Format(_T("%H%M%S"));
		move.nTime = _ttol(strMapTime);

		::SendMessage(m_hParentsWnd, WM_VEHICLEMOVE, move.m_dwVehicle, (LONG)&move);
		::SendMessage(m_hParentsWnd, WM_VEHICLESTATUS, move.m_dwVehicle, (LONG)&move);
	}
	break;
	case GPSSERVER_VEHICLE_MESSAGE:
	{
		STRUCT_GPSSERVER_VEHICLE_MESSAGE msg;
		memcpy((char*)&msg, lp, hdr.dwTotalLen);
		static CString strMessage;
		strMessage.Format(_T("-> [%d] %s"), msg.dwVehicle, msg.msg);
		::SendMessage(m_hParentsWnd, WM_VEHICLEMESSAGE, msg.dwVehicle, (LONG)&strMessage);
		::SendMessage(m_hParentsWnd, WM_VEHICLECONTROL, GPSSERVER_VEHICLE_MESSAGE, 0);
	}
	break;
	case GPSSERVER_VEHICLE_EREASE:
	{
		STRUCT_GPSSERVER_VEHICLE_EREASE msg;
		memcpy((char*)&msg, lp, hdr.dwTotalLen);
		::SendMessage(m_hParentsWnd, WM_VEHICLECONTROL, GPSSERVER_VEHICLE_EREASE, msg.dwVehicle);
	}
	break;
	case GPSSERVER_VEHICLE_ICON:
	{
		STRUCT_GPSSERVER_VEHICLE_ICON msg;
		memcpy((char*)&msg, lp, hdr.dwTotalLen);
		::SendMessage(m_hParentsWnd, WM_VEHICLEICON, msg.dwVehicle, msg.btIcon);
	}
	break;
	case GPSSERVER_VEHICLE_DISPATCH:
	{
		STRUCT_GPSSERVER_VEHICLE_DISPATCH msg;
		memcpy((char*)&msg, lp, hdr.dwTotalLen);
		SMessage smsg;
		smsg.m_dwVehicle = msg.dwVehicle;
		smsg.nTime = msg.uNum;
		smsg.m_wCommand = GPSSERVER_VEHICLE_DISPATCH;
		::SendMessage(m_hParentsWnd, WM_VEHICLECONTROL, GPSSERVER_VEHICLE_DISPATCH, (LPARAM)&smsg);
	}
	break;
	case GPSSERVER_VEHICLE_LOCATION:
	{
		STRUCT_GPSSERVER_VEHICLE_LOCATION msg;
		memcpy((char*)&msg, lp, hdr.dwTotalLen);

		CString strMessage;
		strMessage.Format(_T("%g,%g"), msg.dbLng, msg.dbLat);
		LOCATION* location = (LOCATION*)::SendMessage(m_hParentsWnd, WM_VEHICLELOCATION, msg.dwVehicle, (LONG)&strMessage);

		CString string;
		string = _T("在");
		if(location->RoadEast.IsEmpty()==FALSE)
		{
			string += location->RoadEast;
			string += _T("东边 ");
		}
		if(location->RoadWest.IsEmpty()==FALSE)
		{
			string += location->RoadWest;
			string += _T("西边 ");
		}
		if(location->RoadSouth.IsEmpty()==FALSE)
		{
			string += location->RoadSouth;
			string += _T("南边 ");
		}
		if(location->RoadNorth.IsEmpty()==FALSE)
		{
			string += location->RoadNorth;
			string += _T("北边 ");
		}
		if(location->SpotEast.IsEmpty()==FALSE)
		{
			string += location->SpotEast;
			string += _T("东边 ");
		}
		if(location->SpotWest.IsEmpty()==FALSE)
		{
			string += location->SpotWest;
			string += _T("西边 ");
		}
		if(location->SpotSouth.IsEmpty()==FALSE)
		{
			string += location->SpotSouth;
			string += _T("南边 ");
		}
		if(location->SpotNorth.IsEmpty()==FALSE)
		{
			string += location->SpotNorth;
			string += _T("北边 ");
		}

		STRUCT_GPSCLIENT_VEHICLE_LOCATION* lp = new STRUCT_GPSCLIENT_VEHICLE_LOCATION(msg.dwVehicle, location->Province.GetBuffer(), location->County.GetBuffer(), string.GetBuffer());
		CPortSocket::m_socket.SendMsg((char*)lp, sizeof(*lp));
		location->Province.ReleaseBuffer();
	}
	break;
	default:
		break;
	}
}

bool CPortSocket::SendMessage(const DWORD& dwId, CStringA& string)
{
	STRUCT_GPSCLIENT_VEHICLE_MESSAGE* lp = new STRUCT_GPSCLIENT_VEHICLE_MESSAGE(dwId, string.GetBuffer());
	string.ReleaseBuffer();
	return m_socket.SendMsg((char*)lp, sizeof(*lp));
}

bool CPortSocket::SendControl(const DWORD& dwVehicle, WORD wControl)
{
	STRUCT_MSG_HEADER* lp = new STRUCT_MSG_HEADER(dwVehicle, wControl);
	return m_socket.SendMsg((char*)lp, sizeof(*lp));
}

bool CPortSocket::SendCommand(const DWORD& dwVehicle, WORD wCommand, CStringA strIVU, CCoder::SParameter& parameter)
{
	CCoder* pCoder = CCoder::GetCoder(strIVU);
	if(pCoder==nullptr)
		return false;

	CStringA strCmd;
	if(pCoder->Encoding(wCommand, parameter, strCmd)==true)
	{
		STRUCT_GPSCLIENT_VEHICLE_COMMAND* lp = new STRUCT_GPSCLIENT_VEHICLE_COMMAND(dwVehicle, (WORD)wCommand, strCmd.GetBuffer());
		m_socket.SendMsg((char*)lp, sizeof(*lp));
		strCmd.ReleaseBuffer();

		CString strMsg;
		strMsg.Format(_T("<- [%d] %s"), dwVehicle, strCmd);
		m_pParents->SendMessage(WM_VEHICLEMESSAGE, dwVehicle, (LONG)&strMsg);

		return true;
	}
	else
	{
		AfxMessageBox(_T("要发送的指令，无法根据车机的要求进行编码，请核查车机对该指令的参数要求!"));
		return false;
	}
}

BOOL CPortSocket::Dispatch(CPointF geoPoint, CSizeF geoSize)
{
	SMessage msg;
	msg.lng = geoPoint.x;
	msg.lat = geoPoint.y;
	msg.altitude = geoSize.cx;

	m_pParents->SendMessage(WM_VEHICLECONTROL, GPSCLIENT_VEHICLE_SORT, (LONG)&msg);

	return TRUE;
}
