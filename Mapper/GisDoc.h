#pragma once

#include "GrfDoc.h"

#include "../Information/POU.h"
#include "../Information/POUList.h"
#include "../Information/POUListCtrl.h"
#include "../Thematics/Theme.h"

class CTopoerCtrl;
class CLink;

class CGisDoc : public CGrfDoc
{
public:
	CGisDoc() noexcept;
protected:
	DECLARE_DYNCREATE(CGisDoc)

public:
	std::list<CLink*> m_linklist;
	CPOUList m_poulist;
	CPOUListCtrl m_POUListCtrl;

public:
	void Draw(CView* pView, const CViewinfo& viewinfo, Gdiplus::Graphics& g, const CRect& invalidRect) override;
	void DrawTag(CView* pView, const CViewinfo& viewinfo, Gdiplus::Graphics& g, const CRect& invalidRect) override;

public:
	virtual BOOL ImportGdal(CString strPath, CStringList& files, bool ignoreProjection);
	virtual BOOL ImportJson(CString& strFile);
	void ReleaseWEBCap(UINT nId, CString strFolder, CString strName) override;
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGisDoc)
protected:
	void Serialize(CArchive& ar) override;
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion); // overridden for document i/o
	BOOL OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;
	//}}AFX_VIRTUAL

	// Implementation
public:
	~CGisDoc() override;
#ifdef _DEBUG
	void AssertValid() const override;
	void Dump(CDumpContext& dc) const override;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CGisDoc)		
	afx_msg void OnImportInto();
	afx_msg void OnMapPaste();
	afx_msg void OnDataProjection();

	afx_msg void OnLinkCreate();
	afx_msg void OnLinkEdit();
	afx_msg void OnLinkDelete();

	afx_msg void OnDBInjectTool();
	afx_msg void OnDBPositionTool();
	afx_msg void OnDBPickInfoTool();
	afx_msg void OnPreviewSpinTool();
	
	afx_msg void OnUnassignATT();
	afx_msg void OnRoll();
	afx_msg void OnProject();
	afx_msg void OnGeographFrame();

	afx_msg void OnProcessWipeout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
