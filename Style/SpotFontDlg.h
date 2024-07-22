#pragma once

#include "resource.h"
#include "FontCombox.h"

class CSpotFontDlg : public CDialog
{
public:
	CSpotFontDlg(CWnd* pParent = nullptr, CSpotFont* pSpot = nullptr);

	// Dialog Data
	//{{AFX_DATA(CSpotFontDlg)
	enum
	{
		IDD = IDD_SPOTFONT
	};

	CFontComboBox m_fontcombo;
	long m_nSize;
	//}}AFX_DATA

	CSpotFont* d_pSpot;

	static long COLUMN; //列数
	static long ROW; //行数

	long m_nFontCount; //符号总数
	long m_nLineCount; //当前行数

	long m_nSelected; //当前选择符号

	LOGFONT m_logFont; //字体

	void DrawFont();
	void DrawFontUnit(long nUnitNum, long nType);
	void DrawPreview();
	void SetFontCountFromLOGFONT(LOGFONT& logfont);
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpotFontDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSpotFontDlg)
	BOOL OnInitDialog() override;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnPaint();
	void OnOK() override;
	afx_msg void OnDCColor();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnChangeSize();
	afx_msg void OnSelchangeFontcombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
