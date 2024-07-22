#pragma once

class CMapsvrDlg ;
class CMapSinkSocket;

void AcptClient(SOCKET socket,DWORD dwParent);
	            
void ProcessClientRecv(void* msg, CMapSinkSocket* pSocket,CMapsvrDlg* pMapsvrDlg);

void ProcessSocketError(CMapSinkSocket* pSocket);

