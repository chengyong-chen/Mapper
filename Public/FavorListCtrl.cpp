#include "Public.h"
#include "Global.h"
#include "resource.h"


/////////////////////////////////////////////////////////////////////////////
// TFavorListCtrl
template<class T> TFavorListCtrl<T>::TFavorListCtrl(std::list<T*>& elements)
	:m_elements(elements)
{
	m_bModified = false;
	m_pDocument = nullptr;
}

template<class T> int TFavorListCtrl<T>::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if(CListCtrlEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	HINSTANCE hInst = ::LoadLibrary(_T("Public.dll"));
	HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(hInst);
	CImageList smallImages;
	if(smallImages.Create(IDB_ITEMSMALL, 16, 0, RGB(0,128,128)) == TRUE)
	{
		smallImages.SetOverlayImage(0, 1);
		SetImageList(&smallImages,LVSIL_SMALL);
		smallImages.Detach();
	}
	
	CListCtrlEx::SetExtendedStyle(TVS_SHOWSELALWAYS | LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES);
	CListCtrlEx::SetIconSpacing(0,0);   

	CString columName1;
	CString columName2;
	CString columName3;
	columName1.LoadString(IDS_KEYNAME);
	columName2.LoadString(IDS_KEYMAP);
	columName3.LoadString(IDS_KEYLAYER);

	LV_COLUMN	lvcolumn;

	lvcolumn.mask     = LVCF_FMT | LVCF_IMAGE | LVCF_WIDTH;
	lvcolumn.fmt      = LVCFMT_CENTER;
	lvcolumn.iImage   = 3;
	lvcolumn.cx       = 20;
	this->InsertColumn(0, &lvcolumn);

	lvcolumn.mask     = LVCF_FMT | LVCF_IMAGE | LVCF_WIDTH;
	lvcolumn.fmt      = LVCFMT_CENTER;
	lvcolumn.iImage   = 4;
	lvcolumn.cx       = 18;
	this->InsertColumn(1, &lvcolumn);

	lvcolumn.mask     = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt      = LVCFMT_LEFT;
	lvcolumn.cx       = 125;
	lvcolumn.pszText  = columName1.GetBuffer(0);
	this->InsertColumn(2, &lvcolumn);
	columName1.ReleaseBuffer();
//	CListCtrlEx::SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	CHeaderCtrl* pHeaderCtrl = (CHeaderCtrl*)this->GetHeaderCtrl();   
	if(pHeaderCtrl != nullptr)
	{
		pHeaderCtrl->SetBitmapMargin(0);
		pHeaderCtrl->EnableWindow(FALSE);   
	}
	CListCtrlEx::ModifyStyle(TVS_NOHSCROLL,0);
	CListCtrlEx::ModifyStyle(0,TVS_NOHSCROLL);
	CListCtrlEx::ModifyStyle(WS_HSCROLL,0);
	CListCtrlEx::ModifyStyle(0,WS_VSCROLL);
	CListCtrlEx::ModifyStyleEx(WS_EX_CLIENTEDGE,0);

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
	return 0;
}

template<class T> BOOL TFavorListCtrl<T>::Create(CWnd* parent,bool bEditble)
{ 
	DWORD dwStyle = LVS_REPORT | LVS_ALIGNLEFT | LVS_NOLABELWRAP | LVS_SHOWSELALWAYS | LVS_SINGLESEL | WS_CHILD | WS_VISIBLE | LVS_EX_FULLROWSELECT; 
	dwStyle &= ~LVS_SORTASCENDING;
	dwStyle &= ~LVS_SORTDESCENDING;
	dwStyle &= ~LVS_EDITLABELS;
	dwStyle &= ~WS_EX_CLIENTEDGE;

/*	if(bEditble == true)
		dwStyle |=  LVS_EDITLABELS;
	else
		dwStyle &= ~LVS_EDITLABELS;		
*/

	EnableToolTips(FALSE);
	m_bEditble = bEditble;

	return CListCtrlEx::Create(dwStyle,CRect(0,0,0,0), parent, 0);
}

template<class T> void TFavorListCtrl<T>::OnSize(UINT nType, int cx, int cy) 
{	
	CListCtrlEx::OnSize(nType, cx, cy);

	CHeaderCtrl* pHeaderCtrl = (CHeaderCtrl*)this->GetHeaderCtrl();   
	if(pHeaderCtrl != nullptr)
	{
		HDITEM hditem;
		hditem.mask = HDI_WIDTH;
		hditem.cxy = max(0,cx - 18 - 18 - 1 - GetSystemMetrics(SM_CXVSCROLL));
		pHeaderCtrl->SetItem(2,&hditem);
	}
}

