#include "stdafx.h"
#include "Global.h"
#include "Layer.h"
#include "LayerTreeCtrl.h"
#include "Laypt.h"
#include "Laypy.h"
#include "Laypp.h"
#include "Layif.h"
#include "Laylg.h"
#include "SetIfDlg.h"
#include "PickLayDlg.h"
#include "LayerTree.h"

#include "../Style/Line.h"
#include "../Style/Fill.h"
#include "../Style/Spot.h"
#include "../Style/StyleDlg.h"
#include "../Style/TypeDlg.h"
#include "../Style/HintDlg.h"
#include "../Style/SpotCtrl.h"

#include "../DataView/Global.h"

#include "../Action/Append.h"
#include "../Action/Remove.h"
#include "../Action/Modify.h"
#include "../Action/Document/LayerTreeCtrl.h"
#include "../Viewer/Global.h"

#include "../Public/ODBCDatabase.h"

#include "../Thematics/Theme.h"
#include "../Thematics/ThemeDlg.h"

#include "../Database/ConnectionDlg.h"
#include "../Database/ATTDatasourceDlg.h"
#include "../Database/GEODatasourceDlg.h"

#include "../Information/ColumnPicker.h"

#include <list>
#include "../Style/Type.h"

using namespace Undoredo::Document::LayerTreeCtrl::Layer;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString CLayerTreeCtrl::m_strActiveLayerName;

CLayerTreeCtrl::CLayerTreeCtrl(CDocument& document, const CDatainfo& datainfo, CLayerTree& layertree, Undoredo::CActionStack& actionstack)
	: m_document(document), m_datainfo(datainfo), m_layertree(layertree), m_actionStack(actionstack)
{
	m_bDragging = FALSE;
	m_pDragImage = nullptr;
	m_bMulti = TRUE;
	m_hFixedItem = nullptr;

	m_bShowhidelayer = true;
}

BEGIN_MESSAGE_MAP(CLayerTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CLayerTreeCtrl)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndlabeledit)
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBegindrag)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_LAYER_MOVEDOWN, OnMoveDown)
	ON_UPDATE_COMMAND_UI(ID_LAYER_MOVEDOWN, OnUpdateMoveDown)
	ON_COMMAND(ID_LAYER_MOVEUP, OnMoveUp)
	ON_UPDATE_COMMAND_UI(ID_LAYER_MOVEUP, OnUpdateMoveUp)
	ON_COMMAND(ID_LAYER_DELETE, OnDelete)
	ON_COMMAND(ID_LAYER_NEWAFTER, OnNewAfter)
	ON_COMMAND(ID_LAYER_NEWBEFORE, OnNewBefore)
	ON_COMMAND(ID_LAYER_NEWIN, OnNewIn)
	ON_COMMAND(ID_LAYER_SETTYPE, OnSetType)
	ON_COMMAND(ID_LAYER_SETSTYLE, OnSetStyle)
	ON_COMMAND(ID_LAYER_SETSPOT, OnSetSpot)
	ON_COMMAND(ID_LAYER_ALLON, OnAllon)
	ON_COMMAND(ID_LAYER_ALLOFF, OnAlloff)
	ON_COMMAND(ID_LAYER_PROPERTY, OnSetProperty)
	ON_COMMAND(ID_LAYER_MULTI, OnMulti)
	ON_COMMAND(ID_LAYER_SETHINT, OnSetHint)

	ON_COMMAND(ID_GEODB_CONNECTION, OnGEODatabase)
	ON_COMMAND(ID_GEODB_DATASOURCE, OnGEODatasource)
	ON_COMMAND(ID_ATTDB_CONNECTION, OnATTDatabase)
	ON_COMMAND(ID_ATTDB_DATASOURCE, OnATTDatasource)
	ON_COMMAND(ID_ATTDB_APPLYNAME, OnApplyName)
	ON_COMMAND(ID_ATTDB_APPLYCODE, OnApplyCode)
	ON_COMMAND(ID_ATTDB_STATISTICS, OnATTTheme)
	ON_COMMAND(ID_LAYER_TOLOGICAL, OnToLogical)
	ON_COMMAND(ID_LAYER_SETIF, OnSetIf)
	ON_COMMAND(ID_LAYER_REMATCH, OnRematch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CLayerTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Layer.dll"));
	AfxSetResourceHandle(hInst);
	CImageList stateImages;
	if(stateImages.Create(IDB_LAYERSTATE, 16, 0, RGB(255, 0, 0)) == TRUE)
	{
		SetImageList(&stateImages, LVSIL_STATE);
		stateImages.Detach();
	}
	CImageList typeImages;
	if(typeImages.Create(IDB_LAYERTYPE, 16, 0, RGB(255, 0, 0)) == TRUE)
	{
		SetImageList(&typeImages, LVSIL_NORMAL);
		typeImages.Detach();
	}
	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
	return 1;
}

void CLayerTreeCtrl::BuildTree()
{
	if(this->m_hWnd != nullptr)
	{
		DeleteAllItems();
		AddNode(nullptr, nullptr, &m_layertree.m_root);
		const HTREEITEM hRoot = CTreeCtrl::GetRootItem();
		CTreeCtrl::Expand(hRoot, TVE_EXPAND);
	}
}
void CLayerTreeCtrl::AddNode(HTREEITEM hitemParent, HTREEITEM hitemAfter, CLayer* pLayer)
{
	if(pLayer == nullptr)
		return;
	if(m_bShowhidelayer == false && pLayer->m_bHide == true)
		return;

	TV_INSERTSTRUCT tvstruct;
	tvstruct.hParent = hitemParent;
	tvstruct.hInsertAfter = hitemAfter == nullptr ? TVI_FIRST : hitemAfter;
	tvstruct.item.mask = TVIF_TEXT | TVIF_STATE | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvstruct.item.pszText = pLayer->m_strName.GetBuffer(pLayer->m_strName.GetLength());
	tvstruct.item.stateMask = LVIS_STATEIMAGEMASK;
	tvstruct.item.state = INDEXTOSTATEIMAGEMASK(pLayer->m_bVisible ? 2 : 1);
	tvstruct.item.iSelectedImage = pLayer->Gettype();
	tvstruct.item.iImage = pLayer->Gettype();
	tvstruct.item.lParam = (DWORD)pLayer;
	HTREEITEM hItem = InsertItem(&tvstruct);
	pLayer->m_strName.ReleaseBuffer(pLayer->m_strName.GetLength());
	if(m_strActiveLayerName.IsEmpty() == FALSE && m_strActiveLayerName == pLayer->m_strName)
	{
		m_hFixedItem = hItem;
		SelectItem(m_hFixedItem);
	}
	pLayer = pLayer->FirstChild();
	while(pLayer != nullptr)
	{
		AddNode(hItem, TVI_LAST, pLayer);
		pLayer = pLayer->Nextsibling();
	}	
}

void CLayerTreeCtrl::MoveItem(HTREEITEM hitemBeMoved, HTREEITEM hitemInsertIn, HTREEITEM hitemInsertAfter)
{
	if(hitemBeMoved == nullptr)
		return;

	CLayer* pLayer = (CLayer*)GetItemData(hitemBeMoved);
	if(pLayer->Gettype() == 10)//layif
		return;

	CLayer* pOldParent = pLayer->Parent();
	if(pOldParent == nullptr)
		return;
	CLayer* pOldAfter = pLayer->Prevsibling();

	CLayer* pNewParent = (CLayer*)GetItemData(hitemInsertIn);
	if(pNewParent == nullptr)
		return;
	if(pNewParent->IsControllable() == false)//layon and laylg
		return;

	CLayer* pNewAfter = (hitemInsertAfter == nullptr || hitemInsertAfter == TVI_LAST || hitemInsertAfter == TVI_FIRST) ? nullptr : (CLayer*)GetItemData(hitemInsertAfter);

	pOldParent->DetachChild(pLayer);
	DeleteItem(hitemBeMoved);
	pNewParent->AddChild(pNewAfter, pLayer);
	m_document.SetModifiedFlag(TRUE);
	m_document.UpdateAllViews(nullptr);

	CLayerTreeCtrl::AddNode(hitemInsertIn, hitemInsertAfter, pLayer);
	const std::pair<CLayer*, CLayer*> original(pOldParent, pOldAfter);
	const std::pair<CLayer*, CLayer*> modified(pNewParent, pNewAfter);

	Undoredo::CModify<CLayerTreeCtrl&, CLayer*, std::pair<CLayer*, CLayer*>>* pModify = new Undoredo::CModify<CLayerTreeCtrl&, CLayer*, std::pair<CLayer*, CLayer*>>(*this, pLayer, original, modified);
	pModify->undo = Undoredo::Document::LayerTreeCtrl::Layerlist::Undo_Modify;
	pModify->redo = Undoredo::Document::LayerTreeCtrl::Layerlist::Redo_Modify;
	m_actionStack.Record(pModify);
}

