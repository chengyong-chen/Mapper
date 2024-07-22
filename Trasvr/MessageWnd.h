



#pragma once


/////////////////////////////////////////////////////////////////////////////
// CMessageWnd window
#include "..\Smcom\Global.h"
#include <queue>

using namespace std;

class CMessageWnd : public CEdit
{
// Construction
public:
	CMessageWnd();

// Attributes
public:
	std::queue<CString*> m_msgShowQueue;
	CString m_strContent;
	int     m_nLine;       // 信息滚动框显示信息的行数,不超过1000行

// Operations
public:
	void AddMsg(CString* pStr);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMessageWnd)	
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	virtual ~CMessageWnd();

	// Generated message map functions
protected:
	LRESULT OnAddNewString(WPARAM pstr, LPARAM hMutex);
	//{{AFX_MSG(CMessageWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////