template<class T> void TFavorListCtrl<T>::Build()
{
	this->DeleteAllItems();

	for(typename std::list<T*>::iterator it = m_elements.begin(); it != m_elements.end(); ++it)
	{
		T* pT= *it;
		int nItem = this->GetItemCount();
		int index = this->InsertItem(nItem,nullptr,0);

		this->SetItem(index,0,LVIF_IMAGE,nullptr,pT->m_bVisible == false ? 0:1,0,0,0);   
		this->SetItem(index,1,LVIF_IMAGE,nullptr,pT->m_bShowTag == false ? 0:1,0,0,0);
		const CPOUHeaderProfile& headerprofile = pT->m_datasource.GetHeaderProfile();
		if(pT->m_bVisible == false || headerprofile.m_strAnnoField.IsEmpty() == TRUE)
		{
			this->SetItem(index,1,LVIF_IMAGE,nullptr,2,0,0,0);
		}
		else
		{
		}
		
		this->SetItemText(index,2,pT->m_strName);	
		this->SetItemData(index,(DWORD)pT);
	}   
	
	if(CListCtrlEx::GetItemCount()>0)
	{
		CListCtrlEx::SetItemState(0, LVIS_SELECTED,   LVIS_SELECTED);
	}
}

template<class T> void TFavorListCtrl<T>::OnLButtonDown(UINT nFlags, CPoint point) 
{
	LVHITTESTINFO HitTestInfo;
	HitTestInfo.pt = point;
	long hItem= CListCtrlEx::SubItemHitTest(&HitTestInfo);
	if(/*(HitTestInfo.flags&LVHT_ONITEMICON) == LVHT_ONITEMICON &&*/ hItem>=0)
	{
		T* pT = (T*)this->GetItemData(hItem);
		if(pT != nullptr)
		{
			const CPOUHeaderProfile& headerprofile = pT->m_datasource.GetHeaderProfile();
			if(HitTestInfo.iSubItem == 0)
			{
				if(pT->m_bVisible == TRUE)
				{
					this->SetItem(hItem,0,LVIF_IMAGE,nullptr,0,0,0,0);   
					this->SetItem(hItem,1,LVIF_IMAGE,nullptr,2,0,0,0);   
				}
				else
				{
					this->SetItem(hItem,0,LVIF_IMAGE,nullptr,1,0,0,0);   
					if(headerprofile.m_strAnnoField.IsEmpty() == FALSE)
					{
						this->SetItem(hItem,1,LVIF_IMAGE,nullptr,pT->m_bShowTag == TRUE ? 1:0,0,0,0);   
					}
				}

				pT->m_bVisible = !pT->m_bVisible;
				pT->Invalidate(m_pDocument);

				if(m_pDocument != nullptr && m_bEditble == true)
				{
					m_pDocument->SetModifiedFlag(TRUE);
					m_bModified = true;
				}
			}
			else if(HitTestInfo.iSubItem == 1)
			{
				if(pT->m_bVisible == TRUE && headerprofile.m_strAnnoField.IsEmpty() == FALSE)
				{
					pT->m_bShowTag = !pT->m_bShowTag;
					this->SetItem(hItem,1,LVIF_IMAGE,nullptr,pT->m_bShowTag == TRUE ? 1:0,0,0,0);   

					pT->Invalidate(m_pDocument);
				}
			}
			else if(HitTestInfo.iSubItem == 2)
			{
				return CListCtrlEx::OnLButtonDown(nFlags, point);
			}
		}
	}//58691367 58691368 13911014760

	CListCtrl::OnLButtonDown(nFlags, point);
}

template<class T> T* TFavorListCtrl<T>::GetActive()
{	
	long Item = GetNextItem(-1,LVNI_SELECTED);
	if(Item >= 0)
	{
		T* pT  = (T*)GetItemData(Item);
		return pT;
	}
	else
		return nullptr;
}	

template<class T> void TFavorListCtrl<T>::SetActive(T* pT)
{	
	long Item = GetNextItem(-1,LVNI_SELECTED);
	if(Item >= 0)
	{
		SetItemData(Item,(DWORD)pT);
	}
}	

template<class T> void TFavorListCtrl<T>::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CListCtrlEx::OnEndlabeledit(pNMHDR,pResult);

	if(m_bEditble == true)
	{
		LV_DISPINFO* plvDispInfo = (LV_DISPINFO*)pNMHDR;
		LV_ITEM* plvItem = &plvDispInfo->item;
		if(plvItem->iSubItem == 2)
		{
			T* pT  = (T*)GetItemData(plvItem->iItem);
			if(pT != nullptr)
			{
				pT->m_strName = plvItem->pszText;
				m_bModified = true;
			}
		}
	}
}

