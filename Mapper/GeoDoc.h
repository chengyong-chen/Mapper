#pragma once

class CGisDoc;
class CTopodbCtrl;

class CGeoDoc : public CGisDoc
{
protected: // create from serialization only
	CGeoDoc() noexcept;
	DECLARE_DYNCREATE(CGeoDoc)

	// Operations
public:
	void Draw(CView* pView, const CViewinfo& viewinfo, Gdiplus::Graphics& g, const CRect& invalidRect) override;

	// Overrides
public:
	void Serialize(CArchive& ar) override;
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion); 
	BOOL OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;

	// Implementation
public:
	~CGeoDoc() override;

#ifdef _DEBUG
	void AssertValid() const override;
	void Dump(CDumpContext& dc) const override;
#endif

protected:
	// Generated message map functions
protected:
	//{{AFX_MSG(CGeoDoc)	
	afx_msg void OnTopoCreateEdges();
	afx_msg void OnReleaseViewer();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};