BOOL CLayerTreeCtrl::IsChildOf(HTREEITEM hitemChild, HTREEITEM hitemSuspectedParent) const
{
	do
	{
		if(hitemChild == hitemSuspectedParent)
			break;
	} while((hitemChild = GetParentItem(hitemChild)) != nullptr);

	return (hitemChild != nullptr);
}

BOOL CLayerTreeCtrl::IsUperOf(HTREEITEM hitemSuspectedup, HTREEITEM hitemdown) const
{
	do
	{
		hitemdown = GetUperItem(hitemdown);
		if(hitemdown == hitemSuspectedup)
			return TRUE;
	} while(hitemdown != hitemSuspectedup && hitemdown != nullptr);
	return FALSE;
}

HTREEITEM CLayerTreeCtrl::GetUperItem(HTREEITEM hitem) const
{
	if(hitem == nullptr)
		return nullptr;

	if(this->GetPrevSiblingItem(hitem) == nullptr)
	{
		return this->GetParentItem(hitem);
	}
	else
	{
		hitem = this->GetPrevSiblingItem(hitem);
		while(this->GetChildItem(hitem) != nullptr)
		{
			hitem = this->GetChildItem(hitem);
			while(this->GetNextSiblingItem(hitem) != nullptr)
				hitem = this->GetNextSiblingItem(hitem);
		}

		return hitem;
	}
}

HTREEITEM CLayerTreeCtrl::GetDownItem(HTREEITEM hitem) const
{
	if(this->GetChildItem(hitem) != nullptr)
		return this->GetChildItem(hitem);
	else if(this->GetNextSiblingItem(hitem) != nullptr)
		return this->GetNextSiblingItem(hitem);

	while(this->GetParentItem(hitem) != nullptr)
	{
		hitem = this->GetParentItem(hitem);
		if(this->GetNextSiblingItem(hitem) != nullptr)
		{
			return this->GetNextSiblingItem(hitem);
		}
	}

	return nullptr;
}

HTREEITEM CLayerTreeCtrl::GetItem(DWORD dwData) const
{
	if(dwData == -1)
		return nullptr;

	HTREEITEM hItem = this->GetDownItem(nullptr);
	while(hItem != nullptr)
	{
		if(this->GetItemData(hItem) == dwData)
		{
			return hItem;
		}
		hItem = this->GetDownItem(hItem);
	}

	return nullptr;
}

HTREEITEM CLayerTreeCtrl::GetItemByName(CString strName) const
{
	if(strName.GetLength() <= 0)
		return nullptr;

	HTREEITEM hItem = nullptr;
	hItem = this->GetDownItem(hItem);
	while(hItem != nullptr)
	{
		const CLayer* layer = (CLayer*)this->GetItemData(hItem);
		if(layer->m_strName == strName)
		{
			return hItem;
		}
		hItem = this->GetDownItem(hItem);
	}
	return nullptr;
}

void CLayerTreeCtrl::OnMoveDown()
{
	const HTREEITEM hSelectItem = GetSelectedItem();
	if(hSelectItem == nullptr)
		return;
	const HTREEITEM hAfterItem = this->GetNextSiblingItem(hSelectItem);
	if(hAfterItem == nullptr)
		return;
	const HTREEITEM hParentItem = this->GetParentItem(hSelectItem);
	if(hParentItem == nullptr)
		return;

	CLayerTreeCtrl::MoveItem(hSelectItem, hParentItem, hAfterItem);
}

void CLayerTreeCtrl::OnUpdateMoveDown(CCmdUI* pCmdUI)
{
	const HTREEITEM hSelectItem = GetSelectedItem();
	if(hSelectItem == nullptr)
		pCmdUI->Enable(FALSE);
	else if(this->GetNextSiblingItem(hSelectItem) == nullptr)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

void CLayerTreeCtrl::OnMoveUp()
{
	const HTREEITEM hSelectItem = GetSelectedItem();
	if(hSelectItem == nullptr)
		return;
	const HTREEITEM hPreviousItem = this->GetPrevSiblingItem(hSelectItem);
	if(hPreviousItem == nullptr)
		return;
	const HTREEITEM hAfterItem = this->GetPrevSiblingItem(hPreviousItem);
	const HTREEITEM hParentItem = this->GetParentItem(hSelectItem);
	if(hParentItem == nullptr)
		return;

	CLayerTreeCtrl::MoveItem(hSelectItem, hParentItem, hAfterItem);
}

void CLayerTreeCtrl::OnUpdateMoveUp(CCmdUI* pCmdUI)
{
	const HTREEITEM hSelectItem = GetSelectedItem();
	if(hSelectItem == nullptr)
		pCmdUI->Enable(FALSE);
	else if(this->GetPrevSiblingItem(hSelectItem) == nullptr)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

CLayer* CLayerTreeCtrl::GetSelLayer() const
{
	const HTREEITEM Item = this->GetSelectedItem();;
	if(Item != nullptr)
	{
		CLayer* layer = (CLayer*)this->GetItemData(Item);
		return layer;
	}
	else
		return nullptr;
}

void CLayerTreeCtrl::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult)
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	if(pTVDispInfo->item.pszText != nullptr)
	{
		pTVDispInfo->item.mask = TVIF_TEXT;
		SetItem(&pTVDispInfo->item);

		CLayer* layer = (CLayer*)this->GetItemData(pTVDispInfo->item.hItem);
		const CString original = layer->m_strName;
		layer->m_strName = pTVDispInfo->item.pszText;
		m_document.SetModifiedFlag(TRUE);
		const CString modified = layer->m_strName;

		Undoredo::CModify<CLayerTreeCtrl&, CLayer*, CString>* pModify = new Undoredo::CModify<CLayerTreeCtrl&, CLayer*, CString>(*this, layer, original, modified);
		pModify->undo = Undoredo::Document::LayerTreeCtrl::LayerName::Undo_Modify;
		pModify->redo = Undoredo::Document::LayerTreeCtrl::LayerName::Redo_Modify;
		m_actionStack.Record(pModify);
	}
	*pResult = 0;
}

void CLayerTreeCtrl::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	CPoint ptAction;
	UINT nFlags;

	GetCursorPos(&ptAction);
	ScreenToClient(&ptAction);
	ASSERT(m_bDragging == FALSE);
	m_hitemDrag = HitTest(ptAction, &nFlags);
	if(m_hitemDrag != nullptr && this->GetParentItem(m_hitemDrag) != nullptr)
	{
		m_bDragging = TRUE;
		m_hitemDrop = nullptr;

		ASSERT(m_pDragImage == nullptr);
		m_pDragImage = CreateDragImage(m_hitemDrag);
		if(m_pDragImage != nullptr)
		{
			m_pDragImage->DragShowNolock(TRUE);
			m_pDragImage->SetDragCursorImage(0, CPoint(0, 0));
			m_pDragImage->BeginDrag(0, CPoint(0, 0));
			m_pDragImage->DragMove(ptAction);
			m_pDragImage->DragEnter(this, ptAction);
		}
		SetCapture();
	}

	*pResult = 0;
}

void CLayerTreeCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	HTREEITEM hitem;
	UINT flags;

	if(m_bDragging)
	{
		if((hitem = HitTest(point, &flags)) != nullptr)
		{
			if(m_pDragImage != nullptr)
			{
				m_pDragImage->DragMove(point);
				m_pDragImage->DragLeave(this);
			}
			SelectDropTarget(hitem);
			m_hitemDrop = hitem;
			if(m_pDragImage != nullptr)
			{
				m_pDragImage->DragEnter(this, point);
			}

			if(::GetKeyState(VK_CONTROL) < 0)
				SetCursor(AfxGetApp()->LoadCursor(IDC_ITEMMOVETO));
			else
			{
				CLayer* check1 = (CLayer*)GetItemData(hitem);
				CLayer* check2 = (CLayer*)GetItemData(m_hitemDrag);
				SetCursor(AfxGetApp()->LoadCursor(IDC_ITEMMOVEIN));
			}
		}
	}

	CTreeCtrl::OnMouseMove(nFlags, point);
}

void CLayerTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_bDragging && m_hitemDrag != nullptr && m_hitemDrop != nullptr)
	{
		if(m_pDragImage != nullptr)
		{
			m_pDragImage->DragLeave(this);
			m_pDragImage->EndDrag();
		}

		if(m_hitemDrag != m_hitemDrop && !IsChildOf(m_hitemDrop, m_hitemDrag))
		{
			if(::GetKeyState(VK_CONTROL) < 0)
			{
				const HTREEITEM hParentItem = this->GetParentItem(m_hitemDrop);
				MoveItem(m_hitemDrag, hParentItem, m_hitemDrop);
			}
			else if(GetParentItem(m_hitemDrag) != m_hitemDrop)
			{
				const HTREEITEM hParentItem = m_hitemDrop;
				const HTREEITEM hAfterItem = IsUperOf(m_hitemDrop, m_hitemDrag) ? TVI_LAST : TVI_FIRST;
				MoveItem(m_hitemDrag, hParentItem, hAfterItem);
			}
		}
		else
			MessageBeep(0);

		ReleaseCapture();
	}

	m_bDragging = FALSE;
	SelectDropTarget(nullptr);

	if(m_pDragImage != nullptr)
	{
		m_pDragImage->DeleteImageList();
		m_pDragImage->Detach();
		delete m_pDragImage;
		m_pDragImage = nullptr;
	}

	CTreeCtrl::OnLButtonUp(nFlags, point);
}

void CLayerTreeCtrl::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	if(pNMTreeView->action == 0)
	{
		*pResult = 0;
		return;
	}
	else if(m_hFixedItem = nullptr)
	{
		m_hFixedItem = pNMTreeView->itemNew.hItem;
		return;
	}

	CLayer* pOldLayer = nullptr;
	CLayer* pNewLayer = nullptr;
	const HTREEITEM hOldItem = pNMTreeView->itemOld.hItem;
	if(hOldItem != nullptr)
		pOldLayer = (CLayer*)GetItemData(hOldItem);
	const HTREEITEM hNewItem = pNMTreeView->itemNew.hItem;
	if(hNewItem != nullptr)
		pNewLayer = (CLayer*)GetItemData(hNewItem);
	else
		return;

	m_hFixedItem = hNewItem;
	m_strActiveLayerName = pNewLayer->m_strName;

	if(pNewLayer->IsControllable() == false)
		return;

	long nActiveCount = 0;
	HTREEITEM hUpper = this->GetUperItem(hNewItem);
	while(hUpper != nullptr)
	{
		CLayer* pUpper = (CLayer*)GetItemData(hUpper);
		if(pUpper != nullptr)
		{
			if(pUpper->IsControllable() == false)
			{
			}
			else if(pUpper->m_bLocked == true)
			{
			}
			else if(pUpper->Gettype() >= 10)
			{
			}
			else
			{
				nActiveCount += pUpper->m_nActiveCount;
			}
		}
		hUpper = this->GetUperItem(hUpper);
	}
	HTREEITEM hDown = this->GetDownItem(hNewItem);
	while(hDown != nullptr)
	{
		CLayer* pDown = (CLayer*)GetItemData(hDown);
		if(pDown != nullptr)
		{
			if(pDown->IsControllable() == false)
			{
			}
			else if(pDown->m_bLocked == true)
			{
			}
			else
			{
				nActiveCount += pDown->m_nActiveCount;
			}
		}
		hDown = this->GetDownItem(hDown);
	}

	if(nActiveCount > 0 && AfxMessageBox(_T("Really move the active geometries to this layer?"), MB_YESNO) == IDYES)
	{
		POSITION viewPos1 = m_document.GetFirstViewPosition();
		CView* pView = m_document.GetNextView(viewPos1);
		if(pView == nullptr)
			return;
		CViewinfo* pViewinfo = (CViewinfo*)pView->SendMessage(WM_GETVIEWINFO, 0, 0);
		if(pViewinfo == nullptr)
			return;
		const bool bSomeLocked = false;
		CViewinfo& viewinfo(*pViewinfo);

		HTREEITEM hUpper = this->GetUperItem(hNewItem);
		while(hUpper != nullptr)
		{
			CLayer* pUpper = (CLayer*)GetItemData(hUpper);
			if(pUpper != nullptr)
			{
				if(pUpper->IsControllable() == false)
				{
				}
				else if(pUpper->m_bLocked == true)
				{
				}
				else if(pNewLayer->IsControllable() == false)
				{
				}
				else if(pNewLayer->m_bLocked == true)
				{
				}
				else
					pNewLayer->AddHead(pView, viewinfo, pUpper, m_actionStack);
			}
			hUpper = this->GetUperItem(hUpper);
		}

		HTREEITEM hDown = this->GetDownItem(hNewItem);
		while(hDown != nullptr)
		{
			CLayer* pDown = (CLayer*)GetItemData(hDown);
			if(pDown != nullptr)
			{
				if(pDown->IsControllable() == false)
				{
				}
				else if(pDown->m_bLocked == true)
				{
				}
				else if(pNewLayer->IsControllable() == false)
				{
				}
				else if(pNewLayer->m_bLocked == true)
				{
				}
				else
					pNewLayer->AddTail(pView, viewinfo, pDown, m_actionStack);
			}
			hDown = this->GetDownItem(hDown);
		}

		if(bSomeLocked == true)
		{
			AfxMessageBox(IDS_LAYERLOCKEDNOMOVEON);
		}

		m_document.UpdateAllViews(nullptr);
		m_document.SetModifiedFlag(TRUE);
	}
	POSITION viewPos2 = m_document.GetFirstViewPosition();
	while(viewPos2 != nullptr)
	{
		const CView* pView = m_document.GetNextView(viewPos2);
		pView->SendMessage(WM_LAYERACTIVATED, (LONG)pNewLayer, (LONG)pOldLayer);
	}
	*pResult = 0;
}

void CLayerTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	const CPoint client = point;
	UINT uFlags = 0;
	const HTREEITEM hItem = HitTest(client, &uFlags);
	if(uFlags & TVHT_ONITEMSTATEICON)
	{
		long uState = GetItemState(hItem, TVIF_STATE);
		uState = (uState >> 12);
		SwitchState(hItem, uState == 2 ? 0 : 1);
	}
	else
		CTreeCtrl::OnLButtonDown(nFlags, point);
}

void CLayerTreeCtrl::OnNewAfter()
{
	const HTREEITEM hSelectItem = GetSelectedItem();
	if(hSelectItem == nullptr)
		return;
	CLayer* pSlectlayer = (CLayer*)GetItemData(hSelectItem);
	if(pSlectlayer == nullptr)
		return;
	CLayer* pParentlayer = pSlectlayer->m_parent;
	if(pParentlayer == nullptr)
		return;
	const HTREEITEM hParentItem = GetParentItem(hSelectItem);
	if(hParentItem == nullptr)
		return;

	CLayer* pNewlayer = nullptr;
	if(pSlectlayer->Gettype() == 10)
	{
		pNewlayer = new CLayif(m_layertree, (CLaylg*)pParentlayer, pParentlayer->m_minLevelObj, pParentlayer->m_maxLevelObj, pParentlayer->m_minLevelTag, pParentlayer->m_maxLevelTag);
	}
	else if(strcmp(m_document.GetRuntimeClass()->m_lpszClassName, "CGeoDoc") == 0)
	{
		CPickLayDlg dlg(this);
		if(dlg.DoModal() == IDOK)
		{
			switch(dlg.m_nType)
			{
				case 1:
					pNewlayer = new CLaypt(m_layertree);
					break;
				case 2:
					pNewlayer = new CLaypy(m_layertree);
					break;
				case 3:
					pNewlayer = new CLaypp(m_layertree);
					break;
			}
		}
	}
	else
	{
		pNewlayer = new CLayer(m_layertree);
	}
	if(pNewlayer == nullptr)
		return;
	pNewlayer->m_wId = m_layertree.ApplyId();
	pParentlayer->AddChild(pSlectlayer, pNewlayer);

	this->AddNode(hParentItem, hSelectItem, pNewlayer);
	const std::pair<std::pair<CLayer*, CLayer*>, CLayer*> object(std::pair<CLayer*, CLayer*>(pParentlayer, pSlectlayer), pNewlayer);
	Undoredo::CAppend<CLayerTreeCtrl&, CLayerTree&, std::pair<std::pair<CLayer*, CLayer*>, CLayer*>>* pAppend = new Undoredo::CAppend<CLayerTreeCtrl&, CLayerTree&, std::pair<std::pair<CLayer*, CLayer*>, CLayer*>>(*this, m_layertree, object);
	pAppend->undo = Undoredo::Document::LayerTreeCtrl::Layerlist::Undo_Append_Layer;
	pAppend->redo = Undoredo::Document::LayerTreeCtrl::Layerlist::Redo_Append_Layer;
	pAppend->free = Undoredo::Document::LayerTreeCtrl::Layerlist::Free_Append_Layer;
	m_actionStack.Record(pAppend);
}

