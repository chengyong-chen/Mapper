#pragma once
#include "Ruler.h"
#include "../Framework/GSplitterWnd.h"

class __declspec(dllexport) MSplitterWnd : public GSplitterWnd
{
	DECLARE_DYNAMIC(MSplitterWnd)
public:
	MSplitterWnd(CRuler& ruler);

	~MSplitterWnd() override;

	// Attributes
public:
	CRuler& m_ruler;

public:
	void RecalcLayout() override;

	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
};
