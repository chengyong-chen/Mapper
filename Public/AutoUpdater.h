#pragma once
#include <string>

class __declspec(dllexport) CAutoUpdater
{
public:
	CAutoUpdater();
	virtual ~CAutoUpdater();

public:
	static CString GetNewVersion(std::string strSoftware, std::string strVersion);
};