void CLayerTreeCtrl::OnNewBefore()
{
	const HTREEITEM hSelectItem = GetSelectedItem();
	if(hSelectItem == nullptr)
		return;
	CLayer* pSlectlayer = (CLayer*)GetItemData(hSelectItem);
	if(pSlectlayer == nullptr)
		return;
	CLayer* pParentlayer = pSlectlayer->m_parent;
	if(pParentlayer == nullptr)
		return;
	const HTREEITEM hParentItem = GetParentItem(hSelectItem);
	if(hParentItem == nullptr)
		return;

	CLayer* pNewlayer = nullptr;
	if(pSlectlayer->Gettype() == 10)
	{
		pNewlayer = new CLayif(m_layertree, (CLaylg*)pParentlayer, pParentlayer->m_minLevelObj, pParentlayer->m_maxLevelObj, pParentlayer->m_minLevelTag, pParentlayer->m_maxLevelTag);	
	}
	else if(strcmp(m_document.GetRuntimeClass()->m_lpszClassName, "CGeoDoc") == 0)
	{
		CPickLayDlg dlg(this);
		if(dlg.DoModal() == IDOK)
		{
			switch(dlg.m_nType)
			{
				case 1:
					pNewlayer = new CLaypt(m_layertree);
					break;
				case 2:
					pNewlayer = new CLaypy(m_layertree);
					break;
				case 3:
					pNewlayer = new CLaypp(m_layertree);
					break;
			}
		}
	}
	else
	{
		pNewlayer = new CLayer(m_layertree);
	}

	if(pNewlayer == nullptr)
		return;

	pNewlayer->m_wId = m_layertree.ApplyId();
	const HTREEITEM hPrevItem = this->GetPrevSiblingItem(hSelectItem);
	CLayer* nPrevlayer = hPrevItem == nullptr ? nullptr : (CLayer*)GetItemData(hPrevItem);

	pParentlayer->AddChild(nPrevlayer, pNewlayer);
	this->AddNode(hParentItem, hPrevItem, pNewlayer);
	const std::pair<std::pair<CLayer*, CLayer*>, CLayer*> object(std::pair<CLayer*, CLayer*>(pParentlayer, nPrevlayer), pNewlayer);
	Undoredo::CAppend<CLayerTreeCtrl&, CLayerTree&, std::pair<std::pair<CLayer*, CLayer*>, CLayer*>>* pAppend = new Undoredo::CAppend<CLayerTreeCtrl&, CLayerTree&, std::pair<std::pair<CLayer*, CLayer*>, CLayer*>>(*this, m_layertree, object);
	pAppend->undo = Undoredo::Document::LayerTreeCtrl::Layerlist::Undo_Append_Layer;
	pAppend->redo = Undoredo::Document::LayerTreeCtrl::Layerlist::Redo_Append_Layer;
	pAppend->free = Undoredo::Document::LayerTreeCtrl::Layerlist::Free_Append_Layer;
	m_actionStack.Record(pAppend);
}

void CLayerTreeCtrl::OnNewIn()
{
	const HTREEITEM hSelectItem = GetSelectedItem();
	if(hSelectItem == nullptr)
		return;
	CLayer* pSelectlayer = (CLayer*)GetItemData(hSelectItem);
	if(pSelectlayer == nullptr)
		return;

	CLayer* pNewlayer = nullptr;
	if(pSelectlayer->IsControllable() == false)
		return;
	else if(pSelectlayer->Gettype() == 9)
	{
		pNewlayer = new CLayif(m_layertree, (CLaylg*)pSelectlayer, pSelectlayer->m_minLevelObj, pSelectlayer->m_maxLevelObj, pSelectlayer->m_minLevelTag, pSelectlayer->m_maxLevelTag);
	}
	else if(strcmp(m_document.GetRuntimeClass()->m_lpszClassName, "CGeoDoc") == 0)
	{
		CPickLayDlg dlg(this);
		if(dlg.DoModal() == IDOK)
		{
			switch(dlg.m_nType)
			{
				case 1:
					pNewlayer = new CLaypt(m_layertree, pSelectlayer->m_minLevelObj, pSelectlayer->m_maxLevelObj, pSelectlayer->m_minLevelTag, pSelectlayer->m_maxLevelTag);
					break;
				case 2:
					pNewlayer = new CLaypy(m_layertree, pSelectlayer->m_minLevelObj, pSelectlayer->m_maxLevelObj, pSelectlayer->m_minLevelTag, pSelectlayer->m_maxLevelTag);
					break;
				case 3:
					pNewlayer = new CLaypp(m_layertree, pSelectlayer->m_minLevelObj, pSelectlayer->m_maxLevelObj, pSelectlayer->m_minLevelTag, pSelectlayer->m_maxLevelTag);
					break;
			}
		}
	}
	else
	{
		pNewlayer = new CLayer(m_layertree, pSelectlayer->m_minLevelObj, pSelectlayer->m_maxLevelObj, pSelectlayer->m_minLevelTag, pSelectlayer->m_maxLevelTag);
	}
	if(pNewlayer == nullptr)
		return;

	pNewlayer->m_wId = m_layertree.ApplyId();
	pSelectlayer->AddChild(nullptr, pNewlayer);
	this->AddNode(hSelectItem, nullptr, pNewlayer);
	const std::pair<std::pair<CLayer*, CLayer*>, CLayer*> object(std::pair<CLayer*, CLayer*>(pSelectlayer, (CLayer*)nullptr), pNewlayer);
	Undoredo::CAppend<CLayerTreeCtrl&, CLayerTree&, std::pair<std::pair<CLayer*, CLayer*>, CLayer*>>* pAppend = new Undoredo::CAppend<CLayerTreeCtrl&, CLayerTree&, std::pair<std::pair<CLayer*, CLayer*>, CLayer*>>(*this, m_layertree, object);
	pAppend->undo = Undoredo::Document::LayerTreeCtrl::Layerlist::Undo_Append_Layer;
	pAppend->redo = Undoredo::Document::LayerTreeCtrl::Layerlist::Redo_Append_Layer;
	pAppend->free = Undoredo::Document::LayerTreeCtrl::Layerlist::Free_Append_Layer;
	m_actionStack.Record(pAppend);
}

void CLayerTreeCtrl::OnDelete()
{
	const HTREEITEM hItem = this->GetSelectedItem();
	if(hItem == nullptr)
		return;

	CLayer* pLayer = (CLayer*)this->GetItemData(hItem);
	if(pLayer == nullptr)
		return;
	else if(pLayer->m_parent == nullptr)
		return;
	else if(pLayer->Gettype() == 10)//layif
	{
	}
	else if(pLayer->IsControllable() == false)//layon
		return;
	else if(pLayer->m_bLocked == true)
	{
		AfxMessageBox(IDS_LOCKEDNOLAYERDELETE);
		return;
	}
	CLayer* pParent = pLayer->Parent();
	CLayer* pPrev = pLayer->Prevsibling();

	POSITION viewPos = m_document.GetFirstViewPosition();
	const CView* pView = m_document.GetNextView(viewPos);
	pView->SendMessage(WM_PREREMOVELAYER, (UINT)pLayer, 0);

	pParent->DetachChild(pLayer);
	this->DeleteItem(hItem);

	m_document.SetModifiedFlag();
	m_document.UpdateAllViews(nullptr);
	const std::pair<std::pair<CLayer*, CLayer*>, CLayer*> object(std::pair<CLayer*, CLayer*>(pParent, pPrev), pLayer);
	Undoredo::CRemove<CLayerTreeCtrl&, CLayerTree&, std::pair<std::pair<CLayer*, CLayer*>, CLayer*>>* pRemove = new Undoredo::CRemove<CLayerTreeCtrl&, CLayerTree&, std::pair<std::pair<CLayer*, CLayer*>, CLayer*>>(*this, m_layertree, object);
	pRemove->undo = Undoredo::Document::LayerTreeCtrl::Layerlist::Redo_Append_Layer;
	pRemove->redo = Undoredo::Document::LayerTreeCtrl::Layerlist::Undo_Append_Layer;
	pRemove->free = Undoredo::Document::LayerTreeCtrl::Layerlist::Free_Append_Layer;
	m_actionStack.Record(pRemove);
}

