#pragma once

#include "../Public/Global.h"

class __declspec(dllexport) CPoupin : public CObject
{
public:
	CPoupin();
public:
	DWORD m_dwId;
	CPointF m_Point;
	CString m_strNote;
};
