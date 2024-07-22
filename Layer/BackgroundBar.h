#pragma once

#include "Resource.h"
#include "../../ThirdParty/tinyxml/tinyxml2/tinyxml2.h"

using namespace std;
using namespace tinyxml2;

class AFX_EXT_CLASS CBackgroundBar : public CDialog
{
public:
	CBackgroundBar(CWnd* pParent = nullptr);

	~CBackgroundBar() override;

public:
	void ChangeBackground(int nProvider, int nVariety) const;

	// Dialog Data
	//{{AFX_DATA(CBackgroundBar)
	enum
	{
		IDD = IDD_BACKGROUND
	};

	CComboBox m_ComboProvider;
	CComboBox m_ComboVariety;
	CSliderCtrl m_ForeTransparency;
	CSliderCtrl m_BackTransparency;
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

public:
	tinyxml2::XMLDocument m_xml;
	void Disable();
	void Enable(CStringA strProvider, CStringA strVariety);

	CStringA GetProvider() const;
	CStringA GetVariety() const;
	int GetFroegroundTransparency() const;
	int GetBackgroundTransparency() const;
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBackgroundBar)
protected:
	BOOL OnInitDialog() override;
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBackgroundBar)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnOK() override;
	afx_msg void OnProviderSelChanged();
	afx_msg void OnVarietySelChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	
private:
	static tinyxml2::XMLElement* FindNode(tinyxml2::XMLElement* xml, const char* type);
};