void CLayerTreeCtrl::OnSetSpot()
{
	const HTREEITEM hItem = this->GetSelectedItem();
	if(hItem == nullptr)
		return;

	CLayer* layer = (CLayer*)this->GetItemData(hItem);
	if(layer == nullptr)
		return;

	POSITION viewPos = m_document.GetFirstViewPosition();
	CView* pView = m_document.GetNextView(viewPos);
	if(pView == nullptr)
		return;

	CViewinfo* pViewinfo = (CViewinfo*)pView->SendMessage(WM_GETVIEWINFO, 0, 0);
	if(pViewinfo == nullptr)
		return;
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Style.dll"));
	AfxSetResourceHandle(hInst);

	CSpotCtrl dlg(nullptr, true, m_layertree.m_library, layer->m_pSpot);
	if(dlg.DoModal() == IDOK)
	{
		Undoredo::CModify<CLayerTreeCtrl&, CLayer*, CSpot*>* pModify = new Undoredo::CModify<CLayerTreeCtrl&, CLayer*, CSpot*>(*this, layer, layer->m_pSpot, dlg.d_pSpot);
		pModify->undo = Undoredo::Document::LayerTreeCtrl::Layer::Spot::Undo_Modify;
		pModify->redo = Undoredo::Document::LayerTreeCtrl::Layer::Spot::Redo_Modify;
		pModify->free = Undoredo::Document::LayerTreeCtrl::Layer::Spot::Free_Modify;
		m_actionStack.Record(pModify);

		layer->m_pSpot = dlg.d_pSpot;
		dlg.d_pSpot = nullptr;
		layer->SpotChanged(pView, *pViewinfo);

		m_document.SetModifiedFlag(TRUE);
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

void CLayerTreeCtrl::OnSetStyle()
{
	const HTREEITEM hItem = this->GetSelectedItem();
	if(hItem == nullptr)
		return;

	CLayer* layer = (CLayer*)this->GetItemData(hItem);
	if(layer == nullptr)
		return;

	POSITION viewPos = m_document.GetFirstViewPosition();
	CView* pView = m_document.GetNextView(viewPos);
	if(pView == nullptr)
		return;

	CViewinfo* pViewinfo = (CViewinfo*)pView->SendMessage(WM_GETVIEWINFO, 0, 0);
	if(pViewinfo == nullptr)
		return;
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Style.dll"));
	AfxSetResourceHandle(hInst);

	CStyleDlg dlg(nullptr, true, m_layertree.m_library, layer->m_pLine, (CFill*)layer->m_pFill);
	if(dlg.DoModal() == IDOK)
	{
		Undoredo::CMacro* pMacro = new Undoredo::CMacro();
		{
			Undoredo::CModify<CLayerTreeCtrl&, CLayer*, CLine*>* pModify = new Undoredo::CModify<CLayerTreeCtrl&, CLayer*, CLine*>(*this, layer, layer->m_pLine, dlg.d_pLine);
			pModify->undo = Undoredo::Document::LayerTreeCtrl::Layer::Line::Undo_Modify;
			pModify->redo = Undoredo::Document::LayerTreeCtrl::Layer::Line::Redo_Modify;
			pModify->free = Undoredo::Document::LayerTreeCtrl::Layer::Line::Free_Modify;
			pMacro->Record(pModify);
		}
		{
			Undoredo::CModify<CLayerTreeCtrl&, CLayer*, CFill*>* pModify = new Undoredo::CModify<CLayerTreeCtrl&, CLayer*, CFill*>(*this, layer, (CFill*)layer->m_pFill, dlg.d_pFill);
			pModify->undo = Undoredo::Document::LayerTreeCtrl::Layer::Fill::Undo_Modify;
			pModify->redo = Undoredo::Document::LayerTreeCtrl::Layer::Fill::Redo_Modify;
			pModify->free = Undoredo::Document::LayerTreeCtrl::Layer::Fill::Free_Modify;
			pMacro->Record(pModify);
		}
		m_actionStack.Record(pMacro);

		layer->StyleChanged(pView, *pViewinfo);
		layer->m_pLine = dlg.d_pLine;
		dlg.d_pLine = nullptr;
		layer->m_pFill = (CFillGeneral*)dlg.d_pFill;
		dlg.d_pFill = nullptr;
		layer->StyleChanged(pView, *pViewinfo);

		m_document.SetModifiedFlag(TRUE);
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

void CLayerTreeCtrl::OnSetType()
{
	const HTREEITEM hItem = this->GetSelectedItem();
	if(hItem == nullptr)
		return;

	CLayer* layer = (CLayer*)this->GetItemData(hItem);
	if(layer == nullptr)
		return;

	POSITION viewPos = m_document.GetFirstViewPosition();
	CView* pView = m_document.GetNextView(viewPos);
	if(pView == nullptr)
		return;

	CViewinfo* pViewinfo = (CViewinfo*)pView->SendMessage(WM_GETVIEWINFO, 0, 0);
	if(pViewinfo == nullptr)
		return;
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Style.dll"));
	AfxSetResourceHandle(hInst);

	CType* type = layer->GetType();
	CType* pNew = type == nullptr ? new CType() : type->Clone();
	CTypeDlg dlg(nullptr, *pNew);
	if(dlg.DoModal() == IDOK)
	{
		if(type == nullptr || *pNew != *type)
		{
			Undoredo::CModify<CLayerTreeCtrl&, CLayer*, CType*>* pModify = new Undoredo::CModify<CLayerTreeCtrl&, CLayer*, CType*>(*this, layer, layer->m_pType, pNew);
			pModify->undo = Undoredo::Document::LayerTreeCtrl::Layer::Type::Undo_Modify;
			pModify->redo = Undoredo::Document::LayerTreeCtrl::Layer::Type::Redo_Modify;
			pModify->free = Undoredo::Document::LayerTreeCtrl::Layer::Type::Free_Modify;
			m_actionStack.Record(pModify);

			layer->TypeChanged(pView, *pViewinfo);
			layer->m_pType = pNew;
			layer->TypeChanged(pView, *pViewinfo);

			m_document.SetModifiedFlag(TRUE);
		}
		else
		{
			delete pNew;
			pNew = nullptr;
		}
	}
	else
	{
		delete pNew;
		pNew = nullptr;
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

void CLayerTreeCtrl::OnSetHint()
{
	const HTREEITEM hItem = this->GetSelectedItem();
	if(hItem == nullptr)
		return;

	CLayer* layer = (CLayer*)this->GetItemData(hItem);
	if(layer == nullptr)
		return;

	POSITION viewPos = m_document.GetFirstViewPosition();
	CView* pView = m_document.GetNextView(viewPos);
	if(pView == nullptr)
		return;

	CViewinfo* pViewinfo = (CViewinfo*)pView->SendMessage(WM_GETVIEWINFO, 0, 0);
	if(pViewinfo == nullptr)
		return;
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Style.dll"));
	AfxSetResourceHandle(hInst);

	CHint* pHint = layer->m_pHint != nullptr ? layer->m_pHint->Clone() : new CHint();
	CHintDlg dlg(nullptr, *pHint);
	if(dlg.DoModal() == IDOK)
	{
		Undoredo::CModify<CLayerTreeCtrl&, CLayer*, CHint*>* pModify = new Undoredo::CModify<CLayerTreeCtrl&, CLayer*, CHint*>(*this, layer, layer->m_pHint, pHint);
		pModify->undo = Undoredo::Document::LayerTreeCtrl::Layer::Hint::Undo_Modify;
		pModify->redo = Undoredo::Document::LayerTreeCtrl::Layer::Hint::Redo_Modify;
		pModify->free = Undoredo::Document::LayerTreeCtrl::Layer::Hint::Free_Modify;
		m_actionStack.Record(pModify);

		layer->HintChanged(pView, *pViewinfo);
		layer->m_pHint = pHint;

		layer->HintChanged(pView, *pViewinfo);

		m_document.SetModifiedFlag(TRUE);
	}
	else
	{
		delete pHint;
		pHint = nullptr;
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

void CLayerTreeCtrl::UpdateSelect()
{
	if(m_bMulti == false)
		return;

	HTREEITEM activehItem;
	HTREEITEM hItem = this->GetRootItem();
	if(hItem == nullptr)
		return;

	long selets = 0;
	do
	{
		CLayer* layer = (CLayer*)this->GetItemData(hItem);
		if(layer != nullptr)
		{
			POSITION pos = layer->m_geomlist.GetHeadPosition();
			while(pos != nullptr)
			{
				CGeom* pGeom = layer->m_geomlist.GetNext(pos);
				if(pGeom->m_bActive == false)
					continue;

				selets++;
				activehItem = hItem;
				break;
			}
		}

		if(selets > 1)
			return;
	} while((hItem = this->GetDownItem(hItem)) != nullptr);

	if(selets == 1)
	{
		this->SelectItem(activehItem);
		CLayer* layer = (CLayer*)this->GetItemData(activehItem);
		if(layer != nullptr)
		{
			POSITION viewPos = m_document.GetFirstViewPosition();
			const CView* pView = m_document.GetNextView(viewPos);
			pView->SendMessage(WM_LAYERACTIVATED, 0, (LONG)layer);
		}
	}
	if(selets == 0 && m_hFixedItem != nullptr)
	{
		this->SelectItem(m_hFixedItem);
		CLayer* layer = (CLayer*)this->GetItemData(m_hFixedItem);
		if(layer != nullptr)
		{
			POSITION viewPos = m_document.GetFirstViewPosition();
			const CView* pView = m_document.GetNextView(viewPos);
			pView->SendMessage(WM_LAYERACTIVATED, 0, (LONG)layer);
		}
	}
}

void CLayerTreeCtrl::Select(CLayer* pLayer)
{
	const HTREEITEM hSelItem = this->GetSelectedItem();
	if(hSelItem != nullptr)
	{
		CLayer* selLayer = (CLayer*)this->GetItemData(hSelItem);
		if(selLayer == pLayer)
			return;
	}

	HTREEITEM hItem = this->GetRootItem();
	if(hItem == nullptr)
		return;

	do
	{
		CLayer* layer = (CLayer*)this->GetItemData(hItem);
		if(layer == pLayer)
		{
			SelectItem(hItem);

			POSITION viewPos = m_document.GetFirstViewPosition();
			const CView* pView = m_document.GetNextView(viewPos);
			pView->SendMessage(WM_LAYERACTIVATED, 0, (LONG)layer);
		}
	} while((hItem = GetDownItem(hItem)) != nullptr);
}

void CLayerTreeCtrl::SelectFixed()
{
	if(m_hFixedItem != nullptr)
	{
		if(this->SelectItem(m_hFixedItem) == FALSE)
		{
			m_hFixedItem = this->GetRootItem();
			this->SelectItem(m_hFixedItem);
		}
		CLayer* layer = (CLayer*)this->GetItemData(m_hFixedItem);
		if(layer != nullptr)
		{
			POSITION viewPos = m_document.GetFirstViewPosition();
			const CView* pView = m_document.GetNextView(viewPos);
			pView->SendMessage(WM_LAYERACTIVATED, 0, (LONG)layer);
		}
	}
}

void CLayerTreeCtrl::OnAllon()
{
	HTREEITEM hItem = this->GetRootItem();
	if(hItem == nullptr)
		return;

	do
	{
		CLayer* layer = (CLayer*)this->GetItemData(hItem);
		if(layer != nullptr)
		{
			if(layer->m_bVisible == true)
				continue;

			layer->m_bVisible = true;

			this->SetItemState(hItem, INDEXTOSTATEIMAGEMASK(2), TVIS_STATEIMAGEMASK);
		}
	} while((hItem = this->GetDownItem(hItem)) != nullptr);

	AfxGetMainWnd()->SendMessage(WM_DESTROYHTML, 0, 0);
	m_document.SetModifiedFlag(TRUE);
	m_document.UpdateAllViews(nullptr);
}

void CLayerTreeCtrl::OnAlloff()
{
	HTREEITEM hItem = this->GetRootItem();
	if(hItem == nullptr)
		return;

	do
	{
		CLayer* layer = (CLayer*)this->GetItemData(hItem);
		if(layer != nullptr)
		{
			if(layer->m_bVisible == false)
				continue;

			layer->m_bVisible = false;
			this->SetItemState(hItem, INDEXTOSTATEIMAGEMASK(1), TVIS_STATEIMAGEMASK);
		}
	} while((hItem = this->GetDownItem(hItem)) != nullptr);

	AfxGetMainWnd()->SendMessage(WM_DESTROYHTML, 0, 0);
	m_document.SetModifiedFlag(TRUE);
	m_document.UpdateAllViews(nullptr);
}

void CLayerTreeCtrl::OnMulti()
{
	m_bMulti = 1 - m_bMulti;
}

void CLayerTreeCtrl::OnSetProperty()
{
	const HTREEITEM hItem = this->GetSelectedItem();
	if(hItem != nullptr)
	{
		CLayer* layer = (CLayer*)this->GetItemData(hItem);
		if(layer != nullptr)
		{
			layer->SetAttribute(m_document);
		}
	}
}

BOOL CLayerTreeCtrl::DestroyWindow()
{
	if(m_pDragImage != nullptr)
	{
		m_pDragImage->DeleteImageList();
		m_pDragImage->Detach();
		delete m_pDragImage;
		m_pDragImage = nullptr;
	}
	return CTreeCtrl::DestroyWindow();
}

CImageList* CLayerTreeCtrl::CreateDragImage(HTREEITEM hItem)
{
	/*	if(this->GetImageList(TVSIL_NORMAL) != nullptr)
		{
			CImageList* pNormalImageList = this->GetImageList(TVSIL_NORMAL);

			CImageList* pImageList = new CImageList;
			pImageList->Create(16,16, ILC_COLORDDB | ILC_MASK, 0, 1);
			int nImage1;
			int nImage2;
			this->GetItemImage(hItem,nImage1,nImage2);

			pImageList->Copy(0, pNormalImageList, nImage1, ILCF_MOVE);
			return pImageList;
		}
		else
	*/
	{
		CRect rect;
		this->GetItemRect(hItem, rect, TRUE);
		rect.top = rect.left = 0;

		// Create bitmap
		CClientDC dc(this);
		CDC memDC;

		if(!memDC.CreateCompatibleDC(&dc))
			return nullptr;

		CBitmap bitmap;
		if(!bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height()))
			return nullptr;

		CBitmap* pOldMemDCBitmap = memDC.SelectObject(&bitmap);
		CFont* pOldFont = memDC.SelectObject(GetFont());

		memDC.FillSolidRect(&rect, RGB(0, 255, 0)); // Here green is used as mask color
		memDC.SetTextColor(GetSysColor(COLOR_GRAYTEXT));
		memDC.TextOut(rect.left, rect.top, GetItemText(hItem));

		memDC.SelectObject(pOldFont);
		memDC.SelectObject(pOldMemDCBitmap);

		// Create imagelist
		CImageList* pImageList = new CImageList;
		pImageList->Create(rect.Width(), rect.Height(), ILC_COLORDDB | ILC_MASK, 0, 1);
		pImageList->Add(&bitmap, RGB(0, 255, 0)); // Here green is used as mask color

		return pImageList;
	}
}

void CLayerTreeCtrl::SwitchState(HTREEITEM hItem, long lState)
{
	if(hItem != nullptr)
	{
		this->SetItemState(hItem, INDEXTOSTATEIMAGEMASK(lState ? 2 : 1), TVIS_STATEIMAGEMASK);

		CLayer* layer = (CLayer*)this->GetItemData(hItem);
		if(layer->m_bVisible != lState)
		{
			layer->m_bVisible = lState;

			if(m_bShowhidelayer == false)
			{
				m_document.UpdateAllViews(nullptr);
			}
			else
			{
				layer->Invalid(m_document);
				m_document.SetModifiedFlag(TRUE);
			}
		}

		AfxGetMainWnd()->SendMessage(WM_DESTROYHTML, 0, 0);
	}
	else
	{
		return;
	}

	hItem = this->GetChildItem(hItem);
	if(hItem != nullptr)
	{
		do
		{
			this->SwitchState(hItem, lState);
			hItem = this->GetNextSiblingItem(hItem);
		} while(hItem != nullptr);
	}
}

void CLayerTreeCtrl::OnGEODatabase()
{
	const HTREEITEM hItem = this->GetSelectedItem();
	if(hItem == nullptr)
		return;
	CLayer* layer = (CLayer*)this->GetItemData(hItem);
	if(layer == nullptr)
		return;
	if(layer->Gettype() <= 5)
		return;
	if(layer->Gettype() >= 10)
		return;
	std::list<CString*> activenames;
	for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it != m_layertree.forwend(); ++it)
	{
		CLayer* layer1 = *it;
		if(layer->Gettype() <= 5)
			return;
		if(layer->Gettype() >= 10)
			return;

		CLaydb* laydb1 = (CLaydb*)layer1;
		CGEODatasource& datasource = laydb1->GetGeoDatasource();
		if(datasource.m_strDatabase.IsEmpty() == FALSE)
		{
			activenames.push_back(&datasource.m_strDatabase);
		}
	}

	CLaydb* laydb = (CLaydb*)layer;
	CGEODatasource& pDatasource1 = laydb->GetGeoDatasource();
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Database.dll"));
	AfxSetResourceHandle(hInst);

	CConnectionDlg dlg(this, true, m_layertree.m_GeoDBList, activenames, pDatasource1.m_strDatabase);
	if(dlg.DoModal() == IDOK)
	{
		pDatasource1.m_strDatabase = dlg.m_strDatabase;

		POSITION pos = m_document.GetFirstViewPosition();
		while(pos != nullptr)
		{
			const CView* pView = m_document.GetNextView(pos);
			pView->SendMessage(WM_LAYERACTIVATED, 0, (LONG)layer);
		}
		m_document.SetModifiedFlag(TRUE);
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

void CLayerTreeCtrl::OnGEODatasource()
{
	HTREEITEM hItem = this->GetSelectedItem();
	if(hItem == nullptr)
		return;
	CLayer* layer = (CLayer*)this->GetItemData(hItem);
	if(layer == nullptr)
		return;
	if(layer->Gettype() <= 5)
		return;
	if(layer->Gettype() >= 10)
		return;

	std::list<CString*> activenames;
	for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it != m_layertree.forwend(); ++it)
	{
		CLayer* layer1 = *it;
		if(layer->Gettype() <= 5)
			return;
		if(layer->Gettype() >= 10)
			return;

		CLaydb* laydb1 = (CLaydb*)layer1;
		CGEODatasource& datasource = laydb1->GetGeoDatasource();
		activenames.push_back(&datasource.m_strDatabase);
	}

	CLaydb* laydb = (CLaydb*)layer;
	CGEODatasource& pDatasource1 = laydb->GetGeoDatasource();
	CODBCDatabase* pDatabase = m_layertree.GetGeoDatabase(pDatasource1.m_strDatabase);
	if(pDatabase == nullptr)
		return;

	CGEOHeaderProfile headerfile = pDatasource1.GetHeaderProfile();

	HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Database.dll"));
	AfxSetResourceHandle(hInst);

	CGEODatasourceDlg dlg(this, pDatabase, headerfile);
	if(dlg.DoModal() == IDOK)
	{
		pDatasource1.GetHeaderProfile() = headerfile;
		m_document.SetModifiedFlag(TRUE);
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

void CLayerTreeCtrl::OnATTDatabase()
{
	const HTREEITEM hItem = this->GetSelectedItem();
	if(hItem == nullptr)
		return;
	CLayer* layer = (CLayer*)this->GetItemData(hItem);
	if(layer == nullptr)
		return;
	if(layer->Gettype()==10)
		return;

	const bool bSupport = layer->m_bAttribute;
	CATTDatasource* pDatasource1 = layer->GetAttDatasource();
	CString strDatabase = pDatasource1 == nullptr ? CString("") : pDatasource1->m_strDatabase;

	std::list<CString*> activenames;
	for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it != m_layertree.forwend(); ++it)
	{
		CLayer* layer1 = *it;
		CATTDatasource* pDatasource = layer1->m_attDatasource;
		if(pDatasource != nullptr)
		{
			activenames.push_back(&pDatasource->m_strDatabase);
		}
	}
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Database.dll"));
	AfxSetResourceHandle(hInst);

	CConnectionDlg dlg(this, bSupport, m_layertree.m_AttDBList, activenames, strDatabase);
	if(dlg.DoModal() == IDOK)
	{
		if(dlg.m_bSupport == false)
			layer->DisableAttribute();
		else if(dlg.m_strDatabase.IsEmpty())
			layer->DisableAttribute();
		else
			layer->EnableAttribute(dlg.m_strDatabase);


		if(layer->m_pThem != nullptr && (strDatabase != dlg.m_strDatabase || dlg.m_bSupport == false))
		{
			delete layer->m_pThem;
			layer->m_pThem = nullptr;
			m_document.UpdateAllViews(nullptr);
		}

		POSITION pos = m_document.GetFirstViewPosition();
		while(pos != nullptr)
		{
			const CView* pView = m_document.GetNextView(pos);
			pView->SendMessage(WM_LAYERACTIVATED, 0, (LONG)layer);
		}
		m_document.SetModifiedFlag(TRUE);
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

void CLayerTreeCtrl::OnATTDatasource()
{
	const HTREEITEM hItem = this->GetSelectedItem();
	if(hItem == nullptr)
		return;
	CLayer* layer = (CLayer*)this->GetItemData(hItem);
	if(layer == nullptr)
		return;
	if(layer->Gettype() == 10)
		return;
	if(layer->m_bAttribute == false)
		return;

	CATTDatasource* pDatasource = layer->GetAttDatasource();
	if(pDatasource == nullptr)
		return;

	CODBCDatabase* pDatabase = m_layertree.GetAttDatabase(pDatasource->m_strDatabase);
	if(pDatabase == nullptr)
		return;

	CATTHeaderProfile& headerprofile = (CATTHeaderProfile&)(pDatasource->GetHeaderProfile());
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Database.dll"));
	AfxSetResourceHandle(hInst);

	CATTDatasourceDlg dlg(this, pDatabase, headerprofile);
	if(dlg.DoModal() == IDOK)
	{
		if(pDatasource->GetHeaderProfile() != headerprofile)
		{
			if(layer->m_attDatasource != nullptr)
			{
				layer->m_attDatasource->GetHeaderProfile() = headerprofile;
			}
			else
			{
				layer->EnableAttribute(pDatasource->m_strDatabase);
				layer->GetAttDatasource()->GetHeaderProfile() = headerprofile;
				layer->Assign(pDatabase, headerprofile);
			}
			if(layer->m_pThem != nullptr)
			{
				delete layer->m_pThem;
				layer->m_pThem = nullptr;
				m_document.UpdateAllViews(nullptr);
			}
		}

		POSITION pos = m_document.GetFirstViewPosition();
		while(pos != nullptr)
		{
			const CView* pView = m_document.GetNextView(pos);
			pView->SendMessage(WM_LAYERACTIVATED, 0, (LONG)layer);
		}
		m_document.SetModifiedFlag(TRUE);
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}
void CLayerTreeCtrl::OnApplyName()
{
	const HTREEITEM hItem = this->GetSelectedItem();
	if(hItem == nullptr)
		return;
	CLayer* layer = (CLayer*)this->GetItemData(hItem);
	if(layer == nullptr)
		return;
	if(layer->Gettype() == 10)
		return;
	if(layer->m_bAttribute == false)
		return;

	CATTDatasource* pDatasource = layer->GetAttDatasource();
	if(pDatasource == nullptr)
		return;

	CODBCDatabase* pDatabase = m_layertree.GetAttDatabase(pDatasource->m_strDatabase);
	if(pDatabase == nullptr)
		return;

	CATTHeaderProfile& headerprofile = (CATTHeaderProfile&)(pDatasource->GetHeaderProfile());
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Information.dll"));
	AfxSetResourceHandle(hInst);
	CColumnPicker dlg(nullptr, pDatabase, headerprofile.m_strKeyTable);
	if(dlg.DoModal() == IDOK)
	{
		if(dlg.m_strField.IsEmpty() == FALSE)
		{
			layer->ApplyName(pDatabase, headerprofile, dlg.m_strField);
			headerprofile.m_strAnnoField = dlg.m_strField;
		}
	}
	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}
void CLayerTreeCtrl::OnApplyCode()
{
	const HTREEITEM hItem = this->GetSelectedItem();
	if(hItem == nullptr)
		return;
	CLayer* layer = (CLayer*)this->GetItemData(hItem);
	if(layer == nullptr)
		return;
	if(layer->m_bAttribute == false)
		return;
	if(layer->Gettype() == 10)
		return;

	CATTDatasource* pDatasource = layer->GetAttDatasource();
	if(pDatasource == nullptr)
		return;

	CODBCDatabase* pDatabase = m_layertree.GetAttDatabase(pDatasource->m_strDatabase);
	if(pDatabase == nullptr)
		return;

	CATTHeaderProfile& headerprofile = (CATTHeaderProfile&)(pDatasource->GetHeaderProfile());
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Information.dll"));
	AfxSetResourceHandle(hInst);

	CColumnPicker dlg(nullptr, pDatabase, headerprofile.m_strKeyTable);
	if(dlg.DoModal() == IDOK)
	{
		if(dlg.m_strField.IsEmpty() == FALSE)
		{
			layer->ApplyCode(pDatabase, headerprofile, dlg.m_strField);
			headerprofile.m_strCodeField = dlg.m_strField;
		}
	}
	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}
void CLayerTreeCtrl::OnATTTheme()
{
	const HTREEITEM hItem = this->GetSelectedItem();
	if(hItem == nullptr)
		return;
	CLayer* layer = (CLayer*)this->GetItemData(hItem);
	if(layer == nullptr)
		return;
	CATTDatasource* pATTDatasource = layer->GetAttDatasource();
	if(pATTDatasource == nullptr)
		return;
	CDatabase* pDatabase = m_layertree.GetAttDatabase(pATTDatasource->m_strDatabase);
	if(pDatabase == nullptr)
		return;
	if(pDatabase->IsOpen() == FALSE)
		return;

	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Thematics.dll"));
	AfxSetResourceHandle(hInst);
	CTheme* pTheme = layer->m_pThem == nullptr ? nullptr : layer->m_pThem->Clone();
	CThemeDlg dlg(this, *pDatabase, *pATTDatasource, pTheme);
	if(dlg.DoModal() == IDOK)
	{
		delete layer->m_pThem;
		layer->m_pThem = dlg.m_pTheme;
		dlg.m_pTheme = nullptr;

		m_document.SetModifiedFlag(TRUE);
		AfxGetMainWnd()->PostMessage(WM_CROSSTHREAD, 1, (UINT)&m_document);//==pDocument->UpdateAllViews(nullptr);
	}
	else if(dlg.m_pTheme != nullptr)
	{
		delete dlg.m_pTheme;
		dlg.m_pTheme = nullptr;
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

BOOL CLayerTreeCtrl::OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if(pHandlerInfo == nullptr)
	{
		if(nCode == CN_UPDATE_COMMAND_UI)
		{
			CCmdUI* pCmdUI = (CCmdUI*)pExtra;
			if(pCmdUI->m_bContinueRouting)
			{
				return CTreeCtrl::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
			}

			ASSERT(!pCmdUI->m_bContinueRouting);

			CLayer* layer = this->GetSelLayer();
			switch(nId)
			{
				case ID_LAYER_NEWIN:
					if(layer == nullptr)
						pCmdUI->Enable(FALSE);
					else if(layer->IsControllable() == false)
						pCmdUI->Enable(FALSE);
					else
						pCmdUI->Enable(TRUE);
					return TRUE;
					break;
				case ID_LAYER_SETTYPE:
				case ID_LAYER_SETSTYLE:
				case ID_LAYER_SETSPOT:
				case ID_LAYER_SETHINT:
					if(layer == nullptr)
						pCmdUI->Enable(FALSE);
					else
						pCmdUI->Enable(TRUE);
					return TRUE;
					break;
				case ID_LAYER_NEWAFTER:
				case ID_LAYER_NEWBEFORE:
					if(layer == nullptr)
						pCmdUI->Enable(FALSE);
					else if(layer->m_parent == nullptr)
						pCmdUI->Enable(FALSE);
					else
						pCmdUI->Enable(TRUE);
					return TRUE;
					break;
				case ID_LAYER_DELETE:
					if(layer == nullptr)
						pCmdUI->Enable(FALSE);
					else if(layer->m_bLocked == TRUE)
						pCmdUI->Enable(FALSE);
					else if(layer->m_parent == nullptr)
						pCmdUI->Enable(FALSE);
					else
						pCmdUI->Enable(TRUE);
					return TRUE;
					break;
				case ID_GEODB_CONNECTION:
				case ID_GEODB_DATASOURCE:
					if(layer == nullptr)
						pCmdUI->Enable(FALSE);
					else if(layer->m_bLocked == TRUE)
						pCmdUI->Enable(FALSE);
					else if(layer->Gettype() == 1)
						pCmdUI->Enable(FALSE);
					else if(layer->Gettype() == 10)
						pCmdUI->Enable(FALSE);
					else if(layer->Gettype() == 11)
						pCmdUI->Enable(FALSE);
					else
						pCmdUI->Enable(TRUE);
					return TRUE;
					break;
				case ID_ATTDB_CONNECTION:
				case ID_ATTDB_DATASOURCE:
					if(layer == nullptr)
						pCmdUI->Enable(FALSE);
					else if(layer->m_bLocked == TRUE)
						pCmdUI->Enable(FALSE);
					else if(layer->Gettype() == 10)
						pCmdUI->Enable(FALSE);
					else
						pCmdUI->Enable(TRUE);
					return TRUE;
					break;
				case ID_ATTDB_STATISTICS:
					if(layer == nullptr)
						pCmdUI->Enable(FALSE);
					else if(layer->m_bLocked == TRUE)
						pCmdUI->Enable(FALSE);
					else if(layer->Gettype() == 10)
						pCmdUI->Enable(FALSE);
					else if(m_layertree.m_AttDBList.size() == 0)
						pCmdUI->Enable(FALSE);
					else
					{
						CATTDatasource* pDatasource = layer->GetAttDatasource();
						if(pDatasource == nullptr)
							pCmdUI->Enable(FALSE);
						else if(m_layertree.GetAttDatabase(pDatasource->m_strDatabase) == nullptr)
							pCmdUI->Enable(FALSE);
						else
							pCmdUI->Enable(TRUE);
					}
					return TRUE;
					break;
				case ID_LAYER_MULTI:
					pCmdUI->SetCheck(m_bMulti);
					return TRUE;
					break;
				case ID_LAYER_TOLOGICAL:
					if(layer->Gettype() != 1)
						pCmdUI->Enable(FALSE);
					else if(layer->IsLeaf() == false)
						pCmdUI->Enable(FALSE);
					else
						pCmdUI->Enable(TRUE);
					return TRUE;
				case ID_LAYER_SETIF:
					if(layer->Gettype() == 10)
						pCmdUI->Enable(TRUE);
					else
						pCmdUI->Enable(FALSE);
					return TRUE;
				case ID_LAYER_REMATCH:
					if(layer->Gettype() == 9)
						pCmdUI->Enable(TRUE);
					else
						pCmdUI->Enable(FALSE);
					return TRUE;
				default:
					break;
			}
			pCmdUI->m_bContinueRouting = false;
		}
		if(nCode == CN_COMMAND)
		{
		}
	}

	return CTreeCtrl::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
}
void CLayerTreeCtrl::OnToLogical()
{
	HTREEITEM hSelectItem = GetSelectedItem();
	if(hSelectItem == nullptr)
		return;
	CLayer* pSlectlayer = (CLayer*)GetItemData(hSelectItem);
	if(pSlectlayer == nullptr)
		return;
	else if(pSlectlayer->IsControllable() == false)
		return;
	else if(pSlectlayer->Gettype() != 1)
		return;
	else if(pSlectlayer->IsLeaf() == false)
		return;
	else if(AfxMessageBox(IDS_LOSINGUNDOS, MB_YESNO) == IDYES)
	{
		CLaylg* pLaylg = new CLaylg(m_layertree);
		pSlectlayer->Migrate(pLaylg);
		pSlectlayer->m_geomlist.RemoveAll();
		CTreeCtrl::SetItemData(hSelectItem, (DWORD)pLaylg);
		CTreeCtrl::SetItemImage(hSelectItem, pLaylg->Gettype(), pLaylg->Gettype());
		pSlectlayer->ReplaceBy(pLaylg);

		delete pSlectlayer;
	}
}

void CLayerTreeCtrl::OnSetIf()
{
	HTREEITEM hSelectItem = GetSelectedItem();
	if(hSelectItem == nullptr)
		return;
	CLayer* pSlectlayer = (CLayer*)GetItemData(hSelectItem);
	if(pSlectlayer == nullptr)
		return;
	if(pSlectlayer->Gettype()!=10)
		return;
	CLayif* pLayif = (CLayif*)pSlectlayer;

	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Layer.dll"));
	AfxSetResourceHandle(hInst);

	CSetIfDlg dlg(nullptr, (CString)pLayif->m_strIf);
	if(dlg.DoModal() == IDOK)
	{
		pLayif->m_strIf = dlg.m_strIf;		
		m_document.SetModifiedFlag(TRUE);

		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
	}
	else
	{
		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
		return;
	}

	const HTREEITEM hParentItem = this->GetParentItem(hSelectItem);
	if(hParentItem == nullptr)
		return;
	CLaylg* pLaylg = (CLaylg*)GetItemData(hParentItem);
	if(pLaylg == nullptr)
		return;

	CATTDatasource* pDatasource = pLaylg->GetAttDatasource();
	if(pDatasource == nullptr)
		return;

	CODBCDatabase* pDatabase = m_layertree.GetAttDatabase(pDatasource->m_strDatabase);
	if(pDatabase == nullptr)
		return;

	CATTHeaderProfile& headerprofile = (CATTHeaderProfile&)(pDatasource->GetHeaderProfile());
	pLaylg->Rematch(pDatabase, headerprofile);
	m_document.UpdateAllViews(nullptr);
}
void CLayerTreeCtrl::OnRematch()
{
	HTREEITEM hSelectItem = GetSelectedItem();
	if(hSelectItem == nullptr)
		return;
	CLayer* pSlectlayer = (CLayer*)GetItemData(hSelectItem);
	if(pSlectlayer == nullptr)
		return;
	if(pSlectlayer->Gettype()!=9)
		return;

	CLaylg* pLaylg = (CLaylg*)pSlectlayer;
	CATTDatasource* pDatasource = pLaylg->GetAttDatasource();
	if(pDatasource == nullptr)
		return;

	CODBCDatabase* pDatabase = m_layertree.GetAttDatabase(pDatasource->m_strDatabase);
	if(pDatabase == nullptr)
		return;

	CATTHeaderProfile& headerprofile = (CATTHeaderProfile&)(pDatasource->GetHeaderProfile());
	pLaylg->Rematch(pDatabase, headerprofile);
	m_document.UpdateAllViews(nullptr);
}