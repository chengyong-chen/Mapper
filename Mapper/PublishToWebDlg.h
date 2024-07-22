#pragma once


class CPublishToWebDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPublishToWebDlg)

public:
	CPublishToWebDlg(CWnd* pParent, CString strTitle, CString strDescription, BYTE minMapLevel, BYTE maxMapLevel, bool partialable=true);   // standard constructor
	~CPublishToWebDlg() override;

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PUBLISHTOWEB };
#endif
public:
	BOOL m_bExportProperty;
	CString m_strName;
	CString m_strDescription;
	CString m_strAPIUrl;
	CString m_strClient;
	CString m_strUsername;
	CString m_strPassword;
	BYTE m_minLevel;
	BYTE m_maxLevel;
	bool m_bUpdate;
	BOOL m_bPartial;
private:
	BYTE m_minMapLevel;
	BYTE m_maxMapLevel;
	bool m_partialable;

protected:
	void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	BOOL OnInitDialog() override;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedLocal();
	afx_msg void OnBnClickedUpdate();
	afx_msg void OnBnWebUidChanged();
};