template<class T> void TFavorListCtrl<T>::OnUpdateDelete(CCmdUI* pCmdUI)
{
	long Item = this->GetNextItem(-1,LVNI_SELECTED);
	if(Item < 0)
	{
		pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->Enable(TRUE);
	}
}

template<class T> void TFavorListCtrl<T>::OnSetHint()
{
	long Item = GetNextItem(-1,LVNI_SELECTED);
	if(Item<0)
		return;

	T* pT = (T*)GetItemData(Item);
	if(pT == nullptr)
		return;

	if(pT->SetHint(m_pDocument) == true)	
	{
		if(m_pDocument != nullptr)
		{
			m_pDocument->SetModifiedFlag(TRUE);
		}
		m_bModified = true;

		if(pT->m_bVisible == TRUE)
		{
			pT->Invalidate(m_pDocument);

			if(m_pDocument != nullptr)
			{
				m_pDocument->SetModifiedFlag(TRUE);
			}
		}
	}
}

template<class T> void TFavorListCtrl<T>::OnUpdateSetHint(CCmdUI* pCmdUI)
{
	long Item = this->GetNextItem(-1,LVNI_SELECTED);
	if(Item < 0)
	{
		pCmdUI->Enable(FALSE);
	}
	else
	{
		T* pT = (T*)this->GetItemData(Item);
		if(pT != nullptr)
		{

			if(pT->m_datasource.GetHeaderProfile().m_strAnnoField.IsEmpty() == TRUE)
				pCmdUI->Enable(FALSE);
			else if(pT->m_datasource.GetHeaderProfile().m_strKeyTable.IsEmpty() == TRUE)
				pCmdUI->Enable(FALSE);
			else
				pCmdUI->Enable(TRUE);
		}
	}
}

template<class T> BOOL TFavorListCtrl<T>::DestroyWindow() 
{	
	this->DeleteAllItems();

	return CListCtrlEx::DestroyWindow();
}

template<class T> void TFavorListCtrl<T>::OnAllon()
{
	for(int index=0;index<this->GetItemCount();index++)
	{
		T* pT = (T*)this->GetItemData(index);
		if(pT->m_bVisible == false)
		{
			pT->m_bVisible = true;
			pT->Invalidate(m_pDocument);

			this->SetItem(index,0,LVIF_IMAGE,nullptr,1,0,0,0);   
			if(pT->m_datasource.GetHeaderProfile().m_strAnnoField.IsEmpty() == FALSE)
			{
				this->SetItem(index,1,LVIF_IMAGE,nullptr,pT->m_bShowTag == TRUE ? 1:0,0,0,0);   
			}

			this->SetItemState(index,INDEXTOSTATEIMAGEMASK(2), TVIS_STATEIMAGEMASK);
		}
	}

	if(m_pDocument != nullptr)
	{
		if(m_bEditble == true)
		{
			m_pDocument->SetModifiedFlag(TRUE);
			m_bModified = true;
		}
	}
}

template<class T> void TFavorListCtrl<T>::OnUpdateAllon(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(FALSE);
	for(int index=0;index<GetItemCount();index++)
	{
		T* pT = (T*)GetItemData(index);
		if(pT->m_bVisible == false)
		{
			pCmdUI->Enable(TRUE);
			break;
		}
	}
}

template<class T> void TFavorListCtrl<T>::OnAlloff()
{
	for(int index=0;index<this->GetItemCount();index++)
	{
		T* pT = (T*)this->GetItemData(index);
		if(pT->m_bVisible == TRUE)
		{
			pT->Invalidate(m_pDocument);
			pT->m_bVisible = FALSE;

			this->SetItem(index,0,LVIF_IMAGE,nullptr,0,0,0,0);   
			this->SetItem(index,1,LVIF_IMAGE,nullptr,2,0,0,0);   
		}
	}

	if(m_pDocument != nullptr)
	{
		if(m_bEditble == true)
		{
			m_pDocument->SetModifiedFlag(TRUE);
			m_bModified = true;
		}
	}
}

template<class T> void TFavorListCtrl<T>::OnUpdateAlloff(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(FALSE);
	for(int index=0;index<GetItemCount();index++)
	{
		T* pT = (T*)GetItemData(index);
		if(pT->m_bVisible == TRUE)
		{
			pCmdUI->Enable(TRUE);
			break;
		}
	}
}
template<class T> void TFavorListCtrl<T>::OnUpdateSetup(CCmdUI* pCmdUI)
{
	T* pT = GetActive();
	pCmdUI->Enable(pT == nullptr ? FALSE : TRUE);
}