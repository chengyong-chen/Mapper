#include "stdafx.h"

#include "CatalogTree.h"
#include "Atlas1.h"
#include "PageDlg.h"

#include "../Mapper/Global.h"
#include "../Viewer/Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL AFXAPI AfxComparePath(LPCTSTR lpszPath1, LPCTSTR lpszPath2);
extern BOOL AFXAPI AfxFullPath(LPTSTR lpszPathOut, LPCTSTR lpszFileIn);

CCatalogTree::CCatalogTree()
	: CTree1()
{
	m_pCatalog = nullptr;
}

CCatalogTree::~CCatalogTree()
{
}

BEGIN_MESSAGE_MAP(CCatalogTree, CTree1)
	//{{AFX_MSG_MAP(CCatalogTree)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_COMMAND(ID_ITEM_ADDAFTER, OnAddAfter)
	ON_COMMAND(ID_ITEM_ADDBEFORE, OnAddBefore)
	ON_COMMAND(ID_ITEM_ADDIN, OnAddIn)
	ON_COMMAND(ID_ITEM_ATTRIBUTE, OnAttribute)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCatalogTree message handlers
void CCatalogTree::BuildTree(CCatalog* pCatalog)
{
	if(pCatalog!=nullptr)
	{
		CTree1::BuildTree((CTreeNodeList*)&(pCatalog->m_PageList));
	}
	else
	{
		CTree1::BuildTree(nullptr);
	}

	m_pCatalog = pCatalog;
}

void CCatalogTree::OnAddAfter()
{
	CPage* pNewItem = new CPage();
	pNewItem->m_wType = CPage::typeFolder;
	pNewItem->m_wId = this->GetMaxItemId()+1;

	if(CTree1::InsertAfter(pNewItem)==false)
	{
		delete pNewItem;
	}
}

void CCatalogTree::OnAddBefore()
{
	CPage* pNewItem = new CPage();
	pNewItem->m_wType = CPage::typeFolder;
	pNewItem->m_wId = this->GetMaxItemId()+1;

	if(CTree1::InsertBefore(pNewItem)==false)
	{
		delete pNewItem;
	}
}

void CCatalogTree::OnAddIn()
{
	CPage* pNewItem = new CPage();
	pNewItem->m_wType = CPage::typeFolder;
	pNewItem->m_wId = this->GetMaxItemId()+1;

	if(CTree1::InsertIn(pNewItem)==false)
	{
		delete pNewItem;
	}
}

