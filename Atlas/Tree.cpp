#include "stdafx.h"

#include "Tree.h"
#include "TreeNode.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTree1::CTree1()
{
	m_bDragging = FALSE;
	m_pDragImage = nullptr;

	m_pNodelist = nullptr;
}

BEGIN_MESSAGE_MAP(CTree1, CTreeCtrl)
	//{{AFX_MSG_MAP(CTree1)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndlabeledit)
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBegindrag)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(TVN_DELETEITEM, OnDeleteitem)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_ITEM_DELETE, OnDelete)
END_MESSAGE_MAP()

int CTree1::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CTreeCtrl::OnCreate(lpCreateStruct)==-1)
		return -1;

	SetItemHeight(24);
	SetTextColor(RGB(0, 0, 185));

	HINSTANCE hInst = ::LoadLibrary(_T("Atlas.dll"));
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(hInst);
	CImageList typeImages;
	if(typeImages.Create(IDB_ITEMIMAGE, 16, 0, RGB(255, 0, 0))==TRUE)
	{
		SetImageList(&typeImages, LVSIL_NORMAL);
		typeImages.Detach();
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);

	return 1;
}

void CTree1::BuildTree(CTreeNodeList* pNodelist)
{
	DeleteAllItems();

	if(pNodelist!=nullptr)
	{
		SetItemHeight(24);
		SetTextColor(RGB(0, 0, 185));

		POSITION pos = pNodelist->GetHeadPosition();
		while(pos!=nullptr)
		{
			CTreeNode1* node = pNodelist->GetNext(pos);
			if(node==nullptr)
				break;

			HTREEITEM pParentItem = GetItemById(node->m_wParent);
			if(node->m_wParent==0||pParentItem!=nullptr)
			{
				TV_INSERTSTRUCT tvstruct;
				tvstruct.hParent = pParentItem;
				tvstruct.hInsertAfter = TVI_LAST;
				tvstruct.item.mask = TVIF_TEXT|TVIF_PARAM|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
				tvstruct.item.pszText = node->m_strName.GetBuffer(node->m_strName.GetLength());
				tvstruct.item.iImage = node->Gettype();
				tvstruct.item.iSelectedImage = tvstruct.item.iImage;
				tvstruct.item.lParam = (DWORD)node;
				InsertItem(&tvstruct);

				node->m_strName.ReleaseBuffer(node->m_strName.GetLength());
			}
			else
			{
				TV_INSERTSTRUCT tvstruct;
				tvstruct.hParent = pParentItem;
				tvstruct.hInsertAfter = TVI_LAST;
				tvstruct.item.mask = TVIF_TEXT|TVIF_PARAM|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
				tvstruct.item.pszText = node->m_strName.GetBuffer(node->m_strName.GetLength());
				tvstruct.item.iImage = node->Gettype();
				tvstruct.item.iSelectedImage = tvstruct.item.iImage;
				tvstruct.item.lParam = (DWORD)node;
				InsertItem(&tvstruct);

				node->m_strName.ReleaseBuffer(node->m_strName.GetLength());

				node->m_wParent = 0;
			}
		}

		HTREEITEM hFixedItem = GetRootItem();
		if(hFixedItem!=nullptr)
		{
			SelectItem(hFixedItem);
		}
	}

	m_pNodelist = pNodelist;
}

BOOL CTree1::OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if(pHandlerInfo==nullptr)
	{
		if(nCode==CN_UPDATE_COMMAND_UI)
		{
			CCmdUI* pCmdUI = (CCmdUI*)pExtra;
			if(pCmdUI->m_bContinueRouting)
			{
				return CTreeCtrl::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
			}

			ASSERT(!pCmdUI->m_bContinueRouting);

			switch(nId)
			{
			case ID_ITEM_DELETE:
			{
				if(this->GetSelectedItem()==nullptr)
					pCmdUI->Enable(FALSE);
				else
					pCmdUI->Enable(TRUE);
			}
			return TRUE;
			break;

			default:
				break;
			}
			pCmdUI->m_bContinueRouting = false;
		}
		if(nCode==CN_COMMAND)
		{
		}
	}

	return CTreeCtrl::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
}

