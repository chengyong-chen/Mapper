#pragma once

class CMFCRibbonBar;
class CPOIBar;

class CRibbonBar : public CMFCRibbonBar
{
public:
	CRibbonBar(void);

	~CRibbonBar(void) override;

public:
	CPOIBar* m_pPOIBar;

public:
	void CreatePOIBar(CDatabase* pDatabase);

public:
	BOOL SetActiveCategory(CMFCRibbonCategory* pActiveCategory, BOOL bForceRestore = FALSE) override;

protected:
	//{{AFX_MSG(CRibbonBar)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
