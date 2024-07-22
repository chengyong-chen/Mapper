#pragma once

#include "GpsSinkSocket.h"
#include "../Navigate/Markup.h"

class AFX_EXT_CLASS CXmlSinkSocket : public CGpsSinkSocket
{
public:
	CXmlSinkSocket();

	~CXmlSinkSocket() override;
	CString m_strXmlCmd;
	CMarkup* m_pXmlParser;

public:
	BOOL IsCorrectHeader(char* header, int size = 8) override;
	BOOL IsCorrectMessage(char* msg, int size) override;
	int GetMessageSizeFromHeader(char* header) override;

	//send/receive message attributes
	void* cioRecvBytes(int& length, int& Error) override;
	BOOL cioSendBytes(void* msg, int msg_size) override;

	void* ReceiveData(int& length, int& Error) override;
	bool SendData(void* msg, DWORD length) override;

public:
	BOOL SendMsg(char* lp, DWORD length) override;
	BOOL SendStringToClient(DWORD vhcId, TCHAR* str) override;

	static BOOL AskInspectOneVehicle(DWORD dwID);
	CString GetXmlCmd(CString name) const;
};
