#pragma once

class __declspec(dllexport) CVehicleTag : public CWnd
{
	DECLARE_DYNAMIC(CVehicleTag);
public:
	CVehicleTag(CString string);

private:
	CString m_string;
	CWnd* m_pParentWnd=nullptr;

public:
	BOOL Create(CWnd* pParentWnd);
	void Move(CPoint point);
	void Close();

protected:
	// Generated message map functions
	//{{AFX_MSG(CVehicleTag)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
