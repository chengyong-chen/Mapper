#pragma once

#include <afxcview.h>
#include "../Framework/gfx.h"

#include "StepDlg.h"
#include "../Public/Global.h"
#include "../Style/Symbol.h"
#include "../Style/SymbolLine.h"

extern UNIT a_UnitArray[3];
extern BYTE a_nUnitIndex;
extern __declspec(dllimport) BYTE d_nUnitIndex;


template<class T>
class CTabletView : public CListView
{
	template <class T>	friend class CSymbolDoc;
	template <class T>	friend class CSymbolView;

	DECLARE_GWDYNCREATE_T(CTabletView, T)
protected:
	CTabletView(CObject& parameter);

private:
	CSymbolDoc<T>& m_document;

public:
	BOOL EndueGeomlist(T* symbol = nullptr);
	BOOL FetchGeomlist(T* symbol = nullptr);

public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabletView)
	BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	//}}AFX_VIRTUAL

	// Implementation
protected:
#ifdef _DEBUG
	void AssertValid() const override;
	void Dump(CDumpContext& dc) const override;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CTabletView)		
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSymbolDelete();
	afx_msg void OnSymbolNew();
	afx_msg void OnSymbolStep();
	afx_msg void OnUpdateSymbolStep(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

IMPLEMENT_GWDYNCREATE_T(CTabletView, T, CListView)

BEGIN_TEMPLATE_MESSAGE_MAP(CTabletView, T, CListView)
	//{{AFX_MSG_MAP(CTabletView)
	ON_WM_CREATE()
	ON_COMMAND(ID_SYMBOL_DELETE, OnSymbolDelete)
	ON_COMMAND(ID_SYMBOL_NEW, OnSymbolNew)
	ON_COMMAND(ID_SYMBOL_STEP, OnSymbolStep)
	ON_UPDATE_COMMAND_UI(ID_SYMBOL_STEP, OnUpdateSymbolStep)
	ON_WM_DESTROY()
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemchanged)
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, OnDeleteItem)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
	//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

template<class T>
CTabletView<T>::CTabletView(CObject& parameter)
	:m_document((CSymbolDoc<T>&)parameter)
{
}

#ifdef _DEBUG
template<class T>
void CTabletView<T>::AssertValid() const
{
	CListView::AssertValid();
}
template<class T>
void CTabletView<T>::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTabletView<T> message handlers
template<class T>
BOOL CTabletView<T>::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style &= ~(LVS_LIST | LVS_ICON | LVS_SMALLICON|LVS_TYPEMASK|LVS_SORTASCENDING|LVS_SORTDESCENDING);
	cs.style |= LVS_REPORT;
	cs.style |= LVS_SINGLESEL|LVS_SHOWSELALWAYS|LVS_NOSORTHEADER|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EDITLABELS;

	return CListView::PreCreateWindow(cs);
}

template<class T>
void CTabletView<T>::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	const long hItem = pNMListView->iItem;

	const CListCtrl& pListCtrl = GetListCtrl();;
	T* pSymbol = (T*)pListCtrl.GetItemData(hItem);
	if(pNMListView->uNewState==0)
	{
		if(pNMListView->uOldState==2)
		{
			FetchGeomlist(pSymbol);
		}
		*pResult = 0;
		return;
	}
	else
	{
		EndueGeomlist(pSymbol);
	}

	CSymbolView<T>* pView = (CSymbolView<T>*)m_document.m_pSymbolView;
	pView->UpdateWindow();
	pView->Invalidate();
	*pResult = 0;
}
template<class T>
void CTabletView<T>::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* plvDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM* plvItem = &plvDispInfo->item;
	if(plvItem->pszText!=nullptr)
	{
		const CListCtrl& pListCtrl = (CListCtrl&)GetListCtrl();
		CSymbol* pSymbol = (CSymbol*)pListCtrl.GetItemData(plvItem->iItem);
		pSymbol->m_strName = plvItem->pszText;		
		*pResult = TRUE;
	}
	else
		*pResult = FALSE;
}
template<class T>
int CTabletView<T>::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CListView::OnCreate(lpCreateStruct)==-1)
		return -1;

	m_document.m_pTabletView = this;

	CListCtrl& pListCtrl = GetListCtrl();;
	CRect rect;
	pListCtrl.GetClientRect(rect);

	LV_COLUMN	lvcolumn;
	lvcolumn.mask = LVCF_FMT|LVCF_SUBITEM|LVCF_TEXT|LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_CENTER;


	lvcolumn.pszText = (LPTSTR)_T("Name");
	lvcolumn.iSubItem = 0;
	lvcolumn.cx = rect.Width();
	pListCtrl.InsertColumn(0, &lvcolumn);
	return 0;
}