BOOL CTree1::TransferItem(HTREEITEM hitemFrom, HTREEITEM hitemTo)
{
	HTREEITEM hFirstChild;
	while((hFirstChild = GetChildItem(hitemFrom))!=nullptr)
	{
		TCHAR sztBuffer[50];

		TV_INSERTSTRUCT tvstruct;

		tvstruct.item.mask = TVIF_TEXT|TVIF_PARAM|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
		tvstruct.item.hItem = hFirstChild;
		tvstruct.item.cchTextMax = 49;
		tvstruct.item.pszText = sztBuffer;
		GetItem(&tvstruct.item);

		tvstruct.hParent = hitemTo;
		tvstruct.hInsertAfter = TVI_LAST;
		tvstruct.item.stateMask = LVIS_STATEIMAGEMASK;
		HTREEITEM hNewItem = InsertItem(&tvstruct);

		if(m_pNodelist!=nullptr)
		{
			CTreeNode1* node1 = (CTreeNode1*)GetItemData(hFirstChild);
			POSITION pos1 = m_pNodelist->Find(node1);
			m_pNodelist->RemoveAt(pos1);

			HTREEITEM lastItem = GetUperItem(hNewItem);

			CTreeNode1* node2 = (CTreeNode1*)GetItemData(lastItem);
			POSITION pos = m_pNodelist->Find(node2);
			m_pNodelist->InsertAfter(pos, node1);
		}

		SetItemData(hFirstChild, 0);
		TransferItem(hFirstChild, hNewItem);
		DeleteItem(hFirstChild);
	}

	return TRUE;
}

BOOL CTree1::IsChildOf(HTREEITEM hitemChild, HTREEITEM hitemSuspectedParent) const
{
	do
	{
		if(hitemChild==hitemSuspectedParent)
			break;
	} while((hitemChild = GetParentItem(hitemChild))!=nullptr);

	return (hitemChild!=nullptr);
}

BOOL CTree1::IsUperOf(HTREEITEM hitemSuspectedup, HTREEITEM hitemdown) const
{
	do
	{
		hitemdown = GetUperItem(hitemdown);
		if(hitemdown==hitemSuspectedup)
			return TRUE;
	} while(hitemdown!=hitemSuspectedup&&hitemdown!=nullptr);
	return FALSE;
}

HTREEITEM CTree1::GetUperItem(HTREEITEM hitem) const
{
	if(hitem==nullptr)
		return nullptr;

	if(this->GetPrevSiblingItem(hitem)==nullptr)
	{
		hitem = this->GetParentItem(hitem);
		return hitem;
	}

	hitem = this->GetPrevSiblingItem(hitem);
	while(this->GetChildItem(hitem)!=nullptr)
	{
		hitem = this->GetChildItem(hitem);
		while(this->GetNextSiblingItem(hitem)!=nullptr)
			hitem = this->GetNextSiblingItem(hitem);
	}

	return hitem;
}

HTREEITEM CTree1::GetDownItem(HTREEITEM hitem) const
{
	if(this->GetChildItem(hitem)!=nullptr)
	{
		return this->GetChildItem(hitem);
	}

	if(this->GetNextSiblingItem(hitem)!=nullptr)
	{
		return this->GetNextSiblingItem(hitem);
	}

	while(this->GetParentItem(hitem)!=nullptr)
	{
		hitem = this->GetParentItem(hitem);
		if(this->GetNextSiblingItem(hitem)!=nullptr)
		{
			return this->GetNextSiblingItem(hitem);
		}
	}

	return nullptr;
}

HTREEITEM CTree1::GetItemById(WORD wItemID) const
{
	if(wItemID==0)
		return nullptr;

	HTREEITEM hItem = nullptr;
	hItem = this->GetDownItem(hItem);
	while(hItem!=nullptr)
	{
		const CTreeNode1* node = (CTreeNode1*)this->GetItemData(hItem);
		if(node->m_wId==wItemID)
		{
			return hItem;
		}
		hItem = this->GetDownItem(hItem);
	}

	return nullptr;
}

