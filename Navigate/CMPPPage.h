#pragma once


// CCMPPPage 对话框

class CCMPPPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CCMPPPage)

public:
	CCMPPPage();
	virtual ~CCMPPPage();

// 对话框数据
	enum { IDD = IDD_CMPP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strIP;
	DWORD m_dwPort;
	CString m_strUsername;
	CString m_strPWD;
	virtual void OnOK() override;
};
