#include "stdafx.h"
#include "Pyramid.h"
#include "PyramidTree.h"

#include "Deck.h"
#include "DeckDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPyramidTree::CPyramidTree()
	: CTree1()
{
	m_pPyramid = nullptr;
}

CPyramidTree::~CPyramidTree()
{
}

BEGIN_MESSAGE_MAP(CPyramidTree, CTree1)
	//{{AFX_MSG_MAP(CPyramidTree)
	ON_COMMAND(ID_ITEM_ADDAFTER, OnAddAfter)
	ON_COMMAND(ID_ITEM_ADDBEFORE, OnAddBefore)
	ON_COMMAND(ID_ITEM_ADDIN, OnAddIn)
	ON_COMMAND(ID_ITEM_ATTRIBUTE, OnAttribute)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPyramidTree message handlers
void CPyramidTree::BuildTree(CPyramid* pPyramid)
{
	if(pPyramid!=nullptr)
	{
		CTree1::BuildTree((CTreeNodeList*)&(pPyramid->m_DeckList));
	}
	else
	{
		CTree1::BuildTree(nullptr);
	}

	m_pPyramid = pPyramid;
}

void CPyramidTree::OnAddAfter()
{
	CDeck* pNewItem = new CDeck();
	pNewItem->m_wId = this->GetMaxItemId()+1;
	if(CTree1::InsertAfter(pNewItem)==false)
	{
		delete pNewItem;
	}
	else if(m_pPyramid!=nullptr)
	{
		CDeck* pParent = m_pPyramid->GetDeck(pNewItem->m_wParent);
		pNewItem->m_pParent = pParent;
	}
}

void CPyramidTree::OnAddBefore()
{
	CDeck* pNewItem = new CDeck();
	pNewItem->m_wId = this->GetMaxItemId()+1;
	if(CTree1::InsertBefore(pNewItem)==false)
	{
		delete pNewItem;
	}
	else if(m_pPyramid!=nullptr)
	{
		CDeck* pParent = m_pPyramid->GetDeck(pNewItem->m_wParent);
		pNewItem->m_pParent = pParent;
	}
}

void CPyramidTree::OnAddIn()
{
	CDeck* pNewItem = new CDeck();
	pNewItem->m_wId = this->GetMaxItemId()+1;
	if(CTree1::InsertIn(pNewItem)==false)
	{
		delete pNewItem;
	}
	else if(m_pPyramid!=nullptr)
	{
		if(m_pPyramid->m_DeckList.GetCount()==1)
		{
			m_pPyramid->m_pApex = pNewItem;
		}
		else
		{
			CDeck* pParent = m_pPyramid->GetDeck(pNewItem->m_wParent);
			pNewItem->m_pParent = pParent;
		}
	}
}

