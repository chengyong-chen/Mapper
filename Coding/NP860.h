// Coder_NP850.h: interface for the CNP850 class.

//////////////////////////////////////////////////////////////////////

#pragma once

#include "NP706.h"

class __declspec(dllexport) CNP860 : public CNP706
{
public:
	CNP860();

	~CNP860() override;

public:
	bool DeCoding(CStringA& strMsg, CMessageReceived& msg) override;
	bool Encoding(WORD wCmd, SParameter& parameter, CStringA& strOutput) override;
	void GetStatus(CStringA& strM, CMessageReceived& msg) override;
	BOOL IsPersonal(DWORD dwStatus) override;
};
