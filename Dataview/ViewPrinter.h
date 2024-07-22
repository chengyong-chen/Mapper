#pragma once

#include "Viewinfo.h"

class __declspec(dllexport) CViewPrinter : public CViewinfo
{
public:
	CViewPrinter(CDatainfo& foreground);
	~CViewPrinter(void);
public:
	CRect m_docPrint;

public:
	float m_marginTop;
	float m_marginBottom;
	float m_marginLeft;
	float m_marginRight;

	BOOL m_bHCenter;
	BOOL m_bVCenter;

	float m_scalePrint;

public:
	BOOL DoPreparePrinting(CView* pView, CPrintInfo* pInfo);
	void BeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	void Print(CView* pView, CDC* pDC, CPrintInfo* pInfo);
};
