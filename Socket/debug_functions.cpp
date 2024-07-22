#include "stdafx.h"
#include "../Coding/message.h"

#include "ExStdCFunction.h"

#include <atlbase.h>
#include <math.h>

using namespace std;
#include <ostream>
#include <iosfwd>
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HWND o_hMsgOutHandle = nullptr;

ostream& operator <<(ostream& os, const CMessageReceived& msg)
{
	os<<"msg.m_wCommand =";
	os<<msg.m_wCommand;

	os<<"msg.m_Direct =";
	os<<msg.m_Direct;

	os<<"msg.m_dwStatus =";
	os<<msg.m_dwStatus;

	os<<"msg.m_Height =";
	os<<msg.m_Height;

	os<<"msg.m_Lat =";
	os<<msg.m_Lat;

	os<<"msg.m_Lon =";
	os<<msg.m_Lon;

	os<<"msg.m_Veo=";
	os<<msg.m_Veo;

	os<<"msg.m_dwVehicle=";
	os<<msg.m_dwVehicle;
	return os;
}

//�ж������ǲ�����Ҫд��־�ļ�
bool NeedLog()
{
	//��ʼ����Ϣ����ģ�顣
	CRegKey regKey;
	static DWORD dwNeedLog = 100;
	if(dwNeedLog==100)
	{
		if(ERROR_SUCCESS==regKey.Open(HKEY_CURRENT_USER,
			_T("Software\\Mapday\\GServer\\1.0\\settings"))
			)
		{
			if(ERROR_SUCCESS!=regKey.QueryValue(dwNeedLog, _T("NeedLog")))
			{
				dwNeedLog = 1;
			}
			regKey.Close();
		}
	}
	return dwNeedLog!=0;
}

void Log(const CMessageReceived& msg)
{
	if(!NeedLog())
		return;

	CString strDir = GetCurrentDir();
	strDir += _T("\\Log");
	CreateDirectory(strDir, nullptr);
	const CTime localTime = CTime::GetCurrentTime();
	const CString strFileName = localTime.Format("\\message%Y_%m_%d.log");
	strDir += strFileName;

	static std::ofstream out;
	if(!out.is_open())
	{
		try
		{
			out.open(strDir, ios::out|ios::app|ios::ate);
			out<<"===================================================================="<<endl;
			out<<"===================================================================="<<endl;
			const CTime localTime = CTime::GetCurrentTime();
			const CString str = localTime.Format("Time %Y-%m-%d %H:%M:%S\n");
			CStringA stra(str);
			const LPCSTR lpsz = stra.GetBuffer(0);
			out.write(lpsz, strlen(lpsz));
			out.flush();
			stra.ReleaseBuffer();
		}
		catch(...)
		{
			return;
		}
	}

	out<<msg;
	out<<endl;
	out.flush();
}

void Log(LPCSTR szText)
{
	if(!NeedLog())
		return;

	if(szText==nullptr)
		return;

	CString strDir = GetCurrentDir();
	strDir += _T("\\Log");
	CreateDirectory(strDir, nullptr);
	const CTime localTime = CTime::GetCurrentTime();
	const CString strFileName = localTime.Format("\\Log%Y_%m_%d.log");
	strDir += strFileName;

	static ofstream out;

	if(!out.is_open())
	{
		try
		{
			out.open(strDir, ios::out|ios::app|ios::ate);
		}
		catch(...)
		{
			return;
		}
	}

	CStringA strText;
	for(UINT i = 0; i<strlen(szText); i++)
	{
		if(szText[i]=='\r')
			strText += " CR ";
		else if(szText[i]=='\n')
			strText += " LF ";
		else
			strText += szText[i];
	}

	static int nCout = 0;
	static bool bFirst = true;
	if(bFirst)
	{
		out<<"===================================================================="<<endl;
		out<<"===================================================================="<<endl;
		bFirst = !bFirst;
		const CString str = localTime.Format("Time %Y-%m-%d %H:%M:%S\n");
		CStringA stra(str);
		const LPCSTR lpsz = stra.GetBuffer(0);
		out.write(lpsz, strlen(lpsz));
		out.flush();
		stra.ReleaseBuffer();
	}
	char sz[30];
	itoa(nCout, sz, 10);
	strcat_s(sz, ":    ");
	out.write(sz, strlen(sz));
	out.write(strText, strlen(strText));
	out.write("\n", 1);
	out.flush();
	nCout++;
}

DWORD GetUserId()
{
	return 10000;
}

void SetMsgOutHandle(HWND hWnd)
{
	o_hMsgOutHandle = hWnd;
}

/////////////////////////////////////////////////////////

