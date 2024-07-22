#pragma once

#include "OutputDebugStringCatcher.h"

class __declspec(dllexport) CLogToWnd : public COutputDebugStringCatcher
{
public:
	CLogToWnd(CWnd* pParent);

	~CLogToWnd() override;

protected:
	CWnd* m_pParent;

public:
	void OutputString(const char* str) override;
};
