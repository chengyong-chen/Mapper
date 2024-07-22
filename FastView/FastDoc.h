#pragma once

class CDib;
class CFastView;

class CFastDoc : public CDocument
{
protected: // create from serialization only
	CFastDoc();
	DECLARE_DYNCREATE(CFastDoc)

public:	
	CDib*      m_Dib;
public:
	CDib*      GetDib(void)      { return m_Dib;     }
public:
	void       Draw(CDC* pDC,CFastView* pview);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFastDoc)
	public:
	virtual BOOL OnNewDocument() override;
	virtual void Serialize(CArchive& ar) override;
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName) override;
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFastDoc();
#ifdef _DEBUG
	virtual void AssertValid() const override;
	virtual void Dump(CDumpContext& dc) const override;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CFastDoc)
	afx_msg void OnFileExport();
	afx_msg void OnEditOr();
	afx_msg void OnUpdateEditOr(CCmdUI* pCmdUI);
	afx_msg void OnEditAnd();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};