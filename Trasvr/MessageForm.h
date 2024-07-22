#pragma once

class CMessageReceived;

// CMessageForm 对话框

class CMessageForm : public CDialog
{
	DECLARE_DYNAMIC(CMessageForm)

public:
	CMessageForm(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CMessageForm();

// 对话框数据
	enum { IDD = IDD_MESSAGEFORM };
public:
	CListCtrl m_listCtrl;

public:
	void ShowMessages();
	void AddMessage(CMessageReceived* pMsg);

public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMessageForm)
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV 支持
	virtual BOOL OnInitDialog() override;
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CMessageForm)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMRclickList(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
};
