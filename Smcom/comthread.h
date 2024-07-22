#pragma once

// SmsComuThread.h : header file

//#include <stlhelpper_included.h>

#include "../Socket/Messageq.h"

using namespace std;
#include <map>

struct SVehicle;

class CMessageQueue;
class CMessageReceived;
class CMessageWillSend;

class CSMSThread;
class CUDPThread;
class CTCPThread;
class CHTTPThread;

typedef std::map<DWORD, SVehicle*> CIDVehicleMap;

class AFX_EXT_CLASS CComThread : public CWinThread
{
	DECLARE_DYNCREATE(CComThread)
protected:
	CComThread();

public:
	static CSMSThread* m_pSMSThread;
	static CUDPThread* m_pUDPThread;
	static CTCPThread* m_pTCPThread;
	static CHTTPThread* m_pHTTPThread;

public:
	bool m_bValid;

	// Attributes
public:
	static CMessageQueue g_recv_msg_queue;

	static CIDVehicleMap m_mapIdVhc;
	static HWND g_hMainWnd;
	static HWND g_MapEditMsgWnd;

public:
	CMessageQueue g_send_msg_queue;

public:
	void PushSendMsgToQueue(CMessageWillSend* pMsg);

public:
	static void PushRecvMsgToQueue(CMessageReceived* pMsg);
	static void PutMsgToMessageWnd(LPCTSTR lpMsg);
	static void SetMapEditMsgWndHnd(HWND hWnd);
	static void SetMainWndHnd(HWND hWnd);

	static UINT DealSendMsgThread(LPVOID lParam);
	static UINT DealRecvMsgThread(LPVOID lParam);

	HANDLE DealSendThreadEnded;

	virtual BOOL SendVehicleMsg(WPARAM wParam, LPARAM lParam) { return TRUE; };

public:
	static SVehicle* FindVehicleInfo(LPCSTR lpszTelePhone, bool bUseGPRS = false);
	static SVehicle* FindVehicleInfo(const DWORD& dwId);

	// Operations

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComThread)
public:
	BOOL InitInstance() override;
	int ExitInstance() override;
	//}}AFX_VIRTUAL

	// Implementation
protected:
	~CComThread() override;

	// Generated message map functions
	//{{AFX_MSG(CComThread)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
};

/////////////////////////////////////////////////////////////////////////////