void CCatalogTree::OnAttribute()
{
	const HTREEITEM hSelectItem = GetSelectedItem();
	CPage* pSelectItem = hSelectItem==nullptr ? nullptr : (CPage*)GetItemData(hSelectItem);
	if(pSelectItem==nullptr)
		return;

	HINSTANCE hInst = ::LoadLibrary(_T("Atlas.dll"));
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(hInst);

	CPageDlg dlg(nullptr, pSelectItem->m_strTarget, pSelectItem->m_wType, pSelectItem->m_Anchor);
	if(dlg.DoModal()==IDOK)
	{
		pSelectItem->m_strTarget = dlg.m_strTarget;
		pSelectItem->m_Anchor.x = dlg.m_X;
		pSelectItem->m_Anchor.y = dlg.m_Y;
		if(pSelectItem->m_wType!=dlg.m_wType)
		{
			this->SetItemImage(hSelectItem, dlg.m_wType, dlg.m_wType);
			pSelectItem->m_wType = dlg.m_wType;
		}
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

CPage* CCatalogTree::SelectMap(CString strMap)
{
	HTREEITEM hItem = CTree1::GetRootItem();
	if(hItem==nullptr)
		return nullptr;

	do
	{
		CPage* pPage = (CPage*)CTree1::GetItemData(hItem);
		CString strFile = pPage->m_strTarget;

		TCHAR szMap[_MAX_PATH];
		GetLongPathName(strMap, szMap, _MAX_PATH);
		TCHAR szFile[_MAX_PATH];
		GetLongPathName(strFile, szFile, _MAX_PATH);

		if(AfxComparePath(szMap, szFile)==TRUE)
		{
			CTree1::SelectItem(hItem);
			return pPage;
		}
	} while((hItem = CTree1::GetDownItem(hItem))!=nullptr);

	return nullptr;
}

void CCatalogTree::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	*pResult = 0;

	if(pNMTreeView->itemOld.hItem==0)
		return;
	if(pNMTreeView->action==0)
		return;
	const HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	if(hItem==nullptr)
		return;

	CPage* pPage = (CPage*)GetItemData(hItem);
	if(pPage==nullptr)
	{
		AfxMessageBox(IDS_NOMATCHEDFILE);
		return;
	}
	if(pPage->m_strTarget.IsEmpty()==TRUE)
	{
		//		m_MapTree.Expand(hItem,TVE_TOGGLE);
		return;
	}

	CMDIFrameWnd* pMainFrame = (CMDIFrameWnd*)AfxGetMainWnd();
	if(pMainFrame==nullptr)
	{
		AfxMessageBox(IDS_NOMATCHEDFILE);
		return;
	}

	*pResult = 1;

	switch(pPage->m_wType)
	{
	case 0:
		break;
	case 1:
	{
		AfxGetMainWnd()->SendMessage(WM_DESTROYHTML, 0, 0);
		if(pPage->m_Anchor.x!=0&&pPage->m_Anchor.y!=0)
		{
			AfxGetApp()->WriteProfileInt(_T("NewView"), _T("DocX"), pPage->m_Anchor.x);
			AfxGetApp()->WriteProfileInt(_T("NewView"), _T("DocY"), pPage->m_Anchor.y);
		}
		const CString strMap = pPage->m_strTarget;

		CFrameWnd* pMainChild = (CFrameWnd*)pMainFrame->SendMessage(WM_GETMAINCHILD, 0, 0);
		if(pMainChild!=nullptr)
		{
			CView* pView = pMainChild->GetActiveView();
			if(pView!=nullptr)
			{
				CDocument* pDocument = pView->GetDocument();
				if(pDocument!=nullptr)
				{
					const CString string = pDocument->GetPathName();
					if(string.CollateNoCase(strMap)==0)
					{
						if(pPage->m_Anchor.x!=0&&pPage->m_Anchor.y!=0)
						{
							CRect rClient;
							pView->GetClientRect(rClient);
							CPoint cliPoint = rClient.CenterPoint();

							CPoint docPoint = pPage->m_Anchor;

							pView->SendMessage(WM_FIXATEVIEW, (UINT)&docPoint, (LONG)&cliPoint);
						}
						return;
					}
				}
			}

			pPage->Open(pMainChild, 0);
		}
		else
		{
			AfxGetApp()->OpenDocumentFile(strMap);
		}
	}
	break;
	case 2:
	{
		AfxGetMainWnd()->SendMessage(WM_DESTROYHTML, 0, 0);
		AfxGetApp()->OpenDocumentFile(pPage->m_strTarget);
	}
	break;
	}
}

BOOL CCatalogTree::OnCmdMsg(UINT nId, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
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
				if(m_pCatalog==nullptr)
					pCmdUI->Enable(FALSE);
				else
					pCmdUI->Enable(TRUE);
			}
			return TRUE;
			case ID_ITEM_ADDBEFORE:
			case ID_ITEM_ADDAFTER:
			{
				if(m_pCatalog==nullptr)
					pCmdUI->Enable(FALSE);
				else if(this->GetSelectedItem()==nullptr)
					pCmdUI->Enable(FALSE);
				else if(this->GetRootItem()==this->GetSelectedItem())
					pCmdUI->Enable(FALSE);
				else
					pCmdUI->Enable(TRUE);
			}
			return TRUE;
			case ID_ITEM_ATTRIBUTE:
			{
				if(m_pCatalog==nullptr)
					pCmdUI->Enable(FALSE);
				else if(this->GetSelectedItem()==nullptr)
					pCmdUI->Enable(FALSE);
				else
					pCmdUI->Enable(TRUE);
			}
			return TRUE;
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

//void CCatalogEar::OnLButtonDown(UINT nFlags, CPoint point) 
//{
//	CDialog::OnLButtonDown(nFlags, point);
//
//	if(m_pCatalog == nullptr)
//		return;
//
//	CWnd* ItemAtlas = ((CWnd*)GetDlgItem(IDC_ATLAS));
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
//					pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,rect.left,rect.bottom,this);
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
//	CWnd* ItemMap = ((CWnd*)GetDlgItem(IDC_MAP));
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
//					HTREEITEM hItem = m_CatalogTree.GetSelectedItem();
//					if(hItem == nullptr)
//					{
//						pSubMenu->EnableMenuItem(ID_ITEM_ADDBEFORE, MF_DISABLED | MF_GRAYED);
//						pSubMenu->EnableMenuItem(ID_ITEM_ADDAFTER, MF_DISABLED | MF_GRAYED);
//						pSubMenu->EnableMenuItem(ID_ITEM_DELETE, MF_DISABLED | MF_GRAYED);
//						pSubMenu->EnableMenuItem(ID_ITEM_ATTRIBUTE, MF_DISABLED | MF_GRAYED);
//					}
//
//					
//					ItemMap->GetWindowRect(&rect);
//					pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,rect.left,rect.bottom,&m_CatalogTree);
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
