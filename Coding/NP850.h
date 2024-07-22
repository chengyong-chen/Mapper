// Coder_NP850.h: interface for the CNP850 class.

//////////////////////////////////////////////////////////////////////

#pragma once

#include "NP706.h"

class __declspec(dllexport) CNP850 : public CNP706
{
public:
	CNP850();

	~CNP850() override;

public:
	bool DeCoding(CStringA& strMsg, CMessageReceived& msg) override;
	bool Encoding(WORD wCmd, SParameter& parameter, CStringA& strOutput) override;
};
