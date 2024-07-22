#pragma once

#include "resource.h"
#include "afxwin.h"

#include "GraduatedDlg.h"

class CCylinder;

class CCylinderDlg : public CGraduatedDlg
{
	DECLARE_DYNAMIC(CCylinderDlg)

public:
	CCylinderDlg(CWnd* pParent, CCylinder& graduated, CDatabase& database, CATTDatasource& datasource);

	~CCylinderDlg() override;

	// Dialog Data
	enum
	{
		IDD = IDD_CYLINDER
	};

public:

protected:
	BOOL OnInitDialog() override;
	afx_msg void OnPaint();
	afx_msg LRESULT OnPropertyChanged(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};
