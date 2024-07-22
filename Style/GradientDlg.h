// GradientDlg.h : header file

#pragma once
#include "Resource.h"

#include<map>
using namespace std;

class CColor;

// CGradientDlg dialog
class CGradientDlg : public CDialogEx
{
public:
	CGradientDlg(CWnd* pParent, std::map<unsigned int, CColor*>& stops);

	// Dialog Data
	enum
	{
		IDD = IDD_GRADIENT
	};

	unsigned int m_stopposition;
	CColor* m_stopcolor;
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support

public:
	std::map<unsigned int, CColor*>& m_stops;

public:
	void ActiveStop(int postion);
	void Redrawpreview() const;
	void Redrawactive() const;

	// Implementation
protected:
	// Generated message map functions
	BOOL OnInitDialog() override;
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStnClickedStopperholder();

	afx_msg void OnRemove();
	afx_msg void OnAdd();
	afx_msg void OnEnChangeStopposition();
	afx_msg void OnDblclickColor();
	afx_msg void OnStnClickedRuler();
};
