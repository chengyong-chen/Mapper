#include "stdafx.h"

#include "Port.h"
#include "Vehicle.h"
#include "VehicleInfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DWORD CPort::m_dwUser = 0;

IMPLEMENT_DYNAMIC(CPort, CObject)

//DWORD CPort::m_dwUser = 0;
/////////////////////////////////////////////////////////////////////////////
// CPort

CPort::CPort(CFrameWnd* pParents)
{
	m_pParents = pParents;
	m_pVehicleInfoDlg = nullptr;

	m_dwId = 1001;

	m_hThread = nullptr;
	m_bThreadAlive = false;

	m_message.altitude = 0;
	m_message.nSatelites = 0;
	m_message.bRotateMap = AfxGetApp()->GetProfileInt(_T("Receiver"), _T("Rotatemap"), 0);
}

bool CPort::Open()
{
	/*
		if(m_pVehicleInfoDlg != nullptr)
		{
			m_pVehicleInfoDlg->PostMessage(WM_DESTROY,0,0);
			delete m_pVehicleInfoDlg;
			m_pVehicleInfoDlg = nullptr;
		}
	*/
	return true;
}

void CPort::Close()
{
	if(m_pVehicleInfoDlg!=nullptr)
	{
		m_pVehicleInfoDlg->PostMessage(WM_DESTROY, 0, 0);
		m_pVehicleInfoDlg->DestroyWindow();
		delete m_pVehicleInfoDlg;
		m_pVehicleInfoDlg = nullptr;
	}

	m_database.Close();
}

void CPort::ReceiveMsg()
{
	if(m_pVehicleInfoDlg!=nullptr&&m_pVehicleInfoDlg->m_hWnd!=nullptr)
	{
		m_pVehicleInfoDlg->SetInfo(m_message);
	}
}

bool CPort::RestartMonitor() const
{
	if(m_hThread==nullptr)
	{
		return FALSE;
	}
	else
	{
		TRACE(_T("Thread resumed\n"));
		m_hThread->ResumeThread();
		return TRUE;
	}
}

//
// Suspend the comm thread
//
bool CPort::StopMonitor() const
{
	if(m_hThread==nullptr)
	{
		return FALSE;
	}
	else
	{
		TRACE(_T("Thread suspended\n"));
		m_hThread->SuspendThread();
		return TRUE;
	}
}

bool CPort::GetUserInfo(const DWORD& dwId, Content& content)
{
	if(m_database.IsOpen()==FALSE)
		return false;

	CRecordset rs(&m_database);
	try
	{
		CString strSQL;
		strSQL.Format(_T("SELECT DISTINCT 姓名,颜色,车牌号码,IVUSIM,手机号码 FROM View_Vehicle WHERE ID=%d AND UserID=%d"), dwId, CPort::m_dwUser);
		TRACE(strSQL);
		rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly);
		if(rs.IsEOF()==FALSE)
		{
			rs.GetFieldValue(_T("姓名"), content.strUser);
			content.strUser.TrimRight(_T(" "));
			rs.GetFieldValue(_T("颜色"), content.strColor);
			content.strColor.TrimRight(_T(" "));
			rs.GetFieldValue(_T("车牌号码"), content.strCode);
			content.strCode.TrimRight(_T(" "));
			rs.GetFieldValue(_T("IVUSIM"), content.strLink);
			content.strLink.TrimRight(_T(" "));
			rs.GetFieldValue(_T("手机号码"), content.strDriver);
			content.strDriver.TrimRight(_T(" "));
			TRACE("%s %s %s %s %s\n", content.strUser, content.strCode, content.strLink, content.strDriver, content.strColor);
		}
		rs.Close();

		strSQL.Format(_T("SELECT DISTINCT 用途 FROM View_Vehicle_Info WHERE ID=%d"), dwId);
		TRACE(strSQL);
		rs.Open(CRecordset::snapshot, strSQL, CRecordset::readOnly);
		if(rs.IsEOF()==FALSE)
		{
			rs.GetFieldValue(_T("用途"), content.strType);
			content.strType.TrimRight(_T(" "));
		}
		rs.Close();
	}
	catch(CDBException*ex)
	{
		ex->ReportError();
		rs.Close();
		ex->Delete();
		return FALSE;
	}

	return TRUE;
}

