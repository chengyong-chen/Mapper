#pragma once


#include "History.h"

class __declspec(dllexport) CGo
{
public:
	CGo();
	~CGo();

public:
	BOOL Initlize();

	BOOL GoBack(CFrameWnd* pChildFrm);
	BOOL GoForward(CFrameWnd* pChildFrm);
	BOOL GoHome(CFrameWnd* pChildFrm);
	BOOL Refresh(CFrameWnd* pChildFrm);
	BOOL AddHistory(CHistory& ni);
	BOOL CanBack() const;
	BOOL CanForward() const;
	BOOL CanHome() const;

	CHistory* GetCurentHistory();
	CHistory* GetHistory(int nIndex);

	unsigned __int8 m_nCurent;

protected:
	CArray<CHistory, CHistory&> m_historys;

public:
	const CGo& operator =(const CGo& Src);
};