HTREEITEM CTree1::GetItemByName(CString strName) const
{
	if(strName.GetLength()<=0)
		return nullptr;

	HTREEITEM hItem = nullptr;
	hItem = this->GetDownItem(hItem);
	while(hItem!=nullptr)
	{
		const CTreeNode1* node = (CTreeNode1*)this->GetItemData(hItem);
		if(node->m_strName==strName)
		{
			return hItem;
		}
		hItem = this->GetDownItem(hItem);
	}
	return nullptr;
}

void CTree1::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult)
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here

	if(pTVDispInfo->item.pszText!=nullptr)
	{
		pTVDispInfo->item.mask = TVIF_TEXT;
		SetItem(&pTVDispInfo->item);

		CTreeNode1* node = (CTreeNode1*)this->GetItemData(pTVDispInfo->item.hItem);
		node->m_strName = pTVDispInfo->item.pszText;
	}

	*pResult = 0;
}

void CTree1::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	CPoint ptAction;
	UINT nFlags;

	GetCursorPos(&ptAction);
	ScreenToClient(&ptAction);
	ASSERT(m_bDragging==FALSE);
	m_hitemDrag = HitTest(ptAction, &nFlags);
	if(m_hitemDrag!=nullptr)
	{
		m_bDragging = TRUE;
		m_hitemDrop = nullptr;

		ASSERT(m_pDragImage==nullptr);
		m_pDragImage = CreateDragImage(m_hitemDrag);
		if(m_pDragImage!=nullptr)
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

void CTree1::OnMouseMove(UINT nFlags, CPoint point)
{
	HTREEITEM hitem;
	UINT flags;

	if(m_bDragging)
	{
		if((hitem = HitTest(point, &flags))!=nullptr)
		{
			if(m_pDragImage!=nullptr)
			{
				m_pDragImage->DragMove(point);
				m_pDragImage->DragLeave(this);
			}
			SelectDropTarget(hitem);
			m_hitemDrop = hitem;
			if(m_pDragImage!=nullptr)
			{
				m_pDragImage->DragEnter(this, point);
			}
			HINSTANCE hInst = ::LoadLibrary(_T("Atlas.dll"));
			const HINSTANCE hInstOld = AfxGetResourceHandle();
			AfxSetResourceHandle(hInst);

			if(::GetKeyState(VK_CONTROL)<0)
				SetCursor(AfxGetApp()->LoadCursor(IDC_ITEMMOVETO));
			else
			{
				CTreeNode1* check1 = (CTreeNode1*)GetItemData(hitem);
				CTreeNode1* check2 = (CTreeNode1*)GetItemData(m_hitemDrag);
				SetCursor(AfxGetApp()->LoadCursor(IDC_ITEMMOVEIN));
			}

			AfxSetResourceHandle(hInstOld);
			::FreeLibrary(hInst);
		}
	}

	CTreeCtrl::OnMouseMove(nFlags, point);
}

void CTree1::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_bDragging&&m_hitemDrag!=nullptr&&m_hitemDrop!=nullptr)
	{
		if(m_pDragImage!=nullptr)
		{
			m_pDragImage->DragLeave(this);
			m_pDragImage->EndDrag();
		}

		if(m_hitemDrag!=m_hitemDrop&&!IsChildOf(m_hitemDrop, m_hitemDrag))
		{
			CTreeNode1* check1 = (CTreeNode1*)GetItemData(m_hitemDrop);
			CTreeNode1* check2 = (CTreeNode1*)GetItemData(m_hitemDrag);

			HTREEITEM hNewItem;
			if(::GetKeyState(VK_CONTROL)<0)
			{
				TCHAR sztBuffer[20];

				TV_INSERTSTRUCT tvstruct;
				tvstruct.item.mask = TVIF_TEXT|TVIF_PARAM|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
				tvstruct.item.hItem = m_hitemDrag;
				tvstruct.item.cchTextMax = 19;
				tvstruct.item.pszText = sztBuffer;
				GetItem(&tvstruct.item);

				tvstruct.hParent = GetParentItem(m_hitemDrop);
				tvstruct.hInsertAfter = m_hitemDrop;
				tvstruct.item.stateMask = LVIS_STATEIMAGEMASK;
				hNewItem = InsertItem(&tvstruct);

				HTREEITEM UpItem = GetUperItem(hNewItem);
				CTreeNode1* Upnode = (CTreeNode1*)GetItemData(UpItem);
				CTreeNode1* Dragnode = (CTreeNode1*)GetItemData(m_hitemDrag);
				if(Upnode!=Dragnode&&m_pNodelist!=nullptr)
				{
					POSITION DragPos = m_pNodelist->Find(Dragnode);
					POSITION UpPos = m_pNodelist->Find(Upnode);
					m_pNodelist->RemoveAt(DragPos);
					m_pNodelist->InsertAfter(UpPos, Dragnode);
				}
				SetItemData(m_hitemDrag, 0);

				HTREEITEM ParentItem = GetParentItem(m_hitemDrop);
				if(ParentItem!=nullptr)
				{
					CTreeNode1* Parentnode = (CTreeNode1*)GetItemData(ParentItem);
					Dragnode->m_wParent = Parentnode->m_wId;
				}
				else
				{
					Dragnode->m_wParent = 0;
				}

				TransferItem(m_hitemDrag, hNewItem);
				DeleteItem(m_hitemDrag);
			}
			else if(GetParentItem(m_hitemDrag)!=m_hitemDrop)
			{
				TV_INSERTSTRUCT tvstruct;
				TCHAR sztBuffer[50];

				tvstruct.item.mask = TVIF_TEXT|TVIF_PARAM|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
				tvstruct.item.hItem = m_hitemDrag;
				tvstruct.item.cchTextMax = 49;
				tvstruct.item.pszText = sztBuffer;
				GetItem(&tvstruct.item);

				tvstruct.hParent = m_hitemDrop;
				tvstruct.hInsertAfter = IsUperOf(m_hitemDrop, m_hitemDrag) ? TVI_LAST : TVI_FIRST;
				tvstruct.item.stateMask = LVIS_STATEIMAGEMASK;
				hNewItem = InsertItem(&tvstruct);

				HTREEITEM UpItem = GetUperItem(hNewItem);
				CTreeNode1* Upnode = (CTreeNode1*)GetItemData(UpItem);
				CTreeNode1* Dragnode = (CTreeNode1*)GetItemData(m_hitemDrag);
				if(Upnode!=Dragnode&&m_pNodelist!=nullptr)
				{
					POSITION DragPos = m_pNodelist->Find(Dragnode);
					POSITION UpPos = m_pNodelist->Find(Upnode);
					m_pNodelist->RemoveAt(DragPos);
					m_pNodelist->InsertAfter(UpPos, Dragnode);
				}
				SetItemData(m_hitemDrag, 0);

				CTreeNode1* Dropnode = (CTreeNode1*)GetItemData(m_hitemDrop);
				Dragnode->m_wParent = Dropnode->m_wId;

				TransferItem(m_hitemDrag, hNewItem);
				DeleteItem(m_hitemDrag);
			}

			if(hNewItem!=nullptr)
			{
				SelectItem(hNewItem);
			}
		}
		else
			MessageBeep(0);

		ReleaseCapture();
	}

	m_bDragging = FALSE;
	SelectDropTarget(nullptr);

	if(m_pDragImage!=nullptr)
	{
		m_pDragImage->DeleteImageList();
		m_pDragImage->Detach();
		delete m_pDragImage;
		m_pDragImage = nullptr;
	}

	CTreeCtrl::OnLButtonUp(nFlags, point);
}