/*
bool CPort::CheckDatabase()
{
	m_strODBCDSN = AfxGetApp()->GetProfileString(_T("VechileDB"),_T("DSN"),nullptr);
	m_strServerName = AfxGetApp()->GetProfileString(_T("VechileDB"),_T("ServerName"),nullptr);
	m_strUID = AfxGetApp()->GetProfileString(_T("VechileDB"),_T("UID"),nullptr);
	m_strPWD = AfxGetApp()->GetProfileString(_T("VechileDB"),_T("PWD"),nullptr);

	CDatabase database;
	try
	{
		CString strConnect;
		strConnect.Format(_T("DSN=%s;DBQ=%s;UID=%s;PWD=%s"),m_strODBCDSN,m_strServerName,m_strUID,m_strPWD);
		database.OpenEx(strConnect, CDatabase::noOdbcDialog | CDatabase::openReadOnly);
	}
	catch(CDBException* ex)
	{
		ex->Delete();
	}

	if(database.IsOpen() == FALSE)
	{
		AfxMessageBox(_T("监控数据库无法打开，需重新配置!"));
		return false;
	}
	else
	{
		database.Close();
		return true;
	}
}

*/
bool CPort::ParseRMC(CString sentence, SMessage& message)
{
	if(sentence.IsEmpty()==TRUE)
		return FALSE;

	CString field;

	sentence = sentence.Mid(sentence.Find(_T(','))+1);
	field = sentence.Left(sentence.Find(_T(',')));
	message.nTime = _ttoi(field);

	sentence = sentence.Mid(sentence.Find(_T(','))+1);
	field = sentence.Left(sentence.Find(_T(',')));
	if(field==_T("A"))
		message.m_bAvailable = TRUE;
	else
		message.m_bAvailable = FALSE;

	sentence = sentence.Mid(sentence.Find(_T(','))+1);
	field = sentence.Left(sentence.Find(_T(',')));
	message.lat = _tcstod(field, nullptr);

	sentence = sentence.Mid(sentence.Find(_T(','))+1);
	field = sentence.Left(sentence.Find(_T(',')));
	if(field==_T("S"))
	{
		message.lat *= -1;
	}

	sentence = sentence.Mid(sentence.Find(_T(','))+1);
	field = sentence.Left(sentence.Find(_T(',')));
	message.lng = _tcstod(field, nullptr);

	sentence = sentence.Mid(sentence.Find(_T(','))+1);
	field = sentence.Left(sentence.Find(_T(',')));
	if(field==_T("W"))
	{
		message.lng *= -1;
	}

	sentence = sentence.Mid(sentence.Find(_T(','))+1);
	field = sentence.Left(sentence.Find(_T(',')));
	message.fSpeed = (float)_tcstod(field, nullptr);
	message.fSpeed *= 1.8532f;

	sentence = sentence.Mid(sentence.Find(_T(','))+1);
	field = sentence.Left(sentence.Find(_T(',')));
	message.nBearing = (float)_tcstod(field, nullptr);

	double degree;
	double minints;

	degree = int(message.lng/100);
	minints = message.lng-degree*100;
	message.lng = degree+minints/60;

	degree = int(message.lat/100);
	minints = message.lat-degree*100;
	message.lat = degree+minints/60;

	sentence.Empty();
	return TRUE;
}

bool CPort::ParseGGA(CString sentence, SMessage& message)
{
	if(sentence.IsEmpty()==TRUE)
		return FALSE;

	CString field;

	sentence = sentence.Mid(sentence.Find(_T(','))+1);
	sentence = sentence.Mid(sentence.Find(_T(','))+1);
	sentence = sentence.Mid(sentence.Find(_T(','))+1);
	sentence = sentence.Mid(sentence.Find(_T(','))+1);
	sentence = sentence.Mid(sentence.Find(_T(','))+1);
	sentence = sentence.Mid(sentence.Find(_T(','))+1);

	sentence = sentence.Mid(sentence.Find(_T(','))+1);
	field = sentence.Left(sentence.Find(_T(',')));
	message.nSatelites = _ttoi(field);

	sentence = sentence.Mid(sentence.Find(_T(','))+1);
	sentence = sentence.Mid(sentence.Find(_T(','))+1);
	field = sentence.Left(sentence.Find(_T(',')));
	message.altitude = _ttoi(field);

	sentence.Empty();
	//	if(bValide == 0)
	//	{
	//		sentence.Empty();
	//		return FALSE;
	//	}

	return TRUE;
}
