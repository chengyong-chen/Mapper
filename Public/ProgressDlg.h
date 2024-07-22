#pragma once

class __declspec(dllexport) CProgressDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProgressDlg)

public:
	CProgressDlg(CWnd* pParent = nullptr);   // standard constructor
	~CProgressDlg() override;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROGRESSBAR };
#endif
public:
	CProgressCtrl m_ProgressCtrl;
public:
	void SetRange(int start,int end);
	void UpdateProgressPosition(int pos);

protected:
	void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
};
