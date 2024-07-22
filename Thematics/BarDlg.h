#pragma once


#include "ChartDlg.h"
#include "Resource.h"

using namespace std;
class CBar;

class CBarDlg : public CChartDlg
{
	DECLARE_DYNAMIC(CBarDlg)

public:
	CBarDlg(CWnd* pParent, CBar& bar, CDatabase& database, CATTDatasource& datasource);

	~CBarDlg() override;

	// Dialog Data
	enum
	{
		IDD = IDD_BAR
	};

public:
	CBar& m_bar;

protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	afx_msg void OnPaint();
	BOOL OnInitDialog() override;
	afx_msg LRESULT OnPropertyChanged(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};