template<class T>
void CTabletView<T>::OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	T* pSymbol = (T*)pNMListView->lParam;
	delete pSymbol;
	pSymbol = nullptr;
	*pResult = 0;
}

template<class T>
void CTabletView<T>::OnSymbolDelete()
{
	CListCtrl& pListCtrl = (CListCtrl&)GetListCtrl();;
	const long nItem = pListCtrl.GetNextItem(-1, LVNI_SELECTED);
	if(nItem>=0)
	{
		pListCtrl.DeleteItem(nItem);

		m_document.SetModifiedFlag(TRUE);

		CSymbolView<T>* pView = (CSymbolView<T>*)m_document.m_pSymbolView;
		pView->m_editgeom.m_pGeom = nullptr;
	}
}

template<class T>
void CTabletView<T>::OnSymbolNew()
{
	CListCtrl& pListCtrl = (CListCtrl&)GetListCtrl();;

	WORD maxID = 0;
	for(int index = 0; index<pListCtrl.GetItemCount(); index++)
	{
		T* pSymbol = (T*)pListCtrl.GetItemData(index);
		if(pSymbol!=nullptr)
		{
			maxID = max(maxID, pSymbol->m_wId);
		}
	}

	T* pSymbol = new T();
	pSymbol->m_wId = maxID+1;
	pListCtrl.InsertItem(LVIF_TEXT|LVIF_PARAM, pListCtrl.GetItemCount(), pSymbol->m_strName, 0, 0, -1, (DWORD)pSymbol);
	m_document.SetModifiedFlag(TRUE);
}

template<class T> void CTabletView<T>::OnSymbolStep()
{
	const CListCtrl& pListCtrl = (CListCtrl&)GetListCtrl();
	const long nItem = pListCtrl.GetNextItem(-1, LVNI_SELECTED);
	if(nItem<0)
		return;

	CSymbolLine* pSymbol = (CSymbolLine*)pListCtrl.GetItemData(nItem);
	CStepDlg dlg(this, pSymbol->m_nStep);
	if(dlg.DoModal()!=IDOK)
		return;

	pSymbol->m_nStep = round(dlg.m_fStep*a_UnitArray[d_nUnitIndex].pointpro*10);

	m_document.SetModifiedFlag(TRUE);
	CSymbolView<T>* pView = (CSymbolView<T>*)m_document.m_pSymbolView;
	pView->Invalidate();
}

template<class T> void CTabletView<T>::OnUpdateSymbolStep(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(this->IsKindOf(RUNTIME_CLASS_T(CTabletView, CSymbolLine)));
}

template<class T>
BOOL CTabletView<T>::FetchGeomlist(T* pSymbol)
{
	if(pSymbol==nullptr)
	{
		const CListCtrl& pListCtrl = (CListCtrl&)GetListCtrl();
		const long nItem = pListCtrl.GetNextItem(-1, LVNI_SELECTED);
		if(nItem<0)
			return FALSE;

		pSymbol = (T*)pListCtrl.GetItemData(nItem);
	}

	if(pSymbol==nullptr)
		return FALSE;

	pSymbol->m_geomlist.RemoveAll();
	POSITION pos = m_document.m_layertree.m_root.m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CGeom* geom = m_document.m_layertree.m_root.m_geomlist.GetNext(pos);
		pSymbol->m_geomlist.AddTail(geom);
	}

	return TRUE;
}

template<class T>
BOOL CTabletView<T>::EndueGeomlist(T* pSymbol)
{
	if(pSymbol==nullptr)
	{
		const CListCtrl& pListCtrl = (CListCtrl&)GetListCtrl();
		const long nItem = pListCtrl.GetNextItem(-1, LVNI_SELECTED);
		if(nItem<0)
			return FALSE;

		pSymbol = (T*)pListCtrl.GetItemData(nItem);
	}

	if(pSymbol==nullptr)
		return FALSE;

	m_document.m_layertree.m_root.m_geomlist.RemoveAll();
	POSITION pos = pSymbol->m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CGeom* geom = pSymbol->m_geomlist.GetNext(pos);
		m_document.m_layertree.m_root.m_geomlist.AddTail(geom);
	}

	return TRUE;
}

template<class T>
void CTabletView<T>::OnDestroy()
{
	if(FetchGeomlist()==TRUE)
	{
		m_document.m_layertree.m_root.m_geomlist.RemoveAll();
	}

	CListCtrl& pListCtrl = GetListCtrl();
	pListCtrl.DeleteAllItems();

	CListView::OnDestroy();
}
