#pragma once
#include "MsgHook.h"
#include<Afxmt.h>
// CEditableListCtrl
typedef BOOL(*PFNEDITORCALLBACK)(CWnd** pWnd, int nRow, int nColumn, CString& strSubItemText, DWORD_PTR dwItemData, void* pThis, BOOL bUpdate);

class CEditableListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CEditableListCtrl)

public:
	CEditableListCtrl();

	~CEditableListCtrl() override;
	// Retrieves the data (lParam) associated with a particular item.
	DWORD_PTR GetItemData(int nItem) const;
	// Retrieves the data (lParam) associated with a particular item.
	DWORD_PTR GetItemDataInternal(int nItem) const;
	// Sets the data (lParam) associated with a particular item.
	BOOL SetItemData(int nItem, DWORD_PTR dwData);
	// Removes a single item from the control.
	BOOL DeleteItem(int nItem);
	// Removes all items from the control.
	BOOL DeleteAllItems();
	// Finds an item in the control matching the specified criteria.  
	int FindItem(LVFINDINFO* pFindInfo, int nStart = -1) const;
	// Call to sort items using a custom comparison function.
	BOOL SortItems(PFNLVCOMPARE pfnCompare, DWORD_PTR dwData);
	// Adds an item to the control.
	int InsertItem(const LVITEM* pItem);
	int InsertItem(int nItem, LPCTSTR lpszItem);
	int InsertItem(int nItem, LPCTSTR lpszItem, int nImage);

	BOOL EnsureSubItemVisible(int nItem, int nSubItem, CRect* pRect = nullptr);

	typedef enum Sort
	{
		None = 0,
		Ascending = 1,
		Descending = 2,
		Auto = 4,
		SortBits = 7
	} Sort;

	typedef enum Comparer
	{
		NotSet,
		Int,
		Double,
		String,
		StringNumber,
		StringNoCase,
		StringNumberNoCase,
		Date
	} Comparer;

protected:
	static int CALLBACK CompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static int Compare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static int CompareInt(LPCTSTR pLeftText, LPCTSTR pRightText);
	static int CompareDouble(LPCTSTR pLeftText, LPCTSTR pRightText);
	static int CompareString(LPCTSTR pLeftText, LPCTSTR pRightText);
	static int CompareNumberString(LPCTSTR pLeftText, LPCTSTR pRightText);
	static int CompareNumberStringNoCase(LPCTSTR pLeftText, LPCTSTR pRightText);
	static int CompareStringNoCase(LPCTSTR pLeftText, LPCTSTR pRightText);
	static int CompareDate(LPCTSTR pLeftText, LPCTSTR pRightText);
	CCriticalSection m_oLock;
protected:
	DECLARE_MESSAGE_MAP()
public:
	typedef struct EditorInfo
	{
		PFNEDITORCALLBACK m_pfnInitEditor;
		PFNEDITORCALLBACK m_pfnEndEditor;
		CWnd* m_pWnd;
		BOOL m_bReadOnly;
		EditorInfo();
		EditorInfo(PFNEDITORCALLBACK pfnInitEditor, PFNEDITORCALLBACK pfnEndEditor, CWnd* pWnd = nullptr);
		BOOL IsSet() const
		{
			return (m_pfnInitEditor||m_pWnd);
		}
	} EditorInfo;

	typedef struct CellInfo
	{
		EditorInfo m_eiEditor;
		COLORREF m_clrBack;
		COLORREF m_clrText;
		DWORD_PTR m_dwUserData;
		int m_nColumn;
		CellInfo(int nColumn);
		CellInfo(int nColumn, COLORREF clrBack, COLORREF clrText, DWORD_PTR dwUserData = 0);
		CellInfo(int nColumn, EditorInfo eiEditor, COLORREF clrBack, COLORREF clrText, DWORD_PTR dwUserData =0);
		CellInfo(int nColumn, EditorInfo eiEditor, DWORD_PTR dwUserData =0);
	} CellInfo;

	typedef struct ColumnInfo
	{
		EditorInfo m_eiEditor;
		int m_nColumn;
		COLORREF m_clrBack;
		COLORREF m_clrText;
		Sort m_eSort;
		Comparer m_eCompare;
		PFNLVCOMPARE m_fnCompare;
		ColumnInfo(int nColumn);
		ColumnInfo(int nColumn, PFNEDITORCALLBACK pfnInitEditor, PFNEDITORCALLBACK pfnEndEditor, CWnd* pWnd = nullptr);
	} ColumnInfo;

	typedef struct ItemData
	{
		EditorInfo m_eiEditor;
		CArray<CellInfo*> m_aCellInfo;
		COLORREF m_clrBack;
		COLORREF m_clrText;
		DWORD_PTR m_dwUserData;
		ItemData(DWORD_PTR dwUserData = 0);
		ItemData(COLORREF clrBack, COLORREF clrText, DWORD_PTR dwUserData = 0);

		BOOL IsSet() const
		{
			return (m_dwUserData||m_clrBack!=-1||m_clrText!=-1||!m_aCellInfo.IsEmpty());
		}

		virtual ~ItemData();
	} ItemData;

