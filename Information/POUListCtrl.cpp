#include "stdafx.h"
#include "Global.h"

#include "POUListCtrl.h"
#include "POU.h"
#include "POUList.h"
#include "resource.h"

#include "POU.h"

#include "../Style/Spot.h"

#include "../Public/ODBCDatabase.h"

#include "../Database/ConnectionDlg.h"
#include "../Database/POUDatasourceDlg.h"

using namespace std;
#include <list>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPOUListCtrl

CPOUListCtrl::CPOUListCtrl(CPOUList& poulist)
	: TFavorListCtrl(poulist.m_pous), m_poulist(poulist)
{
}

BEGIN_MESSAGE_MAP(CPOUListCtrl, TFavorListCtrl)
	//{{AFX_MSG_MAP(CPOUListCtrl)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnSelchanged)

	ON_COMMAND(ID_POU_NEW, OnNew)
	ON_UPDATE_COMMAND_UI(ID_POU_NEW, OnUpdateNew)
	ON_COMMAND(ID_POU_DELETE, OnDelete)
	ON_UPDATE_COMMAND_UI(ID_POU_DELETE, OnUpdateDelete)
	ON_COMMAND(ID_POU_SETSPOT, OnSetSpot)
	ON_UPDATE_COMMAND_UI(ID_POU_SETSPOT, OnUpdateSet)
	ON_COMMAND(ID_POU_SETHINT, TFavorListCtrl::OnSetHint)
	ON_UPDATE_COMMAND_UI(ID_POU_SETHINT, TFavorListCtrl::OnUpdateSetHint)
	ON_COMMAND(ID_POU_SETWHERE, OnSetWhere)
	ON_UPDATE_COMMAND_UI(ID_POU_SETWHERE, OnUpdateSetWhere)
	ON_COMMAND(ID_POU_SETTAG, SetTag)
	ON_UPDATE_COMMAND_UI(ID_POU_SETTAG, OnUpdateSetTag)
	ON_COMMAND(ID_POU_ALLON, OnAllon)
	ON_UPDATE_COMMAND_UI(ID_POU_ALLON, TFavorListCtrl::OnUpdateAllon)
	ON_COMMAND(ID_POU_ALLOFF, TFavorListCtrl::OnAlloff)
	ON_UPDATE_COMMAND_UI(ID_POU_ALLOFF, TFavorListCtrl<CPOU>::OnUpdateAlloff)
	ON_UPDATE_COMMAND_UI(ID_POU_DATASOURCE, OnUpdateSet)
	ON_COMMAND(ID_POU_CONNECTION, OnDatabase)
	ON_COMMAND(ID_POU_DATASOURCE, OnDatasource)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CPOUListCtrl::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pnmListView = (NM_LISTVIEW*)pNMHDR;
	const int index = pnmListView->iItem;
	if(index==-1)
		return;

	CPOU* pPOU = (CPOU*)GetItemData(index);
	if(pPOU==nullptr)
		return;

	const CHeaderProfile& headerprofile = pPOU->m_datasource.GetHeaderProfile();
	if(headerprofile.m_strKeyTable=="")
		return;
	else if(m_pDocument!=nullptr)
	{
		POSITION viewPos = m_pDocument->GetFirstViewPosition();
		CView* pView = m_pDocument->GetNextView(viewPos);
		if(pView!=nullptr)
		{
			pView->SendMessage(WM_POUACTIVATED, (DWORD)this, (LONG)pPOU);
		}
	}
}

void CPOUListCtrl::OnNew()
{
	CPOU* pPOU = new CPOU(m_poulist);
	m_poulist.m_pous.push_back(pPOU);

	DWORD dwId = 0;
	for(std::list<CPOU*>::iterator it = m_elements.begin(); it!=m_elements.end(); ++it)
	{
		dwId = max((*it)->m_dwId, dwId);
	}
	pPOU->m_dwId = ++dwId;
	const CPOUHeaderProfile& headerprofile = pPOU->m_datasource.GetHeaderProfile();
	const int nCount = this->GetItemCount();
	const int index = this->InsertItem(nCount, nullptr, 0);

	this->SetItem(index, 0, LVIF_IMAGE, nullptr, pPOU->m_bVisible==false ? 0 : 1, 0, 0, 0);
	this->SetItem(index, 1, LVIF_IMAGE, nullptr, pPOU->m_bShowTag==false ? 0 : 1, 0, 0, 0);
	if(pPOU->m_bVisible==false||headerprofile.m_strAnnoField.IsEmpty()==TRUE)
		this->SetItem(index, 1, LVIF_IMAGE, nullptr, 2, 0, 0, 0);

	this->SetItemText(index, 2, pPOU->m_strName);
	this->SetItemData(index, (DWORD)pPOU);

	if(m_pDocument!=nullptr)
	{
		m_pDocument->SetModifiedFlag(TRUE);
	}

	m_bModified = true;

	if(nCount==0)
	{
		TFavorListCtrl::SetItemState(index, LVIS_SELECTED, LVIS_SELECTED);
	}
	return;
}

