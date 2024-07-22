///////////////////////////////////////////////////
// Insert data to database.  modify database.

#include "StdAfx.h"

#include "../Coding/Message.h"
#include "ExStdCFunction.h"

#include <atlbase.h>
#include "../Coding/Instruction.h"
#include <tchar.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int GPS_MSG_POSITION = -1;
const int GPS_MSG_INIT = -10;

bool WriteReceivedMsgToDataBase(CMessageReceived* pMsg, ADOCG::_ConnectionPtr& pDBConnect)
{
	if(pMsg==nullptr)
		return false;

	try
	{
		CString strSQL;
		switch(pMsg->m_wCommand)
		{
		case GPSSERVER_VEHICLE_MESSAGE:
		{
			CString strMsgBody = pMsg->m_strMessage;
			strMsgBody.Replace(_T("'"), _T("''"));

			CString strTime;
			VariantTimeToYYMMDDhhmmString(pMsg->m_varGPSTime, strTime);

			strSQL.Format(_T("INSERT INTO DualMsg (Vehicle,UserID,方向,时间,内容) VALUES(%d,%d,%d,'%s','%s') "), pMsg->m_dwVehicle, pMsg->m_dwUser, 0, strTime, strMsgBody);
		}
		break;
		case GPSSERVER_VEHICLE_MOVE:
		{
			CString strTime;
			CString strCenterRcvTime;
			VariantTimeToYYMMDDhhmmString(pMsg->m_varGPSTime, strTime);
			VariantTimeToYYMMDDhhmmString(pMsg->m_varRevcTime, strCenterRcvTime);

			strSQL.Format(_T("INSERT INTO RecvPos (Vehicle,UserID,GPS时间,经度,纬度,速度,高度,方向,接收时间,有效性,状态) VALUES(%d,%d,'%s',%.4f, %.4f, %.2f, %.1f, %d,'%s', %d, %d)"), pMsg->m_dwVehicle, pMsg->m_dwUser, strTime, pMsg->m_Lon, pMsg->m_Lat, pMsg->m_Veo, pMsg->m_Height, pMsg->m_Direct, strCenterRcvTime, pMsg->m_bAvailable, pMsg->m_dwStatus);
		}
		break;
		case GPSCLIENT_VEHICLE_DISPATCH:
		{
			strSQL.Format(_T("update VehicleDispatch set 派遣次数=派遣次数+1 where id=%d"), pMsg->m_dwVehicle);
		}
		break;
		}

		if(strSQL.IsEmpty()==FALSE)
		{
			_variant_t varRowsAff;
			pDBConnect->Execute(LPCTSTR(strSQL), &varRowsAff, ADOCG::adCmdText);
		}
	}
	catch(_com_error&)
	{
		_bstr_t bstr;
		_variant_t var(short(0));

		if(pDBConnect->Errors->Count>0)
		{
			bstr = pDBConnect->Errors->Item[var]->Description;
			Log_or_AfxMessageBox(bstr);
		}
	}

	return true;
}