protected:
	CPtrArray m_aItemData;
	CPtrArray m_aColumnInfo;

	BOOL DeleteItemData(int nItem);
	BOOL DeleteAllItemsData();
	BOOL DeleteAllColumnInfo();
	BOOL DeleteColumnInfo(int nIndex);
	EditorInfo* m_pEditor;
	EditorInfo m_eiDefEditor;
	int m_nEditingRow;
	int m_nEditingColumn;
	int m_nRow;
	int m_nColumn;
	COLORREF m_clrDefBack;
	COLORREF m_clrDefText;
	CMsgHook m_msgHook;
	BOOL m_bHandleDelete;
	int m_nSortColumn;
	PFNLVCOMPARE m_fnCompare;
	DWORD_PTR m_dwSortData;
public:
	///////////////////////////////////////////////////////////////////////////
	//Editor stuff
	///////////////////////////////////////////////////////////////////////////
	void SetColumnEditor(int nColumn, PFNEDITORCALLBACK pfnInitEditor = nullptr, PFNEDITORCALLBACK m_pfnEndEditor = nullptr, CWnd* pWnd = nullptr);
	void SetColumnEditor(int nColumn, CWnd* pWnd);
	void SetCellEditor(int nRow, int nColumn, PFNEDITORCALLBACK pfnInitEditor = nullptr, PFNEDITORCALLBACK m_pfnEndEditor = nullptr, CWnd* pWnd = nullptr);
	void SetCellEditor(int nRow, int nColumn, CWnd* pWnd);
	void SetRowEditor(int nRow, PFNEDITORCALLBACK pfnInitEditor, PFNEDITORCALLBACK m_pfnEndEditor = nullptr, CWnd* pWnd = nullptr);
	void SetRowEditor(int nRow, CWnd* pWnd);
	void SetDefaultEditor(PFNEDITORCALLBACK pfnInitEditor, PFNEDITORCALLBACK m_pfnEndEditor = nullptr, CWnd* pWnd = nullptr);
	void SetDefaultEditor(CWnd* pWnd);
	void SetColumnReadOnly(int nColumn, bool bReadOnly = true);
	void SetCellReadOnly(int nRow, int nColumn, bool bReadOnly = true);
	void SetRowReadOnly(int nRow, bool bReadOnly = true);
	BOOL IsColumnReadOnly(int nColumn) const;
	BOOL IsRowReadOnly(int nRow) const;
	BOOL IsCellReadOnly(int nRow, int nColumn);

	BOOL DisplayEditor(int nItem, int nSubItem);
	void HideEditor(BOOL bUpdate = TRUE);

	int GetColumnCount(void) const;

	///////////////////////////////////////////////////////////////////////////
	//Display stuff
	///////////////////////////////////////////////////////////////////////////
	void SetRowColors(int nItem, COLORREF clrBk, COLORREF clrText);
	void SetColumnColors(int nColumn, COLORREF clrBack, COLORREF clrText);
	void SetCellColors(int nRow, int nColumn, COLORREF clrBack, COLORREF clrText);

	///////////////////////////////////////////////////////////////////////////
	//Item stuff
	///////////////////////////////////////////////////////////////////////////
	BOOL AddItem(int ItemIndex, int SubItemIndex, LPCTSTR ItemText, int ImageIndex = -1);
	void DeleteSelectedItems(void);
	void HandleDeleteKey(BOOL bHandle = TRUE);
	void SelectItem(int nItem, BOOL bSelect);
	BOOL DeleteAllColumns(void);
	BOOL Reset(void);

	///////////////////////////////////////////////////////////////////////////
	//Sorting stuff
	///////////////////////////////////////////////////////////////////////////
	void SetColumnSorting(int nColumn, Sort eSort, Comparer eSortType = String);
	void SetColumnSorting(int nColumn, Sort eSort, PFNLVCOMPARE fnCallBack);
	BOOL SortOnColumn(int nColumn, BOOL bChangeOrder = FALSE);

	CellInfo* GetCellInfo(int nItem, int nSubItem) const;
protected:
	afx_msg BOOL OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
	void PreSubclassWindow() override;
	BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	BOOL PreTranslateMessage(MSG* pMsg) override;
	afx_msg BOOL OnNMCustomdraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHdnItemclick(NMHDR* pNMHDR, LRESULT* pResult);

	int GetItemIndexFromData(DWORD_PTR dwData) const;

	CEditableListCtrl::ColumnInfo* GetColumnInfo(int nColumn) const;

	BOOL SetCellData(int nItem, int nSubItem, DWORD_PTR dwData);
	DWORD_PTR GetCellData(int nRow, int nColumn) const;
	virtual BOOL OnAddNew(void) { return FALSE; }
	BOOL m_bInvokeAddNew;
public:
	HACCEL m_hAccel;
};