void CPOUListCtrl::OnUpdateNew(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_poulist.m_DBList.size()>0 ? TRUE : FALSE);
}

void CPOUListCtrl::OnDelete()
{
	const long Item = GetNextItem(-1, LVNI_SELECTED);
	if(Item<0)
		return;

	CPOU* pPOU = (CPOU*)GetItemData(Item);
	DeleteItem(Item);

	if(pPOU==nullptr)
		return;
	const std::list<CPOU*>::iterator it = std::find(m_elements.begin(), m_elements.end(), pPOU);
	if(it!=m_elements.end())
		m_elements.erase(it);
	if(pPOU->m_pSpot!=nullptr)
	{
		pPOU->m_pSpot->Decrease(m_poulist.m_library);
	}

	pPOU->Invalidate(m_pDocument);
	delete pPOU;
	pPOU = nullptr;

	if(m_pDocument!=nullptr)
	{
		m_pDocument->SetModifiedFlag(TRUE);
	}

	m_bModified = true;
}

void CPOUListCtrl::OnSetWhere()
{
	const long Item = GetNextItem(-1, LVNI_SELECTED);
	if(Item<0)
		return;

	CPOU* pPOU = (CPOU*)GetItemData(Item);
	if(pPOU==nullptr)
		return;

	if(pPOU->SetWhere(m_pDocument)==true)
	{
		if(pPOU->m_bVisible==TRUE)
		{
			pPOU->Clear();
			pPOU->Invalidate(m_pDocument);
		}
		if(m_pDocument!=nullptr)
		{
			m_pDocument->SetModifiedFlag(TRUE);
		}
		m_bModified = true;
	}
}

void CPOUListCtrl::OnUpdateSetWhere(CCmdUI* pCmdUI)
{
	const long Item = GetNextItem(-1, LVNI_SELECTED);
	if(Item<0)
		return;

	CPOU* pPOU = (CPOU*)GetItemData(Item);
	if(pPOU==nullptr)
		return;

	CODBCDatabase* pDatabase = m_poulist.GetDatabase(pPOU->m_datasource.m_strDatabase);
	if(pDatabase==nullptr)
		return;

	if(pDatabase->IsOpen()==FALSE)
		pCmdUI->Enable(FALSE);
	else
	{
		const long Item = this->GetNextItem(-1, LVNI_SELECTED);
		if(Item<0)
		{
			pCmdUI->Enable(FALSE);
		}
		else
		{
			CPOU* pPOU = (CPOU*)this->GetItemData(Item);
			if(pPOU!=nullptr)
			{
				pCmdUI->Enable(TRUE);
			}
		}
	}
}

void CPOUListCtrl::SetTag()
{
	const long Item = GetNextItem(-1, LVNI_SELECTED);
	if(Item<0)
		return;

	CPOU* pPOU = (CPOU*)GetItemData(Item);
	if(pPOU==nullptr)
		return;

	if(pPOU->SetTag(m_pDocument)==true)
	{
		if(m_pDocument!=nullptr)
		{
			m_pDocument->SetModifiedFlag(TRUE);
		}
		m_bModified = true;

		if(pPOU->m_bVisible==false)
			return;

		const CPOUHeaderProfile& headerprofile = pPOU->m_datasource.GetHeaderProfile();
		if(headerprofile.m_strAnnoField.IsEmpty()==TRUE)
		{
			this->SetItem(Item, 1, LVIF_IMAGE, nullptr, 2, 0, 0, 0);
		}
		else
		{
			this->SetItem(Item, 1, LVIF_IMAGE, nullptr, pPOU->m_bShowTag==TRUE ? 1 : 0, 0, 0, 0);
		}

		pPOU->Clear();
		pPOU->Invalidate(m_pDocument);
	}
}

