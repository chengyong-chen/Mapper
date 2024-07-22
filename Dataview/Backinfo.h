#pragma once

class __declspec(dllexport) CBackInfo
{
public:
	CBackInfo();

public:
	CString m_strProvider;
	CString m_strVariety;

public:
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
};
