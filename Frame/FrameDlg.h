#pragma once

class CColor;

class AFX_EXT_CLASS CFrameDlg : public CDialog
{
	DECLARE_DYNAMIC(CFrameDlg)

public:	
	CFrameDlg(CWnd* pParent, BOOL enabledSpere, CColor* pSphereColor1, CColor* pSphereColor2, float fSphereWidth, BOOL enabledGraticule, CColor* pGraticuleColor, float fGraticuleWidth, int lngInterval, int latInterval);   // standard constructor
	~CFrameDlg() override;

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD=IDD_FRAME };
#endif

public:
	BOOL m_bEnabledSphere;
	BOOL m_bEnabledGraticule;

	CColor* m_pSphereColor1;
	CColor* m_pSphereColor2;
	float m_fSphereWidth;

	CColor* m_pGraticuleColor;
	float m_fGraticuleWidth;
	int m_lngInterval;
	int m_latInterval;

protected:
	void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	afx_msg void OnSphereColor1();
	afx_msg void OnSphereColor2();
	afx_msg void OnGraticuleColor();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	DECLARE_MESSAGE_MAP()
};