bool CTree1::InsertAfter(CTreeNode1* newNode)
{
	HTREEITEM hSelectItem = GetSelectedItem();
	if(hSelectItem==nullptr)
		return false;

	HTREEITEM hParentItem = GetParentItem(hSelectItem);
	HTREEITEM hBeforItem = hSelectItem;

	CTreeNode1* nSlectnode = (CTreeNode1*)GetItemData(hSelectItem);
	CTreeNode1* nParentnode = hParentItem==nullptr ? nullptr : (CTreeNode1*)GetItemData(hParentItem);
	newNode->m_wParent = nParentnode==nullptr ? 0 : nParentnode->m_wId;

	while(this->GetChildItem(hBeforItem)!=nullptr)
	{
		hBeforItem = this->GetChildItem(hBeforItem);
		while(this->GetNextSiblingItem(hBeforItem)!=nullptr)
			hBeforItem = this->GetNextSiblingItem(hBeforItem);
	}

	CTreeNode1* nBefornode = (CTreeNode1*)this->GetItemData(hBeforItem);
	if(m_pNodelist!=nullptr)
	{
		POSITION pos = m_pNodelist->Find(nBefornode);
		m_pNodelist->InsertAfter(pos, newNode);
	}

	TV_INSERTSTRUCT tvstruct;
	tvstruct.hParent = hParentItem;
	tvstruct.hInsertAfter = hSelectItem;
	tvstruct.item.mask = TVIF_TEXT|TVIF_PARAM|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
	tvstruct.item.iImage = newNode->Gettype();
	tvstruct.item.iSelectedImage = tvstruct.item.iImage;
	tvstruct.item.pszText = (LPTSTR)"New";
	tvstruct.item.lParam = (DWORD)newNode;

	InsertItem(&tvstruct);
	return true;
}

