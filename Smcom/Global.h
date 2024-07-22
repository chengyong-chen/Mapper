#ifndef _SMSCOM_GLOBAL_H_
#define _SMSCOM_GLOBAL_H_

#define WM_DLGMSCOMM_SETFRAME         WM_USER + 2  // 向MSComm界面线程传递共享数据区指针
#define WM_DLGMSCOMM_SENDMSG          WM_USER + 3  // 指示MSComm发送共享区域中的数据

#define WM_COMMCTRL_RECVEVENT         WM_USER + 5  // 新到消息事件
#define WM_DLGMSCOMM_TIMEOUT          WM_USER + 6  // 发送指令超时
#define WM_OPENPORT                   WM_USER + 7  // 发送指令超时
#define CTK_LINK_TEST_TIMER           101

//重新装入车机信息到list
const UINT WM_REFILL_VEHICLEINFO = WM_USER+1;
const UINT WM_SENDVEHICLEMSG = WM_USER+2;

class CUDPSocket;

__declspec(dllexport) char* ConvertToAnsi(OLECHAR* szW);
__declspec(dllexport) BOOL HasWideChar(char* szA);// 判断一个字符串是否还有汉字
__declspec(dllexport) int atohex(char* p, int cnt);
__declspec(dllexport) char* SepToOctets(char* p, int cnt);//septets into octets in ASCII coding
__declspec(dllexport) char* OctToSeptects(char* p, int cnt);// octets into septetcs in ASCII coding p: 8位septects字串内容指针 cnt: 要转换的十六进制字符个数
__declspec(dllexport) void ConvertPDUPhoneNo(char* PhoneNo, int len);
__declspec(dllexport) long GetSafeArraySize(SAFEARRAY* psa);// 得到SAFEARRAY中数据的个数
__declspec(dllexport) void MoveBits(char* start, char* end, BYTE bits);
__declspec(dllexport) void ConvertBits(char* start, char* end, BYTE bits);

struct RecvSMSPackage
{
	CStringA strMsg;
	CStringA strPhone;
	DATE dateRecv;

	RecvSMSPackage()
	{
		dateRecv = COleDateTime::GetCurrentTime();
	}
};

struct RecvUDPPackage
{
	char* buff;
	UINT len;
	CStringA strIP;
	UINT nPort;
	UINT lMaxLen;

	RecvUDPPackage()
	{
		lMaxLen = 1024;
		buff = new char[lMaxLen];
		memset(buff, 0, lMaxLen);
		len = 0;
		nPort = 0;
		strIP.Empty();
	}

	~RecvUDPPackage()
	{
		delete buff;
		buff = nullptr;
		len = 0;
	}
};

struct RecvHTTPPackage
{
	char* buff;
	UINT len;
	UINT lMaxLen;

	RecvHTTPPackage()
	{
		lMaxLen = 1024;
		buff = new char[lMaxLen];
		memset(buff, 0, lMaxLen);
		len = 0;
	}

	~RecvHTTPPackage()
	{
		delete buff;
		buff = nullptr;
		len = 0;
	}
};
#endif
