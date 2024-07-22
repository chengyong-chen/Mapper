#pragma once

#include "Resource.h"
class CODBCDatabase;
class CConnectWizStart : public CPropertyPage
{
public:
	CConnectWizStart(CODBCDatabase& ConnectInfo);

	~CConnectWizStart() override;

	// Dialog Data
	enum
	{
		IDD = IDD_ADOCONNECTWIZ_START
	};

	// Overrides
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()

public:
	BOOL OnSetActive() override;
};
