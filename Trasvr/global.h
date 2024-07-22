
#pragma once

#include "resource.h"
///////////////////////////////////////////////////////////////////////////
#define WM_CUSTOM_LEFTTREE_CONTEXTMENU    WM_USER + 9

#define WM_CUSTOM_GETVHCINFO_BYID         WM_USER + 23
#define WM_CUSTOM_GETVHCINFO_BYTEL        WM_USER + 24

#define WM_DELETECLIENT                   WM_USER + 26  

class CChildView ;
class CGpsSinkSocket;
class CXmlSinkSocket;


UINT DealRecvMsgThread(LPVOID lpParam);

UINT SaveSendMsgThread(LPVOID lpParam);
UINT SaveRecvMsgThread(LPVOID lpParam);

void AcptGpsClient(SOCKET socket,DWORD dwParent);
void AcptXmlClient(SOCKET socket,DWORD dwParent);
void AcptWebClient(SOCKET socket,DWORD dwParent);

void ProcessGpsClientRecv(void* msg, CGpsSinkSocket* pSocket,CChildView* pChildView);
void ProcessXmlClientRecv(void* msg, CXmlSinkSocket* pSocket,CChildView* pChildView);

void ProcessSocketError(CGpsSinkSocket* pSocket,CChildView* pChildView);