void CPyramidTree::OnAttribute()
{
	const HTREEITEM hSelectItem = GetSelectedItem();
	CDeck* pDeck = hSelectItem==nullptr ? nullptr : (CDeck*)GetItemData(hSelectItem);
	if(pDeck==nullptr)
		return;
	HINSTANCE hInst = ::LoadLibrary(_T("Atlas.dll"));
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(hInst);

	CDeckDlg dlg(nullptr, pDeck->m_strFile, pDeck->m_strHtml, pDeck->m_strLegend, pDeck->m_bNavigatable, pDeck->m_bQueryble);
	if(dlg.DoModal()==IDOK)
	{
		pDeck->m_strFile = dlg.m_strFile;
		pDeck->m_strHtml = dlg.m_strHtml;
		pDeck->m_strLegend = dlg.m_strLegend;
		pDeck->m_bNavigatable = dlg.m_bNavigatable;
		pDeck->m_bQueryble = dlg.m_bQueryble;
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

BOOL CPyramidTree::OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if(pHandlerInfo==nullptr)
	{
		if(nCode==CN_UPDATE_COMMAND_UI)
		{
			CCmdUI* pCmdUI = (CCmdUI*)pExtra;
			if(pCmdUI->m_bContinueRouting)
			{
				return CTree1::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
			}

			ASSERT(!pCmdUI->m_bContinueRouting);

			switch(nId)
			{
			case ID_ITEM_ADDIN:
			{
				if(m_pPyramid==nullptr)
					pCmdUI->Enable(FALSE);
				else
					pCmdUI->Enable(TRUE);
			}
			return TRUE;
			break;
			case ID_ITEM_ADDBEFORE:
			case ID_ITEM_ADDAFTER:
			{
				if(m_pPyramid==nullptr)
					pCmdUI->Enable(FALSE);
				else if(this->GetSelectedItem()==nullptr)
					pCmdUI->Enable(FALSE);
				else if(this->GetRootItem()==this->GetSelectedItem())
					pCmdUI->Enable(FALSE);
				else
					pCmdUI->Enable(TRUE);
			}
			return TRUE;
			break;
			case ID_ITEM_ATTRIBUTE:
			{
				if(m_pPyramid==nullptr)
					pCmdUI->Enable(FALSE);
				else if(this->GetSelectedItem()==nullptr)
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

	return CTree1::OnCmdMsg(nId, nCode, pExtra, pHandlerInfo);
}

//void CPyramidEar::OnLButtonDown(UINT nFlags, CPoint point) 
//{
//	CPropertyPage::OnLButtonDown(nFlags, point);
//
//	if(m_pPyramid == nullptr)
//		return;
//
//	CWnd* ItemAtlas = ((CWnd*)GetDlgItem(IDC_ATLAS));
//	CWnd* ItemMap = ((CWnd*)GetDlgItem(IDC_MAP));
//
//	if(ItemAtlas != nullptr && (ItemAtlas->GetStyle()  & WS_VISIBLE) != 0)
//	{
//		CRect rect;
//		ItemAtlas->GetWindowRect(&rect);
//		ScreenToClient(&rect);
//		if(rect.PtInRect(point) == TRUE)
//		{	
//			HINSTANCE hInst = ::LoadLibrary(_T("Atlas.dll"));
//			HINSTANCE hInstOld = AfxGetResourceHandle();
//			AfxSetResourceHandle(hInst);
//
//			CMenu Menu;			
//			if(Menu.LoadMenu(IDR_ATLAS))			
//			{			
//				CMenu* pSubMenu = Menu.GetSubMenu(0);			
//				if(pSubMenu!=nullptr)			
//				{			
//					ItemAtlas->GetWindowRect(&rect);
//					pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,rect.left,rect.bottom,this->GetParent());
//					pSubMenu->DestroyMenu();
//
//				}
//			}			
//			Menu.DestroyMenu();
//
//			AfxSetResourceHandle(hInstOld);
//			::FreeLibrary(hInst);
//			return;
//		}
//	}
//
//	if(ItemMap != nullptr && (ItemMap->GetStyle()  & WS_VISIBLE) != 0)
//	{	
//		CRect rect;
//		ItemMap->GetWindowRect(&rect);
//		ScreenToClient(&rect);
//		if(rect.PtInRect(point) == TRUE)
//		{	
//			HINSTANCE hInst = ::LoadLibrary(_T("Atlas.dll"));
//			HINSTANCE hInstOld = AfxGetResourceHandle();
//			AfxSetResourceHandle(hInst);
//
//			CMenu Menu;			
//			if(Menu.LoadMenu(IDR_ITEM))			
//			{			
//				CMenu* pSubMenu = Menu.GetSubMenu(0);			
//				if(pSubMenu!=nullptr)			
//				{	
//					HTREEITEM hItem = m_PyramidTree.GetSelectedItem();
//					if(hItem == nullptr)
//					{
//						pSubMenu->EnableMenuItem(ID_ITEM_ADDBEFORE, MF_DISABLED | MF_GRAYED);
//						pSubMenu->EnableMenuItem(ID_ITEM_ADDAFTER, MF_DISABLED | MF_GRAYED);
//						pSubMenu->EnableMenuItem(ID_ITEM_DELETE, MF_DISABLED | MF_GRAYED);
//						pSubMenu->EnableMenuItem(ID_ITEM_ATTRIBUTE, MF_DISABLED | MF_GRAYED);
//					}
//					
//					ItemMap->GetWindowRect(&rect);
//					pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,rect.left,rect.bottom,&m_PyramidTree);
//					pSubMenu->DestroyMenu();
//				}
//			}			
//			Menu.DestroyMenu();
//
//			AfxSetResourceHandle(hInstOld);
//			::FreeLibrary(hInst);
//			return;
//		}
//	}
//}