void CPOUListCtrl::OnUpdateSetTag(CCmdUI* pCmdUI)
{
	const long Item = GetNextItem(-1, LVNI_SELECTED);
	if(Item<0)
		return;

	CPOU* pPOU = (CPOU*)GetItemData(Item);
	if(pPOU==nullptr)
		return;

	CODBCDatabase* pDatabase = m_poulist.GetDatabase(pPOU->m_datasource.m_strDatabase);
	if(pDatabase==nullptr)
		return;

	if(pDatabase->IsOpen()==FALSE)
		pCmdUI->Enable(FALSE);
	else
	{
		const long Item = this->GetNextItem(-1, LVNI_SELECTED);
		if(Item<0)
		{
			pCmdUI->Enable(FALSE);
		}
		else
		{
			CPOU* pPOU = (CPOU*)this->GetItemData(Item);
			if(pPOU!=nullptr)
			{
				if(pPOU->m_datasource.GetHeaderProfile().m_strKeyTable.IsEmpty()==TRUE)
					pCmdUI->Enable(FALSE);
				else
					pCmdUI->Enable(TRUE);
			}
		}
	}
}

void CPOUListCtrl::OnSetSpot()
{
	const long Item = GetNextItem(-1, LVNI_SELECTED);
	if(Item<0)
		return;

	CPOU* pPOU = (CPOU*)this->GetItemData(Item);
	if(pPOU==nullptr)
		return;

	if(pPOU->SetSpot(m_pDocument)==true)
	{
		m_bModified = true;
	}
}

void CPOUListCtrl::OnUpdateSet(CCmdUI* pCmdUI)
{
	const long Item = this->GetNextItem(-1, LVNI_SELECTED);
	if(Item<0)
	{
		pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->Enable(TRUE);
	}
}

void CPOUListCtrl::OnDatabase()
{
	const long Item = GetNextItem(-1, LVNI_SELECTED);
	if(Item<0)
		return;

	CPOU* pPou = (CPOU*)this->GetItemData(Item);
	if(pPou==nullptr)
		return;

	std::list<CString*> activenames;
	std::list<CPOU*>;
	for(std::list<CPOU*>::iterator it = m_poulist.m_pous.begin(); it!=m_poulist.m_pous.end(); ++it)
	{
		CPOU* pou = *it;
		activenames.push_back(&pou->m_datasource.m_strDatabase);
	}
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Database.dll"));
	AfxSetResourceHandle(hInst);

	CConnectionDlg dlg(this, true, m_poulist.m_DBList, activenames, pPou->m_datasource.m_strDatabase);
	if(dlg.DoModal()==IDOK)
	{
		pPou->m_datasource.m_strDatabase = dlg.m_strDatabase;

		POSITION pos = m_pDocument->GetFirstViewPosition();
		while(pos!=nullptr)
		{
			const CView* pView = m_pDocument->GetNextView(pos);
			pView->SendMessage(WM_POUACTIVATED, 0, (LONG)pPou);
		}
		m_pDocument->SetModifiedFlag(TRUE);
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

void CPOUListCtrl::OnDatasource()
{
	const long Item = GetNextItem(-1, LVNI_SELECTED);
	if(Item<0)
		return;

	CPOU* pPOU = (CPOU*)this->GetItemData(Item);
	if(pPOU==nullptr)
		return;

	CPOUHeaderProfile headerprofile = (CPOUHeaderProfile&)(pPOU->m_datasource.GetHeaderProfile());
	CODBCDatabase* pDatabase = m_poulist.GetDatabase(pPOU->m_datasource.m_strDatabase);
	if(pDatabase==nullptr)
		return;
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Database.dll"));
	AfxSetResourceHandle(hInst);

	CPOUDatasourceDlg dlg(this, pDatabase, headerprofile);
	if(dlg.DoModal()==IDOK)
	{
		if(pPOU->m_datasource.GetHeaderProfile()!=headerprofile)
		{
			pPOU->m_datasource.GetHeaderProfile() = headerprofile;
			m_pDocument->UpdateAllViews(nullptr);

			POSITION pos = m_pDocument->GetFirstViewPosition();
			while(pos!=nullptr)
			{
				const CView* pView = m_pDocument->GetNextView(pos);
				pView->SendMessage(WM_POUACTIVATED, 0, (LONG)pPOU);
			}
			m_pDocument->SetModifiedFlag(TRUE);
		}
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}