void Log_or_AfxMessageBox(LPCTSTR szText, UINT uType)
{
	CString* pStr = new CString(szText);
	::PostMessage(o_hMsgOutHandle, WM_USER+4, (WPARAM)pStr, 0);
#ifdef _DEBUG
	// AfxMessageBox(szText, uType);	
#else
	//Log(szText);
#endif
}

void Log_or_AfxMessageBox(const _bstr_t& bsText, UINT uType /*= MB_OK*/)
{
	Log_or_AfxMessageBox(LPCTSTR(bsText), uType);
}

//////////////////////////////////////////////////////////////////////////
//       dwUserId û��ʹ�ã���ΪdwSysCode ����Ӳ�����к�
//		 ��dwUserId�ϳɶ��ɡ�
//
bool GenerateSNs(DWORD dwSysCode, DWORD dwUserId, CString* strSNs)
{
	if(strSNs==nullptr)
	{
		return false;
	}
	for(int i = 0; i<4; i++)
		strSNs[i].Empty();

	DWORD dwSeeds[4] = {0, 0, 0, 0};
	// strSNs 0
	DWORD dwSeed = dwSysCode;//^ dwUserId;
	// 0 ��ʾ ʹ�÷������汾��1 ��ʾ���������ʹ��Access ���ݿ�
	DWORD dwVersionType = 0;
	CRegKey regKey;
	if(ERROR_SUCCESS==regKey.Open(HKEY_CURRENT_USER, _T("Software\\Mapday\\GServer\\1.0\\settings")))
	{
		if(ERROR_SUCCESS!=regKey.QueryValue(dwVersionType, _T("VersionType")))
			dwVersionType = 0;
		regKey.Close();
	}

	//����ǵ�����
	if(1==dwVersionType)
	{
		dwSeed = ~dwSeed;
		dwSeed ^= 0x33336666;
	}

	__int64 ret = __int64(dwSeed)*__int64(dwSeed);
	dwSeed = static_cast<DWORD>(ret);
	ret = __int64(dwSeed)*__int64(dwSeed);
	dwSeed = static_cast<DWORD>(ret);
	dwSeeds[0] = dwSeed;
	// strSNs 1
	char* buff;//[MAX_PATH];
	double fRet;
	int decimal, sign;
	dwSeed = dwSysCode;// ^ dwUserId;
	//fRet = double(DWORD(dwSeed));
	fRet = 10*sqrt(double(dwSeed))+0.45665;
	//��double���ַ���. buff ֻ������Ч����,û��С����
	buff = _ecvt(fRet, 11, &decimal, &sign);
	dwSeeds[1] = DWORD(_atoi64(buff));
	// strSNs 2
	fRet = 10*sqrt(double(dwSeeds[1]))+0.45665;
	//��double���ַ���. buff ֻ������Ч����,û��С����
	buff = _ecvt(fRet, 11, &decimal, &sign);
	dwSeeds[2] = DWORD(_atoi64(buff));
	// strSNs 3	
	fRet = 10*sqrt(double(dwSeeds[2]))+0.45665;
	//��double���ַ���. buff ֻ������Ч����,û��С����
	buff = _ecvt(fRet, 11, &decimal, &sign);
	dwSeeds[3] = DWORD(_atoi64(buff));

	char cChar;
	//����ÿ�����ж�
	for(int i = 0; i<4; i++)
	{
		//����4�ֽ���ֵ
		for(int j = 0; j<4; j++)
		{
			cChar = char((((dwSeeds[i]&(0xff<<(8*j)))>>(8*j))%26)+'A');
			_ASSERTE(isupper(cChar));
			strSNs[i].Insert(strSNs[i].GetLength(), cChar);
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// ��ȡ���ص�����Ϣ
int GetLocalHostInfos(CString& host, CString& name, CString& aliase, CString& ip)
{
	char hostname[256];
	const int res = gethostname(hostname, sizeof(hostname));
	if(res!=0)
	{
		printf("Error: %un", WSAGetLastError());
		return -1;
	}
	host = hostname;

	////////////////
	// ������������ȡ������Ϣ. 
	//
	hostent* pHostent = gethostbyname(hostname);
	if(pHostent==nullptr)
	{
		return -1;
	}
	//////////////////
	// �������ص�hostent��Ϣ.
	//
	const hostent& he = *pHostent;
	name = he.h_name;
	aliase.Format(_T("%s"), he.h_aliases);

	sockaddr_in sa;
	for(int nAdapter = 0; he.h_addr_list[nAdapter]; nAdapter++)
	{
		memcpy(&sa.sin_addr.s_addr, he.h_addr_list[nAdapter], he.h_length);
		// ���������IP��ַ.
		ip += inet_ntoa(sa.sin_addr); // ��ʾ��ַ��
	}
	return 1;
}
