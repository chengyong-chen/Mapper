#pragma once

class CGisView;
#include "../Framework/AidView.hpp"

class CGeoView : public CGisView
{
	DECLARE_GWDYNCREATE(CGeoView)
protected:
	CGeoView(CObject& parameter) noexcept;

private:
	CGeoDoc& m_document;
	CAidView<CGeoView>* m_pAidView;

public:
	void SetAidView(CView* pAidView) override { m_pAidView = (CAidView<CGeoView>*)pAidView; };

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGisView)
protected:
	void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) override;
	//}}AFX_VIRTUAL

	// Implementation
public:
	~CGeoView() override;
#ifdef _DEBUG
	void AssertValid() const override;
	void Dump(CDumpContext& dc) const override;
#endif

public:
	void InactivateAll() override;

protected:

	// Generated message map functions
protected:
	//{{AFX_MSG(CGeoView)
	//}}AFX_MSG	
	afx_msg LONG OnGeomActivated(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnNewGeomDrew(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnNewGeomBred(UINT WPARAM, LONG LPARAM);
	afx_msg LONG OnPreDeleteGeom(UINT WPARAM, LONG LPARAM);
	DECLARE_MESSAGE_MAP()
};