bool CTree1::InsertBefore(CTreeNode1* newNode)
{
	HTREEITEM hSelectItem = GetSelectedItem();
	if(hSelectItem==nullptr)
		return false;

	HTREEITEM hParentItem = GetParentItem(hSelectItem);

	CTreeNode1* nSelectnode = (CTreeNode1*)GetItemData(hSelectItem);
	CTreeNode1* nParentnode = hParentItem==nullptr ? nullptr : (CTreeNode1*)GetItemData(hParentItem);
	newNode->m_wParent = nParentnode==nullptr ? 0 : nParentnode->m_wId;

	if(m_pNodelist!=nullptr)
	{
		POSITION pos = m_pNodelist->Find(nSelectnode);
		m_pNodelist->InsertBefore(pos, newNode);
	}

	TV_INSERTSTRUCT tvstruct;
	tvstruct.hParent = hParentItem;
	tvstruct.hInsertAfter = GetPrevSiblingItem(hSelectItem)==nullptr ? TVI_FIRST : GetPrevSiblingItem(hSelectItem);
	tvstruct.item.mask = TVIF_TEXT|TVIF_PARAM|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
	tvstruct.item.pszText = (LPTSTR)"New";
	tvstruct.item.iImage = newNode->Gettype();
	tvstruct.item.iSelectedImage = tvstruct.item.iImage;
	tvstruct.item.lParam = (DWORD)newNode;

	InsertItem(&tvstruct);
	return true;
}

bool CTree1::InsertIn(CTreeNode1* newNode)
{
	HTREEITEM hSelectItem = GetSelectedItem();
	CTreeNode1* nSelectnode = hSelectItem==nullptr ? nullptr : (CTreeNode1*)GetItemData(hSelectItem);

	newNode->m_wParent = nSelectnode==nullptr ? 0 : nSelectnode->m_wId;

	if(m_pNodelist!=nullptr)
	{
		if(hSelectItem!=nullptr)
		{
			POSITION pos = m_pNodelist->Find(nSelectnode);
			m_pNodelist->InsertAfter(pos, newNode);
		}
		else
			m_pNodelist->AddHead(newNode);
	}

	TV_INSERTSTRUCT tvstruct;
	tvstruct.hParent = hSelectItem;
	tvstruct.hInsertAfter = TVI_FIRST;
	tvstruct.item.mask = TVIF_TEXT|TVIF_PARAM|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
	tvstruct.item.pszText = (LPTSTR)"New";
	tvstruct.item.iImage = newNode->Gettype();
	tvstruct.item.iSelectedImage = tvstruct.item.iImage;
	tvstruct.item.lParam = (DWORD)newNode;

	HTREEITEM hItem = InsertItem(&tvstruct);

	if(nSelectnode==nullptr)
	{
		SelectItem(hItem);
	}

	return true;
}

