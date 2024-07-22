#ifndef _SERIALPAGE_H_
#define _SERIALPAGE_H_

#include "Resource.h"

class __declspec(dllexport) CSerialPage : public CPropertyPage
{
	public:
	CSerialPage();

	~CSerialPage() override;

	//{{AFX_DATA(CSerialPage)
	enum
	{
		IDD = IDD_SERIAL
	};

	CComboBox m_ctrlPort;
	//}}AFX_DATA

	DWORD m_dwBaudRate;
	DWORD m_dwDataBits;
	DWORD m_dwParity;
	DWORD m_dwStopBits;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSerialPage)
	protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL
	static void DDX_CBData(CDataExchange* pDX, int nIDC, DWORD& dwData);
	// Implementation
	protected:

	// Generated message map functions
	//{{AFX_MSG(CSerialPage)
	BOOL OnInitDialog() override;
	void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // _DEVICEDLG_H_
