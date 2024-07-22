#pragma once

#include "Resource.h"

class CConnectWizType : public CPropertyPage
{
public:
	CConnectWizType(CODBCDatabase& ConnectInfo);

	~CConnectWizType() override;

	// Dialog Data
	enum
	{
		IDD = IDD_ADOCONNECTWIZ_DBTYPE
	};

	CStatic m_stcHelpText;

	CComboBox m_cmbDBTypes;

	// Overrides
public:
	BOOL OnSetActive() override;

	LRESULT OnWizardNext() override;

protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support

	// Implementation
protected:
	BOOL OnInitDialog() override;

	afx_msg void OnSelchangeCmbDbtype();
	DECLARE_MESSAGE_MAP()

	void SetWizardButtons() const;

	bool m_bFirstShow;

	CODBCDatabase& m_ConnectInfo;
};
