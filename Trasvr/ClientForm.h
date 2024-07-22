#pragma once

class CChildView;
class CGpsSinkSocket;

// CClientForm 对话框

class CClientForm : public CDialog
{
	DECLARE_DYNAMIC(CClientForm)

public:
	CClientForm(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CClientForm();

// 对话框数据
	enum { IDD = IDD_CLIENTFORM };
public:
	CListCtrl m_listCtrl;

public:
	CChildView* m_pParentView;
public:
	void ShowLoginUsers(CPtrList& ClientList);
	void AddClient(CGpsSinkSocket *pSocket);
	void RemoveClient(CGpsSinkSocket *pSocket);
	void ChangeValue(CGpsSinkSocket *pSocket);

public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMessageForm)
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV 支持
	virtual BOOL OnInitDialog() override;
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CClientForm)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDisconnectUser();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickList(NMHDR *pNMHDR, LRESULT *pResult);
};