bool VariantTimeToYYMMDDhhmmString(DATE vDate, CString& strDate)
{
	SYSTEMTIME systm;
	VariantTimeToSystemTime(vDate, &systm);
	strDate.Format(_T("%d/%d/%d %d:%d:%d"), systm.wYear, systm.wMonth, systm.wDay, systm.wHour, systm.wMinute, systm.wSecond);

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool OpenCenterDataBase(ADOCG::_ConnectionPtr& pDBConnect, LPCTSTR strConnectionString, LPCTSTR lpszUserName, LPCTSTR lpszPassWord)
{
	try
	{
		const CString dbms = AfxGetApp()->GetProfileString(_T("VechileDB"), _T("DBMS"), _T("ACCESS"));
		const CString strDSN = AfxGetApp()->GetProfileString(_T("VechileDB"), _T("DSN"), _T("MS Access Database"));
		const CString strServerName = AfxGetApp()->GetProfileString(_T("VechileDB"), _T("ServerName"), _T("Trasvr"));
		const CString strUID = AfxGetApp()->GetProfileString(_T("VechileDB"), _T("UID"), nullptr);
		const CString strPWD = AfxGetApp()->GetProfileString(_T("VechileDB"), _T("PWD"), nullptr);

		CString strConnection;
		if(dbms.Find(_T("MICROSOFT SQL SERVER"))>=0)
		{
			strConnection.Format(_T("Provider=sqloledb;Data Source=%s;Initial Catalog=%s;User Id=%s;Password=%s;"), strServerName, strDSN, strUID, strPWD);
		}
		else if(dbms.Find(_T("ORACLE"))>=0)
		{
			strConnection.Format(_T("Provider=MSDAORA.1;Data Source=%s;User Id=%s;Password=%s;"), strServerName, strUID, strPWD);
		}
		else if(dbms.Find(_T("ACCESS"))>=0)
		{
			strConnection.Format(_T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s;"), strServerName);
		}
		else if(dbms.Find(_T("DBASE"))>=0)
		{
			strConnection.Format(_T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s.dbf;User Id=%s;Password=%s;"), strServerName, strUID, strPWD);
		}

		try
		{
			if
				FAILED(CoInitialize(nullptr))
				return false;

			pDBConnect.CreateInstance(_uuidof(ADOCG::Connection));
			const _bstr_t strCnn(strConnection);
			pDBConnect->CommandTimeout = 30;
			pDBConnect->ConnectionTimeout = 30;
			pDBConnect->Open(strCnn, "", "", 0);
		}
		catch(_com_error&)
		{
			_bstr_t bstr;
			const _variant_t var(short(0));

			if(pDBConnect->Errors->Count>0)
			{
				bstr = pDBConnect->Errors->Item[var]->Description;
				Log_or_AfxMessageBox(bstr);
			}
			Log_or_AfxMessageBox(CString(false ? "数据库连接失败" : "Failed when opening database"));
			return false;
		}
		Log_or_AfxMessageBox(CString(false ? "数据库连接成功" : "Succeed opening database!"));
	}
	catch(CException*ex)
	{
		ex->Delete();
		return false;
	}

	return true;
}

bool CloseCenterDatabase(const ADOCG::_ConnectionPtr& pDBConnect)
{
	if(pDBConnect!=nullptr)
	{
		try
		{
			pDBConnect->Close();
		}
		catch(_com_error&)
		{
			_bstr_t bstr;
			const _variant_t var(short(0));

			if(pDBConnect->Errors->Count>0)
			{
				bstr = pDBConnect->Errors->Item[var]->Description;
				Log_or_AfxMessageBox(bstr);
			}
			return false;
		}
	}

	return true;
}

void ImportRouteFileToDatabase(const ADOCG::_ConnectionPtr& pDBConnect, LPCTSTR lpszSourceFile, DWORD dwIdParam)
{
	CString strFile;
	if(lpszSourceFile==nullptr)
	{
		TCHAR CurrentDir[256];
		GetCurrentDirectory(255, CurrentDir);
		CString strPath = AfxGetApp()->GetProfileString(_T(""), _T("Gps Route Path"), nullptr)+_T("*.Grf");
		CFileDialog dlg(TRUE,
			_T("*.Grf"),
			strPath,
			OFN_HIDEREADONLY,
			_T("Gps route Files (*.Grf)|*.Grf||"),
			nullptr);

		dlg.m_ofn.lpstrTitle = _T("请选择要导入的历史文件");
		if(dlg.DoModal()!=IDOK)
		{
			SetCurrentDirectory(CurrentDir);
			return;
		}
		SetCurrentDirectory(CurrentDir);
		strFile = dlg.GetPathName();
	}
	else
	{
		strFile = lpszSourceFile;
	}

	double lng;
	double lat;
	double altitude = -1.0;
	float fSpeed;
	float fBearing;
	CString strDate;
	long nTime;
	DATE fvarTime = 0;

	CFile m_file;
	DWORD dwId = dwIdParam;
	bool bNewFileVersion = false;
	DWORD dwFileVer = 0;

	try
	{
		if(m_file.Open(strFile, CFile::modeRead|CFile::shareDenyWrite|CFile::typeBinary)==FALSE)
		{
			DWORD hr = GetLastError();
			_ASSERTE(!"Cannot open the .grf file");
			return;
		}
		else
		{
			char szFileFlag[4];
			memset(szFileFlag, 0, sizeof(szFileFlag));
			m_file.Read(szFileFlag, 3);
			bNewFileVersion = strcmp(szFileFlag, "GRF")==0;
			//旧版本
			if(!bNewFileVersion)
			{
				m_file.SeekToBegin();
			}
			//新版本
			else
			{
				m_file.Read(&dwFileVer, 4);
			}
			m_file.Read(&dwId, 4);
		}
		CString szSQLCommand;
		ULONG length = m_file.GetLength();
		int nCount = 0;
		//旧版本
		if(!bNewFileVersion)
		{
			nCount = (length-4)/(8*2+4*3);
		}
		//新版本
		else
		{
			nCount = (length-3-4-4)/(sizeof(double)*3+sizeof(float)*2);
		}

		_variant_t varRowsAff;
		for(int i = 0; i<nCount; i++)
		{
			m_file.Read(&lng, 8);
			m_file.Read(&lat, 8);
			m_file.Read(&fSpeed, 4);
			m_file.Read(&fBearing, 4);
			if(!bNewFileVersion)
			{
				m_file.Read(&nTime, 4); //
				strDate = _T("2003/03/29 10:22");
			}
			else
			{
				m_file.Read(&fvarTime, sizeof(DATE));
				VariantTimeToYYMMDDhhmmString(fvarTime, strDate);
			}

			szSQLCommand.Format(_T("INSERT INTO RecvPos (经度, 纬度, 速度, 方向, 时间, ID) Values (%g,%g,%g,%g,'%s',%d)"),
				lng,
				lat,
				fSpeed,
				fBearing, // should %g
				strDate,
				dwId);
			ADOCG::_RecordsetPtr rs = pDBConnect->Execute(LPCTSTR(szSQLCommand), &varRowsAff, ADOCG::adCmdText);
		}
	}
	catch(_com_error&)
	{
		_bstr_t bstr;
		_variant_t var(short(0));

		if(pDBConnect->Errors->Count>0)
		{
			bstr = pDBConnect->Errors->Item[var]->Description;
			Log_or_AfxMessageBox(bstr);
		}
		return;
	}
	catch(CFileException*ex)
	{
		ex->ReportError();
		ex->Delete();
		return;
	}
	return;
}

UINT GetRecordsCount(LPCTSTR lpszTableName, const ADOCG::_ConnectionPtr& pDBConnec)
{
	if(pDBConnec==nullptr)
	{
		return 0;
	}
	try
	{
		CString szSQLCommand;
		szSQLCommand.Format(_T("SELECT COUNT(*) AS nCount FROM VehicleTable"), lpszTableName);
		TRACE(szSQLCommand);
		_variant_t varRowsAff;

		// 1 or 0, excute sql.
		ADOCG::_RecordsetPtr rs = pDBConnec->Execute(LPCTSTR(szSQLCommand), &varRowsAff, ADOCG::adCmdText);
		_variant_t var;
		//if(rs->RecordCount <=0)
		if(rs->EndofFile)
		{
			// error username . no records
			return 0;
		}
		var = (rs->Fields->Item[_T("nCount")]->Value);
		return long(var);
	}

	catch(_com_error&)
	{
		_bstr_t bstr;
		_variant_t var(short(0));

		if(pDBConnec->Errors->Count>0)
		{
			bstr = pDBConnec->Errors->Item[var]->Description;
			Log_or_AfxMessageBox(bstr);
		}
		//throw;
		return 0;
	}

	return 0;
}
