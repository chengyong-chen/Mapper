#pragma once

#include "resource.h"

// CProxyDlg 对话框

class __declspec(dllexport) CProxyDlg : public CDialog
{
	DECLARE_DYNAMIC(CProxyDlg)

public:
	CProxyDlg(CWnd* pParent = nullptr); // 标准构造函数
	CProxyDlg(CWnd* pParent, CString strServer, int nPort); // 标准构造函数
	~CProxyDlg() override;

	// 对话框数据
	enum
	{
		IDD = IDD_PROXY
	};

private:
	int m_nNormalWidth;
	int m_nExpandedWidth;
	BOOL m_bExpanded;

protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strServer;
	int m_nPort;
	afx_msg void OnSelchangeProxy();
	BOOL OnInitDialog() override;
	afx_msg void OnClickedSetup();
};