#pragma once

#include <afxsplitterwndex.h>
#include "2DArray.h"

using namespace std;
#include <list>

class __declspec(dllexport) GSplitterWnd : public CSplitterWndEx
{
	DECLARE_DYNAMIC(GSplitterWnd)
public:
	GSplitterWnd();

	~GSplitterWnd() override;

private:
	std::list<int> m_shown_cols; //shown  column list
	std::list<int> m_hiden_cols; //hidden column list
	std::list<int> m_shown_rows; //shown  rows list
	std::list<int> m_hiden_rows; //hidden rows list

	CRowColInfo* m_pSavedColInfo;
	CRowColInfo* m_pSavedRowInfo;

protected:
	C2DArray m_panwnds; // array of pointers to splitter panes

public:
	void HideColumn(int colHide);
	void ShowColumn(int colShow);
	void HideRow(int colRow);
	void ShowRow(int row);
public:
	int GetMaxColumnCount() const
	{
		return m_nMaxCols;
	};
protected:
	void Init();
	int AbsToRelPosition(std::list<int>& list, int col) const;
	void RestoreColInfo(int place, int col) const;
	void RestoreRowInfo(int place, int col) const;
	void RemoveColInfo(int place, int col) const;
	void RemoveRowInfo(int place, int col) const;
	void RenumeratePanes();
	CPoint RelToAbsPosition(int i, int j);

	static int RelToAbsPosition(std::list<int>& vis_list, std::list<int>& hid_list, int cur_index);
public:
	BOOL IsPaneVisible(int row, int col);
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtSplitter)
public:
	BOOL CreateStatic(CWnd* pParentWnd, int nRows, int nCols, DWORD dwStyle = WS_CHILD|WS_VISIBLE, UINT nId = AFX_IDW_PANE_FIRST) override;
	BOOL CreateView(int row, int col, CRuntimeClass* pViewClass, SIZE sizeInit, CCreateContext* pContext) override;
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CExtSplitter)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
