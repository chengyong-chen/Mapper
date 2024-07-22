#pragma once

#include "Chart.h"

class __declspec(dllexport) CChartBar : public CChart
{
public:
	CChartBar();

public:
	void DrawSeries(CDC* pDC, CRecordset& rs, long nPos) override;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChartBar)
	//}}AFX_VIRTUAL

	// Implementation
public:
	~CChartBar() override;

	// Generated message map functions
protected:
	//{{AFX_MSG(CChartBar)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
