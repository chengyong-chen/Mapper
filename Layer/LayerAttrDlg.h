#pragma once

#include "afxwin.h"
#include "Resource.h"

class CLayerAttrDlg : public CDialog
{
public:
	CLayerAttrDlg(CWnd* pParent = nullptr, WORD wId = 0, BYTE bLevel = 0, float minLevelObj = 0, float maxLevelObj = 0, float minLevelTag = 0, float maxLevelTag = 0, BOOL bKeyQuery = false, bool bLocked = false, bool bHide = false, bool bTagShow = true, BYTE bFixed = 0XFF, bool bDetour = false, bool bTagPivot = false, bool bTagOblige = false, bool bCentroid = false, CStringA strGeoCatogery = "", bool tagResistance=false);

	// Dialog Data
	//{{AFX_DATA(CLayerAttrDlg)
	enum
	{
		IDD = IDD_LAYERATTR
	};

	WORD m_wId;
	BOOL m_bKeyQuery;
	BOOL m_bLocked;
	BOOL m_bHide;
	BOOL m_bTagShow;
	BOOL m_bSpotDynamic;
	BOOL m_bLineDynamic;
	BOOL m_bFillDynamic;
	BOOL m_bTypeDynamic;
	BOOL m_bDetour;
	BOOL m_bTagPivot;
	BOOL m_bTagOblige;
	BOOL m_bCentroid;
	BOOL m_bTagResistant;

	float m_minLevelObj;
	float m_maxLevelObj;
	float m_minLevelTag;
	float m_maxLevelTag;
	CString m_strGeoCatogery;
	//}}AFX_DATA

	DWORD m_dwLevel;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLayerAttrDlg)
	BOOL OnInitDialog() override;
	void OnOK() override;
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL
	static void DDX_CBData(CDataExchange* pDX, int nIDC, DWORD& dwData);

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLayerAttrDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};