bool ForceDelete = FALSE;

void CTree1::OnDelete()
{
	const HTREEITEM hItem = this->GetSelectedItem();
	if(hItem!=nullptr)
	{
		CTreeNode1* node = (CTreeNode1*)this->GetItemData(hItem);
		if(node!=nullptr)
		{
			ForceDelete = TRUE;
			this->DeleteItem(hItem);
			ForceDelete = FALSE;
		}
	}
}

void CTree1::OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult)
{
	if(m_pNodelist!=nullptr)
	{
		NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
		if(ForceDelete==TRUE)
		{
			CTreeNode1* node = (CTreeNode1*)pNMTreeView->itemOld.lParam;
			if(node!=nullptr)
			{
				const POSITION pos = m_pNodelist->Find(node);
				if(pos!=nullptr)
				{
					m_pNodelist->RemoveAt(pos);
				}
				delete node;
			}
		}
	}
	*pResult = 0;
}

void CTree1::Select(CTreeNode1* pLayer)
{
	const HTREEITEM hSelItem = this->GetSelectedItem();
	if(hSelItem==nullptr)
		return;

	CTreeNode1* selLayer = (CTreeNode1*)this->GetItemData(hSelItem);
	if(selLayer==pLayer)
		return;

	HTREEITEM hItem = this->GetRootItem();
	if(hItem==nullptr)
		return;

	do
	{
		CTreeNode1* node = (CTreeNode1*)this->GetItemData(hItem);
		if(node==pLayer)
		{
			SelectItem(hItem);
		}
	} while((hItem = GetDownItem(hItem))!=nullptr);
}

BOOL CTree1::DestroyWindow()
{
	if(m_pDragImage!=nullptr)
	{
		m_pDragImage->DeleteImageList();
		m_pDragImage->Detach();
		delete m_pDragImage;
		m_pDragImage = nullptr;
	}

	return CTreeCtrl::DestroyWindow();
}

CImageList* CTree1::CreateDragImage(HTREEITEM hItem)
{
	/*	if(this->GetImageList(TVSIL_NORMAL) != nullptr)
		{
			CImageList* pNormalImageList = this->GetImageList(TVSIL_NORMAL);

			CImageList* pImageList = new CImageList();
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
		CDC mdc;

		if(!mdc.CreateCompatibleDC(&dc))
			return nullptr;

		CBitmap bitmap;
		if(!bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height()))
			return nullptr;

		CBitmap* pOldMemDCBitmap = mdc.SelectObject(&bitmap);
		CFont* pOldFont = mdc.SelectObject(GetFont());

		mdc.FillSolidRect(&rect, RGB(0, 255, 0)); // Here green is used as mask color
		mdc.SetTextColor(GetSysColor(COLOR_GRAYTEXT));
		mdc.TextOut(rect.left, rect.top, GetItemText(hItem));

		mdc.SelectObject(pOldFont);
		mdc.SelectObject(pOldMemDCBitmap);

		// Create imagelist
		CImageList* pImageList = new CImageList();
		pImageList->Create(rect.Width(), rect.Height(), ILC_COLORDDB|ILC_MASK, 0, 1);
		pImageList->Add(&bitmap, RGB(0, 255, 0)); // Here green is used as mask color

		return pImageList;
	}
}

DWORD CTree1::GetMaxItemId() const
{
	WORD wMaxId = 0;
	if(m_pNodelist!=nullptr)
	{
		POSITION pos = m_pNodelist->GetHeadPosition();
		while(pos!=nullptr)
		{
			CTreeNode1* node = m_pNodelist->GetNext(pos);
			if(node->m_wId>wMaxId)
			{
				wMaxId = node->m_wId;
			}
		}
	}

	return wMaxId;
}
