#pragma once

class __declspec(dllexport) CHistory
{
public:
	CHistory();

public:
	void Into(CWnd* pWnd);

	CString m_strMap;
	CPoint m_Anchor;
	float m_fScale;
public:
	const CHistory& operator =(const CHistory& Src);
};
