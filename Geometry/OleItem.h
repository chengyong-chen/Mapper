#pragma once

class COleObj;

class AFX_EXT_CLASS COleItem : public COleClientItem
{
	DECLARE_SERIAL(COleItem)

	// Constructors
public:
	COleItem(COleDocument* pContainer = nullptr, COleObj* pGeom = nullptr);

	// Attributes
public:
	COleDocument* GetDocument() const
	{
		return (COleDocument*)COleClientItem::GetDocument();
	}
	CView* GetActiveView() const
	{
		return (CView*)COleClientItem::GetActiveView();
	}

	COleObj* m_pOleObj;
	BOOL UpdateExtent(CWnd* pWnd);
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COleItem)
public:
	void OnChange(OLE_NOTIFICATION wNotification, DWORD dwParam) override;
	void OnActivate() override;
protected:
	void OnGetItemPosition(CRect& rPosition) override;
	void OnDeactivateUI(BOOL bUndoable) override;
	BOOL OnChangeItemPosition(const CRect& rectPos) override;
	//}}AFX_VIRTUAL

	// Implementation
public:
	~COleItem() override;
#ifdef _DEBUG
	void AssertValid() const override;
	void Dump(CDumpContext& dc) const override;
#endif
	virtual void Serialize(CArchive& ar, const DWORD& dwVersion);
};
